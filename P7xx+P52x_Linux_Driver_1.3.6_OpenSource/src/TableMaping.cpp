/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright 2014-2015 HiTi Digital, Inc.  All Rights Reserved
**********************************************************/

//#ifdef WIN32
//#include "stdafx.h"
//#endif

#include <stdio.h>
#include <stdlib.h>
#include "TableMaping.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTableMaping::CTableMaping()
{
//	pMapTable = new CMapingTable;
	MinB=MinC=MinG=MinCBR=MinCBG=MinCBB=-100;//-128;
	MaxB=MaxC=MaxG=MaxCBR=MaxCBG=MaxCBB=100;//127;
	RMinB=RMinC=RMinG=RMinCBR=RMinCBG=RMinCBB=RMinS=0;
	RMaxB=RMaxC=RMaxG=RMaxCBR=RMaxCBG=RMaxCBB=RMaxS=255;

	MinS=-100;
	MaxS=100;
}

CTableMaping::~CTableMaping()
{
//	delete pMapTable;

}

bool CTableMaping::MapTableImage(BITMAP &SrcBMP,BITMAP &DstBMP)
{
	if ( SrcBMP.bmPlanes!=1 || SrcBMP.bmBitsPixel!=24 || DstBMP.bmPlanes!=1 || DstBMP.bmBitsPixel!=24 )
		return false;
	if ( SrcBMP.bmHeight!=DstBMP.bmHeight || SrcBMP.bmWidth!=DstBMP.bmWidth )
		return false;
	cMapTable.MapTableToImage((unsigned char*) SrcBMP.bmBits,(unsigned char*)DstBMP.bmBits,SrcBMP.bmHeight,SrcBMP.bmWidth,SrcBMP.bmWidthBytes);
	return true;
}

void CTableMaping::SetBrightTable(int Bright)
{
	unsigned char RealV;
	RealV = GetRealValue(Bright,MinB,MaxB,RMinB,RMaxB);
	cMapTable.ModifyBrightTable(RealV);
	return;
}

void CTableMaping::SetContrastTable(int Contrast)
{
	unsigned char RealV;
	RealV = GetRealValue(Contrast,MinC,MaxC,RMinC,RMaxC);
	cMapTable.ModifyContrastTable(RealV);
	return;
}

void CTableMaping::SetGammaTable(int Gamma)
{
	unsigned char RealV;
	RealV = GetRealValue(Gamma,MinG,MaxG,RMinG,RMaxG);
	cMapTable.ModifyGammaTable(RealV);
	return;
}

//from 100 to 999 and 100 means 0.1, 999 means 100
void CTableMaping::SetGammaTableByfloat(int gamma)
{
	double fGamma;
	fGamma = ((double)gamma)/100.0;
	cMapTable.ModifyGammaTable(fGamma);
	return;
}

void CTableMaping::SetColorBalanceTable(int CB,unsigned char Channel)
{
	unsigned char RealV;
	switch (Channel) {
	case 0:
		RealV = GetRealValue(CB,MinCBR,MaxCBR,RMinCBR,RMaxCBR);
		break;
	case 1:
		RealV = GetRealValue(CB,MinCBG,MaxCBG,RMinCBG,RMaxCBG);
		break;
	case 2:
		RealV = GetRealValue(CB,MinCBB,MaxCBB,RMinCBB,RMaxCBB);
		break;
	default:
		return;
		break;
	}
	cMapTable.ModifyColorBalanceTable(RealV,Channel);
	return;
}

void CTableMaping::ResetAllTable()
{
	cMapTable.ResetAll();
	return;
}


unsigned char CTableMaping::GetRealBright(int Bright)
{
	unsigned char RealV;
	RealV = GetRealValue(Bright,MinB,MaxB,RMinB,RMaxB);
	return RealV;
}

unsigned char CTableMaping::GetRealContrast(int Contrast)
{
	unsigned char RealV;
	RealV = GetRealValue(Contrast,MinC,MaxC,RMinC,RMaxC);
	return RealV;
}

unsigned char CTableMaping::GetRealGamma(int Gamma)
{
	unsigned char RealV;
	RealV = GetRealValue(Gamma,MinG,MaxG,RMinG,RMaxG);
	return RealV;
}

unsigned char CTableMaping::GetRealColorBalance(int CB,unsigned char Channel)
{
	unsigned char RealV;
	switch (Channel) {
	case 0:
		RealV = GetRealValue(CB,MinCBR,MaxCBR,RMinCBR,RMaxCBR);
		break;
	case 1:
		RealV = GetRealValue(CB,MinCBG,MaxCBG,RMinCBG,RMaxCBG);
		break;
	case 2:
		RealV = GetRealValue(CB,MinCBB,MaxCBB,RMinCBB,RMaxCBB);
		break;
	default:
		return 128;
		break;
	}
	return RealV;
}


bool CTableMaping::SetBrightSlider(int Min,int Max,unsigned char OffsetN,unsigned char OffsetP)
{
	if ( Min>=Max ) return false;
	if ( Min>0 )	MinB = 0;
	else			MinB = Min;
	if ( Max<0 )	MaxB = 0;
	else			MaxB = Max;
	if ( OffsetN>128 )	RMinB = 0;
	else				RMinB = 128-OffsetN;
	if ( OffsetP>127 )	RMaxB = 255;
	else				RMaxB = 128+OffsetP;
	return true;
}

bool CTableMaping::SetContrastSlider(int Min,int Max,unsigned char OffsetN,unsigned char OffsetP)
{
	if ( Min>=Max ) return false;
	if ( Min>0 )	MinC = 0;
	else			MinC = Min;
	if ( Max<0 )	MaxC = 0;
	else			MaxC = Max;
	if ( OffsetN>128 )	RMinC = 0;
	else				RMinC = 128-OffsetN;
	if ( OffsetP>127 )	RMaxC = 255;
	else				RMaxC = 128+OffsetP;
	return true;
}

bool CTableMaping::SetGammaSlider(int Min,int Max,unsigned char OffsetN,unsigned char OffsetP)
{
	if ( Min>=Max ) return false;
	if ( Min>0 )	MinG = 0;
	else			MinG = Min;
	if ( Max<0 )	MaxG = 0;
	else			MaxG = Max;
	if ( OffsetN>128 )	RMinG = 0;
	else				RMinG = 128-OffsetN;
	if ( OffsetP>127 )	RMaxG = 255;
	else				RMaxG = 128+OffsetP;
	return true;
}

bool CTableMaping::SetColorBalanceSlider(int Min,int Max,unsigned char Channel,unsigned char OffsetN,unsigned char OffsetP)
{	
	if ( Min>=Max ) return false;
	if ( Min>0 )	Min = 0;
	if ( Max<0 )	Max = 0;
	switch ( Channel ) {
	case 0:
		MinCBR = Min;
		MaxCBR = Max;
		if ( OffsetN>128 )	RMinCBR = 0;
		else				RMinCBR = 128-OffsetN;
		if ( OffsetP>127 )	RMaxCBR = 255;
		else				RMaxCBR = 128+OffsetP;
		break;
	case 1:
		MinCBG = Min;
		MaxCBG = Max;
		if ( OffsetN>128 )	RMinCBG = 0;
		else				RMinCBG = 128-OffsetN;
		if ( OffsetP>127 )	RMaxCBG = 255;
		else				RMaxCBG = 128+OffsetP;
		break;
	case 2:
		MinCBB = Min;
		MaxCBB = Max;
		if ( OffsetN>128 )	RMinCBB = 0;
		else				RMinCBB = 128-OffsetN;
		if ( OffsetP>127 )	RMaxCBB = 255;
		else				RMaxCBB = 128+OffsetP;
		break;
	default:
		return false;
		break;
	}
	return true;
}

int CTableMaping::GetSliderContrast(unsigned char RealV)
{
	int SliderV;
	SliderV = GetSliderValue(RealV,MinC,MaxC,RMinC,RMaxC);
	return SliderV;
}

int CTableMaping::GetSliderBright(unsigned char RealV)
{
	int SliderV;
	SliderV = GetSliderValue(RealV,MinB,MaxB,RMinB,RMaxB);
	return SliderV;
}

int CTableMaping::GetSliderGamma(unsigned char RealV)
{
	int SliderV;
	SliderV = GetSliderValue(RealV,MinG,MaxG,RMinG,RMaxG);
	return SliderV;
}

int CTableMaping::GetSliderColorB(unsigned char RealV,unsigned char Channel)
{
	int SliderV;
	switch (Channel) {
	case 0:
		SliderV = GetSliderValue(RealV,MinCBR,MaxCBR,RMinCBR,RMaxCBR);
		break;
	case 1:
		SliderV = GetSliderValue(RealV,MinCBG,MaxCBG,RMinCBG,RMaxCBG);
		break;
	case 2:
		SliderV = GetSliderValue(RealV,MinCBB,MaxCBB,RMinCBB,RMaxCBB);
		break;
	default:
		return 0;
		break;
	}
	return SliderV;
}

unsigned char CTableMaping::GetRealValue(int SliderV,int Min,int Max,unsigned char MinR,unsigned char MaxR)
{
	unsigned char RealV;
	if ( SliderV<Min )		SliderV = Min;
	else if ( SliderV>Max ) SliderV = Max;
	if ( SliderV<0 ) {
		RealV = (unsigned char) (128.0-((double)SliderV/Min*(128-MinR)+0.5));
	}
	else {
		RealV = (unsigned char) (128.0+((double)SliderV/Max*(MaxR-128)+0.5));
	}
	return RealV;
}

int CTableMaping::GetSliderValue(unsigned char RealV,int Min,int Max,unsigned char MinR,unsigned char MaxR)
{
	int SliderV;
	if ( RealV<MinR )		RealV = MinR;
	else if ( RealV>MaxR )	RealV = MaxR;
	if ( RealV<128 ) {
		SliderV = (int) ((double)(128-RealV)/(128-MinR)*Min-0.5);
	}
	else {
		SliderV = (int) ((double)(RealV-128)/(MaxR-128)*Max+0.5);
	}
	return SliderV;
}

//===============================================================
bool CTableMaping::SetSaturateTable(int Sat)
{
	unsigned char RealV= 128;
	long nret;
	RealV = GetRealValue(Sat,MinS,MaxS,RMinS,RMaxS);
	nret = m_CSat.MakeSaturationTable(RealV);
	if(nret==0)
		return false;
	return true;
}

bool CTableMaping::SetSaturateSlider(int Min,int Max,unsigned char OffsetN,unsigned char OffsetP)
{
	if ( Min>=Max ) return false;
	if ( Min>0 )	MinS = 0;
	else			MinS = Min;
	if ( Max<0 )	MaxS = 0;
	else			MaxS = Max;
	if ( OffsetN>128 )	RMinS = 0;
	else				RMinS = 128-OffsetN;
	if ( OffsetP>127 )	RMaxS = 255;
	else				RMaxS = 128+OffsetP;
	return true;
}

long CTableMaping::ApplySaturate( BITMAP &SrcBMP,BITMAP &DstBMP)
{
    //==
	if ( SrcBMP.bmPlanes!=1 || SrcBMP.bmBitsPixel!=24 || DstBMP.bmPlanes!=1 || DstBMP.bmBitsPixel!=24 )
		return false;
	if ( SrcBMP.bmHeight!=DstBMP.bmHeight || SrcBMP.bmWidth!=DstBMP.bmWidth )
		return false;

 //   long nRet = HT_RETURN_SUCCESS;
    unsigned long NOS = 0;
    unsigned long BPS = 0;
    unsigned long PPS = 0;
    unsigned char *pSrcImg = NULL;
    unsigned char *pDstImg = NULL;
	long nRet;

    //==
    BPS = DstBMP.bmWidthBytes;
    PPS = DstBMP.bmWidth;
    NOS = DstBMP.bmHeight;
    pSrcImg = (unsigned char *) SrcBMP.bmBits;
    pDstImg = (unsigned char *) DstBMP.bmBits;


    //==
    nRet = m_CSat.DoImageSaturation( pSrcImg, pDstImg, NOS, PPS, BPS, 3, 2, 1, 0);

    return nRet;
}
//================================================================