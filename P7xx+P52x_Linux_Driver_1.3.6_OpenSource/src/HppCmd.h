/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright 2014-2015 HiTi Digital, Inc.  All Rights Reserved
**********************************************************/

#ifndef __HPPCMD_H__
#define __HPPCMD_H__

#include "UsbLink_cups.h"


#define	HITI_ERROR_REPRINT						3113
#define	HITI_ERROR_SKIP_PAGE					3114
#define	HITI_ERROR_NEED_RESEND					3115

//=============================== HPP Command Set =============================
#define	CMD_HDR_LEN			10
#define	RSP_HDR_LEN			6
#define SIZE_8K				(8 * 1024)


static unsigned char		lpCmdHdr[CMD_HDR_LEN] = {0xA5, 0x00, 0x03, 0x50, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

//0:use DoCommand0, 1:use DoCommand1, 2:use DoCommand2, 3:use DoCommand3,

//3.2 Request Device Characteristics (RDC)
#define RDC_RS			0x0100		//0			//Request Summary
#define RDC_ROC			0x0104		//0			//Request Option Characteristics

//3.3 Printer Configuration Control (PCC)
#define PCC_RP			0x0301		//1			//Reset Printer
#define RESET_PRINTER	0x01
#define RESET_SOFT		0x02
#define PCC_STP			0x030F		//1			//Set Target Printer

//3.4 Request Device Status (RDS)
#define RDS_RSS			0x0400		//0			//Request Status Summary
#define RDS_RIS			0x0401		//0			//Request Input Status
#define RDS_RIA			0x0403		//0			//Request Input Alert
#define RDS_RJA			0x0405		//0			//Request Jam Alert
#define RDS_ROIRA		0x0406		//0			//Request Operator Intervention Required Alert
#define RDS_RW			0x0407		//0			//Request Warnings
#define RDS_DSRA		0x0408		//0			//Device Service Required Alerts
#define RDS_SA			0x040A		//0			//Supplies Alerts
#define RDS_RPS			0x040B		//0			//Request Printer Statistics
#define RDS_RSUS		0x040C		//1			//Request Supplies Status

//3.5 Job Control (JC)
#define JC_SJ			0x0500					//Start Job
#define JC_EJ			0x0501					//End Job
#define JC_QJC			0x0502					//Query Job Completed
#define JC_QQA			0x0503					//Query Job(s) Queued or Active
#define JC_RSJ			0x0510		//2			//Resume Suspended Job

//3.6 Ext Read Device Characteristics (ERDC)
#define ERDC_RS			0x8000		//0			//Request Summary
#define ERDC_RCC		0x8001		//0			//Request Calibration Characteristics
#define ERDC_RPC		0x8005		//1			//Request Print Count
#define ERDC_RSN		0x8007		//1			//Request Serial Number
#define ERDC_RPIDM		0x8009		//0			//Request PID and Model Code
#define ERDC_RRVC		0x800F		//0			//Request Ribbon Vendor Code


//3.7 Ext Format Data (EFD)
#define EFD_SF			0x8100					//Sublimation Format
#define EFD_CHS			0x8101		//2			//Color & Heating Setting


//3.8 Ext Page Control (EPC)
#define EPC_SP			0x8200		//0			//Start Page
#define EPC_EP			0x8201		//0			//End Page
#define EPC_SYP			0x8202		//0			//Start Yellow Plane
#define EPC_SMP			0x8204		//0			//Start Magenta Plane
#define EPC_SCP			0x8206		//0			//Start Cyne Plane


//3.9 Ext Send Data (ESD)
#define ESD_SEHT		0x8304					//Send Ext Heating Table
#define ESD_SD			0x8308		//n			//Send Data
#define ESD_SHPTC		0x830B		//n			//Send Heating Parameters & Tone Curve
#define ESD_C_SHPTC		0x830C		//n			//Send Heating Parameters & Tone Curve



//3.10 Ext Write Flash/Nvram (EWFN)
#define EWFN_WCV		0x8403		//n			//Write Calibration Value
#define EWFN_RNV		0x8405		//1			//Read Nvram


#define EDM_CVD			0xE002		//n			//Common Voltage Drop Values
#define EDM_CPP			0xE023		//1			//Clean Paper Path (clean time)

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//for SUBL_FORMAT.byMediaType
#define	FW_MEDIA_PHOTO_4X6		0	//Photo paper - 4 x 6
#define	FW_MEDIA_PHOTO_5X7		2	//Photo paper - 5 x 7
#define	FW_MEDIA_PHOTO_6X8		3	//Photo paper - 6 x 8
#define	FW_MEDIA_PHOTO_6X9		6	//Photo paper - 6 x 9
#define	FW_MEDIA_PHOTO_6X9_2UP	7	//Photo paper - 6 x 9 2-UP
#define	FW_MEDIA_PHOTO_5X3dot5	8	//Photo paper - 3.5 x 5
#define	FW_MEDIA_PHOTO_6X4_2UP	9	//Photo paper - 6 x 4 2-UP
#define	FW_MEDIA_PHOTO_6X2		10	//Photo paper - 6 x 2
#define	FW_MEDIA_PHOTO_5X7_2UP	11	//Photo paper - 5 x 7 2-UP


typedef struct tagSUBL_FORMAT
{
	unsigned short	wXRes;			//Horizontal logical units per length unit of image
	unsigned short	wYRes;			//Vertical logical units per length unit of image
	unsigned short	wWidth;			//Horizontal logical units of image
	unsigned short	wHeight;		//Vertical logical units of image

	unsigned char	byMediaType;	//Media type
	char			chXOffset;		//Number of horizontal offset logical units
	char			chYOffset;		//Number of vertical offset logical units
	unsigned char	byColorSeq;		//Print color sequence
	unsigned char	byCopies;
	unsigned char	byPrintMode;	//0x02=Fine mode

	unsigned char	byReserved1;
	unsigned char	byReserved2;

}SUBL_FORMAT;

//for ERDC_RS
typedef struct tagEXT_DEV_CHAR
{
	UINT8	byTphSeg;				//TPH segment number
	UINT8	byReserve1;				//2006.03.30
	UINT8	byReserve2;				//2006.03.30
	UINT8	byReserve3;				//2006.03.30
	UINT16	wTphPixel;				//TPH pixels per print line
	UINT16	wMaxHoriUnit;			//Maximum horizontal logical units per length unit

	UINT16	wMaxVertUnit;			//Maximum vertical logical units per length unit
	UINT16	wHoriPrintableUnit;		//Number of printable horizontal logical units
	UINT16	wVertPrintableUnit;		//Number of printable vertical logical units
	UINT16	wVertUnitInstalled;		//Number of vertical logical units installed

	UINT8	byYSpeed;				//Print of yellow in speed unit
	UINT8	byMSpeed;				//Print of magenta in speed unit
	UINT8	byCSpeed;				//Print of cyne in speed unit
	UINT8	byOSpeed;				//Print of overcoating in speed unit
	UINT8	byOverheatTemp;			//Overheat temperature of TPH
	UINT8	byHeaterOffTemp;		//Heater Off temperature of TPH
	UINT8	byHwFeature1;			//Printer hardware feature #1
	UINT8	byFwFeature1;			//Printer firmware feature #1
	//UINT8	byPreheatTemp;			//Preheat temperature of TPH

	UINT16	wVertPrintableUnit2;	//Number of 2nd printable vertical logical units, 1.5.0
	UINT16	wVertPrintableUnit3;	//Number of 3rd printable vertical logical units, 1.5.0

	UINT8	byPreheatTemp;			//Preheat temperature of TPH, 2006.03.30 move to here
	UINT8	byFwFeature2;			//Printer firmware feature #2, 1.6.0
	UINT8	byOverheatTempS;		//Overheat temperature of TPH for sticker print, 1.6.2
	UINT8	byOverheatTempOT;		//Overheat temperature of TPH for O texture print, 1.6.4

	//2010.04.27, add for card printer
	UINT8	byHwFeature2;			//Printer hardware feature #2
	UINT8	byFanOffTemp;			//Fan Off temperature of TPH
	UINT8	byOverheatTempMono;		//Overheat temperature for mono
	UINT8	byOverheatTempLam;		//Overheat temperature for Lamination
	UINT8	byReserve4;
	UINT8	byReserve5;
	UINT8	byReserve6;
	UINT8	byReserve7;

}EXT_DEV_CHAR;//, FAR *LPEXT_DEV_CHAR;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef WIN32
class CHppCmd
#else
class __attribute__((__visibility__("hidden"))) CHppCmd
#endif
{
// Attributes
public:

	CUsbLink			*m_cpUsbLink;

private:

	//members to keep info about output data
	unsigned long		m_dwSendRtrn;

	//members to keep info about input data
	unsigned long		m_dwRecvRtrn;
	unsigned char		m_RspHeader[10],
						m_RspData[SIZE_8K];
	unsigned short		m_wRspLen;

// Operations
public:

	CHppCmd();
	~CHppCmd();

	void			SetUsbLink(CUsbLink *cpLink)	{ m_cpUsbLink = cpLink; };

	void			SetLogEnable(unsigned long dwLevel, const TCHAR* szFileName = NULL, bool bJobBegin=true);

	unsigned long	CheckDeviceStatus(unsigned long* lpdwErrCode);
	unsigned long	CheckDeviceAlert(unsigned short wCmd, unsigned long* lpdwErrCode);
	unsigned long	WaitBusy(unsigned long dwSleep = 250, bool bCheckOtherBits=false);
	bool			IsBusy(bool bShowError = true);

	//====================== HPP Command Set ======================
	unsigned char	DoCommand0(unsigned short wCmd);
	unsigned char	DoCommand1(unsigned short wCmd, unsigned char byPara1);
	unsigned char	DoCommand2(unsigned short wCmd, unsigned char byPara1, unsigned char byPara2);
	unsigned char	DoCommand3(unsigned short wCmd, unsigned char byPara1, unsigned char byPara2, unsigned char byPara3);
	unsigned char	DoCmdSendData(unsigned short wCmd, unsigned char* lpData, unsigned short wDataLen, bool bCypressCode=false);

	unsigned char	SendCommand(unsigned char *lpData, unsigned long dwDataLen);
	unsigned char	GetResponse();

	void			GetRspData(unsigned char** ppBuf, unsigned long* lpdwBuf);

	//Job Control (JC)
	unsigned char	StartJob(unsigned short *lpwJobID);
	unsigned char	EndJob(unsigned short wJobID);
	unsigned char	QueryJobCompleted(unsigned short wJobID);
	unsigned char	QueryJobQueuedActive(unsigned short wJobID, long* lpnJobNum=0);
	unsigned char	ResumeAllSuspendedJob();

	//Ext Send Data (ESD)
	unsigned char	ExtSendExtPrintData(unsigned short wStartLineNum, unsigned short wTotalLines, unsigned char *lpData, unsigned long dwDataLen);
	unsigned char	ExtSublimationFormat(SUBL_FORMAT* lpFormat);

	unsigned char	QueryCalibrationValue(unsigned char* lpbyH, unsigned char* lpbyV);
	unsigned char	QueryRibbonCount(unsigned long* lpdwRibbonCount);
	unsigned char	QueryFwVersion(char* szFwVer);
	unsigned char	QuerySerial(char* szSerial, unsigned long* lpdwSize);

	unsigned char	ExtSendHeatingTable(unsigned char byColor, unsigned char *lpHTblCode, unsigned long dwTblLen);
	unsigned char	DoCmdSendData(unsigned short wCmd, unsigned char* lpData, unsigned short wDataLen);
	unsigned char	DoCmdGetDataOnlyForERDC_RS(unsigned short wCmd, unsigned char* lpBuffer, unsigned short wBufLen);

};

#endif//#ifndef __HPPCMD_H__
