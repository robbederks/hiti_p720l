/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright 2014-2015 HiTi Digital, Inc.  All Rights Reserved
**********************************************************/

//#ifdef WIN32
//#include "stdafx.h"
//#endif

#include <stdio.h>
#include <stdlib.h>
#include "MapTable.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapingTable::CMapingTable()
{
	m_wTableBit = 0;
	m_wTableLen_1 = 0;				
	m_dwTableLen = 0;
	m_pMemory = (unsigned short*)NULL;
	m_pColorBalanceTable[0] = NULL;
	m_pColorBalanceTable[1] = NULL;
	m_pColorBalanceTable[2] = NULL;
	m_pBrightnessTable = NULL;		
	m_pContrastTable = NULL;			
	m_pGammaTable = NULL;
	m_pCombinedTable[0] = NULL;
	m_pCombinedTable[1] = NULL;
	m_pCombinedTable[2] = NULL;
	CreateTableData();
}

CMapingTable::~CMapingTable()
{
	Release();
	return;
}

void CMapingTable::Release()
{
	if( m_pMemory )
		delete [] m_pMemory;

	//::VirtualFree((LPVOID)m_pMemory, 0, MEM_RELEASE);
	return;
}

void CMapingTable::ResetAll()
{
//	MakeBrightnessTable(m_pBrightnessTable,128);
//	MakeContrastTable(m_pContrastTable,128);
//	MakeGammaTable(m_pGammaTable,(int)128);
//	MakeGammaTable(m_pColorBalanceTable[0],(int)128);
//	MakeGammaTable(m_pColorBalanceTable[1],(int)128);
//	MakeGammaTable(m_pColorBalanceTable[2],(int)128);
//	CombineTable();

	unsigned short i;
	for (i=0;i<m_dwTableLen;i++) {
		m_pColorBalanceTable[0][i] = i;
		m_pColorBalanceTable[1][i] = i;
		m_pColorBalanceTable[2][i] = i;
		m_pBrightnessTable[i] = i;
		m_pContrastTable[i] = i;
		m_pGammaTable[i] = i;
		m_pCombinedTable[0][i] = i;
		m_pCombinedTable[1][i] = i;
		m_pCombinedTable[2][i] = i;
	}


	return;
}

unsigned char CMapingTable::CreateTableData(unsigned char Bright,
								unsigned char Contrast,
								unsigned char Gamma,
								unsigned char CB_RC,
								unsigned char CB_GM,
								unsigned char CB_BY,
								unsigned char TableBit)
{
	unsigned char nError = 0;
	nError = CreatTable(TableBit);
	if ( nError!= 0 )	return nError;
	MakeBrightnessTable(m_pBrightnessTable,Bright);
	MakeContrastTable(m_pContrastTable,Contrast);
	MakeGammaTable(m_pGammaTable,Gamma);
	MakeGammaTable(m_pColorBalanceTable[0],CB_RC);
	MakeGammaTable(m_pColorBalanceTable[1],CB_GM);
	MakeGammaTable(m_pColorBalanceTable[2],CB_BY);
	CombineTable();
	return nError;
}

void CMapingTable::ModifyBrightTable(unsigned char Bright)
{
	MakeBrightnessTable(m_pBrightnessTable,Bright);
	CombineTable();
	return;
}

void CMapingTable::ModifyContrastTable(unsigned char Contrast)
{
	MakeContrastTable(m_pContrastTable,Contrast);
	CombineTable();
	return;
}

void CMapingTable::ModifyGammaTable(unsigned char Gamma)
{
	MakeGammaTable(m_pGammaTable,Gamma);
	CombineTable();
	return;
}

void CMapingTable::ModifyGammaTable(double fGamma)
{
	MakeGammaTable(m_pGammaTable,fGamma);
	CombineTable();
}


void CMapingTable::ModifyColorBalanceTable(unsigned char CB,unsigned char Channel)
{
	MakeBrightnessTable(m_pColorBalanceTable[Channel],CB);
	CombineTable();
	return;
}

void CMapingTable::MapTableToImage(unsigned char *pSrcImg,
								unsigned char *pDstImg,
								unsigned long Height,
								unsigned long Width,
								unsigned long BytePerLine,
								unsigned char BitsPerColor)
{
	unsigned long i,j;
	unsigned char *pOrgImg,*pResImg;
	
	if ( BitsPerColor!=8 )	return;

	pOrgImg = pSrcImg;
	pResImg = pDstImg;
	for (j=0;j<Height;j++) {
		for (i=0;i<Width;i++) {
			pResImg[3*i] = (unsigned char) m_pCombinedTable[2][pOrgImg[3*i]];
			pResImg[3*i+1] = (unsigned char) m_pCombinedTable[1][pOrgImg[3*i+1]];
			pResImg[3*i+2] = (unsigned char) m_pCombinedTable[0][pOrgImg[3*i+2]];
		}
		pOrgImg += BytePerLine;
		pResImg += BytePerLine;
	}
	return;
}

unsigned char CMapingTable::CreatTable(unsigned short TableBit)
{
	unsigned char nError = 0;
	unsigned char nTotalChannelNum = 0;
	unsigned short *pMemory = NULL;

	m_wTableBit = TableBit;
	if (m_wTableBit>16) m_wTableBit = 16;
	if (m_wTableBit<8 ) m_wTableBit = 8;
	m_dwTableLen = (unsigned long)pow(2, m_wTableBit);
	m_wTableLen_1 = (unsigned short)(m_dwTableLen -1);

	nTotalChannelNum += 3;	// ColorBalance
	nTotalChannelNum += 1;	// Brightness
	nTotalChannelNum += 1;	// Contrast
	nTotalChannelNum += 1;	// Gamma
	nTotalChannelNum += 3;	// Combined

	//m_pMemory = (unsigned short *)VirtualAlloc(NULL, nTotalChannelNum*m_dwTableLen*sizeof(unsigned short),
	//								 MEM_COMMIT, PAGE_READWRITE);
	m_pMemory = new unsigned short[nTotalChannelNum*m_dwTableLen];
	if ( m_pMemory==NULL )	
		return MEM_ALLOC_ERROR;
	pMemory = m_pMemory;
	m_pColorBalanceTable[0] = pMemory;	pMemory += m_dwTableLen;
	m_pColorBalanceTable[1] = pMemory;	pMemory += m_dwTableLen;
	m_pColorBalanceTable[2] = pMemory;	pMemory += m_dwTableLen;
	m_pBrightnessTable = pMemory;	pMemory += m_dwTableLen;
	m_pContrastTable = pMemory;	pMemory += m_dwTableLen;
	m_pGammaTable = pMemory;	pMemory += m_dwTableLen;
	m_pCombinedTable[0] = pMemory;	pMemory += m_dwTableLen;
	m_pCombinedTable[1] = pMemory;	pMemory += m_dwTableLen;
	m_pCombinedTable[2] = pMemory;

	ResetAll();//2012.12.27

	return nError;
}
void CMapingTable::CombineTable()
{
	unsigned short i,M;
	for (i=0; i<=m_wTableLen_1; i++) {
		M = i;
		//___Gamma
		M = m_pGammaTable[M];
		//___Brightness
		M = m_pBrightnessTable[M];
		//___Contrast
		M = m_pContrastTable[M];
		//___TPO Combined
		m_pCombinedTable[0][i] = m_pColorBalanceTable[0][M];
		m_pCombinedTable[1][i] = m_pColorBalanceTable[1][M];
		m_pCombinedTable[2][i] = m_pColorBalanceTable[2][M];
	}
	return;
}

void CMapingTable::MakeGammaTable(unsigned short *pTable,unsigned char Gamma)
{
	float fGamma;
	unsigned short i;

	if (Gamma<128) {
		fGamma = 1.0f+ (float)(128-Gamma)/64;
	}
	else if (Gamma>128) {
		fGamma = 0.2f+ (float)(255-Gamma)/160;
	}
	else
		fGamma = 1.0f;
	//
	if (fGamma == 1.0f) {
		for (i=0; i<=m_wTableLen_1; i++) {
			pTable[i] = i;
		}
	} else {
		for (i=0; i<=m_wTableLen_1; i++) {
			pTable[i] = (unsigned short)(m_wTableLen_1 * pow((float)i/m_wTableLen_1, fGamma));
		}
	}
	//
	return;
}


//By Real Gamma value
void CMapingTable::MakeGammaTable(unsigned short *pTable, double fGamma)
{
	double fValue,temp,Y32;
	unsigned short i,Bound;
	Bound = 32; // Level

	double fInvGamma;
	fInvGamma = 1.0/ fGamma;

	if (fInvGamma < 1.415f && fInvGamma > 0.7067f) {  // 0.7067 = 1/1.415
		for (i=0; i<=m_wTableLen_1; i++) {
			pTable[i] = (unsigned short)(m_wTableLen_1 * pow((double)i/m_wTableLen_1, fInvGamma));
		}
	}
	else{
		for (i=0; i<=m_wTableLen_1; i++) {
			temp = (double)i/m_wTableLen_1;
			Y32  = (double)m_wTableLen_1*pow((double)Bound/m_wTableLen_1,fInvGamma);
			if (i < Bound){
				fValue = 1.4f*temp - 2.3375f*pow(temp,2) - 6.7734f*pow(temp,3);
				pTable[i] = (unsigned short)((fValue*m_wTableLen_1/Bound)*Y32);
			}
			else
				pTable[i] = (unsigned short)(m_wTableLen_1*pow(temp,fInvGamma));
		}
	}
	//
	return;
}

void CMapingTable::MakeBrightnessTable(unsigned short *pTable,unsigned char Level)
{
	unsigned short Bright,i,Bond;
	if (Level>128) {
		Bright = (Level-128)*m_wTableBit/8/2;
		Bond = m_wTableLen_1-Bright;
		for (i=0; i<Bond; i++) {
			pTable[i] = i+Bright;
		}
		for (i=Bond; i<=m_wTableLen_1; i++) {
			pTable[i] = m_wTableLen_1;
		}
	}
	else if (Level<128)	{
		Bright = (128-Level)*m_wTableBit/8/2;
		Bond = Bright;
		for (i=0; i<Bond; i++) {
			pTable[i] = 0;
		}
		for (i=Bond; i<=m_wTableLen_1; i++) {
			pTable[i] = i-Bright;
		}
	}
	else {
		for (i=0; i<=m_wTableLen_1; i++) {
			pTable[i] = i;
		}
	}
	return;
}

void CMapingTable::MakeContrastTable(unsigned short *pTable,unsigned char Level)
{
	float Data;
	unsigned short Band,i,Band2;
	if (Level>128) {
		Band = (Level-128)*m_wTableBit/8/2;
		Band2 = m_wTableLen_1-Band;
		for (i=0;i<Band;i++)	
			pTable[i] = 0;
		for (i=Band;i<Band2;i++) {
			Data = (float) m_wTableLen_1/(m_wTableLen_1-Band-Band)*(i-Band);
			pTable[i] = (unsigned short) (Data+0.5);
		}
		for (i=Band2;i<=m_wTableLen_1;i++)	
			pTable[i] = m_wTableLen_1;
	}
	else if (Level<128) {
		Band = (128-Level)*m_wTableBit/8/2;
		for (i=0;i<=m_wTableLen_1;i++) {
			Data = (float) (m_wTableLen_1-Band-Band)/m_wTableLen_1*i + Band;
			pTable[i] = (unsigned short) (Data+0.5);
		}
	}
	else {
		for (i=0; i<=m_wTableLen_1; i++) {
			pTable[i] = i;
		}
	}
}
