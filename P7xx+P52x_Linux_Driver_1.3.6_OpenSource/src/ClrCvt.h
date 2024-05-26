/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright 2014-2015 HiTi Digital, Inc.  All Rights Reserved
**********************************************************/

#ifndef __CLRCVT_H__
#define __CLRCVT_H__

//__Define Data Mode - From interleaved to non - interleaved
#define CVT_BGR2YMC		0x01	// B is LSB
#define CVT_BGRA2YMC	0x02	// B is LSB
#define CVT_ABGR2YMC	0x03	// A is LSB
#define CVT_RGB2YMC		0x04	// R is LSB
#define CVT_RGBA2YMC	0x05	// R is LSB
#define CVT_ARGB2YMC	0x06	// A is LSB


#define MODEL_HITI_P720				103		//P72x
#define MODEL_HITI_P520				108		//P52x
#define MODEL_HITI_P750				107		//p75x

#define USBMODEL_P720L				49
#define USBMODEL_P750L				67//2013.03.06
#define USBMODEL_P520L				68//2013.03.06
#define USBMODEL_P525N				91//2022.08.04

#define DNP_PAPER_MEDIA1			0x00
#define DNP_STICKER_MEDIA1			0x01


//__Define Media Type and Printing MODE
#define MEDIA1_MODE0				0x00	// Normal
#define MEDIA1_MODE1				0x06	// Fine mode
#define MEDIA2_MODE0				0x40	// Normal
#define MEDIA2_MODE1				0x41	// Fine mode


//__Define Color MODE
#define COLOR_REVERSE				0x00	// No Color preference
#define COLOR_CLASSICAL				0x06	// HiTi classic color
#define COLOR_VIVID_MODE			0x07	// HiTi classic color
#define COLOR_BASIC					0xF1	// Basic color mode



//__Error Code
#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS                   0L
#endif
#define ERROR_INVALID_FUNCTION          1L
#define ERROR_FILE_NOT_FOUND            2L
#define ERROR_PATH_NOT_FOUND            3L
#define ERROR_TOO_MANY_OPEN_FILES       4L
#define ERROR_ACCESS_DENIED             5L
#define ERROR_NOT_ENOUGH_MEMORY         8L
#define ERROR_BAD_FORMAT                11L
#define ERROR_INVALID_DATA              13L
#define ERROR_OUTOFMEMORY               14L
#define ERROR_VERSION_MISMATCH			20000// Error user defined



typedef struct {
  unsigned char    rgbBlue;
  unsigned char    rgbGreen;
  unsigned char    rgbRed;
  unsigned char    rgbK;
} DEFRGBK;

#ifdef WIN32
class CClrCvt
#else
class __attribute__((__visibility__("hidden"))) CClrCvt
#endif
{
	bool m_bTableDynamic;
	
public:
	CClrCvt();
	virtual ~CClrCvt();

	unsigned long LoadColorTable(char* pPath, unsigned char MediaType, unsigned long ColorMode, unsigned long dwDrvModel, bool bNewTPH);
	bool Convert(unsigned char *pOrgImage,unsigned char *pDstImage,unsigned long BytePerLine,unsigned long ImageWidth,unsigned long ImageHeight,unsigned char DataMode);


protected:
	void InitInterpolatePara();
	void Interp33To256C3(unsigned char *DstRGB, unsigned char *SrcRGB, unsigned char* pTable);

	unsigned char *m_pTable;
	unsigned long m_pTB1089[33];
	unsigned long m_pTB33[33];
	unsigned short m_pTB_256_8[2304];//256*9



};


unsigned long ConvertBGR2YMC(unsigned char* lpImageOrg, unsigned char* lpYMC, unsigned long dwImageWidth, unsigned long dwBytePerLine, unsigned long dwImageHeight, unsigned char byPrintMode, unsigned short wRbnVen, unsigned long dwDrvModel, unsigned char byDataModeIn, bool bNewTPH);

#endif // !defined(__CLRCVT_H__)
