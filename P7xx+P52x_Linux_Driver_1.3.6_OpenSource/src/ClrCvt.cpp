/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright 2014-2015 HiTi Digital, Inc.  All Rights Reserved
**********************************************************/

//#ifdef WIN32
//#include "stdafx.h"
//#endif

#include "ClrCvt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mytypes.h"
#include "Util0.h"
//#include "AFile.h"

#include "P52x_ColorTable.h"
#include "P72x_ColorTable.h"
#include "P75x_ColorTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClrCvt::CClrCvt()
{

	m_pTable = (unsigned char*)NULL;
	InitInterpolatePara();
	m_bTableDynamic = false;

}

CClrCvt::~CClrCvt()
{
    if(m_bTableDynamic)
    {
        if(m_pTable != NULL) {
            //2020.08.05 changed by Bill
            //delete[] m_pTable;
            free(m_pTable);

            m_pTable = NULL;
        }
        m_bTableDynamic = false;
    }
}

unsigned long CClrCvt::LoadColorTable(char* pPath, unsigned char MediaType, unsigned long Mode, unsigned long dwDrvModel, bool bNewTPH)
{
	unsigned long ErrorCode = ERROR_SUCCESS;

	FILE	*fp = NULL;
	char	pFileName[512]={0};
	//char	pFullName[512]={0};
	char	slash[4]={0};
	char	media[8]={0};
	char	model[8]={0};
	char	mode[4] ={0};
	char	ver[4]={0};
	char    ProfileName[512]={0};

	FILE 	*Profile = NULL;

	unsigned long  size					= 0;
	unsigned short ColorMode			= 0;
	unsigned short ColorTableVersion	= 0;

	ColorMode = (unsigned short)Mode;
	ColorTableVersion = (unsigned short)(Mode>>16);
	size = 33*33*33*3;

	
	switch(dwDrvModel){

   
        case USBMODEL_P720L:
            strcpy(model,"rd.bin");
            break;
        case USBMODEL_P750L:
            strcpy(model,"rh.bin");
            break;
        case USBMODEL_P520L:
		case USBMODEL_P525N:
	{
	    if(bNewTPH)
            	strcpy(model,"ri1.bin");
	    else
		strcpy(model,"ri.bin");
	}
           break;
            
        default:
            break;
    }

    switch(MediaType){
        case MEDIA1_MODE0://DNP_TKC_PAPER:
            strcpy(media,"CMP");
            break;
        case MEDIA1_MODE1://DNP_TRC_Quality:
            strcpy(media,"CMQ");
            break;
        case MEDIA2_MODE1://MCT_ROLL_Qulaity:
            strcpy(media,"CCQ");
            break;
        case MEDIA2_MODE0://C for CHC
        default:
            strcpy(media,"CCP");
            break;
    }

    switch(ColorMode)
    {
        case COLOR_REVERSE:
            //if  (dwDrvModel == MODEL_HITI_P510)
            //{
            //    strcpy(mode,"I");
            //}
            //else
            //{
                goto END_Label;
            //}
            break;
        case COLOR_CLASSICAL:
            strcpy(mode,"CL");
            break;
        case COLOR_VIVID_MODE:	//is equal to Portrait
            strcpy(mode,"P");
            break;

        default:
            strcpy(mode,"P");
            break;
    }

    strcpy(slash,"/");
    strcpy(pFileName,slash);
    strcat(pFileName,media);
    strcat(pFileName,mode);

    if ( ColorTableVersion > 0 )
    {
        // Get Version character
        if(ColorTableVersion < 10 )
        {
            ver[0] = 48 + ColorTableVersion;
        }
        else if (ColorTableVersion == 10 )
            ver[0] = 48;
        else if (ColorTableVersion < 37 )
        {
            ver[0] = 96 + (ColorTableVersion - 10);
        }
        else
        {
            ErrorCode = ERROR_VERSION_MISMATCH;
            ver[0] = 122;
        }
        strcat(pFileName,ver);
    }

    strcat(pFileName,model);

    MYTRACE("LoadColorTable, pFileName = %s,\n", pFileName);

    if(pPath == 0)
    	strcpy(ProfileName, "./");
    else
     	strcpy(ProfileName, pPath);

    strcat(ProfileName, pFileName);

    MYTRACE("LoadColorTable, ProfileName = %s, ColorTableVersion = %d,\n", ProfileName, ColorTableVersion);
    // Load Table
    Profile = _tfopen(ProfileName, "rb");

    if(Profile == NULL)//!Profile.Open(ProfileName,CAFile::modeRead,0))
    {
        //bret = false; // Can't open file
        ErrorCode = ERROR_FILE_NOT_FOUND;
        if (ColorTableVersion > 0)
        {
            INT32 NewVer;
            bool bFileExist=false;
            NewVer = ColorTableVersion-1;
            while( NewVer > -1 && !bFileExist )
            {
                strcpy(pFileName,slash);
                strcat(pFileName,media);
                strcat(pFileName,mode);


                if ( NewVer > 0 )
                {
                    // Get Version character
                    if(NewVer < 10 )
                    {
                        ver[0] = 48 + NewVer;//sprintf(ver,"%d",NewVer);
                    }
                    else if (NewVer == 10 )
                        ver[0] = 48;
                    else if (NewVer < 37)
                    {
                        ver[0] = 96 + ( NewVer - 10);
                    }
                    else
                    {
                        NewVer = 36;
                        ver[0] = 122;
                    }
                    strcat(pFileName,ver);
                }

                strcat(pFileName,model);
                strcpy(ProfileName,pPath);
                strcat(ProfileName,pFileName);

		MYTRACE("LoadColorTable, ProfileName = %s, NewVer = %d\n", ProfileName, NewVer);
                //bFileExist = Profile.Open(ProfileName,CAFile::modeRead,0);
		Profile = _tfopen(ProfileName, "rb");

                if(Profile!=NULL)
                {
		    MYTRACE("LoadColorTable, final ProfileName = %s,\n", ProfileName);
                    ErrorCode = ERROR_VERSION_MISMATCH;
                    goto CREATE_TABLE;
                }
                NewVer -= 1;
            }
        }

        goto END_Label;
    }
    else
    {
	MYTRACE("LoadColorTable, ProfileName = %s, ColorTableVersion = %d,\n", ProfileName, ColorTableVersion);
    }

    CREATE_TABLE:

    m_pTable = (unsigned char*)calloc(sizeof(UINT8), size);

    m_bTableDynamic = true;

    if(m_pTable==(unsigned char *)NULL)
    {
        m_bTableDynamic = false;//2020.07.31 added by Bill

        ErrorCode = ERROR_OUTOFMEMORY;
        goto END_Label;
    }

    //Read Table
    //if(Profile.Read(m_pTable,size)!=(INT32)size)
    MYTRACE("LoadColorTable, befor read ProfileName = %s, \n", ProfileName);

    if(fread(m_pTable, sizeof(unsigned char), size, Profile)!=(INT32)size)
    {
	MYTRACE("LoadColorTable, read failed!! ProfileName = %s, \n", ProfileName);
        free(m_pTable);
        m_bTableDynamic = false;
        m_pTable = (unsigned char*)NULL;
        ErrorCode = ERROR_INVALID_DATA;
        goto END_Label;
    }
    //End of Get Profile Table=============================================================

    END_Label:
    //Profile.Close();
    fclose(Profile);

    if( m_pTable ==(unsigned char *)NULL)
    {

        if ( USBMODEL_P520L == dwDrvModel || USBMODEL_P525N == dwDrvModel)
        {
            m_pTable = (unsigned char *)P52x_CCPPri;
        }
        else if ( USBMODEL_P750L == dwDrvModel )
        {
            m_pTable = (unsigned char *)P75x_CCQPrh;
        }
	else if ( USBMODEL_P720L == dwDrvModel )
	{

		if ( MEDIA1_MODE0 == MediaType )

			m_pTable = (unsigned char *)P72x_CMPPrd;

		else if ( MEDIA1_MODE1 == MediaType )

			m_pTable = (unsigned char *)P72x_CMQPrd;

		else if ( MEDIA2_MODE0 == MediaType )

		{

			switch(ColorTableVersion)

			{

				case 0:		m_pTable = (unsigned char *)P72x_CCPPrd;	break;

				case 1:		m_pTable = (unsigned char *)P72x_CCPP1rd;	break;

				case 2:		m_pTable = (unsigned char *)P72x_CCPP2rd;	break;

				case 3:		m_pTable = (unsigned char *)P72x_CCPP3rd;	break;

				default:

				case 4:		m_pTable = (unsigned char *)P72x_CCPP4rd;	break;			

			}

		}

		else if ( MEDIA2_MODE1 == MediaType )

		{

			switch(ColorTableVersion)

			{

				case 0:		m_pTable = (unsigned char *)P72x_CCQPrd;	break;

				case 1:		m_pTable = (unsigned char *)P72x_CCQP1rd;	break;

				case 2:		m_pTable = (unsigned char *)P72x_CCQP2rd;	break;

				case 3:		m_pTable = (unsigned char *)P72x_CCQP3rd;	break;

				default:

				case 4:		m_pTable = (unsigned char *)P72x_CCQP4rd;	break;			

			}

		}

	}
    }


	return ErrorCode;
}

//Convert from Interleave RGB to NonInterleave YMC(K)
//ImageWidth should be double word alignment.
bool CClrCvt::Convert(unsigned char *pOrgImage,unsigned char *pDstImage,unsigned long BytePerLine,unsigned long ImageWidth,unsigned long ImageHeight,unsigned char DataMode)
{
	unsigned long  JmpP = 3;
	unsigned long  ShiftR = 2;
	unsigned long  ShiftG = 1;
	unsigned long  ShiftB = 0;
	unsigned long  i,j,i3;
	unsigned long  SBPS;
	unsigned long  PlaneSize;


	unsigned char *pSrc=NULL;
	unsigned char *pDstC=NULL;
	unsigned char *pDstM=NULL;
	unsigned char *pDstY=NULL;
	unsigned char *pDstK=NULL;
	unsigned char R=0;
	unsigned char G=0;
	unsigned char B=0;
	unsigned char OldRGB[3],DstRGB[3];
	unsigned char Curve[256];


	switch(DataMode)
	{
		case CVT_BGRA2YMC:
			JmpP = 4;
			ShiftR = 2;
			ShiftG = 1;
			ShiftB = 0;
		break;

		case CVT_ABGR2YMC:
			JmpP = 4;
			ShiftR = 3;
			ShiftG = 2;
			ShiftB = 1;
		break;

		case CVT_RGBA2YMC:
			JmpP = 4;
			ShiftR = 0;
			ShiftG = 1;
			ShiftB = 2;
		break;

		case CVT_ARGB2YMC:
			JmpP = 4;
			ShiftR = 1;
			ShiftG = 2;
			ShiftB = 3;
		break;

		case CVT_RGB2YMC:
			JmpP = 3;
			ShiftR = 0;
			ShiftG = 1;
			ShiftB = 2;
		break;

		case CVT_BGR2YMC:
		default:
		break;
	}

	//==Pointer arrange
	SBPS = (ImageWidth+3)/4*4;
	PlaneSize = SBPS*ImageHeight;

	pSrc  = pOrgImage;
	pDstY = pDstImage;
	pDstM = pDstY + PlaneSize;
	pDstC = pDstM + PlaneSize;
	pDstK = pDstC + PlaneSize;


	// Mapping Curve initial
	for (i=0; i<256; i++)
	{
		Curve[i] = (unsigned char)(255-i);
	}

	//== If no table select
	if( m_pTable == (unsigned char*)NULL )
	{
		for ( j=0; j<ImageHeight; j++ )
		{
			i3 = 0;
			for ( i=0; i<ImageWidth; i++,i3+=JmpP)
			{
				pDstY[i] = Curve[ pSrc[ i3+ShiftB ] ];
				pDstM[i] = Curve[ pSrc[ i3+ShiftG ] ];
				pDstC[i] = Curve[ pSrc[ i3+ShiftR ] ];
			}
			pSrc += BytePerLine;
			pDstY += SBPS;
			pDstM += SBPS;
			pDstC += SBPS;
		}
		return true;
	}

	OldRGB[0] = OldRGB[1] = OldRGB[2] = 255;

	Interp33To256C3(DstRGB,OldRGB,m_pTable);
	for ( j=0; j<ImageHeight; j++ )
	{
		i3 = 0;
		for ( i=0; i<ImageWidth; i++,i3+=JmpP)
		{
			R = pSrc[i3+ShiftR];
			G = pSrc[i3+ShiftG];
			B = pSrc[i3+ShiftB];
			if ( R==OldRGB[0] && G==OldRGB[1] && B==OldRGB[2] ) {
			}
			else {
				OldRGB[0] = R;
				OldRGB[1] = G;
				OldRGB[2] = B;
				Interp33To256C3(DstRGB,OldRGB,m_pTable);
			}
			pDstY[i] = Curve[DstRGB[2]];
			pDstM[i] = Curve[DstRGB[1]];
			pDstC[i] = Curve[DstRGB[0]];
		}
		pSrc += BytePerLine;
		pDstY += SBPS;
		pDstM += SBPS;
		pDstC += SBPS;
	}

	return true;
}
void CClrCvt::InitInterpolatePara()
{
	unsigned long i;
	unsigned short j;
	for(i=0;i<33;i++)
	{
		m_pTB1089[i] = i*1089;
		m_pTB33[i]	= i*33;
	}
	unsigned short *pPre,*pCur;//,fi;
	memset(m_pTB_256_8,0,2304*sizeof(unsigned short)); // 4608=256*9*2:2 for WORD
	pPre = m_pTB_256_8;
	pCur = m_pTB_256_8 + 256;
	for(i=1;i<9;i++)
	{
		for(j=0;j<256;j++)
		{
			pCur[j] = pPre[j]+j;
		}
		pPre += 256;
		pCur += 256;
	}
	return;
}

// Table is 33*33*33 Interpolate RGB
inline void CClrCvt::Interp33To256C3(unsigned char *DstRGB, unsigned char *SrcRGB, unsigned char* pTable)
{
	DEFRGBK P1_Pos,P2_Pos,P3_Pos,P4_Pos; // Using rgbquad as rgb Postion
	DEFRGBK	P1_Val,P2_Val,P3_Val,P4_Val; // Using rgbquad as rgb Postion

	unsigned char	R_Weight,G_Weight,B_Weight;
	unsigned short	W1,W2,W3,W4;
	unsigned long	Pos;

	//==Get Grid Position
	P1_Pos.rgbRed   = SrcRGB[0]>>3;// divide 8
	P1_Pos.rgbGreen = SrcRGB[1]>>3;
	P1_Pos.rgbBlue  = SrcRGB[2]>>3;

	P4_Pos.rgbRed	= P1_Pos.rgbRed + 1;
	P4_Pos.rgbGreen = P1_Pos.rgbGreen + 1;
	P4_Pos.rgbBlue	= P1_Pos.rgbBlue + 1;

	//==Get Wight
	R_Weight = SrcRGB[0] & 0x07;// 0 ~ 7
	G_Weight = SrcRGB[1] & 0x07;//
	B_Weight = SrcRGB[2] & 0x07;//
	if(SrcRGB[0]==255) R_Weight = 8;
	if(SrcRGB[1]==255) G_Weight = 8;
	if(SrcRGB[2]==255) B_Weight = 8;
	//
	if(R_Weight >= G_Weight){
		if(G_Weight >= B_Weight){		// R > G > B
			W1 = 8 - R_Weight;
			W2 = R_Weight - G_Weight;
			W3 = G_Weight - B_Weight;
			W4 = B_Weight;
			P2_Pos.rgbRed	= P1_Pos.rgbRed + 1;
			P2_Pos.rgbGreen = P1_Pos.rgbGreen;
			P2_Pos.rgbBlue	= P1_Pos.rgbBlue;

			P3_Pos.rgbRed	= P1_Pos.rgbRed + 1;
			P3_Pos.rgbGreen = P1_Pos.rgbGreen + 1;
			P3_Pos.rgbBlue	= P1_Pos.rgbBlue;
		}
		else{
			if(R_Weight >=B_Weight){	// R > B > G
				W1 = 8 - R_Weight;
				W2 = R_Weight - B_Weight;
				W3 = B_Weight - G_Weight;
				W4 = G_Weight;
				P2_Pos.rgbRed	= P1_Pos.rgbRed + 1;
				P2_Pos.rgbGreen = P1_Pos.rgbGreen;
				P2_Pos.rgbBlue	= P1_Pos.rgbBlue;

				P3_Pos.rgbRed	= P1_Pos.rgbRed + 1;
				P3_Pos.rgbGreen = P1_Pos.rgbGreen;
				P3_Pos.rgbBlue	= P1_Pos.rgbBlue + 1;
			}
			else{						// B > R > G
				W1 = 8 - B_Weight;
				W2 = B_Weight - R_Weight;
				W3 = R_Weight - G_Weight;
				W4 = G_Weight;
				P2_Pos.rgbRed	= P1_Pos.rgbRed;
				P2_Pos.rgbGreen = P1_Pos.rgbGreen;
				P2_Pos.rgbBlue	= P1_Pos.rgbBlue + 1;

				P3_Pos.rgbRed	= P1_Pos.rgbRed + 1;
				P3_Pos.rgbGreen = P1_Pos.rgbGreen;
				P3_Pos.rgbBlue	= P1_Pos.rgbBlue + 1;
			}
		}
	}
	else{
		if(R_Weight >= B_Weight){		// G > R > B
			W1 = 8 - G_Weight;
			W2 = G_Weight - R_Weight;
			W3 = R_Weight - B_Weight;
			W4 = B_Weight;
			P2_Pos.rgbRed	= P1_Pos.rgbRed;
			P2_Pos.rgbGreen = P1_Pos.rgbGreen + 1;
			P2_Pos.rgbBlue	= P1_Pos.rgbBlue;

			P3_Pos.rgbRed	= P1_Pos.rgbRed + 1;
			P3_Pos.rgbGreen = P1_Pos.rgbGreen + 1;
			P3_Pos.rgbBlue	= P1_Pos.rgbBlue;
		}
		else{
			if(G_Weight >=B_Weight){	// G > B > R
				W1 = 8 - G_Weight;
				W2 = G_Weight - B_Weight;
				W3 = B_Weight - R_Weight;
				W4 = R_Weight;
				P2_Pos.rgbRed	= P1_Pos.rgbRed;
				P2_Pos.rgbGreen = P1_Pos.rgbGreen + 1;
				P2_Pos.rgbBlue	= P1_Pos.rgbBlue;

				P3_Pos.rgbRed	= P1_Pos.rgbRed;
				P3_Pos.rgbGreen = P1_Pos.rgbGreen + 1;
				P3_Pos.rgbBlue	= P1_Pos.rgbBlue + 1;
			}
			else{						// B > G > R
				W1 = 8 - B_Weight;
				W2 = B_Weight - G_Weight;
				W3 = G_Weight - R_Weight;
				W4 = R_Weight;
				P2_Pos.rgbRed	= P1_Pos.rgbRed;
				P2_Pos.rgbGreen = P1_Pos.rgbGreen;
				P2_Pos.rgbBlue	= P1_Pos.rgbBlue + 1;

				P3_Pos.rgbRed	= P1_Pos.rgbRed;
				P3_Pos.rgbGreen = P1_Pos.rgbGreen + 1;
				P3_Pos.rgbBlue	= P1_Pos.rgbBlue + 1;
			}
		}
	}

	Pos = (m_pTB1089[P1_Pos.rgbBlue] + m_pTB33[P1_Pos.rgbGreen] + P1_Pos.rgbRed)*3; //__TBsizeS=17, TBsizeS2=17*17;
	P1_Val.rgbRed   = pTable[Pos  ];
	P1_Val.rgbGreen = pTable[Pos+1];
	P1_Val.rgbBlue  = pTable[Pos+2];
	Pos = (m_pTB1089[P2_Pos.rgbBlue] + m_pTB33[P2_Pos.rgbGreen] + P2_Pos.rgbRed)*3; //__TBsizeS=17, TBsizeS2=17*17;
	P2_Val.rgbRed   = pTable[Pos  ];
	P2_Val.rgbGreen = pTable[Pos+1];
	P2_Val.rgbBlue  = pTable[Pos+2];
	Pos = (m_pTB1089[P3_Pos.rgbBlue] + m_pTB33[P3_Pos.rgbGreen] + P3_Pos.rgbRed)*3; //__TBsizeS=17, TBsizeS2=17*17;
	P3_Val.rgbRed   = pTable[Pos  ];
	P3_Val.rgbGreen = pTable[Pos+1];
	P3_Val.rgbBlue  = pTable[Pos+2];
	Pos = (m_pTB1089[P4_Pos.rgbBlue] + m_pTB33[P4_Pos.rgbGreen] + P4_Pos.rgbRed)*3; //__TBsizeS=17, TBsizeS2=17*17;
	P4_Val.rgbRed   = pTable[Pos  ];
	P4_Val.rgbGreen = pTable[Pos+1];
	P4_Val.rgbBlue  = pTable[Pos+2];

	unsigned short *pW1,*pW2,*pW3,*pW4;
	pW1 = m_pTB_256_8 + (W1<<8);
	pW2 = m_pTB_256_8 + (W2<<8);
	pW3 = m_pTB_256_8 + (W3<<8);
	pW4 = m_pTB_256_8 + (W4<<8);

	DstRGB[0]=(unsigned char)( (pW1[P1_Val.rgbRed]  + pW2[P2_Val.rgbRed]   + pW3[P3_Val.rgbRed]   + pW4[P4_Val.rgbRed]  ) >>3);
	DstRGB[1]=(unsigned char)( (pW1[P1_Val.rgbGreen]+ pW2[P2_Val.rgbGreen] + pW3[P3_Val.rgbGreen] + pW4[P4_Val.rgbGreen]) >>3);
	DstRGB[2]=(unsigned char)( (pW1[P1_Val.rgbBlue] + pW2[P2_Val.rgbBlue]  + pW3[P3_Val.rgbBlue]  + pW4[P4_Val.rgbBlue] ) >>3);

	return;
}

//=============================================================================
#ifdef WIN32
unsigned long ConvertBGR2YMC(unsigned char* lpImageOrg, unsigned char* lpYMC, unsigned long dwImageWidth, unsigned long dwBytePerLine, unsigned long dwImageHeight, unsigned char byPrintMode, unsigned short wRbnVen, unsigned long dwDrvModel, unsigned char byDataModeIn, bool bNewTPH)
#else
__attribute__((__visibility__("hidden"))) unsigned long ConvertBGR2YMC(unsigned char* lpImageOrg, unsigned char* lpYMC, unsigned long dwImageWidth, unsigned long dwBytePerLine, unsigned long dwImageHeight, unsigned char byPrintMode, unsigned short wRbnVen, unsigned long dwDrvModel, unsigned char byDataModeIn, bool bNewTPH)
#endif
{
	unsigned long		dwError = 0;
	unsigned long		dwColorMode = 0;
    unsigned short		wCMode = 0;
    unsigned short		wVersion = 0;

	unsigned char		byMediaType = 0;

    bool				bRet = false;

	CClrCvt				cColorTB;
	char	szCurDir[MAX_PATH] = {0};

       
	if(dwDrvModel == USBMODEL_P520L || dwDrvModel == USBMODEL_P525N)
	{
		strcpy(szCurDir, "/usr/share/HiTi/tables/P52x");
		byMediaType = MEDIA2_MODE0;
	}
	else if(dwDrvModel == USBMODEL_P720L)
	{
		strcpy(szCurDir, "/usr/share/HiTi/tables/P720");
		byMediaType = MEDIA2_MODE1;
	}
    	else if(dwDrvModel == USBMODEL_P750L)
	{
		strcpy(szCurDir, "/usr/share/HiTi/tables/P750");
		byMediaType = MEDIA2_MODE1;
	}


	wCMode = COLOR_VIVID_MODE;
    wVersion = wRbnVen & 0x003F;
    dwColorMode = (((unsigned long)wVersion)<<16) + wCMode;

	dwError = cColorTB.LoadColorTable(szCurDir, byMediaType, dwColorMode, dwDrvModel, bNewTPH);

    bRet = cColorTB.Convert(lpImageOrg, lpYMC, dwBytePerLine, dwImageWidth, dwImageHeight, byDataModeIn);

	return dwError;
}

