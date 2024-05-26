/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright HiTi Digital, Inc. 2013-2015 All Rights Reserved
**********************************************************/

#ifndef _UTIL0_H_
#define _UTIL0_H_

#include "mytypes.h"

//#define DEBUG_LOG	1
#define DEBUG_LOG2	1

#if defined(DEBUG_LOG)||defined(DEBUG_LOG2)

void MYTRACE(const TCHAR* lpszFormat, ...);

#else

#define MYTRACE(...)

#endif

bool          ReadBinFile(char* szBinFile, unsigned char** ppBuf, unsigned long* lpdwFileSize);
unsigned long HexToValue(TCHAR* szBuf, unsigned char* lpbyValue);
void          GetErrorString(UINT32 dwInErrCode, TCHAR* szBuf);
bool          SaveToBmpFile(TCHAR* szFileName, BITMAP* lpBmp, UINT32 dwResolution);



#endif //_UTIL0_H_
