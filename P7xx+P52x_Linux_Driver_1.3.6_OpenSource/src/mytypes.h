/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright 2014-2015 HiTi Digital, Inc.  All Rights Reserved
**********************************************************/
#ifndef __MYTYPES_H__
#define __MYTYPES_H__

#ifndef WIN32
#include <unistd.h>
#include <string.h>

#define FALSE 0
#define TRUE  (!FALSE)

#ifndef _BITMAP_
#define _BITMAP_

typedef struct tagRGBQUAD
{
    unsigned char		rgbBlue;
    unsigned char		rgbGreen;
    unsigned char		rgbRed;
    unsigned char		rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPFILEHEADER
{
    unsigned short		bfType;
    unsigned long		bfSize;
    unsigned short		bfReserved1;
    unsigned short		bfReserved2;
    unsigned long		bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    unsigned long		biSize;
    long				biWidth;
    long				biHeight;
    unsigned short		biPlanes;
    unsigned short		biBitCount;
    unsigned long		biCompression;
    unsigned long		biSizeImage;
    long				biXPelsPerMeter;
    long				biYPelsPerMeter;
    unsigned long		biClrUsed;
    unsigned long		biClrImportant;
} BITMAPINFOHEADER;


typedef struct tagBITMAP {

	long				bmType;
	long				bmWidth;
	long				bmHeight;
	long				bmWidthBytes;
	short				bmPlanes;
	short				bmBitsPixel;
	void*				bmBits;

} BITMAP;

#endif//#define _BITMAP_

#define __stdcall

typedef char				TCHAR;
typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef long				LONG;
typedef unsigned long		DWORD;

typedef long				INT32;
typedef unsigned long		UINT32;
typedef short				INT16;
typedef unsigned short		UINT16;
typedef char				INT8;
typedef unsigned char		UINT8;

typedef char*				LPTSTR;

typedef long				HWND;

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#define min(x, y)			((x) < (y) ? (x) : (y))
#define max(x, y)			((x) > (y) ? (x) : (y))
#define	WORD_ALIGN(x)		((x+1)/2*2)
#define	DWORD_ALIGN(x)		((x+3)/4*4)

#define _T(x)				x
#define lstrcpy				strcpy
#define lstrcat				strcat
#define lstrcpyn			strncpy
#define _tcsicmp			strcasecmp
#define _tcsstr				strstr
#define _stprintf			sprintf
#define _sntprintf			snprintf
#define _vstprintf			vsprintf
#define lstrlen				strlen
#define _tcstoul			strtoul
#define _tcslen				strlen
#define _tfopen				fopen
#define Sleep(x)			usleep(x*1000)

#define _ftprintf			fprintf


#define _tcscmp				strcmp
#define _tstoi				atoi
#define _taccess			access

#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS				0
#endif
#define MAX_PATH					256
#define	ERROR_NOT_SUPPORTED			50
#define	ERROR_PRINT_CANCELLED		63
#define	ERROR_INVALID_PARAMETER		87

#endif//#ifndef WIN32


#endif//__MYTYPES_H__
