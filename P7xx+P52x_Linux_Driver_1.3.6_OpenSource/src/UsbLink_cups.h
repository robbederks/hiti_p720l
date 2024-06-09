/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright HiTi Digital, Inc. 2013-2015 All Rights Reserved
**********************************************************/

//////////////////////////////////////////////////////////////////////
#ifndef __USBLINK_H__
#define __USBLINK_H__

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include "mytypes.h"

#include "Util0.h"

#include <cups/cups.h>
#include <cups/sidechannel.h>

#define LOG_BUF_LEN				1024

#define MAX_BLOCK_4K			4096
#define MAX_BLOCK_64K			65536

#define	PIPE_READ				0x01
#define	PIPE_WRITE				0x02
#define	PIPE_INT				0x04

#define ENUM_MODE_ALL			0x00000000
#define ENUM_MODE_PRESENT		0x00000002
#define ENUM_MODE_MATCH_VIDPID	0x00000100
#define ENUM_MODE_MATCH_PORT	0x00000200

#define READ_TIMEOUT			30000
#define WRITE_TIMEOUT			30000
#define INT_TIMEOUT				30000

#define INT_BYTES_AMOUNT		8

//0x10;		//Write OK
//0x13;		//Write pointer is invalid
//0x14;		//Write Pipe is invalid
//0x15;		//WriteFile function error
//0x16;		//WriteFile time out
//0x17:		//WriteFile success, but length = 0, ---> new
//0x18:		//WriteFile success, but length error, ---> new
//0x19:		//WriteFile data CRC error
//0x1A:		//WritePort fail

//0x20;		//Read Ok
//0x21;		//Read header error
//0x22;		//Read length error
//0x23;		//Read pointer is invalid
//0x24;		//Read Pipe is invalid
//0x25;		//ReadFile function error
//0x26;		//ReadFile time out
//0x27:		//ReadFile success, but length = 0, printer has no response
//0x28:		//ReadFile success, but length != need to read, printer has no response
//0x29:		//ReadFile data CRC error
//0x2A:		//ReadPort fail

//0x30;		//Int Ok
//0x33;		//Read Int pointer is invalid
//0x34;		//Read Int Pipe is invalid
//0x35;		//Int ReadFile function error
//0x36;		//Int ReadFile time out

//0x41;		//Pipe direction error
//0x42;		//Pipe not open yet

//0x50;		//Data Loop Back......
//0x51;		//Data_Loop_Back send command O.K
//0x52;		//Data_Loop_Back get response o.k
//0x54;		//Data_Loop_Back match error
//0x55;		//Data_Loop_Back sending command
//0x56;		//Data_Loop_Back getting response
//0x57;		//Data_Loop_Back checking receive data
//0x58;		//Data_Loop_Back match o.k

//0x61;		//Kernel error

//0x71;		//Data lenth error
//0x81;		//Job ID error

//0xFE;		//Create a test file error
//0xFF;		// Allocate memory error


/////////////////////////////////////////////////////////////////////////////
// CUsbLink:
// See UsbLink.cpp for the implementation of this class
//
class CUsbLink
{
// Attributes
public:

	UINT32		m_dwMaxBlock;
	UINT32		m_dwDelayUsb;

	bool				m_bOpen;
	UINT32		m_dwModelSN;

//#ifdef USE_LOG
	UINT32		m_dwLogLevel;
	FILE*				m_fpLog;
//#endif

//public:
	CUsbLink()	{
						//m_dwMaxBlock = 4096;
						m_dwMaxBlock = 8192;
						m_dwDelayUsb = 0;

						m_bOpen = true;
						m_dwModelSN = 0;

					//#ifdef USE_LOG
						m_dwLogLevel = 0;
						m_fpLog = NULL;
					//#endif
				};

	 ~CUsbLink(){

	 				//#ifdef USE_LOG
	 					if ( m_fpLog )
	 					{
							LOG_STRING(1, _T("========================== End ========================\n\n"));
							fclose(m_fpLog);
						}
	 				//#endif
	 			};


// Operations
//public:

	bool		IsOpen(){ return m_bOpen; };

	UINT8		WriteData(const UINT8* lpOutData, UINT32 dwLen, UINT32* lpdwWriteAmount)
				{
					UINT32			dwSize = dwLen;

					UINT8	 	byRet = 0x10;
					UINT32		dwWritten = 0;

					LOG_STRING(2, _T("CUsbLink::WriteData(), dwLen = %d,\n"), dwLen);

					*lpdwWriteAmount = 0;

					//transfer data by small block size
					const UINT8			*lpTmp = NULL;
					long				dwBytesToWrite = 0,
										dwBytesWritten = 0,
										dwWriteLen = 0;

					int					fd = 0;
					struct timeval		tv;
					fd_set				writefds;
					int					ret = 0;

					dwBytesToWrite = dwLen;
					dwWriteLen = m_dwMaxBlock;

					lpTmp = lpOutData;

					while ( dwBytesToWrite > 0 )
					{
						if ( dwBytesToWrite < m_dwMaxBlock )
							dwWriteLen = dwBytesToWrite;

						fd = fileno_unlocked(stdout);
						FD_ZERO(&writefds);
						FD_SET(fd, &writefds);

						tv.tv_sec = 10;
						tv.tv_usec = 0;

						ret = select(fd+1, NULL, &writefds, NULL, &tv);
						LOG_STRING(2, _T("[W0]: select() return = %d,\n"), ret);
						if ( ret == -1 )//error
						{
							LOG_STRING(1, _T("[W0]: select() return = -1,\n"));
							return 0x1A;
						}
						else if ( ret == 0 )//timeout
						{
							LOG_STRING(1, _T("[W0]: select() return = 0,\n"));
							return 0x16;
						}

						dwBytesWritten = 0;

						if ( FD_ISSET(fd, &writefds) )
						{
							dwBytesWritten = fwrite(lpTmp, 1, dwWriteLen, stdout);
							fflush(stdout);
							LOG_STRING(2, _T("CUsbLink::WriteData:: fwrite(), dwWriteLen = %d, dwBytesWritten = %d,\n"), dwWriteLen, dwBytesWritten);

							if ( dwBytesWritten < 0 )
							{
								LOG_STRING(1, _T("CUsbLink::WriteData:: write() return -1, errno = %d,\n"), errno);//ENODEV=19
								return 0x1A;
							}

							if ( dwBytesWritten == dwWriteLen )
								byRet = 0x10;
							else if ( dwWriteLen != 0 && dwBytesWritten == 0 )
								byRet = 0x17;
							else if ( dwWriteLen != 0 && dwWriteLen != dwBytesWritten )
								byRet = 0x18;

							lpTmp += dwBytesWritten;

							dwBytesToWrite -= dwBytesWritten;
							*lpdwWriteAmount += dwBytesWritten;

							if ( dwBytesWritten == 0 && (byRet != 0x10 && byRet != 0x18) )
								break;

							if ( m_dwDelayUsb )
								Sleep(m_dwDelayUsb);
						}//end if ( FD_ISSET(fd, &writefds) )
					}

					//LOG_STRING(2, "[W]:: dwLen = %d, dwSize = %d, result = 0x%x, Cmd = 0x%02X%02X\n", dwLen, dwSize, result, lpOutData[4], lpOutData[5]);

					if ( *lpdwWriteAmount < 257 )
						LOG_BUFFER(1, lpOutData, (*lpdwWriteAmount<257)?(*lpdwWriteAmount):256, 1);
					else
						LOG_STRING(1, _T("OUT> dwDataLen = %d, dwSendRtrn = %d,\n"), dwLen, *lpdwWriteAmount);

					if ( byRet == 0x10 && dwLen > 1*1024*1024 )
						Sleep(1000);

					return byRet;


MYTRACE(_T("CUsbLink::WriteData(), dwLen = %d, dwSize = %d, *lpdwWriteAmount = %d,\n"), dwLen, dwSize, *lpdwWriteAmount);

				};

	UINT8		ReadData(UINT8* lpInData, UINT32 dwLen, UINT32* lpdwReadAmount)
				{
					INT32			nResult = 0;
					UINT32			dwSize = dwLen;

					UINT8	 		byRet = 0x20;
					UINT32			dwReadTotal = 0;
					UINT8			*lpTmp = 0;

					long			dwBytesToRead = 0,
									dwBytesRead = 0,
									dwReadLen = 0;
					long			nRetry = 0;

					*lpdwReadAmount = 0;

					dwBytesToRead = dwLen;
					dwReadLen = m_dwMaxBlock;

					lpTmp = lpInData;
					LOG_STRING(2, "CUsbLink::ReadData(), Length To Read = %d,\n", dwLen);
					while ( dwReadTotal < dwLen )
					{
						if ( dwBytesToRead < m_dwMaxBlock )
							dwReadLen = dwBytesToRead;

						dwBytesRead = cupsBackChannelRead((char*)lpTmp, dwReadLen, 10.0);
						LOG_STRING(2, "[R]:: dwBytesRead = %d, dwReadLen = %d,\n", dwBytesRead, dwReadLen);

						if ( dwBytesRead <= 0 )
							break;

						dwReadTotal += dwBytesRead;
						lpTmp += dwBytesRead;
						dwBytesToRead -= dwBytesRead;
						LOG_STRING(2, "[R]:: dwReadTotal = %d, dwSize = %d, dwBytesRead = %d,\n", dwReadTotal, dwSize, dwBytesRead);
					}

					*lpdwReadAmount = dwReadTotal;

					if ( dwReadTotal == dwLen )
						byRet = 0x20;
					else if ( dwLen != 0 && dwReadTotal == 0 )
						byRet = 0x27;
					else if ( dwLen != 0 && dwLen != dwReadTotal )
						byRet = 0x28;

					LOG_BUFFER(1, lpInData, (*lpdwReadAmount<257)?(*lpdwReadAmount):256, 2);//2010.07.27

					return byRet;

				};


	unsigned long		OpenUsbPrinter()
				{
					unsigned long	 	result = 0x00000111;
					bool				bOpen = false;

					int					nRet = 0;
					unsigned long		VIDPID[10] = {0};
					unsigned long		dwVidPid = 0;

					unsigned char		lpCmd[] = {0xA5, 0x00, 0x03, 0x50, 0x80, 0x09};
					unsigned char		lpBuf[64] = {0};
					unsigned long		dwBuf = 0;
					unsigned char		byRet = 0;
					unsigned long		dwWritten = 0,
										dwRead = 0;


					m_dwModelSN = 0;

					byRet = WriteData(lpCmd, 6, &dwWritten);
					//MYTRACE(_T("m_fd = %d, WriteData return = 0x%02X,\n"), m_fd, byRet);

					if ( byRet == 0x10 )
					{
						byRet = ReadData(lpBuf, 6, &dwRead);
						//MYTRACE(_T("m_fd = %d, ReadData return = 0x%02X,\n"), m_fd, byRet);
					}
					if ( byRet == 0x20 )
					{
						byRet = ReadData(lpBuf+6, 3, &dwRead);
						//MYTRACE(_T("m_fd = %d, ReadData return = 0x%02X,\n"), m_fd, byRet);
					}

					if ( byRet == 0x20 )
					{
						dwVidPid = lpBuf[6];
						dwVidPid <<= 8;
						dwVidPid += lpBuf[7];
						dwVidPid += 0x0D160000;
					}

					/*not work for network
					nRet = ioctl(m_fd, LPIOC_GET_VID_PID(10) , VIDPID);
					MYTRACE(_T("m_fd = %d, ioctl(6) return %d, VIDPID = 0x%04X%04X,\n"), m_fd, nRet, VIDPID[0], VIDPID[1]);
					dwVidPid = (VIDPID[0]<<16) + VIDPID[1];
					*/
					MYTRACE(_T("VIDPID = 0x%08X,\n"), dwVidPid);

					//if ( dwVidPid == 0x0D160009 )
					//	m_dwModelSN = USBMODEL_P720L;



					return dwVidPid;
				};


	UINT8		ReadInt(UINT8*){return 0x20;};
	void		CleanRWIo(){};

	//settings
	void		SetRWTimeOut(UINT32 dwReadTimeOut, UINT32 dwWriteTimeOut){};
	void		SetMaxBlock(UINT32 dwMaxBlock){ m_dwMaxBlock = dwMaxBlock; };
	void		SetDelayUsb(UINT32 dwDelayUsb){ m_dwDelayUsb = dwDelayUsb; };


//#ifdef USE_LOG
	void		SetLogEnable(UINT32 dwLevel, const TCHAR* szFileName, bool bJobBegin=true)
				{
					TCHAR			szFile[512] = {0};
					TCHAR			szFile2[512] = {0};
					bool			bCreateLog = false;
					int				nRet = 0;

					//MYTRACE(_T("Begin CUsbLink::SetLogEnable(), dwLevel = %d, szFileName = \"%s\", m_fpLog = %d,\n"), dwLevel, szFileName, m_fpLog);

					m_dwLogLevel = dwLevel;

					if ( dwLevel )
					{
						if ( !m_fpLog )
						{
							if ( szFileName )
								//_stprintf(szFile, _T("/Library/Printers/HiTi/%s.log"), szFileName);
								_stprintf(szFile, _T("/tmp/%s.log"), szFileName);
							else
								//lstrcpy(szFile, _T("/Library/Printers/HiTi/HppCmd.log"));
								lstrcpy(szFile, _T("/tmp/HppCmd.log"));

							m_fpLog = fopen(szFile, "a");

							if ( m_fpLog )
								bCreateLog = true;
						}

						if ( m_fpLog )
						{
							nRet = fseek(m_fpLog, 0, SEEK_END);

							if ( bCreateLog )
							{
								LOG_STRING(1, _T("========================== Start ========================\n\n"));
							}
						}
					}
					else
					{
						if ( m_fpLog )
						{
							fclose(m_fpLog);
							m_fpLog = NULL;
						}
					}
				};

	void 		LOG_STRING(UINT32 dwLevel, const TCHAR* fmt, ... )
				{
					va_list 		ap;
					UINT32	dwLogWritten = 0;
					TCHAR			szLogText[LOG_BUF_LEN] = {0};

					if ( m_dwLogLevel && m_fpLog && (dwLevel <= m_dwLogLevel) )
					{
						va_start(ap, fmt);
						_vstprintf(szLogText, fmt, ap);
						va_end(ap);

						dwLogWritten = fwrite((void*)szLogText, 1, lstrlen(szLogText), m_fpLog);
						fflush(m_fpLog);
					}
				};

	void		LOG_BUFFER(UINT32 dwLevel, const UINT8* lpBuf, UINT32 dwBuf, int nDirection)//2009.06.19
				{
					UINT32			dwi = 0,
									dwLen = 0,
									dwRemain = 0;
					TCHAR			szTemp[32] = {0},
									szLogText[LOG_BUF_LEN*4] = {0};
					UINT32			dwLogWritten = 0;
					const UINT8		*lpTmp = 0;

					if ( !lpBuf || !m_dwLogLevel || !m_fpLog || (dwLevel > m_dwLogLevel) )//2009.06.18
						return;

					if ( nDirection == 1 )
						lstrcpy(szLogText, _T("OUT> "));
					else if ( nDirection == 2 )
						lstrcpy(szLogText, _T("IN > "));
					else
					lstrcpy(szLogText, _T("--> "));


					if ( dwBuf > LOG_BUF_LEN || dwBuf == 0 )
						lstrcat(szLogText, _T("\n"));

					fseek(m_fpLog, 0, SEEK_END);
					dwLogWritten = fwrite((void*)szLogText, 1, lstrlen(szLogText), m_fpLog);

					lpTmp = lpBuf;
					dwRemain = dwBuf;
					while ( dwRemain > 0 )
					{
						if ( dwRemain < LOG_BUF_LEN )
							dwLen = dwRemain;
						else
							dwLen = LOG_BUF_LEN;
						memset(szLogText, 0, LOG_BUF_LEN*4);

						for(dwi=0;dwi<dwLen;dwi++)
						{
							if ( (dwi > 0) && (dwi % 16 == 0) )
								lstrcat(szLogText, _T("\n     "));
							_stprintf(szTemp, _T("%02X "), lpTmp[dwi]);
							lstrcat(szLogText, szTemp);
						}

						lstrcat(szLogText, _T("\n"));

						dwLogWritten = fwrite((void*)szLogText, 1, lstrlen(szLogText), m_fpLog);

						dwRemain -= dwLen;
						lpTmp += dwLen;

					}//end while ()

					if ( nDirection == 0 )
					{
						lstrcpy(szLogText, _T("\n"));
						dwLogWritten = fwrite((void*)szLogText, 1, lstrlen(szLogText), m_fpLog);
					}

					fflush(m_fpLog);
				};

//#endif

					//exclusive IO functions
					UINT32	EnableMutex(bool bEnableMutex){ return 0;};
					bool	WaitIORight(){ return true;};
					bool	ReleaseIORight(){ return true;};


};


#endif//__USBLINK_H__

