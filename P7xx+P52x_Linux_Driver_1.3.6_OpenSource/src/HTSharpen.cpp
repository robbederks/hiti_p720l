/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright 2014-2015 HiTi Digital, Inc.  All Rights Reserved
**********************************************************/

//#ifdef WIN32
//#include "stdafx.h"
//#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HTSharpen.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHTSharpen::CHTSharpen()
{
	m_dbRate = 0.0;
	m_dbThreshold = 255;
	m_pSharp3Lines = (unsigned short *)NULL;
	m_nImgWidth = 0;
	m_nChnJmp = 1;
	m_nPixelJmp = 3;
	m_nAvgRate	 = 12;

}

CHTSharpen::~CHTSharpen()
{
	MemoryFree();
}

void CHTSharpen::MemoryFree()
{
	if ( m_pSharp3Lines != (unsigned short *)NULL )
	{
		delete [] m_pSharp3Lines;
		m_pSharp3Lines = (unsigned short *)NULL;
	}
	return;
}

void CHTSharpen::SetBitsPerPixel(unsigned long nBitsPerPixel)
{
	m_nPixelJmp = nBitsPerPixel / 8;
}

void CHTSharpen::SetLevel(unsigned char nSharp)
{
	double Max,Min,RateS;
	RateS = (double) nSharp/100;//255;
	Max = 5.0;
	Min = 2.0;

	m_nAvgRate	 = 12;
	m_dbRate = (Max-Min)*RateS/3+Min;	// m_dbRate: 2.0 - 3.0
	Max = 16.0;
	Min = 8.0;
	m_dbThreshold = (Max-Min)*(1.0-RateS)*3+Min; // m_dbThreshold: 32.0 - 8.0
	Max = 96.0;
	Min = 32.0;
	m_dbMaxBound = (Max-Min)*RateS/3+Min;		//  m_dbMaxBound: 32.0 - 53.333
	return;
}

unsigned long CHTSharpen::DoSharpenImg(	unsigned char *pOrgImage,
								unsigned char *pDstImage,
								unsigned long nBytePerLine,
								unsigned long nImageWidth,
								unsigned long nImageHeight	)
{

	unsigned short *pL1 = NULL;
	unsigned short *pL2 = NULL;
	unsigned short *pL3 = NULL;
	unsigned long NOS_1 = 0;
	unsigned long NOS_2 = 0;
	unsigned long PPSp2WORD = 0;
	unsigned long i = 0;
	unsigned char *pSrc = NULL;
	unsigned char *pDst = NULL;


	if ( nImageHeight<3 )
	{
		return 13;//ERROR_INVALID_DATA;
	}

	//___initial parameters___
	m_nImgWidth = nImageWidth;
	NOS_1  = nImageHeight - 1;
	NOS_2  = NOS_1 - 1;
	PPSp2WORD = (m_nImgWidth+2)*sizeof(unsigned short);

	//___Allocate memory___
	MemoryFree();
	m_pSharp3Lines = new unsigned short[(m_nImgWidth+2)*3];
	if ( !m_pSharp3Lines )
	{
		return 8;//ERROR_NOT_ENOUGH_MEMORY;
	}

	//___assigne pointer
	pL1 = m_pSharp3Lines;
	pL2 = pL1 + m_nImgWidth + 2;
	pL3 = pL2 + m_nImgWidth + 2;
	pSrc = pOrgImage;
	pDst = pDstImage;


	//___Process boundary
	GetLineLight(pSrc,pL1);
	memcpy(pL2,pL1,PPSp2WORD);
	GetLineLight(pSrc+nBytePerLine,pL3);


	//___Do sharpen
	for (i=0;i<NOS_2;i++) {
		DoSharpPerLine(pSrc,pDst,pL1,pL2,pL3);
		memcpy(pL1,pL2,PPSp2WORD);
		memcpy(pL2,pL3,PPSp2WORD);
		pSrc += nBytePerLine;
		pDst += nBytePerLine;
		GetLineLight(pSrc+nBytePerLine,pL3);
	}
	for(i=NOS_2;i<nImageHeight;i++) {
		DoSharpPerLine(pSrc,pDst,pL1,pL2,pL3);
		memcpy(pL1,pL2,PPSp2WORD);
		memcpy(pL2,pL3,PPSp2WORD);
		pSrc += nBytePerLine;
		pDst += nBytePerLine;
	}
	MemoryFree();

	return 0;//NO_ERROR;
}

//add R+G+B to the tmp memory pLine that size is (PPS+2)
inline void CHTSharpen::GetLineLight(unsigned char *pSrc,unsigned short *pLine)
{
	unsigned char *pChn1,*pChn2,*pChn3;
	unsigned long i;
	pChn1 = pSrc;
	pChn2 = pChn1 + m_nChnJmp;
	pChn3 = pChn2 + m_nChnJmp;
	for (i=0;i<m_nImgWidth;i++) {
		pLine[i+1] = (unsigned short) *pChn1 + *pChn2 + *pChn3;
		pChn1 += m_nPixelJmp;
		pChn2 += m_nPixelJmp;
		pChn3 += m_nPixelJmp;
	}
	// First & Last Pixel
	pLine[0] = pLine[1];
	pLine[m_nImgWidth+1] = pLine[m_nImgWidth];
	return;
}

inline void CHTSharpen::DoSharpPerLine(unsigned char *pSrc,unsigned char *pDst,unsigned short *pL1,unsigned short *pL2,unsigned short *pL3)
{
	unsigned char *pChn1S,*pChn2S,*pChn3S;
	unsigned char *pChn1D,*pChn2D,*pChn3D;
	unsigned long i;//,AvgRate;
	double Total;
	pChn1S = pSrc;
	pChn2S = pChn1S + m_nChnJmp;
	pChn3S = pChn2S + m_nChnJmp;
	pChn1D = pDst;
	pChn2D = pChn1D + m_nChnJmp;
	pChn3D = pChn2D + m_nChnJmp;

	for (i=0;i<m_nImgWidth;i++)
	{
		Total = (double) (pL2[1]<<3) + (pL2[1]<<2) - (pL1[0]+pL1[2]+pL3[0]+pL3[2]) - ((pL1[1]+pL2[0]+pL2[2]+pL3[1])<<1);
		Total /= m_nAvgRate;
		if ( Total<m_dbThreshold && Total>-m_dbThreshold )
		{
			*pChn1D = *pChn1S;
			*pChn2D = *pChn2S;
			*pChn3D = *pChn3S;
		}
		else
		{
			//___calculate___
			if ( Total>0.0 )
				Total -= m_dbThreshold;
			else if ( Total<0.0 )
				Total += m_dbThreshold;
			Total *= m_dbRate;
			if ( Total>m_dbMaxBound )
				Total = m_dbMaxBound;
			else if ( Total<-m_dbMaxBound )
				Total = -m_dbMaxBound;

			//___update Chn1___
			if ( (Total + *pChn1S) >255 )
				*pChn1D = 255;
			else if ( (Total + *pChn1S) < 0 )
				*pChn1D = 0;
			else
				*pChn1D = (unsigned char) (Total + *pChn1S + 0.5);
			//___update Chn2___
			if ( (Total + *pChn2S) > 255 )
				*pChn2D = 255;
			else if ( (Total + *pChn2S) < 0 )
				*pChn2D = 0;
			else
				*pChn2D = (unsigned char) (Total + *pChn2S + 0.5);
			//___update Chn3___
			if ( (Total + *pChn3S) > 255 )
				*pChn3D = 255;
			else if ( (Total + *pChn3S) < 0 )
				*pChn3D = 0;
			else
				*pChn3D = (unsigned char) (Total + *pChn3S + 0.5);
		}
		pChn1S += m_nPixelJmp;
		pChn2S += m_nPixelJmp;
		pChn3S += m_nPixelJmp;
		pChn1D += m_nPixelJmp;
		pChn2D += m_nPixelJmp;
		pChn3D += m_nPixelJmp;
		pL1 ++;
		pL2 ++;
		pL3 ++;
	}
	return;
}

//================================================================================================================================
#ifdef WIN32
unsigned long SharpenImage(unsigned char* lpImageOrg, unsigned char* lpImageDst, unsigned long dwBytePerLine, unsigned long dwImageWidth, unsigned long dwImageHeight, unsigned char nSharpenValue, unsigned long nBitsPerPixel)
#else
__attribute__((__visibility__("hidden"))) unsigned long SharpenImage(unsigned char* lpImageOrg, unsigned char* lpImageDst, unsigned long dwBytePerLine, unsigned long dwImageWidth, unsigned long dwImageHeight, unsigned char nSharpenValue, unsigned long nBitsPerPixel)
#endif
{
	unsigned long		dwError = 0;

	CHTSharpen			cSharpen;

	cSharpen.SetBitsPerPixel(nBitsPerPixel);

	if( nSharpenValue > 100 ) nSharpenValue = 100;
	cSharpen.SetLevel(nSharpenValue);

    dwError = cSharpen.DoSharpenImg(lpImageOrg, lpImageDst, dwBytePerLine, dwImageWidth, dwImageHeight);

	return 0;
}