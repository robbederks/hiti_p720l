/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright 2014-2015 HiTi Digital, Inc.  All Rights Reserved
**********************************************************/

#ifndef __HTSHARPEN_H__
#define __HTSHARPEN_H__

#define BI_INTERLEAVE 0x01
#define BI_NONINTERLEAVE 0x02

#ifdef WIN32
class CHTSharpen
#else
class __attribute__((__visibility__("hidden"))) CHTSharpen
#endif
{
public:
	CHTSharpen();
	virtual ~CHTSharpen();

	void	SetBitsPerPixel(unsigned long nBitsPerPixel);
	void	SetLevel(unsigned char nSharp); // Sharpen Level is from 0 to 100
	unsigned long	DoSharpenImg(unsigned char *pOrgImage, unsigned char *pDstImage, unsigned long nBytePerLine, unsigned long nImageWidth, unsigned long nImageHeight);

protected:
	double m_dbRate;
	double m_dbMaxBound;
	double m_dbThreshold;
	unsigned short *m_pSharp3Lines;
	unsigned long m_nImgWidth;
	unsigned long m_nChnJmp;
	unsigned long m_nPixelJmp;
	unsigned long m_nAvgRate;

	void MemoryFree();
	void GetLineLight(unsigned char *pSrc,unsigned short *pLine);
	void DoSharpPerLine(unsigned char *pSrc,unsigned char *pDst,unsigned short *pL1,unsigned short *pL2,unsigned short *pL3);
};

unsigned long SharpenImage(unsigned char* lpImageOrg, unsigned char* lpImageDst, unsigned long dwBytePerLine, unsigned long dwImageWidth, unsigned long dwImageHeight, unsigned char nSharpenValue, unsigned long nBitsPerPixel);

#endif // !defined(__HTSHARPEN_H__)
