/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright HiTi Digital, Inc. 2013-2015 All Rights Reserved
**********************************************************/

#ifndef __HTRTAPI_H__
#define __HTRTAPI_H__

#include "mytypes.h"


#ifdef __cplusplus
extern "C" {
#endif


//HiTi defined Device Status used for HITI_CheckPrinterStatus and MSG_DEVICE_STATUS
#define HITI_DS_BUSY						0x00080000	//!< Printer is busy
#define HITI_DS_OFFLINE						0x00000080	//!< Printer is disconnected or power off
#define HITI_DS_PRINTING					0x00000002	//!< Printer is printing
#define HITI_DS_PROCESSING_DATA				0x00000005	//!< Driver is processing print data
#define HITI_DS_SENDING_DATA				0x00000006	//!< Driver is sending data to printer
#define HITI_DS_COVER_OPEN					0x00050001	//!< Cover open/Ribbon cassette door open
#define HITI_DS_COVER_OPEN2					0x00050101	//!< Cover open/Ribbon cassette door open
#define HITI_DS_PAPER_OUT					0x00008000	//!< Paper out or feeding error
#define HITI_DS_PAPER_LOW					0x00008001	//!< Paper low
#define HITI_DS_PAPER_JAM					0x00030000	//!< Paper jam
#define HITI_DS_PAPER_TYPE_MISMATCH			0x000100FE	//!< Paper type mismatch
#define HITI_DS_PAPER_TRAY_MISMATCH			0x00008010	//!< Paper tray mismatch
#define HITI_DS_TRAY_MISSING				0x00008008	//!< Paper tray missing
#define HITI_DS_RIBBON_MISSING				0x00080004	//!< Ribbon missing
#define HITI_DS_OUT_OF_RIBBON				0x00080103	//!< Out of ribbon
#define HITI_DS_RIBBON_TYPE_MISMATCH		0x00080200	//!< Ribbon type mismatch
#define HITI_DS_RIBBON_ERROR				0x000802FE	//!< Ribbon error
#define HITI_DS_SRAM_ERROR					0x00030001	//!< SRAM error
#define HITI_DS_SDRAM_ERROR					0x00030101	//!< SDRAM error
#define HITI_DS_ADC_ERROR					0x00030201	//!< ADC error
#define HITI_DS_NVRAM_ERROR					0x00030301	//!< NVRAM read/write error
#define HITI_DS_FW_CHECKSUM_ERROR			0x00030302	//!< Check sum error - SDRAM
#define HITI_DS_DSP_CHECKSUM_ERROR			0x00030402	//!< DSP code check sum error
#define HITI_DS_CAM_PLATEN_ERROR			0x00030501	//!< Cam Platen error
#define HITI_DS_ADF_ERROR					0x00030601	//!< Adf Cam error
#define HITI_DS_WRITE_FAIL					0x0000001F	//!< Send data to printer fail
#define HITI_DS_READ_FAIL					0x0000002F	//!< Get data from printer fail

#define HITI_DS_CMD_SEQ_ERROR				0x000301FE	//!< Command sequence error
#define HITI_DS_NAND_FLASH_UNFORMATED		0x000302FE	//!< Nand flash unformatted
#define HITI_DS_NAND_FLASH_NOT_ENOUGH		0x000303FE	//!< Nand flash space is not enough
#define HITI_DS_HEAT_PARA_INCOMPATIBLE		0x000304FE	//!< Heating parameter table incompatible
#define HITI_DS_DUSTBOX_FULL				0x000502FE	//!< Dust box needs cleaned
#define HITI_DS_NVRAM_CRC_ERROR				0x00030502	//!< Nvram CRC error
#define HITI_DS_CHECKSUM_ERROR_SRAM			0x00030602	//!< Check sum error - SRAM
#define HITI_DS_CHECKSUM_ERROR_FLASH		0x00030702	//!< Check sum error - FLASH
#define HITI_DS_CHECKSUM_ERROR_FW			0x00030802	//!< Check sum error - wrong firmware
#define HITI_DS_NAND_FLASH_ERROR			0x00031201	//!< Nand flash error
#define HITI_DS_CUTTER_ERROR				0x00031302	//!< Cutter error
#define HITI_DS_RIBBON_AUTH_FAIL			0x000803FE	//!< Ribbon is not authenticated yet
#define HITI_DS_RIBBON_IC_ERROR				0x000804FE	//!< Ribbon IC R/W error
#define HITI_DS_RIBBON_UNSUPPORTED			0x000806FE	//!< Unsupported ribbon
#define HITI_DS_RIBBON_UNKNOW				0x000808FE	//!< Unknown ribbon
#define HITI_DS_PAPER_JAM_01				0x00030200	//!< Paper Jam in paper path (01)
#define HITI_DS_PAPER_JAM_02				0x00030300	//!< Paper Jam in paper path (02)
#define HITI_DS_PAPER_JAM_03				0x00030400	//!< Paper Jam in paper path (03)
#define HITI_DS_PAPER_JAM_04				0x00030500	//!< Paper Jam in paper path (04)
#define HITI_DS_PAPER_JAM_05				0x00030600	//!< Paper Jam in paper path (05)
#define HITI_DS_PAPER_JAM_06				0x00030700	//!< Paper Jam in paper path (06)
#define HITI_DS_PAPER_JAM_07				0x00030800	//!< Paper Jam in paper path (07)
#define HITI_DS_PAPER_JAM_08				0x00030900	//!< Paper Jam in paper path (08)
#define HITI_DS_PAPER_JAM_09				0x00030A00	//!< Paper Jam in paper path (09)

//following errors are used for P720L/P728L/P520L
#define HITI_DS_0100_COVER_OPEN				0x00000100	//!< 0100 Cover open
#define HITI_DS_0101_COVER_OPEN_FAIL		0x00000101	//!< 0101 Cover open fail
#define HITI_DS_0200_IC_CHIP_MISSING		0x00000200	//!< 0200 IC chip missing
#define HITI_DS_0201_RIBBON_MISSING			0x00000201	//!< 0201 Ribbon missing
#define HITI_DS_0202_RIBBON_MISMATCH		0x00000202	//!< 0202 Ribbon mismatch 01
#define HITI_DS_0203_SECURITY_CHECK_FAIL	0x00000203	//!< 0203 Security check fail
#define HITI_DS_0204_RIBBON_MISMATCH		0x00000204	//!< 0204 Ribbon mismatch 02
#define HITI_DS_0205_RIBBON_MISMATCH		0x00000205	//!< 0205 Ribbon mismatch 03
#define HITI_DS_0300_RIBBON_OUT				0x00000300	//!< 0300 Ribbon out 01
#define HITI_DS_0301_RIBBON_OUT				0x00000301	//!< 0301 Ribbon out 02
#define HITI_DS_0302_PRINTING_FAIL			0x00000302	//!< 0302 Printing fail
#define HITI_DS_0400_PAPER_OUT				0x00000400	//!< 0400 Paper out 01
#define HITI_DS_0401_PAPER_OUT				0x00000401	//!< 0401 Paper out 02
#define HITI_DS_0402_PAPER_NOT_READY		0x00000402	//!< 0402 Paper not ready
#define HITI_DS_0500_PAPER_JAM				0x00000500	//!< 0500 Paper jam 01
#define HITI_DS_0501_PAPER_JAM				0x00000501	//!< 0501 Paper jam 02
#define HITI_DS_0502_PAPER_JAM				0x00000502	//!< 0502 Paper jam 03
#define HITI_DS_0503_PAPER_JAM				0x00000503	//!< 0503 Paper jam 04
#define HITI_DS_0504_PAPER_JAM				0x00000504	//!< 0504 Paper jam 05
#define HITI_DS_0600_PAPER_MISMATCH			0x00000600	//!< 0600 Paper mismatch
#define HITI_DS_0700_CAM_ERROR_01			0x00000700	//!< 0700 Cam error 01
#define HITI_DS_0800_CAM_ERROR_02			0x00000800	//!< 0800 Cam error 02
#define HITI_DS_0900_NVRAM_ERROR			0x00000900	//!< 0900 NVRAM error
#define HITI_DS_1000_IC_CHIP_ERROR			0x00001000	//!< 1000 IC chip error
#define HITI_DS_1200_ADC_ERROR				0x00001200	//!< 1200 ADC error
#define HITI_DS_1300_FW_CHECK_ERROR			0x00001300	//!< 1300 FW check error
#define HITI_DS_1500_CUTTER_ERROR			0x00001500	//!< 1500 Cutter Error




//HiTi defined Paper Type used for setting HITI_JOB_PROPERTY_RT.dwPaperType
#define HITI_PAPER_SIZE_6X4_PHOTO				3		//!< 1844x1240 pixels
#define HITI_PAPER_SIZE_6X4B_PHOTO				4		//!< 1844x1216 pixels
#define HITI_PAPER_SIZE_5X7_PHOTO				6		//!< 1548x2140 pixels
#define HITI_PAPER_SIZE_6X8_PHOTO				9		//!< 1844x2434 pixels
#define HITI_PAPER_SIZE_6X9_PHOTO				12		//!< 1844x2740 pixels
#define HITI_PAPER_SIZE_6X8_PHOTO_SPLIT			14		//!< 1844x2492 pixels
#define HITI_PAPER_SIZE_6X8_PHOTO_2UP			15
#define HITI_PAPER_SIZE_6X4_PHOTO_SPLIT			17
#define HITI_PAPER_SIZE_5X7_PHOTO_2UP			20


//HiTi defined Ribbon Type used for HITI_QueryRibbonInfo
#define HITI_RIBBON_TYPE_4X6					1
#define HITI_RIBBON_TYPE_5X7					2
#define HITI_RIBBON_TYPE_6X8					4


typedef struct tagHITI_JOB_PROPERTY_RT
{
	unsigned long		dwSize;
	unsigned long		dwModelSN;
	unsigned long		dwReserve2;
	unsigned long		dwReserve3;
	unsigned long		dwReserve4;
	unsigned long		dwReserve5;
	unsigned long		dwReserve6;
	unsigned long		dwReserve7;

	short				shCopies;
	short				shReserve1;
	short				shReserve2;
	short				shReserve3;
	short				shReserve4;
	short				shReserve5;
	short				shReserve6;
	short				shReserve7;

	unsigned char		byPrintMode;
	unsigned char		byPaperSize;
	unsigned char		bySplit2up;
	unsigned char		byRibbonType;
	unsigned char		byApplyMatte;
	unsigned char		bySharpenMode;
	unsigned char		byReserve6;
	unsigned char		byReserve7;

	//color settings
	short				shBrightness;
	short				shContrast;
	short				shSaturation;
	short				shRed;
	short				shGreen;
	short				shBlue;
	short				shSharpness;
	short				shGamma;

	unsigned short		wColorTableMode;
	unsigned short		wPageWidth;
	unsigned short		wPageHeight;
	unsigned short		wReserve1;
	unsigned short		wReserve2;
	unsigned short		wReserve3;
	unsigned short		wReserve4;
	unsigned short		wReserve5;


}HITI_JOB_PROPERTY_RT;

#ifdef __cplusplus
}
#endif

#endif //__HTRTAPI_H__
