/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright HiTi Digital, Inc. 2013-2015 All Rights Reserved
**********************************************************/

#include <stdio.h>

#include "mytypes.h"
#include "Util0.h"
#include "MsgDef.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if defined(DEBUG_LOG)

#include <stdarg.h>

__attribute__((__visibility__("hidden"))) void MYTRACE(const TCHAR* lpszFormat, ...)
{
	static TCHAR			szBuffer[1024] = {0};

	va_list			args;
	int				nBuf = 0;

	va_start(args, lpszFormat);
	nBuf = _vstprintf(szBuffer, lpszFormat, args);
	va_end(args);


	fprintf(stderr, "STATUS: %s", szBuffer);
	fflush(stderr);

}

#elif defined(DEBUG_LOG2)

#include <stdarg.h>

__attribute__((__visibility__("hidden"))) void MYTRACE(const TCHAR* lpszFormat, ...)
{
	static TCHAR			szBuffer[1024] = {0};
	static TCHAR			szFile[512] = {0};
	static TCHAR			szFile2[512] = {0};

	va_list			args;
	int				nBuf = 0;
	FILE			*fpTrace = 0;

	lstrcpy(szFile, _T("/tmp/P750.log"));

	fpTrace = _tfopen(szFile, _T("a+"));

	if ( !fpTrace )
		return;

	nBuf = fseek(fpTrace, 0, SEEK_END);

	va_start(args, lpszFormat);
	nBuf = vsprintf(szBuffer, lpszFormat, args);
	va_end(args);

	nBuf = fseek(fpTrace, 0, SEEK_END);
	_ftprintf(fpTrace, _T("%s"), szBuffer);

	fflush(fpTrace);
	fclose(fpTrace);
}
#endif


//=============================================================================
__attribute__((__visibility__("hidden"))) bool ReadBinFile(char* szBinFile, unsigned char** ppBuf, unsigned long* lpdwFileSize)
{
	FILE				*fpBin = 0;
	unsigned long		dwFileSize = 0;
	unsigned char		*lpBuf = 0;
	int					nRet = 0;
	long				lPos = 0;
	unsigned long		dwRet = 0;
	unsigned long		dwReadOneTime = 0,
						dwTotal = 0,
						dwRead = 0;
	unsigned char		*lpTmp = 0;

	fpBin = fopen(szBinFile, "rb");
	if ( !fpBin )
		return false;

	nRet = fseek(fpBin, 0, SEEK_END);
	lPos = ftell(fpBin);
	dwFileSize = (unsigned long)lPos;

	if ( ppBuf == 0 )
	{
		fclose(fpBin);
		if ( lpdwFileSize )
			*lpdwFileSize = dwFileSize;
		return true;
	}

	if ( *ppBuf == 0 )
		lpBuf = new unsigned char[dwFileSize];
	else
		lpBuf = *ppBuf;

	fseek(fpBin, 0, SEEK_SET);

	dwRet = fread(lpBuf, dwFileSize, 1, fpBin);
	fclose(fpBin);

	*ppBuf = lpBuf;
	*lpdwFileSize = dwFileSize;

	return true;
}

//=============================================================================
__attribute__((__visibility__("hidden"))) unsigned long  HexToValue(TCHAR* szBuf, unsigned char* lpbyValue)
{
	TCHAR				*lpChar = 0;
	unsigned char		byValue = 0;
	unsigned long		dwLen = 0;

	lpChar = szBuf;

	while ( *lpChar != 0 )
	{
		while ( lpChar[0] == ' ' )
			lpChar++;

		if ( *lpChar == 0 )
			return dwLen;

		if ( lpChar[0] >= 'a' && lpChar[0] <= 'f' )
			lpChar[0] = lpChar[0] - 'a' + 'A';

		if ( lpChar[1] >= 'a' && lpChar[1] <= 'f' )
			lpChar[1] = lpChar[1] - 'a' + 'A';

		if ( lpChar[0] >= 'A' && lpChar[0] <= 'F' )
			byValue = (unsigned char)(lpChar[0] - 'A' + 10);
		else if ( lpChar[0] >= '0' && lpChar[0] <= '9' )
			byValue = (unsigned char)(lpChar[0] - '0');
		else
			return dwLen;//ERROR_INPUT;

		byValue <<= 4;
		if ( lpChar[1] >= 'A' && lpChar[1] <= 'F' )
			byValue += (unsigned char)(lpChar[1] - 'A' + 10);
    	else if ( lpChar[1] >= '0' && lpChar[1] <= '9' )
			byValue += (unsigned char)(lpChar[1] - '0');
		else if ( lpChar[1] == 0 || lpChar[1] == ' ' )//2005.08.18
			byValue >>= 4;
		else
			return dwLen;//ERROR_INPUT;

		*lpbyValue = byValue;
		lpbyValue++;
		lpChar+=2;
		dwLen++;
	}//end while

	return dwLen;//HEX_TO_VALUE_OK;
}

//=============================================================================
__attribute__((__visibility__("hidden"))) void GetErrorString(UINT32 dwInErrCode, TCHAR* szBuf)
{
	const ERR_MSG_UNIT	*lpMsgTbl = 0;

	UINT32			i = 0,
					dwIndex = 0;
	UINT32			dwErrCode = dwInErrCode;


	lpMsgTbl = MSG_TABLE_ROLL_TYPE;

	while ( lpMsgTbl[i].dwErrCode != 0xFFFFFFFF )
	{
		if ( lpMsgTbl[i].dwErrCode == dwErrCode )
		{
			dwIndex = i;
			break;
		}
		i++;
	}

	lstrcpy(szBuf, lpMsgTbl[dwIndex].pErrDesc);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
__attribute__((__visibility__("hidden"))) bool  SaveToBmpFile(char* szFileName, BITMAP* lpBmp, UINT32 dwResolution)
{
	BITMAPFILEHEADER	BmpFileHdr = {0};
	BITMAPINFOHEADER	BmpInfoHdr = {0};
	RGBQUAD				IndexColors[256] = {0};

	UINT32				dwTotal = 0,
						dwWriteOneTime = 0;
	size_t				dwWritten = 0;
	bool				bRet = true;
	UINT8				*lpTmp = 0;
	int					nBitCount = 0, i = 0;
	double				fRes = 0.0;
	FILE				*fp = 0;

	UINT8				lpBuf[1024] = {0};
	char				szFullName[256] = {0};

	_stprintf(szFullName, _T("/tmp/%s"), szFileName);

	fRes = (double)dwResolution / 25.4 * 1000;

	BmpFileHdr.bfType		= 0x4D42;//'BM'
	BmpFileHdr.bfSize		= 14;//sizeof(BITMAPFILEHEADER);
	BmpFileHdr.bfReserved1	= 0;
	BmpFileHdr.bfReserved2	= 0;
	BmpFileHdr.bfOffBits	= 54;//sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);// + sizeof(RGBQUAD)*256;

	//MYTRACE(_T("SaveToBmpFile(1), BITMAPFILEHEADER:: bfType = 0x%04X, bfSize = 0x%08X, bfOffBits = 0x%08X,\n"), BmpFileHdr.bfType, BmpFileHdr.bfSize, BmpFileHdr.bfOffBits);

	BmpInfoHdr.biSize			= 40;//sizeof(BITMAPINFOHEADER);
	BmpInfoHdr.biWidth			= lpBmp->bmWidth;
	BmpInfoHdr.biHeight			= lpBmp->bmHeight;
	BmpInfoHdr.biPlanes			= lpBmp->bmPlanes;
	BmpInfoHdr.biBitCount		= lpBmp->bmBitsPixel;
	BmpInfoHdr.biCompression	= 0;//BI_RGB;
	BmpInfoHdr.biSizeImage		= 0;
	BmpInfoHdr.biXPelsPerMeter	= (int)fRes + 1;//0;
	BmpInfoHdr.biYPelsPerMeter	= (int)fRes + 1;//0;
	BmpInfoHdr.biClrUsed		= 0;
	BmpInfoHdr.biClrImportant	= 0;

	//MYTRACE(_T("SaveToBmpFile(2), BITMAPINFOHEADER:: biSize = 0x%08X, biWidth = 0x%08X, biHeight = 0x%08X, biPlanes = 0x%04X, biBitCount = 0x%04X, biCompression = 0x%08X,\n"), BmpInfoHdr.biSize, BmpInfoHdr.biWidth, BmpInfoHdr.biHeight, BmpInfoHdr.biPlanes, BmpInfoHdr.biBitCount, BmpInfoHdr.biCompression);

	nBitCount = BmpInfoHdr.biPlanes * BmpInfoHdr.biBitCount;

	if ( nBitCount == 8 )
	{
		BmpFileHdr.bfOffBits		= 1078;//sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256;

		BmpInfoHdr.biClrUsed		= 256;
		BmpInfoHdr.biClrImportant	= 256;
		for(i=0;i<256;i++)
		{
			IndexColors[i].rgbBlue	= (UINT8)i;
			IndexColors[i].rgbGreen	= (UINT8)i;
			IndexColors[i].rgbRed	= (UINT8)i;
			IndexColors[i].rgbReserved	= (UINT8)i;
		}
	}

	fp = _tfopen(szFullName, _T("wb"));
	if ( fp == 0 )
		return false;//GetLastError();

	lpBuf[0]  = LOBYTE(BmpFileHdr.bfType);
	lpBuf[1]  = HIBYTE(BmpFileHdr.bfType);
	lpBuf[2]  = LOBYTE(LOWORD(BmpFileHdr.bfSize));
	lpBuf[3]  = HIBYTE(LOWORD(BmpFileHdr.bfSize));
	lpBuf[4]  = LOBYTE(HIWORD(BmpFileHdr.bfSize));
	lpBuf[5]  = HIBYTE(HIWORD(BmpFileHdr.bfSize));
	lpBuf[6]  = LOBYTE(BmpFileHdr.bfReserved1);
	lpBuf[7]  = HIBYTE(BmpFileHdr.bfReserved1);
	lpBuf[8]  = LOBYTE(BmpFileHdr.bfReserved2);
	lpBuf[9]  = HIBYTE(BmpFileHdr.bfReserved2);
	lpBuf[10] = LOBYTE(LOWORD(BmpFileHdr.bfOffBits));
	lpBuf[11] = HIBYTE(LOWORD(BmpFileHdr.bfOffBits));
	lpBuf[12] = LOBYTE(HIWORD(BmpFileHdr.bfOffBits));
	lpBuf[13] = HIBYTE(HIWORD(BmpFileHdr.bfOffBits));

	lpBuf[14] = LOBYTE(LOWORD(BmpInfoHdr.biSize));
	lpBuf[15] = HIBYTE(LOWORD(BmpInfoHdr.biSize));
	lpBuf[16] = LOBYTE(HIWORD(BmpInfoHdr.biSize));
	lpBuf[17] = HIBYTE(HIWORD(BmpInfoHdr.biSize));
	lpBuf[18] = LOBYTE(LOWORD(BmpInfoHdr.biWidth));
	lpBuf[19] = HIBYTE(LOWORD(BmpInfoHdr.biWidth));
	lpBuf[20] = LOBYTE(HIWORD(BmpInfoHdr.biWidth));
	lpBuf[21] = HIBYTE(HIWORD(BmpInfoHdr.biWidth));
	lpBuf[22] = LOBYTE(LOWORD(BmpInfoHdr.biHeight));
	lpBuf[23] = HIBYTE(LOWORD(BmpInfoHdr.biHeight));
	lpBuf[24] = LOBYTE(HIWORD(BmpInfoHdr.biHeight));
	lpBuf[25] = HIBYTE(HIWORD(BmpInfoHdr.biHeight));
	lpBuf[26] = LOBYTE(BmpInfoHdr.biPlanes);
	lpBuf[27] = HIBYTE(BmpInfoHdr.biPlanes);
	lpBuf[28] = LOBYTE(BmpInfoHdr.biBitCount);
	lpBuf[29] = HIBYTE(BmpInfoHdr.biBitCount);
	lpBuf[30] = LOBYTE(LOWORD(BmpInfoHdr.biCompression));
	lpBuf[31] = HIBYTE(LOWORD(BmpInfoHdr.biCompression));
	lpBuf[32] = LOBYTE(HIWORD(BmpInfoHdr.biCompression));
	lpBuf[33] = HIBYTE(HIWORD(BmpInfoHdr.biCompression));
	lpBuf[34] = LOBYTE(LOWORD(BmpInfoHdr.biSizeImage));
	lpBuf[35] = HIBYTE(LOWORD(BmpInfoHdr.biSizeImage));
	lpBuf[36] = LOBYTE(HIWORD(BmpInfoHdr.biSizeImage));
	lpBuf[37] = HIBYTE(HIWORD(BmpInfoHdr.biSizeImage));
	lpBuf[38] = LOBYTE(LOWORD(BmpInfoHdr.biXPelsPerMeter));
	lpBuf[39] = HIBYTE(LOWORD(BmpInfoHdr.biXPelsPerMeter));
	lpBuf[40] = LOBYTE(HIWORD(BmpInfoHdr.biXPelsPerMeter));
	lpBuf[41] = HIBYTE(HIWORD(BmpInfoHdr.biXPelsPerMeter));
	lpBuf[42] = LOBYTE(LOWORD(BmpInfoHdr.biYPelsPerMeter));
	lpBuf[43] = HIBYTE(LOWORD(BmpInfoHdr.biYPelsPerMeter));
	lpBuf[44] = LOBYTE(HIWORD(BmpInfoHdr.biYPelsPerMeter));
	lpBuf[45] = HIBYTE(HIWORD(BmpInfoHdr.biYPelsPerMeter));
	lpBuf[46] = LOBYTE(LOWORD(BmpInfoHdr.biClrUsed));
	lpBuf[47] = HIBYTE(LOWORD(BmpInfoHdr.biClrUsed));
	lpBuf[48] = LOBYTE(HIWORD(BmpInfoHdr.biClrUsed));
	lpBuf[49] = HIBYTE(HIWORD(BmpInfoHdr.biClrUsed));
	lpBuf[50] = LOBYTE(LOWORD(BmpInfoHdr.biClrImportant));
	lpBuf[51] = HIBYTE(LOWORD(BmpInfoHdr.biClrImportant));
	lpBuf[52] = LOBYTE(HIWORD(BmpInfoHdr.biClrImportant));
	lpBuf[53] = HIBYTE(HIWORD(BmpInfoHdr.biClrImportant));

	dwWritten = fwrite((void*)lpBuf, 1, 54, fp);

	if ( nBitCount == 8 )
		dwWritten = fwrite((void*)IndexColors, 1, sizeof(RGBQUAD)*256, fp);

	if ( nBitCount == 24 )
	{
		for(i=0;i<lpBmp->bmHeight;i++)
		{
			lpTmp = (UINT8*)lpBmp->bmBits + lpBmp->bmWidthBytes * (lpBmp->bmHeight-1-i);
			dwWritten = fwrite(lpTmp, 1, lpBmp->bmWidthBytes, fp);
		}
	}
	else
	{
	dwTotal = lpBmp->bmWidthBytes * lpBmp->bmHeight * lpBmp->bmPlanes;

	dwWriteOneTime = 1 * 1024 * 1024;//1M Bytes
	lpTmp = (UINT8*)lpBmp->bmBits;

	while ( dwTotal > 0 )
	{
		if ( dwTotal >= dwWriteOneTime )
		{
			dwWritten = fwrite(lpTmp, 1, dwWriteOneTime, fp);
			dwTotal -= dwWriteOneTime;
			lpTmp += dwWriteOneTime;
		}
		else
		{
			dwWritten = fwrite(lpTmp, 1, dwTotal, fp);
			dwTotal = 0;
		}
	}
	}

	fclose(fp);

	return true;
}

