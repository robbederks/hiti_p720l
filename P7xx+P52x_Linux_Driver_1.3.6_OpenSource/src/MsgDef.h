/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright HiTi Digital, Inc. 2013-2015 All Rights Reserved
**********************************************************/

#ifndef _MSGDEF_H_
#define _MSGDEF_H_

#ifdef Q_OS_WIN
#include <windef.h>
#include <tchar.h>
#endif

typedef struct tagERR_MSG_UNIT {
	UINT32				dwErrCode;
	const TCHAR*		pErrDesc;
	const TCHAR*		pSubDesc;
} ERR_MSG_UNIT;

static const ERR_MSG_UNIT	MSG_TABLE_ROLL_TYPE[] = {
	{0x00000000, _T(""),											NULL},
	//write error
	{0x00000013, _T("Write pointer is invalid."),					NULL},
	{0x00000014, _T("Write Pipe is invalid."),						NULL},
	{0x00000015, _T("WriteFile function error."),					NULL},
	{0x00000016, _T("WriteFile time out."),							NULL},
	{0x00000017, _T("Printer has no response."),					NULL},//_T("WriteFile length error.");
	{0x00000018, _T("Printer has no response."),					NULL},//_T("WriteFile length error.");
	{0x00000019, _T("Output data CRC error."),						NULL},
	{0x0000001A, _T("Printer has no response."),					NULL},//_T("WritePort fail.");
	//read error
	{0x00000021, _T("Read header error."),							NULL},
	{0x00000022, _T("Read length error."),							NULL},
	{0x00000023, _T("Read pointer is invalid."),					NULL},
	{0x00000024, _T("Read Pipe is invalid."),						NULL},
	{0x00000025, _T("ReadFile function error."),					NULL},
	{0x00000026, _T("ReadFile time out."),							NULL},
	{0x00000027, _T("Printer has no response."),					NULL},//_T("Printer response error.");
	{0x00000028, _T("Printer has no response."),					NULL},//_T("Printer response error.");
	{0x00000029, _T("Printer response data CRC error."),			NULL},
	{0x0000002A, _T("Printer has no response."),					NULL},//ReadPort fail.
	{0x00000042, _T("Read/Write pipe has been closed!"),			NULL},
	{0x00000061, _T("Kernel error."),								NULL},//_T("Printer is off-line!");

	//2007.01.16
	{0x0000274D, _T("Connection refused."),									NULL},
	{0x00000080, _T("Printer is off-line!!"),								NULL},
	{0x11000002, _T("Data format error!"),									_T("This print job will be cancelled.")},
	{0x11000003, _T("Not support to query paper box type."), 				NULL},
	{0x11000004, _T("The track data has wrong character."), 				NULL},
	{0x11000005, _T("Connect magnetic stripe encoding module fail!"), 		NULL},
	{0x11000006, _T("Encoding magnetic stripe fail!"), 						NULL},
	{0x11000007, _T("Memory is not enough!!"), 								NULL},
	{0x11000008, _T("System resource is insufficient to print this page."), _T("Please reboot your system.")},
	{0x11000009, _T("No embeded magnetic encoding module."), 				NULL},
	{0x1100000C, _T("Print data is blank."), 								NULL},

	//2011.06.22
	{0x1100000F, _T("Table is too old."),									_T("Please update printer driver and firmware.")},

	//WarningAlert
	{0x000100FE, _T("Paper roll mismatch!"),						NULL},
	{0x000300FE, _T("Printing system encountered buffer under run."),
				_T("It is recommended to use Direct Printing mode printing your photo.")},//_T("Printing data underrun!");

	{0x000301FE, _T("Command sequence error."),						NULL},
	{0x000302FE, _T("Nand flash unformatted."),						NULL},//1.8.1, 2006.07.17
	{0x000303FE, _T("Nand flash space is not enough."),				NULL},//1.8.1, 2006.07.17
	//{0x000304FE, _T("Heating parameter table incompatible."),		NULL},//2.0.2, 2010.12.10
	{0x000304FE, _T("Heating parameter table incompatible."),		_T("Please update printer driver and firmware.")},//2011.09.07
	{0x000502FE, _T("Dust box needs cleaned."),						NULL},//1.9.9, 2010.06.10

	//DeviceServiceRequiredAlert
	{0x00030001, _T("SRAM error!"),									NULL},
	{0x00030101, _T("Cutter error!"),								NULL},
	{0x00030201, _T("ADC error!"),									NULL},
	{0x00030301, _T("NVRAM R/W error!"),							NULL},
	{0x00030302, _T("Check sum error - SDRAM!"),					NULL},
	{0x00030402, _T("DSP code check sum error!"),					NULL},
	{0x00030501, _T("Cam TPH error!"),								NULL},
	{0x00030601, _T("Cam pinch error!"),							NULL},
	{0x00030701, _T("Firmware write error!"),						NULL},
	//2006.02.08
	{0x00030502, _T("Nvram CRC error!"),							NULL},
	{0x00030602, _T("Check sum error - SRAM!"),						NULL},
	{0x00030702, _T("Check sum error - FLASH!"),					NULL},
	{0x00030802, _T("Check sum error - wrong firmware!"),			NULL},
	{0x00030901, _T("ADC error in slave printer!"),					NULL},
	{0x00030A01, _T("Cam Platen error in slave printer!"),			NULL},
	{0x00030B01, _T("Nvram R/W error in slave printer!"),			NULL},
	{0x00030C02, _T("Nvram CRC error in slave printer!"),			NULL},
	{0x00030D02, _T("Check sum error in slave printer - SDRAM!"),	NULL},
	{0x00030E02, _T("Check sum error in slave printer - SRAM!"),	NULL},
	{0x00030F02, _T("Check sum error in slave printer - FLASH!"),	NULL},
	{0x00031002, _T("Check sum error in slave printer - wrong firmware!"),		NULL},
	{0x00031101, _T("Communication error with slave printer!"),		NULL},
	{0x00031201, _T("Nand flash error."),							NULL},//1.8.1, 2006.07.17
	{0x00031302, _T("Cutter error."),								NULL},//1.9.4, 2009.07.06

	//OperatorInterventionRequiredAlert
	{0x00050001, _T("Cover open/Ribbon cassette door open!"),		_T("Please close the door before continue.")},
	{0x00050101, _T("Cover open/Ribbon cassette door open!"),		_T("Please close the door before continue.")},

	//SuppliesAlerts
	{0x00080004, _T("Ribbon missing!"),								_T("Please put in the ribbon before continue.")},
	{0x00080103, _T("Out of ribbon!"),								_T("Please reload a new ribbon cartridge.")},
	{0x00080104, _T("Out of ribbon!"),								_T("Please reload a new ribbon cartridge.")},
	//2007.05.10
	{0x00080105, _T("Printing fails!!"),							_T("Please reload a new ribbon cartridge.")},
	//{0x000802FE, _T("Ribbon chip error!"),							_T("Please reload a new ribbon cartridge.")},//2004.11.04, 640GOLD
	{0x000802FE, _T("Ribbon error!"),								_T("Please reload a new ribbon cartridge.")},//2004.11.04, 640GOLD
	{0x000803FE, _T("Ribbon is not authenticated yet"), 			_T("Please reload a new ribbon cartridge.")},//1.8.2, 2006.07.17

	//2006.02.08
	{0x00080204, _T("Ribbon missing in slave printer!"),			_T("Please put in the ribbon before continue.")},
	{0x00080303, _T("Ribbon out in slave printer!"),				_T("Please reload a new ribbon cartridge.")},
	{0x00080007, _T("Ribbon is just inserted."),					NULL},
	{0x00080207, _T("Ribbon is just inserted in slave printer."),	NULL},
	{0x000804FE, _T("Ribbon IC R/W error."),						NULL},
	{0x000805FE, _T("Ribbon IC R/W error in slave printer."),		NULL},
	{0x000806FE, _T("Unsupported ribbon."),							NULL},
	{0x000807FE, _T("Unsupported ribbon in slave printer."),		NULL},
	{0x000808FE, _T("Unknown ribbon."),								NULL},
	{0x000809FE, _T("Unknown ribbon in slave printer."),			NULL},

	//JamAlert
	{0x00030000, _T("Paper Jam!"),									NULL},//_T("Please follow the instructions on printer LCD monitor before continuing the print job.")},
	{0x0003000F, _T("Paper Jam!"),									_T("Printer has no response.")},//2007.07.24
	//2006.02.08
	{0x00030100, _T("Paper Jam in paper path of slave printer!"),	_T("Please follow the procedure.")},

	//2009.07.06, v1.9.4
	{0x00030200, _T("Paper Jam in paper path (01)."),				NULL},
	{0x00030300, _T("Paper Jam in paper path (02)."),				NULL},
	{0x00030400, _T("Paper Jam in paper path (03)."),				NULL},
	{0x00030500, _T("Paper Jam in paper path (04)."),				NULL},
	{0x00030600, _T("Paper Jam in paper path (05)."),				NULL},
	{0x00030700, _T("Paper Jam in paper path (06)."),				NULL},
	{0x00030800, _T("Paper Jam in paper path (07)."),				NULL},
	{0x00030900, _T("Paper Jam in paper path (08)."),				NULL},
	{0x00030A00, _T("Paper Jam in paper path (09)."),				NULL},

	//InputAlert
	{0x00008000, _T("Paper out or feeding error."),					_T("Please pull out the paper box and insert again after papers refill or sorting.")},
	//2006.04.21, add for BS400 series
	{0x10008000, _T("Paper out or paper low."),						_T("Please pull out the paper box and insert again after papers refill or sorting.")},
	{0x00008008, _T("Paper box missing!"),							_T("Please put in the paper box before continue.")},
	{0x00000008, _T("Paper box missing!"),							_T("Please put in the paper box before continue.")},
	{0x00008010, _T("Paper roll mismatch!"),						_T("Please put in the correct paper roll before continue.")},
	{0x00080200, _T("Ribbon type mismatch!"),						_T("Please put in the correct ribbon cassette before continue printing.")},


	//2004.07.29, for 641PS, if PictBridge or BlueTooth is linked
//	{0x00007538, _T("Device attached to printer!"),					_T("Please remove the device before continue printing.")},
	{0x00007538, _T("Attention:"),									_T("Please remove any USB digital camera or storage device from the frontal USB port.")},

	{0x00007539, _T("Printer is at Mobile Mode!"),					_T("Please exit Mobile Mode before continue printing.")},

	//2008.05.13, if printer is at standalone mode, do not print
	{0x00007540, _T("Printer is at Standalone Mode!"),				_T("Please exit Standalone Mode before continue printing.")},

	//2010.04.20, add to check FW >= 1.04 for fine mode by CSD request
	{0x00007542, _T("Firmware error."),								_T("Please update firmware to v1.04 or later to support fine mode.")},

	//2010.04.20, add to check FW to support new media type such as 2x6
	{0x00007543, _T("Firmware error."),							_T("Please update firmware to support new media type.")},

	//2010.04.20, add to check FW to support matte
	{0x00007544, _T("Firmware error."),							_T("Please update firmware to v1.10 or later to support matte.")},

	//2010.12.07, add to check P720L FW to use new HAC
	{0x00007545, _T("Firmware error."),							_T("Please update firmware to v0.90 or later.")},

	//2011.01.03, add to check P720L FW >= 0.92 for fine mode by CSD request
	{0x00007546, _T("Firmware error."),								_T("Please update firmware to v0.92 or later to support fine mode.")},

	//2010.03.08, add for P720L
	{0x00000100, _T("0100 Cover open."),						_T("Please close the front door.")},
	{0x00000101, _T("0101 Cover open fail."),					_T("Please press the open button again and reset the error.")},

	{0x00000200, _T("0200 IC chip missing."),					_T("Please insert the smart chip key.")},
	{0x00000201, _T("0201 Ribbon missing."),					_T("Please load the ink ribbon roll.")},
	{0x00000202, _T("0202 Ribbon mismatch 01."),				_T("Please replace the right smart chip key and ribbon roll.")},
	{0x00000203, _T("0203 Security check fail."),				NULL},
	{0x00000204, _T("0204 Ribbon mismatch 02."),				NULL},
	{0x00000205, _T("0205 Ribbon mismatch 03."),				_T("Please replace the right smart chip key and ribbon roll.")},

	{0x00000300, _T("0300 Ribbon out 01."),						_T("Please replace the ink ribbon or check if ribbon roll is normal.")},
	{0x00000301, _T("0301 Ribbon out 02."),						_T("Please replace smart chip key or ribbon/smart chip key.")},
	{0x00000302, _T("0302 Printing fail."),						_T("Please remove jammed paper in printer.")},

	{0x00000400, _T("0400 Paper out 01."),						_T("Please load the paper roll.")},
	{0x00000401, _T("0401 Paper out 02."),						_T("Please replace the paper roll.")},
	{0x00000402, _T("0402 Paper not ready."),					_T("Please reload the paper roll in right direction.")},

	{0x00000500, _T("0500 Paper jam 01."),						_T("Please reload the paper roll.")},
	{0x00000501, _T("0501 Paper jam 02."),						_T("Please remove jammed paper in printer.")},
	{0x00000502, _T("0502 Paper jam 03."),						_T("Please reload the paper roll.")},
	{0x00000503, _T("0503 Paper jam 04."),						_T("Please remove jammed paper in printer.")},
	{0x00000504, _T("0504 Paper jam 05."),						_T("Please reload the paper roll.")},

	{0x00000600, _T("0600 Paper mismatch."),					_T("Please replace the right paper roll size.")},

	{0x00000700, _T("0700 Cam error 01."),						_T("Please turn on the power again. If status remain happen, please contact customer sevices.")},

	{0x00000800, _T("0800 Cam error 02."),						_T("Please turn on the power again. If status remain happen, please contact customer sevices.")},

	{0x00000900, _T("0900 NVRAM error."),						_T("Please turn on the power again. If status remain happen, please contact customer sevices.")},

	{0x00001000, _T("1000 IC chip error."),						_T("Please insert the smart chip key again. If status remain happen, please contact customer sevices.")},

	//{0x00001100, _T("1100 Error."),							NULL},

	{0x00001200, _T("1200 ADC error."),							_T("Please turn on the power again. If status remain happen, please contact customer sevices.")},

	{0x00001300, _T("1300 FW check error."),					_T("Please update FW again. If status remain happen, please contact customer sevices.")},

	//{0x00001400, _T("1400 Error."),							NULL},

	{0x00001500, _T("1500 Cutter error."),						_T("Please turn on the power again. If status remain happen, please contact customer sevices.")},


	//for mac status string, 2006.09.11
	{0x00000001, _T("Processing data...."),						NULL},
	{0x00000002, _T("Sending data...."),						NULL},
	{0x00000003, _T("Printing...."),							NULL},
	{0x00000004, _T("End page...."),							NULL},
	{0x00000005, _T(" "),										NULL},
	{0x00000006, _T("Starting Job..."),							NULL},

	{0x000000A1, _T("Sending YYY...."),							NULL},
	{0x000000A2, _T("Sending MMM...."),							NULL},
	{0x000000A3, _T("Sending CCC...."),							NULL},
	{0x000000A4, _T("Sending OOO...."),							NULL},
	{0x000000A5, _T("Sending KKK...."),							NULL},

	{0xF0000001, _T("Environment temperature is too low."),		NULL},
	{0xF0000002, _T("Environment temperature is too high."),	NULL},

	{0xF0000011, _T(" Test AAAAA... "),							NULL},
	{0xF0000012, _T(" Test BBBBB... "),							NULL},
	{0xF0000013, _T("Before WaitRelinkUsb.... "),				NULL},
	{0xF0000014, _T("After WaitRelinkUsb.... "),				NULL},

	{0xF0000020, _T("waiting to relink Usb...."),				NULL},
	{0xF0000021, _T("waiting to relink Usb....| "),				NULL},
	{0xF0000022, _T("waiting to relink Usb..../ "),				NULL},
	{0xF0000023, _T("waiting to relink Usb....- "),				NULL},
	{0xF0000024, _T("waiting to relink Usb....| "),				NULL},
	{0xF0000025, _T("waiting to relink Usb....\\ "),			NULL},

	{0xFFFFFFFF, _T(""),										NULL},
};


#endif //_MSGDEF_H_
