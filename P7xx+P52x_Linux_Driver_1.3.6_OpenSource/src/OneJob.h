/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright HiTi Digital, Inc. 2014-2015 All Rights Reserved
**********************************************************/

#ifndef __ONEJOB_H__
#define __ONEJOB_H__

#include "HTRTApi.h"

#include "Util0.h"

#include "HppCmd.h"
#include "UsbLink_cups.h"


typedef struct {
    unsigned char nTagCode;
    UINT32 nSizeOfWord;
    UINT32 nOffset; // For recode Offset in file (orginal)
    UINT32 nBufferOffset;// For recode Offset in Buffer (Destination)
} HEATTB_HEADER,*PHEATTB_HEADER;

#define HT_NONE		0x0000
#define HT_Y		0x0004
#define HT_M		0x0002
#define HT_C		0x0001

#define HT_O		0x0080 // O, MO, KO, Only 1 plane will be copied.
#define HT_KO		0x0090 // O, MO, KO, Only 1 plane will be copied.	//2010.5.12 old papameter defined 0x0088
#define HT_MO		0x00C0 // O, MO, KO, Only 1 plane will be copied.

#define HT_K		0x0010 // Dye K,
#define HT_RSN_TYPE 0x0008 // For conpatible to old version
#define HT_R		0x0108 // R, RK, Only 1 plane will be copied.//2011.4.14 old papameter defined 0x0010
#define HT_RK		0x0208 // For Card printer, YMCKO dye and KO dye	//2010.5.12 old papameter defined 0x0018

//#define HT_K		0x0008 // Dye K, R, RK, Only 1 plane will be copied.
//#define HT_R		0x0108 // R, RK, Only 1 plane will be copied.//2010.5.12 old papameter defined 0x0010
//#define HT_RK		0x0208 // For Card printer, YMCKO dye and KO dye	//2010.5.12 old papameter defined 0x0018

#define HT_L		0x0400												//2010.5.12 old papameter defined 0x0020
#define HT_FO		0x0800												//2010.5.12 old papameter defined 0x0020

#define HT_W_RW		0x1000												//2015.12.21 Add Write tag for rewrite card
#define HT_E_RW		0x2000												//2015.12.21 Add Erase tag for rewrite card

// Tone Curve table type, only one table will be loaded.
#define CT_NONE				0x0000
#define CT_INVERT			0x0001
#define CT_HITI_CLASSIC		0x0002
#define CT_HITI_IDPASSP		0x0004

//For Insillica
#define INDEX		0x00
#define HT_D_Y		0x01
#define HT_D_M		0x02
#define HT_D_C		0x03
#define HT_D_K		0x04

#define HT_R_K		0x10	//Resin for KO and YMCKO ribbon in card printer
#define HT_R_R		0x11	//Resin for K ONLY K ribbon in card printer
#define HT_R_L		0x12	//Lamination
#define HT_R_FO		0x13	//Fluorescent


#define HT_D_O		0x20
#define HT_D_KO		0x21
#define HT_D_MO		0x22

#define HT_CVD		0x40


#define CT_Invert	0x80
#define CT_Classic	0x81
#define CT_IDPass	0x82

#define TC_Compensation 0x90

#define MAIN_VERSION	0x50
#define MINOR_VERSION	0x51

#define HEAT_ACCUM_COMP_Y	0xA1
#define HEAT_ACCUM_COMP_M	0xA2
#define HEAT_ACCUM_COMP_C	0xA3
#define HEAT_ACCUM_COMP_DK	0xA4
#define HEAT_ACCUM_COMP_RK	0xA5
#define HEAT_ACCUM_COMP_R	0xA6
#define HEAT_ACCUM_COMP_O	0xA7
#define HEAT_ACCUM_COMP_MO	0xA8
#define HEAT_ACCUM_COMP_FO	0xA9
#define HEAT_ACCUM_COMP_YMC	0xAE
#define HEAT_ACCUM_COMP_ALL	0xAF

#define LONG_SMEAR_Y	0xB1
#define LONG_SMEAR_M	0xB2
#define LONG_SMEAR_C	0xB3
#define LONG_SMEAR_DK	0xB4
#define LONG_SMEAR_RK	0xB5
#define LONG_SMEAR_R	0xB6
#define LONG_SMEAR_O	0xB7
#define LONG_SMEAR_MO	0xB8
#define LONG_SMEAR_FO	0xB9
#define LONG_SMEAR_YMC	0xBE
#define LONG_SMEAR_ALL	0xBF

#define GHOST_LINE_COMP_Y	0xC1
#define GHOST_LINE_COMP_M	0xC2
#define GHOST_LINE_COMP_C	0xC3
#define GHOST_LINE_COMP_DK	0xC4
#define GHOST_LINE_COMP_RK	0xC5
#define GHOST_LINE_COMP_R	0xC6
#define GHOST_LINE_COMP_O	0xC7
#define GHOST_LINE_COMP_MO	0xC8
#define GHOST_LINE_COMP_FO	0xC9
#define GHOST_LINE_COMP_YMC	0xCE
#define GHOST_LINE_COMP_ALL	0xCF

#define ENERGY_Y	0xD1
#define ENERGY_M	0xD2
#define ENERGY_C	0xD3
#define ENERGY_DK	0xD4
#define ENERGY_RK	0xD5
#define ENERGY_R	0xD6
#define ENERGY_O	0xD7
#define ENERGY_MO	0xD8
#define ENERGY_FO	0xD9
#define ENERGY_YMC	0xDE
#define ENERGY_ALL	0xDF

#ifdef WIN32
class COneJob
#else
class __attribute__((__visibility__("hidden"))) COneJob
#endif
{
public:

	HITI_JOB_PROPERTY_RT	m_JobProp;

	CUsbLink*			m_Usb;
	CHppCmd*			m_Hpp;

	SUBL_FORMAT			m_SublFormat;

	UINT32				m_dwDrvModel;
	UINT16				m_wRbnVendor;

	UINT16				m_wPageReceived;

	BITMAP				m_Bmp1;
	BITMAP				m_Bmp2;
	BITMAP				m_BmpFinal;
	bool				m_bAllocFinalBmp;

public:

	COneJob();
	~COneJob();

	UINT32				SendOnePageData(HITI_JOB_PROPERTY_RT* lpJobPropIn, BITMAP* lpBmpIn, int nCopies, int nPageNum);

	UINT32				EndJob();

private:

	UINT32				CheckPrinterStatus();
	UINT32				CheckRealRibbon();
	UINT32				CheckRealPaper();

	UINT32				ApplyTableMap(BITMAP* lpBmpSrc, BITMAP* lpBmpDst);

	UINT32				ApplyCompensateHeatAccu(unsigned char *pData,unsigned long Height,unsigned long Width, unsigned char ChnNo);

	UINT32				PrintOnePage(int nCopies);

	UINT8				SendPlaneData(UINT8* lpSrc, UINT16 wWidth, UINT16 wHeight, short shStartLine, UINT16 wLinesOneTime);
	bool 				SendHeatingTableForRoller();
	bool 				GetInsillicaHeatData(
                				TCHAR* Path,
				                unsigned char *Buffer, UINT32 BufferSize, UINT32 *NeedSize,
				                unsigned short HeatPlanes, bool CVDPlane, unsigned short ColorTableType,
				                unsigned char MediaType);
	bool 				GetPlaneNumWithHAC( PHEATTB_HEADER pNewPlaneTag, unsigned char &nSelTagNum, PHEATTB_HEADER pTB_Header,  unsigned char nTagNum,unsigned short HeatPlanes, bool CVDPlane, unsigned short ColorTableType);

	bool FindPlaneIndex(unsigned char &nTagCount, PHEATTB_HEADER pNewPlaneTag, PHEATTB_HEADER pTB_Header, UINT32 nTagNum, unsigned char nTargetTagCode);
};

#endif//__ONEJOB_H__
