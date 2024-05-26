/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright 2014-2015 HiTi Digital, Inc.  All Rights Reserved
**********************************************************/

//#ifdef WIN32
//#include "stdafx.h"
//#endif

#include "mytypes.h"

#include "HppCmd.h"
#include "Util0.h"

/////////////////////////////////////////////////////////////////////////////
// CHppCmd construction/destruction

CHppCmd::CHppCmd()
{
	m_cpUsbLink = NULL;

	m_dwSendRtrn = 0;
	m_dwRecvRtrn = 0;

	memset(m_RspHeader, 0, 10);
	memset(m_RspData, 0, SIZE_8K);

	m_wRspLen = 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CHppCmd::~CHppCmd()
{

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CHppCmd::SetLogEnable(unsigned long dwLevel, const TCHAR* szFileName, bool bJobBegin)
{
	if ( m_cpUsbLink != (CUsbLink*)NULL )
		m_cpUsbLink->SetLogEnable(dwLevel, (char*)szFileName, bJobBegin);
}

//=============================================================================
unsigned char CHppCmd::SendCommand(unsigned char *lpData, unsigned long dwDataLen)
{
	unsigned char		byRet = 0x61;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	m_wRspLen = 0;
	m_dwRecvRtrn = 0;

	byRet = m_cpUsbLink->WriteData(lpData, dwDataLen, &m_dwSendRtrn);

	m_cpUsbLink->LOG_STRING(2, _T("CHppCmd::SendCommand, byRet = 0x%02X, dwDataLen = %d, m_dwSendRtrn = %d,\n"), byRet, dwDataLen, m_dwSendRtrn);

	return byRet;
}

//=============================================================================
unsigned char CHppCmd::GetResponse()
{
	unsigned char		byRet = 0x61;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	m_cpUsbLink->LOG_STRING(2, _T("Begin CHppCmd::GetResponse\n"));

	m_wRspLen = 0;
	memset(m_RspHeader, 0, 6);
	byRet = 0x20;

	byRet = m_cpUsbLink->ReadData(m_RspHeader, 6, &m_dwRecvRtrn);

	if ( byRet == 0x20 && m_dwRecvRtrn != 6 )
		byRet = 0x22;//Read length error

	m_cpUsbLink->LOG_STRING(2, _T("CHppCmd::GetResponse(1)::return 0x%02X, BytesToRead = 6, m_dwRecvRtrn = %d,\n"), byRet, m_dwRecvRtrn);

	if ( byRet == 0x20 )
	{
		if ( m_RspHeader[0] == 0xA5 )
		{
			m_wRspLen = MAKEWORD(m_RspHeader[2], m_RspHeader[1]) - 3;

			m_cpUsbLink->LOG_STRING(2, _T("CHppCmd::GetResponse(2):: m_wRspLen = %d,\n"), m_wRspLen);

			if ( m_wRspLen == 0 )
			{
				byRet = 0x20;
			}
			else
			{
				byRet = m_cpUsbLink->ReadData(m_RspData, m_wRspLen, &m_dwRecvRtrn);

				if ( m_wRspLen > 0 && m_wRspLen != m_dwRecvRtrn )
					byRet = 0x22;//Read length error

				m_cpUsbLink->LOG_STRING(2, _T("CHppCmd::GetResponse(3)::return 0x%02X, need RspLen = %d, m_dwRecvRtrn = %d,\n"), byRet, m_wRspLen, m_dwRecvRtrn);

				m_wRspLen = m_dwRecvRtrn;
			}
		}
		else
		{
			byRet = 0x21;//Read header error
		}
	}

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CHppCmd::GetRspData(unsigned char** ppBuf, unsigned long* lpdwBuf)
{
	if ( m_wRspLen == 0 )
	{
		if ( ppBuf )
			*ppBuf = NULL;
		if ( lpdwBuf )
			*lpdwBuf = 0;
	}
	else
	{
		if ( ppBuf )
			*ppBuf = m_RspData;
		if ( lpdwBuf )
			*lpdwBuf = m_dwRecvRtrn;
	}
}

//******************************  HPP Command Set  *********************************//
unsigned char CHppCmd::DoCommand0(unsigned short wCmd)
{
	unsigned char		lpCmd[64] = {0};
	unsigned char		byRet = 0x61;
	unsigned short		wLen = 6;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	m_wRspLen = 0;
	memset(lpCmd, 0, 64);
	memcpy(lpCmd, lpCmdHdr, CMD_HDR_LEN);

	lpCmd[4] = HIBYTE(wCmd);
	lpCmd[5] = LOBYTE(wCmd);

	byRet = SendCommand(lpCmd, wLen);

	if ( byRet == 0x10 )
		byRet = GetResponse();

	m_cpUsbLink->LOG_STRING(1, _T("==>DoCommand0(0x%04X):: return = 0x%02X, m_wRspLen = %d,\n\n"), wCmd, byRet, m_wRspLen);

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::DoCommand1(unsigned short wCmd, unsigned char byPara1)
{
	unsigned char		byRet = 0x61;
	unsigned char		lpCmd[8] = {0xA5, 0x00, 0x04, 0x50, 0xE0, 0x00, 0x00, 0x00};
	unsigned short		wLen = 7;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	lpCmd[4] = HIBYTE(wCmd);
	lpCmd[5] = LOBYTE(wCmd);
	lpCmd[6] = byPara1;

	byRet = SendCommand(lpCmd, wLen);

	if ( byRet == 0x10 )
		byRet = GetResponse();

	if ( byRet == 0x20 && wCmd == PCC_RP )
	{
		Sleep(5000);
	}

	m_cpUsbLink->LOG_STRING(1, _T("==>DoCommand1(0x%04X):: return = 0x%02X, byPara1 = %d, m_wRspLen = %d,\n\n"), wCmd, byRet, byPara1, m_wRspLen);

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::DoCommand2(unsigned short wCmd, unsigned char byPara1, unsigned char byPara2)
{
	unsigned char		byRet = 0x61;
	unsigned char		lpCmd[8] = {0xA5, 0x00, 0x05, 0x50, 0xE0, 0x00, 0x00, 0x00};
	unsigned short		wLen = 8;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	lpCmd[4] = HIBYTE(wCmd);
	lpCmd[5] = LOBYTE(wCmd);
	lpCmd[6] = byPara1;
	lpCmd[7] = byPara2;

	byRet = SendCommand(lpCmd, wLen);

	if ( byRet == 0x10 )
		byRet = GetResponse();

	m_cpUsbLink->LOG_STRING(1, _T("==>DoCommand2(0x%04X):: return = 0x%02X, byPara1 = %d, byPara2 = %d, m_wRspLen = %d,\n\n"), wCmd, byRet, byPara1, byPara2, m_wRspLen);

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::DoCommand3(unsigned short wCmd, unsigned char byPara1, unsigned char byPara2, unsigned char byPara3)
{
	unsigned char		byRet = 0x61;
	unsigned char		lpCmd[9] = {0xA5, 0x00, 0x06, 0x50, 0xE0, 0x00, 0x00, 0x00, 0x00};
	unsigned short		wLen = 9;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	lpCmd[4] = HIBYTE(wCmd);
	lpCmd[5] = LOBYTE(wCmd);
	lpCmd[6] = byPara1;
	lpCmd[7] = byPara2;
	lpCmd[8] = byPara3;

	byRet = SendCommand(lpCmd, wLen);

	if ( byRet == 0x10 )
		byRet = GetResponse();

	m_cpUsbLink->LOG_STRING(1, _T("==>DoCommand3(0x%04X):: return = 0x%02X, byPara1 = %d, byPara2 = %d, byPara3 = %d, m_wRspLen = %d,\n\n"), wCmd, byRet, byPara1, byPara2, byPara3, m_wRspLen);

	return byRet;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::DoCmdSendData(unsigned short wCmd, unsigned char* lpData, unsigned short wDataLen, bool bCypressCode)
{
	unsigned char		lpCmd[64] = {0};
	unsigned short		wLen = 0;
	unsigned char		byRet = 0x61;
	unsigned long		dwSendRtrn = 0;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	if ( wDataLen > 0xFFFC )
		return byRet;

	memcpy(lpCmd, lpCmdHdr, CMD_HDR_LEN);

	wLen = wDataLen + 3;

	lpCmd[0] = 0xA5;
	lpCmd[1] = HIBYTE(wLen);
	lpCmd[2] = LOBYTE(wLen);
	lpCmd[3] = 0x50;
	lpCmd[4] = HIBYTE(wCmd);
	lpCmd[5] = LOBYTE(wCmd);

	if(!m_cpUsbLink->WaitIORight())
		return byRet;

	char				szMssg[128] = {0};
	//2016.02.04
	if ( wCmd == EWFN_WCV )
	{
		memcpy(lpCmd+6, lpData, wDataLen);

		sprintf(szMssg,"DoCmdSendData(0x%04X)EWFN_WCV before WriteData %d,\n", wCmd, wDataLen + 6);
		

		byRet = m_cpUsbLink->WriteData(lpCmd, 6+wDataLen, &dwSendRtrn);
		if ( byRet == 0x10 ) {
			byRet = GetResponse();
			sprintf(szMssg,"DoCmdSendData(0x%04X) after GetResponse return = %d,\n", wCmd, byRet);
		}
		else
			sprintf(szMssg,"DoCmdSendData(0x%04X) after SendCommand return = %d,\n", wCmd, byRet);
		

	}
	else
	{
		sprintf(szMssg,"DoCmdSendData(0x%04X) before WriteData 6,\n", wCmd);
		

		byRet = m_cpUsbLink->WriteData(lpCmd, 6, &dwSendRtrn);

		//if ( wCmd != EFD_C_CHS )
		if ( wCmd == EDM_CVD || wCmd == ESD_SD || wCmd == ESD_SHPTC || wCmd == ESD_C_SHPTC )//2016.02.04
		{
			if ( byRet == 0x10 )
			{
				byRet = GetResponse();
				sprintf(szMssg,"DoCmdSendData(0x%04X)_0 after GetResponse return = %d,\n", wCmd, byRet);
				
			}

			Sleep(10);

			if ( byRet == 0x20 ) {

				sprintf(szMssg,"DoCmdSendData(0x%04X)_0 before WriteData %d,\n", wCmd, wDataLen);
				
				byRet = m_cpUsbLink->WriteData(lpData, wDataLen, &dwSendRtrn);


				//Albert add
//				if(wCmd == ESD_SHPTC)
//				{
//					FILE *pFile7;
//					pFile7 = fopen("/sdcard/Android/data/com.hiti.test/files/debug.txt", "a");
//					if (NULL == pFile7) {
//						printf("open failure");
//						return 1;
//					} else {
//						char stringq[100] = {0};
//						sprintf(stringq, "return = %d, wDataLen = %d, dwSendRtrn = %lu", byRet, wDataLen, dwSendRtrn);
//						fwrite(stringq, 1, lstrlen(stringq), pFile7);
//					}
//					fclose(pFile7);
//				}
			}
			Sleep(200);

			if ( byRet == 0x10 )
				byRet = 0x20;
		}
		else
		{
			if ( byRet == 0x10 ) {
				sprintf(szMssg,"DoCmdSendData(0x%04X)_1 before WriteData %d,\n", wCmd, wDataLen);
				
				byRet = m_cpUsbLink->WriteData(lpData, wDataLen, &dwSendRtrn);
			}
			if ( byRet == 0x10 ) {
				byRet = GetResponse();
				sprintf(szMssg,"DoCmdSendData(0x%04X)_1 after GetResponse %d,\n", wCmd, byRet);
				

			}
		}
	}

	m_cpUsbLink->ReleaseIORight();

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::DoCmdGetDataOnlyForERDC_RS(unsigned short wCmd, unsigned char* lpBuffer, unsigned short wBufLen)
{
	unsigned char		byRet = 0x20;

	unsigned char		lpCmd[64] = {0};
	unsigned short		wLen = 6;
	unsigned long		dwBuf = 0,
						dwLen = 0;
	unsigned char		*lpBuf = NULL,
						*lpTmp = NULL;
	byRet = 0x61;

	EXT_DEV_CHAR*		lpExtDevChar = NULL;
	//CALIBRATION_VALUE*	lpCalValue = NULL;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	memset(lpCmd, 0, 64);

	memcpy(lpCmd, lpCmdHdr, CMD_HDR_LEN);
	lpCmd[4] = HIBYTE(wCmd);
	lpCmd[5] = LOBYTE(wCmd);

	if ( wCmd == RDS_RSUS )
	{
		lpCmd[2] = 0x04;
		wLen = 7;
	}

	byRet = SendCommand(lpCmd, wLen);
	if ( byRet == 0x10 )
		byRet = GetResponse();

	if ( byRet == 0x20 )
	{
		GetRspData(&lpBuf, &dwBuf);
		if ( lpBuf )
		{
			switch(wCmd)
			{

				case ERDC_RS://get EXT_DEV_CHAR
					lpExtDevChar = (EXT_DEV_CHAR*)lpBuffer;
					memset(lpExtDevChar, 0, sizeof(EXT_DEV_CHAR));

					lpExtDevChar->byTphSeg				= lpBuf[0];
					lpExtDevChar->wTphPixel				= MAKEWORD(lpBuf[2], lpBuf[1]);
					lpExtDevChar->wMaxHoriUnit			= MAKEWORD(lpBuf[4], lpBuf[3]);

					lpExtDevChar->wMaxVertUnit			= MAKEWORD(lpBuf[6], lpBuf[5]);
					lpExtDevChar->wHoriPrintableUnit	= MAKEWORD(lpBuf[8], lpBuf[7]);
					lpExtDevChar->wVertPrintableUnit	= MAKEWORD(lpBuf[10], lpBuf[9]);
					lpExtDevChar->wVertUnitInstalled	= MAKEWORD(lpBuf[12], lpBuf[11]);

					lpExtDevChar->byYSpeed				= lpBuf[13];
					lpExtDevChar->byMSpeed				= lpBuf[14];
					lpExtDevChar->byCSpeed				= lpBuf[15];
					lpExtDevChar->byOSpeed				= lpBuf[16];
					lpExtDevChar->byOverheatTemp		= lpBuf[17];
					lpExtDevChar->byHeaterOffTemp		= lpBuf[18];
					lpExtDevChar->byHwFeature1			= lpBuf[19];
					lpExtDevChar->byFwFeature1			= lpBuf[20];
					lpExtDevChar->byPreheatTemp			= lpBuf[21];

					lpExtDevChar->byOverheatTempS		= lpExtDevChar->byOverheatTemp;
					lpExtDevChar->byOverheatTempOT		= lpExtDevChar->byOverheatTemp;
					//dwLen = sizeof(EXT_DEV_CHAR);

					if ( dwBuf >= 26 )//sizeof(EXT_DEV_CHAR)-1 )//for 1.5.x
					{
						lpExtDevChar->wVertPrintableUnit2	= MAKEWORD(lpBuf[23], lpBuf[22]);
						lpExtDevChar->wVertPrintableUnit3	= MAKEWORD(lpBuf[25], lpBuf[24]);

						if ( dwBuf >= 27 )//sizeof(EXT_DEV_CHAR) )//for 1.6.0
							lpExtDevChar->byFwFeature2	= lpBuf[26];

						if ( dwBuf >= 28 )//sizeof(EXT_DEV_CHAR) )//for 1.6.2
							lpExtDevChar->byOverheatTempS = lpBuf[27];

						if ( dwBuf >= 29 )//sizeof(EXT_DEV_CHAR) )//for 1.6.4
							lpExtDevChar->byOverheatTempOT = lpBuf[28];
					}
					

				break;

				
			}//end switch(wCmd)
		}
	}
	//else
	//	m_dwLastError = byRet;

	m_cpUsbLink->LOG_STRING(1, _T("==>DoCmdGetData(0x%04X):: return = 0x%02X, m_wRspLen = %d,\n"), wCmd, byRet, m_wRspLen);


	return byRet;
}

/////////////////////////////////////////////////////////////////////////
//check functions
/////////////////////////////////////////////////////////////////////////
unsigned long CHppCmd::CheckDeviceStatus(unsigned long* lpdwErrCode)
{
	unsigned char		byRet = 0x61;
	unsigned long		dwReturn = 0;
	unsigned char*		lpBuf = NULL;
	unsigned long		dwBuf = 0;
	unsigned short		wCmdAlert = 0;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	if ( lpdwErrCode )
		*lpdwErrCode = 0;

	m_cpUsbLink->LOG_STRING(1, _T("==>Begin CheckDeviceStatus()\n"));//2009.06.17

	//RDS-RequestStatusSummary
	byRet = DoCommand0(RDS_RSS);
	//MYTRACE("In CheckDeviceStatus()::after DoCommand0(RDS_RSS), byRet = 0x%02X,\n", byRet);

	if ( byRet != 0x20 )
	{
		if ( lpdwErrCode )
			*lpdwErrCode = byRet;

		return ERROR_PRINT_CANCELLED;
	}

	GetRspData(&lpBuf, &dwBuf);
	if ( !lpBuf )
	{
		return ERROR_PRINT_CANCELLED;
	}

	//m_cpUsbLink->LOG_STRING(1, _T("==>In CheckDeviceStatus, RequestStatusSummary()::return 0x%02X, dwBuf = %d, lpBuf = 0x%08X\n"), byRet, dwBuf, lpBuf);
	//m_cpUsbLink->LOG_BUFFER(1, lpBuf, dwBuf);

	if ( !(lpBuf[2] & 0x0E) && !(lpBuf[1] & 0x0B) && !(lpBuf[0] & 0x64))
		return 0;

	//---------------------------------------------------------------------
	//check Device Alert Byte 2
	if ( lpBuf[2] & 0x02 )//Warnings alert
	{
		wCmdAlert = RDS_RW;
	}
	else if ( lpBuf[2] & 0x04 )//Device service required alert
	{
		wCmdAlert = RDS_DSRA;
	}
	else if ( lpBuf[2] & 0x08 )//Operator intervention required alert
	{
		wCmdAlert = RDS_ROIRA;
	}

	//---------------------------------------------------------------------
	//check Device Alert Byte 1
	else if ( lpBuf[1] & 0x02 )//Paper Jam alert
	{
		wCmdAlert = RDS_RJA;
	}
	else if ( lpBuf[1] & 0x08 )//Input alert
	{
		wCmdAlert = RDS_RIA;
	}
	else if ( lpBuf[1] & 0x01 )//Printing supply alert
	{
		wCmdAlert = RDS_SA;
	}

	//---------------------------------------------------------------------
	else if ( lpBuf[0] & 0x04 )//Data resending request
	{
		dwReturn = HITI_ERROR_NEED_RESEND;
	}

	if ( wCmdAlert )
		dwReturn = CheckDeviceAlert(wCmdAlert, lpdwErrCode);

	m_cpUsbLink->LOG_STRING(1, _T("==>End CheckDeviceStatus(), dwReturn = %d,\n\n"), dwReturn);

	return dwReturn;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned long CHppCmd::CheckDeviceAlert(unsigned short wCmd, unsigned long* lpdwErrCode)
{
	unsigned char		byRet = 0x61,
						byWarnNum = 0,
						byZero = 0;
	unsigned char		*lpBuf = NULL,
						*lpTemp = NULL;
	unsigned long		dwBuf = 0,
						dwErrState = 0;

	bool				bRet = false;

	char				szTextA[36] = {0};
	unsigned char		lpErrorBytes[8] = {0};
	unsigned long		dwLen = 0;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	m_cpUsbLink->LOG_STRING(1, _T("==>In CheckDeviceAlert()\n"));

	byRet = DoCommand0(wCmd);

	if ( byRet != 0x20 )
	{
		if ( lpdwErrCode )
			*lpdwErrCode = byRet;

		return ERROR_PRINT_CANCELLED;
	}

	GetRspData(&lpBuf, &dwBuf);

	//m_cpUsbLink->LOG_STRING(1, _T("==>CheckDeviceAlert(0x%04X)::return 0x%02X, dwBuf = %d, lpBuf = 0x%08X,\n"), wCmd, byRet, dwBuf, lpBuf);
	//m_cpUsbLink->LOG_BUFFER(1, lpBuf, dwBuf);

	//check this alert & show message....
	if ( lpBuf )
	{
		byWarnNum = lpBuf[0];
		lpTemp = lpBuf + 1;

		//--------------------------------
		if ( byWarnNum == 0 )
		{
			if ( lpdwErrCode )
				*lpdwErrCode = 0;
			return ERROR_SUCCESS;
		}
		//--------------------------------

		dwErrState = MAKELONG(MAKEWORD(lpTemp[2], lpTemp[1]), MAKEWORD(lpTemp[0], byZero));
		//MYTRACE("In CHppCmd::CheckDeviceAlert(), dwErrState = 0x%08X,\n", dwErrState);

		if ( dwBuf > 8 )
		{
			memcpy(szTextA, lpBuf + 5, 4);
			dwLen = HexToValue(szTextA, lpErrorBytes);

			if ( dwLen == 2 )
				dwErrState = MAKELONG(MAKEWORD(lpErrorBytes[1], lpErrorBytes[0]), 0);
		}
	}

	if ( lpdwErrCode )
		*lpdwErrCode = dwErrState;

	m_cpUsbLink->LOG_STRING(1, _T("==>End CheckDeviceAlert(), dwErrState = 0x%08X,\n\n"), dwErrState);

	return 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned long CHppCmd::WaitBusy(unsigned long dwSleep, bool bCheckOtherBits)//2006.04.11
{
	unsigned char		byRet = 0x61;
	unsigned char		*lpBuf = NULL;
	unsigned long		dwBuf = 0;

	bool				bBusy = true;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return true;

	m_cpUsbLink->LOG_STRING(1, _T("==>In WaitBusy()\n"));

	while ( 1 )
	{
		byRet = DoCommand0(RDS_RSS);
		if ( byRet != 0x20 )
		{
			return byRet;
		}

		GetRspData(&lpBuf, &dwBuf);

		if ( lpBuf )
		{
			//MYTRACE("In WaitBusy, return 0x%02X, dwBuf = %d, lpBuf = %02X %02X %02X,\n", byRet, dwBuf, lpBuf[0], lpBuf[1], lpBuf[2]);
			if ( bCheckOtherBits )
			{
				if ( (lpBuf[2] & 0x0E) || (lpBuf[1] & 0x0B) || (lpBuf[0] & 0x64) )
					break;
			}

			if ( !(lpBuf[0] & 0x81) )//0x01=Power On, 0x80=Busy
				break;
		}

		Sleep(dwSleep);
	}

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CHppCmd::IsBusy(bool bShowError)
{
	unsigned char		byRet = 0x61;
	unsigned char		*lpBuf = NULL;
	unsigned long		dwBuf = 0;
	bool				bRet = false;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return false;

	m_cpUsbLink->LOG_STRING(1, _T("==>In IsBusy()\n"));

	//m_cpUsbLink->LOG_STRING(2, _T("Begin CHppCmd::IsBusy,\n"));

	byRet = DoCommand0(RDS_RSS);
	if ( byRet != 0x20 )
	{
		return false;
	}

	GetRspData(&lpBuf, &dwBuf);

	//m_cpUsbLink->LOG_STRING(1, _T("In IsBusy, RequestStatusSummary()::return 0x%02X, dwBuf = %d, lpBuf = 0x%08X,\n"), byRet, dwBuf, lpBuf);
	//m_cpUsbLink->LOG_BUFFER(1, lpBuf, dwBuf);

	if ( lpBuf )
	if ( lpBuf[0] & 0x81 )//0x01=Power On, 0x80=Busy
		bRet = true;

	//MYTRACE("In Hpp->IsBusy, lpBuf[0] = 0x%02X,\n", lpBuf[0]);

	return bRet;
}

//=========================================================
// Job Control (JC)
//=========================================================
unsigned char CHppCmd::StartJob(unsigned short *lpwJobID)
{
	unsigned char		lpCmd[9] = {0xA5, 0x00, 0x06, 0x50, 0x05, 0x00, 0x00, 0x00, 0x00};
	unsigned char		byRet = 0x61;
	unsigned char		*lpBuf = 0;
	unsigned long		dwBuf = 0;
	unsigned short		wJobId = 0;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	byRet = SendCommand(lpCmd, 9);

	if ( byRet == 0x10 )
		byRet = GetResponse();

	if ( byRet == 0x20 )
	{
		GetRspData(&lpBuf, &dwBuf);
		if ( lpBuf )
			wJobId = MAKEWORD(lpBuf[2], lpBuf[1]);
	}

	m_cpUsbLink->LOG_STRING(1, _T("==>StartJob:: return = 0x%02X, JobId = 0x%04X,\n\n"), byRet, wJobId);

	if ( lpwJobID )
		*lpwJobID = wJobId;

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::EndJob(unsigned short wJobID)
{
	unsigned char		lpCmd[9] = {0xA5, 0x00, 0x06, 0x50, 0x05, 0x01, 0x00, 0x00, 0x00};
	unsigned char		byRet = 0x61;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	lpCmd[7] = HIBYTE(wJobID);
	lpCmd[8] = LOBYTE(wJobID);

	byRet = SendCommand(lpCmd, 9);

	if ( byRet == 0x10 )
		byRet = GetResponse();

	m_cpUsbLink->LOG_STRING(1, _T("==>EndJob:: return = 0x%02X, JobID = 0x%04X,\n\n"), byRet, wJobID);

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::QueryJobCompleted(unsigned short wJobID)
{
	unsigned char		lpCmd[11] = {0xA5, 0x00, 0x08, 0x50, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
	unsigned char		byRet = 0x61;
	unsigned char		*lpBuf = 0;
	unsigned long		dwBuf = 0;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	if ( wJobID != 0 )
	{
		lpCmd[7] = HIBYTE(wJobID);
		lpCmd[8] = LOBYTE(wJobID);
		lpCmd[10] = 1;//query one job only
	}

	byRet = SendCommand(lpCmd, 11);

	if ( byRet == 0x10 )
		byRet = GetResponse();

	GetRspData(&lpBuf, &dwBuf);

	m_cpUsbLink->LOG_STRING(1, _T("==>QueryJobCompleted:: return = 0x%02X,\n\n"), byRet);

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::QueryJobQueuedActive(unsigned short wJobID, long* lpnJobNum)
{
	unsigned char		lpCmd[9] = {0xA5, 0x00, 0x06, 0x50, 0x05, 0x03, 0x00, 0x00, 0x00};
	unsigned char		byRet = 0x61;
	unsigned char		*lpBuf = 0;
	unsigned long		dwBuf = 0;
	long				nJobNum = 0;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	lpCmd[7] = HIBYTE(wJobID);
	lpCmd[8] = LOBYTE(wJobID);

	byRet = SendCommand(lpCmd, 9);

	if ( byRet == 0x10 )
		byRet = GetResponse();

	GetRspData(&lpBuf, &dwBuf);

	if ( lpBuf )
	{
		nJobNum = lpBuf[0];
		if ( lpnJobNum )
			*lpnJobNum = nJobNum;
	}

	m_cpUsbLink->LOG_STRING(1, _T("==>QueryJobQueuedActive:: return = 0x%02X,\n\n"), byRet);

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::ResumeAllSuspendedJob()
{
	unsigned char		byRet = 0x61;
	int					nJobNum = 0;
	int					i = 0;
	unsigned char		*lpBuf = 0;
	unsigned long		dwBuf = 0;
	unsigned char		*lpTmp = 0;
	unsigned char		byStatus = 0;
	unsigned char		lpJobs[128] = {0};
	unsigned char		lpData[12] = {0};
	bool				bNoError = false;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	//MYTRACE("Begin CHppCmd::ResumeAllSuspendedJob(),\n");
	m_cpUsbLink->LOG_STRING(1, _T("==>In ResumeAllSuspendedJob(),\n"));

	byRet = QueryJobQueuedActive(0);
	if ( byRet != 0x20 )
		return byRet;

	GetRspData(&lpBuf, &dwBuf);
	if ( dwBuf == 0 )
		return 0x20;

	if ( lpBuf[0] == 0 )
		return 0x20;

	memcpy(lpJobs, lpBuf, dwBuf);

	byRet = DoCommand1(PCC_RP, RESET_SOFT);
	if ( byRet != 0x20 )
		return byRet;

	i = 1;
	while ( i <= 1 )
	{
		byRet = DoCommand0(RDS_RSS);
		GetRspData(&lpBuf, &dwBuf);
		//MYTRACE("CHppCmd::ResumeAllSuspendedJob(), RDS_RSS(%d) = 0x%02X, lpBuf = %02X %02X %02X,\n", i, byRet, (lpBuf)?lpBuf[0]:' ', (lpBuf)?lpBuf[1]:' ', (lpBuf)?lpBuf[2]:' ');

		if ( (lpBuf[2] & 0x0E) || (lpBuf[1] & 0x0B) || (lpBuf[0] & 0x64) )
		{
			//return byRet;
			//Sleep(500);
		}
		else if ( byRet != 0x20 )
		{
			break;
		}
		else
		{
			bNoError = true;
			break;
		}

		i++;
	}

	if ( byRet != 0x20 )
		return byRet;

	if ( !bNoError && i > 1 )
		return 0xFF;

	nJobNum = lpJobs[0];
	lpTmp = lpJobs + 1;
	for(i=0;i<nJobNum;i++,lpTmp+=4)
	{
		byStatus = lpTmp[3];

		if ( byStatus == 0x03 )//Suspended
		{
			lpData[0] = 0;//logic unit number
			lpData[1] = lpTmp[1];
			lpData[2] = lpTmp[2];
			byRet = DoCommand3(JC_RSJ, lpData[0], lpData[1], lpData[2]);

			Sleep(500);
		}
	}

	m_cpUsbLink->LOG_STRING(1, _T("==>ResumeAllSuspendedJob:: return = 0x%02X,\n"), byRet);
	//MYTRACE("End CHppCmd::ResumeAllSuspendedJob(),\n");

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::QueryCalibrationValue(unsigned char* lpbyH, unsigned char* lpbyV)
{
	unsigned char		byRet = 0x61;

	unsigned char		*lpBuf = 0;
	unsigned long		dwBuf = 0;
	unsigned char		byH = 0, byV = 0;


	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	byRet = DoCommand0(ERDC_RCC);
	if ( byRet != 0x20 )
		return byRet;

	GetRspData(&lpBuf, &dwBuf);
	if ( lpBuf && dwBuf > 0 )
	{
		byH = lpBuf[0];
		byV = lpBuf[1];
	}

	*lpbyH = byH;
	*lpbyV = byV;

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::ExtSendExtPrintData(unsigned short wStartLineNum, unsigned short wTotalLines, unsigned char *lpData, unsigned long dwDataLen)
{
	unsigned char		lpCmd[11] = {0xA5, 0x00, 0x08, 0x50, 0x83, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00};
	unsigned short		wLenH = 0, wLenL = 0;
	unsigned char		byRet = 0x61;
	unsigned long		dwSendRtrn = 0;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	wLenH = (unsigned short)((dwDataLen + 8)>>16);
	wLenL = (unsigned short)(dwDataLen + 8);

	lpCmd[1] = LOBYTE(wLenH);
	lpCmd[2] = HIBYTE(wLenL);
	lpCmd[6] = LOBYTE(wLenL);

	lpCmd[7] = HIBYTE(wStartLineNum);
	lpCmd[8] = LOBYTE(wStartLineNum);

	lpCmd[9] = HIBYTE(wTotalLines);
	lpCmd[10] = LOBYTE(wTotalLines);

	byRet = m_cpUsbLink->WriteData(lpCmd, 11, &dwSendRtrn);

	if ( byRet == 0x10 )
	{
		Sleep(10);
		byRet = GetResponse();
	}

	if ( byRet == 0x20 )
	{
		//Sleep(200);
		byRet = m_cpUsbLink->WriteData(lpData, dwDataLen, &dwSendRtrn);
		Sleep(200);
	}

	if ( byRet == 0x10 )
		byRet = 0x20;

	m_cpUsbLink->LOG_STRING(1, _T("==>ExtSendExtPrintData:: return = 0x%02X, wStartLineNum = %d, wTotalLines = %d, dwDataLen = %d, dwSendRtrn = %d,\n\n"),
					byRet, wStartLineNum, wTotalLines, dwDataLen, dwSendRtrn);//2009.06.17

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::ExtSublimationFormat(SUBL_FORMAT* lpFormat)
{
	unsigned char		byRet = 0x61;
	unsigned char		lpCmd[6] = {0xA5, 0x00, 0x03, 0x50, 0x81, 0x00};
	unsigned char		lpData[32] = {0};
	unsigned char		*lpTmp = 0;
	unsigned short		wFormatLen = 0,
						wLen = 0;

	SUBL_FORMAT			*lpFormat1 = (SUBL_FORMAT*)lpFormat;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	memset(lpData, 0, 32);
	lpTmp = lpData + 6;

	wFormatLen = 14;

	lpTmp[0]  = lpFormat1->byMediaType;
	lpTmp[1]  = HIBYTE(lpFormat1->wXRes);
	lpTmp[2]  = LOBYTE(lpFormat1->wXRes);
	lpTmp[3]  = HIBYTE(lpFormat1->wYRes);
	lpTmp[4]  = LOBYTE(lpFormat1->wYRes);
	lpTmp[5]  = HIBYTE(lpFormat1->wWidth);
	lpTmp[6]  = LOBYTE(lpFormat1->wWidth);
	lpTmp[7]  = HIBYTE(lpFormat1->wHeight);
	lpTmp[8]  = LOBYTE(lpFormat1->wHeight);
	lpTmp[9]  = lpFormat1->chXOffset;
	lpTmp[10] = lpFormat1->chYOffset;
	lpTmp[11] = lpFormat1->byColorSeq;
	lpTmp[12] = lpFormat1->byCopies;
	lpTmp[13] = lpFormat1->byPrintMode;

	wLen = wFormatLen + 3;

	lpCmd[1] = HIBYTE(wLen);
	lpCmd[2] = LOBYTE(wLen);

	memcpy(lpData, lpCmd, 6);

	byRet = SendCommand(lpData, 6+wFormatLen);

	if ( byRet == 0x10 )
		byRet = GetResponse();

	m_cpUsbLink->LOG_STRING(1, _T("==>ExtSublimationFormat:: return = 0x%02X,\n\n"), byRet);

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::QueryRibbonCount(unsigned long* lpdwRibbonCount)
{
	unsigned char		byRet = 0x61;

	unsigned char		*lpBuf = 0, *lpTmp = 0;
	unsigned char		byType = 0, byNumReport = 0, i = 0;
	unsigned long		dwBuf = 0,
						dwCount = 0;
	bool				bRet = false;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	*lpdwRibbonCount = 0;

	byRet = DoCommand0(RDS_RPS);
	if ( byRet != 0x20 )
		return byRet;

	GetRspData(&lpBuf, &dwBuf);
	if ( lpBuf && dwBuf > 0 )
	{
		byNumReport = lpBuf[0];

		lpTmp = lpBuf + 1;
		for(i=0;i<byNumReport;i++)
		{
			byType = lpTmp[0];

			if ( byType == 0x03 )//Total counter units remaining on current printing supplies
			{
				dwCount = MAKELONG(MAKEWORD(lpTmp[4], lpTmp[3]), MAKEWORD(lpTmp[2], lpTmp[1]));
			}
			lpTmp += 5;
		}
	}

	*lpdwRibbonCount = dwCount;

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::QueryFwVersion(char* szFwVer)
{
	unsigned char		byRet = 0x61;

	unsigned char		*lpBuf = NULL,
						*lpTmp = NULL;
	unsigned long		dwBuf = 0,
						dwLen = 0;
	unsigned char		lpData[12] = {0};
	char				szVer[24] = {0};
	unsigned char		byArea = 0;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	if ( szFwVer == NULL )
		return 0;

	//RDC-RequestSummary
	byRet = DoCommand0(RDC_RS);
	if ( byRet != 0x20 )
		return byRet;

	dwBuf = m_dwRecvRtrn;
	lpBuf = m_RspData;

	if ( lpBuf )
	{
		lpTmp = lpBuf + 33;
		dwLen = lpTmp[0];

		lpTmp = lpTmp + 1 + dwLen;
		dwLen = lpTmp[0];
		memcpy(szVer, lpTmp + 1, dwLen);

		strcpy(szFwVer, szVer);
	}

	//get area
	byRet = DoCommand0(ERDC_RPIDM);
	GetRspData(&lpBuf, &dwBuf);
	if ( lpBuf != NULL )
		byArea = lpBuf[2];

	switch(byArea)
	{
		case 0x11:		strcat(szFwVer, " GB");			break;
		case 0x12:		strcat(szFwVer, " CN");			break;
		case 0x22:		strcat(szFwVer, " CN2");		break;
		case 0x13:		strcat(szFwVer, " NA");			break;
		case 0x14:		strcat(szFwVer, " SA");			break;
		case 0x15:		strcat(szFwVer, " EU");			break;
		case 0x16:		strcat(szFwVer, " IN");			break;
		case 0x17:		strcat(szFwVer, " DB");			break;
	}

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::QuerySerial(char* szSerial, unsigned long* lpdwSize)
{
	unsigned char		byRet = 0x61;
	long				nMax = 0;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	nMax = 14;
	byRet = DoCommand1(ERDC_RSN, 14);
	if ( byRet == 0x20 )
		memcpy(szSerial, m_RspData, 14);

	szSerial[14] = 0;

	*lpdwSize = strlen(szSerial);

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::ExtSendHeatingTable(unsigned char byColor, unsigned char *lpHTblCode, unsigned long dwTblLen)
{
	unsigned char		byRet = 0x61;
	unsigned char		lpCmd[8] = {0xA5, 0x00, 0x00, 0x50, 0x83, 0x03, 0x00, 0x00};
	unsigned long		dwHeadLen = 7;
	unsigned short		wLenH = 0,
						wLenL = 0;
	unsigned long		dwSendRtrn = 0;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	dwHeadLen = 8;

	wLenH = (unsigned short)((dwTblLen + 5)>>16);
	wLenL = (unsigned short)(dwTblLen + 5);

	lpCmd[1] = LOBYTE(wLenH);
	lpCmd[2] = HIBYTE(wLenL);
	lpCmd[5] = 0x04;
	lpCmd[6] = LOBYTE(wLenL);
	lpCmd[7] = byColor;

	MYTRACE(_T("==>ExtSendHeatingTable:: byColor = %d, dwTblLen = %d,\n"), byColor, dwTblLen);


	byRet = m_cpUsbLink->WriteData(lpCmd, dwHeadLen, &dwSendRtrn);
	//MYTRACE(_T("==>ExtSendHeatingTable:: after SendCommand(1) = 0x%02X,\n"), byRet);

	if ( byRet == 0x10 )
		byRet = GetResponse();
	//MYTRACE(_T("==>ExtSendHeatingTable:: after GetResponse(1) = 0x%02X,\n"), byRet);

	Sleep(10);

	if ( byRet == 0x20 )
	{
		byRet = m_cpUsbLink->WriteData(lpHTblCode, dwTblLen, &dwSendRtrn);
		//MYTRACE(_T("==>ExtSendHeatingTable:: after SendCommand(2) = 0x%02X,\n"), byRet);
	}

	Sleep(200);
	if ( byRet == 0x10 )
		byRet = 0x20;

	m_cpUsbLink->LOG_STRING(1, _T("==>ExtSendHeatingTable:: return = 0x%02X, byColor = %d, dwTblLen = %d,\n\n"), byRet, byColor, dwTblLen);//2009.06.17

	return byRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char CHppCmd::DoCmdSendData(unsigned short wCmd, unsigned char* lpData, unsigned short wDataLen)
{
	unsigned char		lpCmd[64] = {0};
	unsigned short		wLen = 0;
	unsigned char		byRet = 0x61;
	unsigned long		dwSendRtrn = 0;

	if ( m_cpUsbLink == (CUsbLink*)NULL )
		return byRet;

	MYTRACE(_T("==>DoCmdSendData(0x%04X):: wDataLen = %d,\n"), wCmd, wDataLen);

	if ( wDataLen > 0xFFFC )
		return byRet;

	memcpy(lpCmd, lpCmdHdr, CMD_HDR_LEN);

	wLen = wDataLen + 3;
	lpCmd[1] = HIBYTE(wLen);
	lpCmd[2] = LOBYTE(wLen);
	lpCmd[4] = HIBYTE(wCmd);
	lpCmd[5] = LOBYTE(wCmd);

	byRet = m_cpUsbLink->WriteData(lpCmd, 6, &dwSendRtrn);
	MYTRACE(_T("==>DoCmdSendData(1):: byRet = 0x%02X, dwSendRtrn = %d,\n"), byRet, dwSendRtrn);

	if ( byRet == 0x10 )
		byRet = GetResponse();
	MYTRACE(_T("==>DoCmdSendData(2):: GetResponse return 0x%02X,\n"), byRet);

	Sleep(10);

	if ( byRet == 0x20 )
	{
		byRet = m_cpUsbLink->WriteData(lpData, wDataLen, &dwSendRtrn);
		MYTRACE(_T("==>DoCmdSendData(3):: byRet = 0x%02X, dwSendRtrn = %d,\n"), byRet, dwSendRtrn);
	}

	Sleep(200);
	if ( byRet == 0x10 )
		byRet = 0x20;


	m_cpUsbLink->LOG_STRING(1, _T("==>DoCmdSendData(0x%04X):: return = 0x%02X, wDataLen = %d,\n\n"), wCmd, byRet, wDataLen);//2009.06.17

	return byRet;
}

