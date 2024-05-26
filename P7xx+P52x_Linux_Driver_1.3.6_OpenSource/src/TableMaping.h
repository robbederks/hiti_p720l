/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright 2014-2015 HiTi Digital, Inc.  All Rights Reserved
**********************************************************/

#ifndef __TABLEMAPPING_H__
#define __TABLEMAPPING_H__

//class CMapingTable;
#include "mytypes.h"
#include "MapTable.h"
#include "CSaturation.h"


#ifdef WIN32
class CTableMaping
#else
class __attribute__((__visibility__("hidden"))) CTableMaping
#endif
{
public:
	CTableMaping();
	virtual ~CTableMaping();
	void ResetAllTable();
	
	bool MapTableImage( BITMAP &SrcBMP,BITMAP &DstBMP);
	long ApplySaturate( BITMAP &SrcBMP,BITMAP &DstBMP);

	// Map Table
	void SetBrightTable(int Bright);
	void SetContrastTable(int Contrast);
	void SetGammaTable(int Gamma);
	void SetGammaTableByfloat(int gamma);
	void SetColorBalanceTable(int CB,unsigned char Channel);

	bool SetSaturateTable(int Sat);
	

	// Set Slider map
	bool SetBrightSlider(int Min,int Max,unsigned char OffsetN=128,unsigned char OffsetP=127);
	bool SetContrastSlider(int Min,int Max,unsigned char OffsetN=128,unsigned char OffsetP=127);
	bool SetGammaSlider(int Min,int Max,unsigned char OffsetN=128,unsigned char OffsetP=127);
	bool SetColorBalanceSlider(int Min,int Max,unsigned char Channel,unsigned char OffsetN=128,unsigned char OffsetP=127);

	bool SetSaturateSlider(int Min,int Max,unsigned char OffsetN,unsigned char OffsetP);
	
	int GetSliderContrast(unsigned char RealV);
	int GetSliderBright(unsigned char RealV);
	int GetSliderGamma(unsigned char RealV);
	int GetSliderColorB(unsigned char RealV,unsigned char Channel);
	unsigned char GetRealBright(int Bright);
	unsigned char GetRealContrast(int Contrast);
	unsigned char GetRealGamma(int Gamma);
	unsigned char GetRealColorBalance(int CB,unsigned char Channel);
protected:
	CMapingTable cMapTable;
	CSaturation		m_CSat;
	int MinB,MaxB,MinC,MaxC,MinG,MaxG,MinCBR,MaxCBR,MinCBG,MaxCBG,MinCBB,MaxCBB;
	unsigned char RMinB,RMaxB,RMinC,RMaxC,RMinG,RMaxG,RMinCBR,RMaxCBR,RMinCBG,RMaxCBG,RMinCBB,RMaxCBB;
	int MinS,MaxS;
	unsigned char RMinS,RMaxS;

	unsigned char GetRealValue(int SliderV,int Min,int Max,unsigned char MinR,unsigned char MaxR);
	int GetSliderValue(unsigned char RealV,int Min,int Max,unsigned char MinR,unsigned char MaxR);

};

#endif // !defined(__TABLEMAPING_H__)
