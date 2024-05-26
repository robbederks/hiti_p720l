/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright HiTi Digital, Inc. 2014-2015 All Rights Reserved
**********************************************************/

//#ifdef WIN32
//#include "stdafx.h"
//#endif

#include "mytypes.h"
#include "HTRTApi.h"

#include "Util0.h"

#include "OneJob.h"

#include "HTSharpen.h"
#include "TableMaping.h"

#include "ClrCvt.h"


COneJob::COneJob()
{
	memset(&m_JobProp, 0, sizeof(HITI_JOB_PROPERTY_RT));

	memset(&m_SublFormat, 0, sizeof(SUBL_FORMAT));

	memset(&m_Bmp1, 0, sizeof(BITMAP));
	memset(&m_Bmp2, 0, sizeof(BITMAP));
	memset(&m_BmpFinal, 0, sizeof(BITMAP));

	m_Usb = new CUsbLink();

	m_dwDrvModel = USBMODEL_P520L;

	MYTRACE("COneJob(0), default m_dwDrvModel = %d,\n", m_dwDrvModel);

	if(m_Usb)
	{
		unsigned long dwVidPid = m_Usb->OpenUsbPrinter();

		if ( dwVidPid == 0x0D160009 )
			m_dwDrvModel = USBMODEL_P720L;
		else if ( dwVidPid == 0x0D160502 )
			m_dwDrvModel = USBMODEL_P520L;
		else if ( dwVidPid == 0x0D16050E )
			m_dwDrvModel = USBMODEL_P525N;
		else if ( dwVidPid == 0x0D160501 )
			m_dwDrvModel = USBMODEL_P750L;

		MYTRACE("COneJob(0.1), m_dwDrvModel = %d,\n", m_dwDrvModel);
	}
	m_Hpp = new CHppCmd();

	m_Hpp->SetUsbLink(m_Usb);

//for debug
//-------------------
//m_Hpp->SetLogEnable(1, "P720_usb", true);
//-------------------

	m_wPageReceived = 0;
	m_wRbnVendor = 0;

	m_bAllocFinalBmp = false;
}

COneJob::~COneJob()
{
	if ( m_Usb )
		delete m_Usb;

	if ( m_Hpp )
		delete m_Hpp;

	if ( m_bAllocFinalBmp )
		delete [] (UINT8*)m_BmpFinal.bmBits;
}

UINT32 COneJob::SendOnePageData(HITI_JOB_PROPERTY_RT* lpJobPropIn, BITMAP* lpBmpIn, int nCopies, int nPageNum)
{
	bool				bRet = true;
	UINT32				dwRet = 0;

	int					nWidth = 0,
						nHeight = 0,
						nHalfHeight = 0,
						nSize = 0,
						nOffsetHori = 0,
						i = 0;

	UINT8				*lpSrc = 0,
						*lpDst = 0;
	UINT8				*lpSrcTmp = 0,
						*lpDstTmp = 0;

	m_wPageReceived++;

	memcpy(&m_JobProp, lpJobPropIn, sizeof(HITI_JOB_PROPERTY_RT));
	memcpy(&m_Bmp1, lpBmpIn, sizeof(BITMAP));

	MYTRACE("SendOnePageData(0), m_wPageReceived = %d, m_JobProp.byPaperSize = %d,\n", m_wPageReceived, m_JobProp.byPaperSize);
	MYTRACE("BmpIn:: bmWidth = %d, bmHeight = %d, bmWidthBytes = %d,\n", m_Bmp1.bmWidth, m_Bmp1.bmHeight, m_Bmp1.bmWidthBytes);
	MYTRACE("BmpIn:: bmPlanes = %d, bmBitsPixel = %d, bmBits = %p,\n", m_Bmp1.bmPlanes, m_Bmp1.bmBitsPixel, m_Bmp1.bmBits);

	if ( m_Bmp1.bmWidth == 0 || m_Bmp1.bmHeight == 0 || m_Bmp1.bmBits == 0 || m_Bmp1.bmBitsPixel != 24 )
		return 87;//ERROR_INVALID_PARAMETER

	m_JobProp.shCopies = (short)nCopies;
	//m_dwDrvModel = USBMODEL_P520L;
	//m_dwDrvModel = USBMODEL_P720L;

	//
	if ( HITI_PAPER_SIZE_6X8_PHOTO_2UP == m_JobProp.byPaperSize )
	{
		nWidth = 1844;
		nHeight = 2492;
		nHalfHeight = 1240;
	}
	else if ( HITI_PAPER_SIZE_5X7_PHOTO_2UP == m_JobProp.byPaperSize )
	{
		nWidth = 1844;
		nHeight = 2152;
		nHalfHeight = 1072;
	}
	else if ( HITI_PAPER_SIZE_5X7_PHOTO == m_JobProp.byPaperSize )
	{
		nWidth = 1844;
		nHeight = 2140;
	}

	if ( m_wPageReceived == 1
		&&	(HITI_PAPER_SIZE_6X8_PHOTO_2UP == m_JobProp.byPaperSize
			|| HITI_PAPER_SIZE_5X7_PHOTO_2UP == m_JobProp.byPaperSize
			|| HITI_PAPER_SIZE_5X7_PHOTO == m_JobProp.byPaperSize)
		)
	{
	nSize = nWidth * nHeight * 3;

	m_BmpFinal.bmType		= 0x5250;
	m_BmpFinal.bmHeight		= nHeight;
	m_BmpFinal.bmWidthBytes	= nWidth * 3;
	m_BmpFinal.bmWidth		= nWidth;
	m_BmpFinal.bmPlanes		= 1;
	m_BmpFinal.bmBitsPixel	= 24;

	if ( !m_bAllocFinalBmp )
		m_BmpFinal.bmBits		= new UINT8[nSize];

	memset(m_BmpFinal.bmBits, 0xFF, nSize);

	m_bAllocFinalBmp = true;

	MYTRACE("SendOnePageData(0a), allocate m_BmpFinal, bmWidth = %d, bmHeight = %d,\n", m_BmpFinal.bmWidth, m_BmpFinal.bmHeight);
	}

	if ( HITI_PAPER_SIZE_6X8_PHOTO_2UP == m_JobProp.byPaperSize )
	{
		lpSrc = (UINT8*)m_Bmp1.bmBits;
		nSize = nWidth * 3 * nHalfHeight;

		if ( (m_wPageReceived % 2) == 1 )
			lpDst = (UINT8*)m_BmpFinal.bmBits + nWidth * 3 * (nHeight-nHalfHeight);
		else
			lpDst = (UINT8*)m_BmpFinal.bmBits;

		memcpy(lpDst, lpSrc, nSize);

		if ( (m_wPageReceived % 2) == 1 )
			return 0;
	}
	else if ( HITI_PAPER_SIZE_5X7_PHOTO_2UP == m_JobProp.byPaperSize )
	{
		lpSrc = (UINT8*)m_Bmp1.bmBits;
		nOffsetHori = (nWidth - m_Bmp1.bmWidth) / 2;

		if ( (m_wPageReceived % 2) == 1 )
			lpDst = (UINT8*)m_BmpFinal.bmBits + nWidth * 3 * (nHeight-nHalfHeight);
		else
			lpDst = (UINT8*)m_BmpFinal.bmBits;

		lpDst += nOffsetHori * 3;

		for(i=0;i<nHalfHeight;i++)
		{
			lpSrcTmp = lpSrc + m_Bmp1.bmWidthBytes * i;
			lpDstTmp = lpDst + m_BmpFinal.bmWidthBytes * i;
			memcpy(lpDstTmp, lpSrcTmp, m_Bmp1.bmWidthBytes);
		}

		if ( (m_wPageReceived % 2) == 1 )
			return 0;
	}
	else if ( HITI_PAPER_SIZE_5X7_PHOTO == m_JobProp.byPaperSize )
	{
	lpSrc = (UINT8*)m_Bmp1.bmBits;
	nOffsetHori = (nWidth - m_Bmp1.bmWidth) / 2;

	lpDst = (UINT8*)m_BmpFinal.bmBits + nOffsetHori * 3;

	for(i=0;i<nHeight;i++)
	{
		lpSrcTmp = lpSrc + m_Bmp1.bmWidthBytes * i;
		lpDstTmp = lpDst + m_BmpFinal.bmWidthBytes * i;
		memcpy(lpDstTmp, lpSrcTmp, m_Bmp1.bmWidthBytes);
	}
	}
	else
	{
		memcpy(&m_BmpFinal, &m_Bmp1, sizeof(BITMAP));
	}

	dwRet = PrintOnePage(nCopies);
	if ( dwRet != 0 )
		return dwRet;

	return dwRet;
}

UINT32 COneJob::EndJob()
{
	bool				bRet = true;
	UINT32				dwRet = 0;

	if ( HITI_PAPER_SIZE_6X8_PHOTO_2UP == m_JobProp.byPaperSize
		|| HITI_PAPER_SIZE_5X7_PHOTO_2UP == m_JobProp.byPaperSize
		)
	{
		if ( (m_wPageReceived % 2) == 0 )
			return 0;
	}
	else
	{
		return 0;
	}

	dwRet = PrintOnePage(m_JobProp.shCopies);

	return dwRet;
}

UINT32 COneJob::CheckPrinterStatus()
{
	UINT32				dwRet = 0;
	UINT32				dwStatus = 0;

	bool				bBusy = true;

	while ( bBusy )
	{
		dwRet = m_Hpp->CheckDeviceStatus((unsigned long*)&dwStatus);
		MYTRACE("CheckPrinterStatus(1), dwStatus = %d,\n", dwStatus);

		if ( dwRet != 0 )
			break;

		bBusy = m_Hpp->IsBusy(false);
		MYTRACE("CheckPrinterStatus(2), bBusy = %d,\n", bBusy);

		if ( !bBusy )
			break;

		Sleep(1000);
	}

	return dwStatus;
}

UINT32 COneJob::CheckRealRibbon()
{
	UINT8				byRet = 0;

	UINT8				*lpData = 0;
	UINT32				dwDataLen = 0;

	UINT8				byNeedRibbon = 0;
	UINT8				byRealRibbon = 0;
	UINT32				dwRibbonErr = 0;

	UINT16				wRibbonVendor = 0;
	UINT16				wCmdRRVC = ERDC_RRVC;

	UINT8				i = 0, byNumReport = 0, byType = 0;
	UINT32				dwRemainCount = 0;
	UINT8				*lpTmp = 0;


	switch(m_JobProp.byPaperSize)
	{
		case HITI_PAPER_SIZE_6X4_PHOTO:
		case HITI_PAPER_SIZE_6X4B_PHOTO:
		case HITI_PAPER_SIZE_6X4_PHOTO_SPLIT:
				byNeedRibbon = 0x01;
				break;

		case HITI_PAPER_SIZE_5X7_PHOTO:
		case HITI_PAPER_SIZE_5X7_PHOTO_2UP:
				byNeedRibbon = 0x02;
				break;

		case HITI_PAPER_SIZE_6X8_PHOTO_SPLIT:
		case HITI_PAPER_SIZE_6X8_PHOTO_2UP:
		case HITI_PAPER_SIZE_6X8_PHOTO:
				byNeedRibbon = 0x04;
				break;
	}

	MYTRACE("CheckRealRibbon(0), m_JobProp.byPaperSize = %d, byNeedRibbon = %d,\n", m_JobProp.byPaperSize, byNeedRibbon);

	byRet = m_Hpp->DoCommand0(RDS_RSUS);
	if ( byRet == 0x20 )
	{
		m_Hpp->GetRspData(&lpData, (unsigned long*)&dwDataLen);
		if ( lpData )
			byRealRibbon = lpData[2] & 0x7F;
	}

	MYTRACE("CheckRealRibbon(1), byRealRibbon = %d, byNeedRibbon = %d,\n", byRealRibbon, byNeedRibbon);

	if ( byNeedRibbon == 0x04 && (byRealRibbon == 0x03 || byRealRibbon == 0x04) )
		dwRibbonErr = 0;
	else if ( byRealRibbon == 0x00 )
		dwRibbonErr = 0x00080004;//ribbon missing
	else if ( byRealRibbon != byNeedRibbon )
		dwRibbonErr = 0x00080200;


	byRet = m_Hpp->DoCommand0(wCmdRRVC);
	if ( byRet == 0x20 )
	{
		m_Hpp->GetRspData(&lpData, &dwDataLen);
		if ( dwDataLen == 2 )
		{
			wRibbonVendor = lpData[0];
			wRibbonVendor <<= 8;
			wRibbonVendor += lpData[1];
		}
	}

	m_wRbnVendor = wRibbonVendor;
	MYTRACE("CheckRealRibbon(2), wRibbonVendor = 0x%04X,\n", wRibbonVendor);

	byRet = m_Hpp->DoCommand0(RDS_RPS);
	if ( byRet == 0x20 )
	{
		m_Hpp->GetRspData(&lpData, (unsigned long*)&dwDataLen);
		if ( lpData )
		{
			byNumReport = lpData[0];

			lpTmp = lpData + 1;
			for(i=0;i<byNumReport;i++)
			{
				byType = lpTmp[0];

				if ( byType == 0x03 )
				{
					dwRemainCount = MAKELONG(MAKEWORD(lpTmp[4], lpTmp[3]), MAKEWORD(lpTmp[2], lpTmp[1]));
				}

				lpTmp += 5;
			}
		}
	}

	MYTRACE("CheckRealRibbon(3), dwRemainCount = %d,\n", dwRemainCount);

	if ( dwRibbonErr == 0 && dwRemainCount == 0 )
		dwRibbonErr = 0x00080103;

	return dwRibbonErr;
}

UINT32 COneJob::CheckRealPaper()
{
	UINT32				dwError = 0;
	UINT8				byRet = 0;

	UINT8				*lpData = 0;
	UINT32				dwDataLen = 0;

	UINT8				byNeedPaper = 0;
	UINT8				byRealPaper = 0;

	if ( m_JobProp.byPaperSize == HITI_PAPER_SIZE_5X7_PHOTO
		|| m_JobProp.byPaperSize == HITI_PAPER_SIZE_5X7_PHOTO_2UP )
		byNeedPaper = 2;
	else
		byNeedPaper = 1;

	byRet = m_Hpp->DoCommand0(RDS_RIS);
	m_Hpp->GetRspData(&lpData, &dwDataLen);
	if ( lpData )
		byRealPaper = lpData[1];

	MYTRACE("CheckRealPaper(1), byRealPaper = %d,\n", byRealPaper);

	if ( 0 == byRealPaper )
		dwError = 0x00008008;//paper box missing
	else if ( byRealPaper != byNeedPaper )
		dwError = 0x00008010;//Paper tray mismatch

	return dwError;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT32 COneJob::ApplyTableMap(BITMAP* lpBmpSrc, BITMAP* lpBmpDst)
{
	UINT32				dwRet = 0;
	bool				bSucc = false;
	CTableMaping		TableMap;

	//swap RGB to BGR
	UINT8				*lpTmp = 0;
	UINT8				byTmp = 0;
	int					i = 0, k = 0;

	for(i=0;i<lpBmpSrc->bmHeight;i++)
	{
		lpTmp = (UINT8*)lpBmpSrc->bmBits + lpBmpSrc->bmWidthBytes * i;
		for(k=0;k<lpBmpSrc->bmWidth;k++)
		{
			byTmp = *lpTmp;
			*lpTmp = *(lpTmp+2);
			*(lpTmp+2) = byTmp;

			lpTmp += 3;
		}
	}


	TableMap.SetBrightTable(m_JobProp.shBrightness);
	TableMap.SetContrastTable(m_JobProp.shContrast);
	TableMap.SetColorBalanceTable(m_JobProp.shRed, 0);
	TableMap.SetColorBalanceTable(m_JobProp.shGreen, 1);
	TableMap.SetColorBalanceTable(m_JobProp.shBlue, 2);
	TableMap.SetGammaTableByfloat(m_JobProp.shGamma);
	MYTRACE(_T("In ApplyTableMap(), shBrightness = %d, shContrast = %d, shRed = %d, shGreen = %d, shBlue = %d, shGamma = %d,\n"), m_JobProp.shBrightness, m_JobProp.shContrast, m_JobProp.shRed, m_JobProp.shGreen, m_JobProp.shBlue, m_JobProp.shGamma);

	bSucc = TableMap.MapTableImage(*lpBmpSrc, *lpBmpDst);
	MYTRACE(_T("In ApplyTableMap(), after MapTableImage(), bSucc = %d,\n"), bSucc);

	if(m_JobProp.shSaturation != 0)//2021.09.23 added by Bill to avoid applying saturatation at 0
	{
		TableMap.SetSaturateTable(m_JobProp.shSaturation);
		bSucc = TableMap.ApplySaturate(*lpBmpSrc, *lpBmpDst);
		MYTRACE(_T("In ApplyTableMap(), after ApplySaturate(), bSucc = %d,\n"), bSucc);
	}
	return dwRet;
}

UINT32 COneJob::PrintOnePage(int nCopies)
{
	UINT32				dwRet = 0;
	UINT32				dwSize = 0;
	UINT8				*lpTmp = 0;

	UINT8				byRet = 0;
	SUBL_FORMAT			SublFormat = {0};
	UINT16				wJobId = 0;
	UINT8				byH = 0, byV = 0;
	INT16				shH = 0,
						shV = 0,
						shTemp = 0,
						shH_max = 11,
						shV_max = 8,
						shH_mid = 6,
						shV_mid = 5,
						shStepH = 4,
						shStepV = 4;

	UINT32				dwTphWidth = 1844;
	UINT32				dwSrcHeight = 0;
	UINT8				*lpDataYMC = 0;
	bool				bRet = false;

	//test
	char				szRawFile[256] = {0};

	//0. prepare format
	SublFormat.wXRes			= 300;
	SublFormat.wYRes			= 300;
	SublFormat.wWidth			= m_BmpFinal.bmWidth;
	SublFormat.wHeight			= m_BmpFinal.bmHeight;
	SublFormat.chXOffset		= 0;
	SublFormat.chYOffset		= 0;
	SublFormat.byMediaType		= 0;
	SublFormat.byColorSeq		= 0x87;
	SublFormat.byCopies			= (UINT8)nCopies;
	SublFormat.byPrintMode		= 0x08;

	//always applied fine mode for P750 and P720
	if ( m_dwDrvModel == USBMODEL_P750L || m_dwDrvModel == USBMODEL_P720L)
		m_JobProp.byPrintMode = 1;

	if ( 0x01 == m_JobProp.byPrintMode )//fine mode
		SublFormat.byPrintMode |= 0x02;

	if ( 1 == m_JobProp.byApplyMatte )
		SublFormat.byColorSeq |= 0xC0;

	SublFormat.byCopies = (UINT8)m_JobProp.shCopies;

	MYTRACE("PrintOnePage(1), m_JobProp.byPaperSize = %d,\n", m_JobProp.byPaperSize);

	switch(m_JobProp.byPaperSize)
	{
		case HITI_PAPER_SIZE_6X4_PHOTO:
				SublFormat.byMediaType = FW_MEDIA_PHOTO_4X6;
				SublFormat.wHeight = 1240;//4x6
				break;
		case HITI_PAPER_SIZE_6X4B_PHOTO:
				SublFormat.byMediaType = FW_MEDIA_PHOTO_4X6;
				SublFormat.wHeight = 1216;//4x3.94
				break;

		case HITI_PAPER_SIZE_6X9_PHOTO:
				SublFormat.byMediaType = FW_MEDIA_PHOTO_6X9;
				SublFormat.wHeight = 2740;
				break;

		case HITI_PAPER_SIZE_6X8_PHOTO_SPLIT:
		case HITI_PAPER_SIZE_6X8_PHOTO_2UP:
				SublFormat.byMediaType = FW_MEDIA_PHOTO_6X9_2UP;
				SublFormat.wHeight = 2492;
				break;

		case HITI_PAPER_SIZE_6X8_PHOTO:
				SublFormat.byMediaType = FW_MEDIA_PHOTO_6X8;
				SublFormat.wHeight = 2434;
				break;

		case HITI_PAPER_SIZE_5X7_PHOTO:
				SublFormat.byMediaType = FW_MEDIA_PHOTO_5X7;
				SublFormat.wHeight = 2140;
				break;

		case HITI_PAPER_SIZE_5X7_PHOTO_2UP:
				SublFormat.byMediaType = FW_MEDIA_PHOTO_5X7_2UP;
				SublFormat.wHeight = 2152;
				break;

		case HITI_PAPER_SIZE_6X4_PHOTO_SPLIT:
				SublFormat.byMediaType = FW_MEDIA_PHOTO_6X4_2UP;
				SublFormat.wHeight = 1248;//1240; //2017.01.18 changed by Bill
				break;
	}

//-------------------
	/*FILE				*fp = 0;

	size_t				dwWritten = 0;

	sprintf(szRawFile, "/tmp/org%dx%d.raw", (int)m_BmpFinal.bmWidth, (int)m_BmpFinal.bmHeight);

	fp = _tfopen(szRawFile, "wb");

	dwWritten = fwrite(m_BmpFinal.bmBits, 1, dwTphWidth * SublFormat.wHeight * 3, fp);

	fclose(fp);*/
//-------------------

	//color
	dwRet = ApplyTableMap(&m_BmpFinal, &m_BmpFinal);
	MYTRACE("PrintOnePage(1), ApplyTableMap() return %d,\n", dwRet);


//-------------------
	/*//FILE				*fp = 0;

	//size_t				dwWritten = 0;

	sprintf(szRawFile, "/tmp/Map%dx%d.raw", (int)m_BmpFinal.bmWidth, (int)m_BmpFinal.bmHeight);

	fp = _tfopen(szRawFile, "wb");

	dwWritten = fwrite(m_BmpFinal.bmBits, 1, dwTphWidth * SublFormat.wHeight * 3, fp);

	fclose(fp);

	//return 0;*/

//-------------------

	dwRet = SharpenImage((unsigned char*)m_BmpFinal.bmBits, (unsigned char*)m_BmpFinal.bmBits, m_BmpFinal.bmWidthBytes, m_BmpFinal.bmWidth, m_BmpFinal.bmHeight, m_JobProp.shSharpness, m_BmpFinal.bmBitsPixel);
	MYTRACE("PrintOnePage(1), SharpenImage() return %d,\n", dwRet);

//-------------------
/*	sprintf(szRawFile, "P%dx%d.bmp", (int)m_BmpFinal.bmWidth, (int)m_BmpFinal.bmHeight);
	SaveToBmpFile(szRawFile, &m_BmpFinal, 300);
	return 0;*/
//-------------------

	dwRet = CheckPrinterStatus();
	if ( dwRet != 0 )
		return dwRet;

	dwRet = CheckRealRibbon();
	if ( dwRet != 0 )
		return dwRet;

	dwRet = CheckRealPaper();
	if ( dwRet != 0 )
		return dwRet;


	//
	dwSrcHeight = SublFormat.wHeight;
	dwSize = dwTphWidth * dwSrcHeight * 3;
	lpDataYMC = new unsigned char[dwSize];

	memset(lpDataYMC, 0, dwSize);

	bool bNewTPH = false;
	if(m_dwDrvModel == USBMODEL_P520L || m_dwDrvModel == USBMODEL_P525N) {
                EXT_DEV_CHAR		ExtDevChar = {0};
                m_Hpp->DoCmdGetDataOnlyForERDC_RS(ERDC_RS, (UINT8 *) &ExtDevChar, sizeof(ExtDevChar));
                if (ExtDevChar.byHwFeature1 & 0x80) {
                    bNewTPH = true;//true for Toshiba
                }
            }


//-------------------
	/*//FILE				*fp = 0;
	//size_t				dwWritten = 0;

	sprintf(szRawFile, "/tmp/color%dx%d.raw", (int)m_BmpFinal.bmWidth, (int)m_BmpFinal.bmHeight);

	fp = _tfopen(szRawFile, "wb");

	dwWritten = fwrite(m_BmpFinal.bmBits, 1, dwSize, fp);
	fclose(fp);*/
//-------------------


	dwRet = ConvertBGR2YMC((unsigned char*)m_BmpFinal.bmBits, lpDataYMC, m_BmpFinal.bmWidth, m_BmpFinal.bmWidthBytes, m_BmpFinal.bmHeight, m_JobProp.byPrintMode, m_wRbnVendor, m_dwDrvModel, CVT_BGR2YMC, bNewTPH);
	MYTRACE("PrintOnePage(1), ConvertBGR2YMC() return %d,\n", dwRet);



//-------------------
	/*FILE				*fp = 0;
	size_t				dwWritten = 0;

	sprintf(szRawFile, "/tmp/%dx%d.raw", (int)m_BmpFinal.bmWidth, (int)m_BmpFinal.bmHeight);

	fp = _tfopen(szRawFile, "wb");
	dwWritten = fwrite(lpDataYMC, 1, dwSize, fp);
	fclose(fp);*/

	//return 0;
//-------------------


	byRet = m_Hpp->QueryCalibrationValue(&byH, &byV);
	MYTRACE("PrintOnePage(2), QueryCalibrationValue() return 0x%02X, byH = %d, byV = %d,\n", byRet, byH, byV);

	if ( (byH >= 1) && (byH <= shH_max) && (byV >= 1) && (byV <= shV_max) )
	{
		shH = byH;
		shV = byV;
	}

	if ( shH <= shH_max )
		shTemp = (shH - shH_mid) * shStepH;

	SublFormat.chXOffset = shTemp;

	if ( (shV >= 1) && (shV <= shV_max) )
		shTemp = (shV_mid - shV) * shStepV;

	SublFormat.chYOffset = shTemp;
	MYTRACE("PrintOnePage(2.5), chXOffset = %d, chYOffset = %d,\n", SublFormat.chXOffset, SublFormat.chYOffset);

	//
	byRet = m_Hpp->ExtSublimationFormat(&SublFormat);
	MYTRACE("PrintOnePage(3), ExtSublimationFormat() return 0x%02X,\n", byRet);

	byRet = m_Hpp->StartJob(&wJobId);
	MYTRACE("PrintOnePage(4), StartJob() return 0x%02X, wJobId = %d,\n", byRet, wJobId);

	bRet = SendHeatingTableForRoller();

	//2021.09.27 changed by Bill, no need to send this command if sending heating ok, this command will use embedded heating.
	if(bRet == false)
	{
		byRet = m_Hpp->DoCommand2(EFD_CHS, 0, 1);
		MYTRACE("PrintOnePage(5), DoCommand2(EFD_CHS) return 0x%02X,\n", byRet);
	}

	byRet = m_Hpp->DoCommand0(EPC_SP);
	MYTRACE("PrintOnePage(6), DoCommand0(EPC_SP) return 0x%02X,\n", byRet);

	//===============================================================
	dwSize = dwTphWidth * SublFormat.wHeight;

	//send Y data
	lpTmp = lpDataYMC;
	byRet = m_Hpp->DoCommand0(EPC_SYP);
	MYTRACE("PrintOnePage(7), DoCommand0(EPC_SYP) return 0x%02X,\n", byRet);
	byRet = SendPlaneData(lpTmp, dwTphWidth, SublFormat.wHeight, 0, 3000);

//-------------------
	/*FILE				*fp = 0;
	size_t				dwWritten = 0;
	sprintf(szRawFile, "/tmp/%dx%d_2.raw", (int)m_BmpFinal.bmWidth, (int)m_BmpFinal.bmHeight);
	fp = _tfopen(szRawFile, "wb");
	dwWritten = fwrite(lpTmp, 1, dwSize, fp);*/
//-------------------

	MYTRACE("PrintOnePage(8), SendPlaneData() return 0x%02X,\n", byRet);

	dwRet = CheckPrinterStatus();
	if ( dwRet != 0 )
	{
		delete [] lpDataYMC;
		return dwRet;
	}

	//send M data
	lpTmp = lpDataYMC + dwSize;
	byRet = m_Hpp->DoCommand0(EPC_SMP);
	MYTRACE("PrintOnePage(9), DoCommand0(EPC_SMP) return 0x%02X,\n", byRet);
	byRet = SendPlaneData(lpTmp, dwTphWidth, SublFormat.wHeight, 0, 3000);

//-------------------
	//dwWritten = fwrite(lpTmp, 1, dwSize, fp);
//-------------------

	MYTRACE("PrintOnePage(10), SendPlaneData() return 0x%02X,\n", byRet);

	dwRet = CheckPrinterStatus();
	if ( dwRet != 0 )
	{
		delete [] lpDataYMC;
		return dwRet;
	}

	//send C data
	lpTmp = lpDataYMC + dwSize*2;
	byRet = m_Hpp->DoCommand0(EPC_SCP);
	MYTRACE("PrintOnePage(11), DoCommand0(EPC_SCP) return 0x%02X,\n", byRet);
	byRet = SendPlaneData(lpTmp, dwTphWidth, SublFormat.wHeight, 0, 3000);

//-------------------
	//dwWritten = fwrite(lpTmp, 1, dwSize, fp);
	//fclose(fp);
//-------------------

	MYTRACE("PrintOnePage(12), SendPlaneData() return 0x%02X,\n", byRet);

	dwRet = CheckPrinterStatus();
	if ( dwRet != 0 )
	{
		delete [] lpDataYMC;
		return dwRet;
	}

	byRet = m_Hpp->DoCommand0(EPC_EP);
	MYTRACE("PrintOnePage(13), DoCommand0(EPC_EP) return 0x%02X,\n", byRet);

	byRet = m_Hpp->EndJob(wJobId);
	MYTRACE("PrintOnePage(14), EndJob() return 0x%02X,\n", byRet);

	delete [] lpDataYMC;

	return 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool COneJob::SendHeatingTableForRoller()
{
    	char szModelExt[8]= {0};
	char	szCurDir[MAX_PATH] = {0};

        

	if(m_dwDrvModel == USBMODEL_P520L || m_dwDrvModel == USBMODEL_P525N)
	{
		lstrcpy(szCurDir, "/usr/share/HiTi/tables/P52x");
		strcpy(szModelExt, "hri");
	}
	else if(m_dwDrvModel == USBMODEL_P720L)
	{
		lstrcpy(szCurDir, "/usr/share/HiTi/tables/P720");
		strcpy(szModelExt, "hrd");
	}
    	else if(m_dwDrvModel == USBMODEL_P750L)
	{
		lstrcpy(szCurDir, "/usr/share/HiTi/tables/P750");
		strcpy(szModelExt, "hrh");
	}
    	else
		return false;

	MYTRACE("SendHeatingTableForRoller, szCurDir = %s \n", szCurDir);
    	
	char	szHeatTbl[256] = {0};
        char	szTemp[256] = {0};
        

        //2011.08.31, for CHC, use another heating table file
        if ( 0x1000 == (m_wRbnVendor & 0xF000) //CHC
             || 0x0000 == (m_wRbnVendor & 0xF000) )//CHC, 2011.09.07
        {
            char				chTableVersion = 0;

            chTableVersion = m_wRbnVendor & 0x003F;

            memset(szTemp, 0, 256*sizeof(char));
            _stprintf(szTemp, _T("heatt%s.bin"), szModelExt);

	    if(m_dwDrvModel == USBMODEL_P520L || m_dwDrvModel == USBMODEL_P525N) {
                EXT_DEV_CHAR		ExtDevChar = {0};
                m_Hpp->DoCmdGetDataOnlyForERDC_RS(ERDC_RS, (UINT8 *) &ExtDevChar, sizeof(ExtDevChar));
                if (ExtDevChar.byHwFeature1 & 0x80) {
                    _stprintf(szTemp, _T("heatt%s1.bin"), szModelExt);
                }
            }

            szTemp[5] = 'c';

            if ( chTableVersion >= 0 && chTableVersion <= 9 )
                szTemp[3] = '0' + chTableVersion;
            else
                szTemp[3] = 'a' + chTableVersion - 10;

            //2018.12.06 changed by Bill for special print mode(Bit7 == 1)
            //if ( 1 == m_JobProp.byPrintMode )
            if ( 1 == (m_JobProp.byPrintMode & 0x0F))
                szTemp[4] = 'q';
            else
                szTemp[4] = 't';

            _stprintf(szHeatTbl, _T("%s/%s"), szCurDir, szTemp);

            MYTRACE("SendHeatingTableForRoller, szHeatTbl = %s, chTableVersion = %d \n", szHeatTbl, chTableVersion);

	    FILE 	*Profile = NULL;

            do
            {
		MYTRACE("SendHeatingTableForRoller try open szHeatTbl = %s, \n", szHeatTbl);
		Profile =  _tfopen(szHeatTbl, "rb");
		if(Profile != NULL)
		{
		    MYTRACE("SendHeatingTableForRoller found szHeatTbl = %s, \n", szHeatTbl);
		    break;
		}
                if ( szTemp[3] == '0' )
                {
                    szTemp[3] = 't';
                }
                else if ( szTemp[3] == 't' )
                {
                    //SendMessageToDesktop((void*)this, MSG_SHOW_ERROR_DLG, 0x1100000F);
                    //return false;
		    MYTRACE("SendHeatingTableForRoller final 1 szHeatTbl = %s \n", szHeatTbl);
                    return false;//ERROR_PRINT_CANCELLED;//2015.02.12
                }
                else if ( szTemp[3] == 'a' )
                    szTemp[3] = '9';
                else
                    szTemp[3] -= 1;

                _stprintf(szHeatTbl, _T("%s/%s"), szCurDir, szTemp);
		
            }
	    while(Profile == NULL);

	    if(Profile != NULL)
		fclose(Profile);

            //2012.11.14

            MYTRACE("SendHeatingTableForRoller final 0 szHeatTbl = %s \n", szHeatTbl);
        }
        else
        {
            //2018.12.06 changed by Bill for special print mode(Bit7 == 1)
            //if ( 0x01 == m_JobProp.byPrintMode )
            if ( 0x01 == (m_JobProp.byPrintMode & 0x0F))
                _stprintf(szHeatTbl, _T("%s/heatq%s.bin"), szCurDir, szModelExt);
            else
                _stprintf(szHeatTbl, _T("%s/heatt%s.bin"), szCurDir, szModelExt);

            MYTRACE("SendHeatingTableForRoller final 2 szHeatTbl = %s \n", szHeatTbl);
        }

        UINT32		dwNeedSize = 0, dwBufSize = 0;
        UINT16				wHeatPlane = 0x87;//YMCO 0xC7 for matte

        //Albert add. If matte wHeatPlane = 0xC7
        if(m_JobProp.byApplyMatte == 1)
        {
            wHeatPlane = 0xC7;
        }

        bool				bSendCVD = TRUE;
        unsigned short		wToneCurve = 0x0002;//CT_HITI_CLASSIC

        //changed by Bill

        bool bRet = GetInsillicaHeatData(szHeatTbl, 0, 0, &dwNeedSize, wHeatPlane, bSendCVD, wToneCurve, 0);


        dwBufSize = dwNeedSize;
        //2020.08.05 changed by Bill
        //UINT8 *lpHeatTable = new UINT8[dwBufSize];
        UINT8 *lpHeatTable = NULL;

	if(dwBufSize > 0)	
		lpHeatTable = static_cast<UINT8 *>(calloc(sizeof(UINT8), dwBufSize));

        MYTRACE("SendHeatingTableForRoller @@@@ COneJob::SendHeatingTableForRoller(3a), GetInsillicaHeatData return %d, dwNeedSize = %d, lpHeatTable = 0x%08X,\n", bRet, dwNeedSize, lpHeatTable);

        if ( lpHeatTable )
        {
            
            bRet = GetInsillicaHeatData(szHeatTbl, lpHeatTable, dwBufSize, &dwNeedSize, wHeatPlane, bSendCVD, wToneCurve, 0);
            MYTRACE("SendHeatingTableForRoller After GetInsillicaHeatData 2  retrun:%d, \n", bRet);

//-------------------
		/*FILE				*fp = 0;
		size_t				dwWritten = 0;
		char				szRawFile[256] = {0};

		sprintf(szRawFile, "/tmp/heat.bin");

		fp = _tfopen(szRawFile, "wb");
		dwWritten = fwrite(lpHeatTable, 1, dwBufSize, fp);
		fclose(fp);*/
//-------------------

            UINT16				wCmdSHPTC = ESD_SHPTC;

	    if(bRet)
	    {
            	UINT8 byRet = m_Hpp->DoCmdSendData(wCmdSHPTC, lpHeatTable, dwBufSize, false);
            	MYTRACE("SendHeatingTableForRoller After DoCmdSendData BufferSize:%d return = 0x%02X,\n", dwBufSize, byRet);

            	if ( byRet != 0x20 )
            	{		
                	bRet = false;
		}
            }

            //2020.08.05 changed by Bill
            //delete [] lpHeatTable;
            free(lpHeatTable);
        }
        else
        {
            bRet = false;
        }
    
    return bRet;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool COneJob::GetInsillicaHeatData
        (
                char* Path,
                //unsigned short *Buffer, UINT32 BufferSize, UINT32 *NeedSize,//2019.08.29 changed by Bill
                unsigned char *Buffer, UINT32 BufferSize, UINT32 *NeedSize,
                unsigned short HeatPlanes, bool CVDPlane, unsigned short ColorTableType,
                unsigned char MediaType
        )
{
    // DECLARE

    //CAFile	file;
    bool	bret = true;

    unsigned char	nSelTBNum = 0;
    UINT32	HeaderSize = 0 ;
    UINT32	DataSize = 0;
    UINT32	i;
    unsigned char	nFilePlaneNum = 0 ;	// Total File Tag Number
    unsigned char	nPlaneCode = 0 ;	// Tag Code
    UINT32	PlaneSize = 0;		// Tag size
    UINT32	PlaneOffset = 0;	// File Data Offset

    unsigned char	*pBuf;				// New Table Buffer
    //UINT32	nBufferIndex = 0 ;
    UINT32	tmpSize = 0;		// Tmp Need size
    UINT32	BufOffset = 0;

    PHEATTB_HEADER pTB_Header = NULL;
    PHEATTB_HEADER pNewTB_Header = NULL;


    FILE 	*file = NULL;

    //CString msg;

    // PARAMETER CHECK
    if ( Path == (char*)NULL || (HeatPlanes==0 && CVDPlane==false && ColorTableType==0) )
    {
        return false;
    }


    // PARAMETER CHECK
    file = _tfopen(Path, "rb");
    if(file == NULL)
        return false;

    //Seek( 0, CAFile::begin);
    fseek(file, 0, SEEK_SET);
    //file.Read( &nFilePlaneNum, 1);
    fread(&nFilePlaneNum, sizeof(unsigned char), 1, file);
    

    pTB_Header = (HEATTB_HEADER*) calloc(sizeof(HEATTB_HEADER), nFilePlaneNum);

    if ( pTB_Header == (PHEATTB_HEADER)NULL )
    {
        //file.Close();
	fclose(file);
        return false;
    }

    //2020.08.05 changed by Bill
    //pNewTB_Header = new HEATTB_HEADER[nFilePlaneNum];
    pNewTB_Header = (HEATTB_HEADER*) calloc(sizeof(HEATTB_HEADER), nFilePlaneNum);

    if ( pNewTB_Header == (PHEATTB_HEADER)NULL)
    {
        //2020.08.05 changed by Bill
        //delete [] pTB_Header;
        free(pTB_Header);

        //file.Close();
	fclose(file);
        return false;
    }

    for ( i = 0; i <  nFilePlaneNum; i++ )
    {
        //file.Read( &nPlaneCode, 1);
	fread(&nPlaneCode, sizeof(unsigned char), 1, file);

        //file.Read( &PlaneSize, 4);
	fread(&PlaneSize, sizeof(unsigned char), 4, file);



        //file.Read( &PlaneOffset, 4);
	fread(&PlaneOffset, sizeof(unsigned char), 4, file);


        
        pTB_Header[i].nTagCode		= nPlaneCode;
        pTB_Header[i].nSizeOfWord	= PlaneSize;
        pTB_Header[i].nOffset		= PlaneOffset;


    }
    //==End of Read Header



    // INITIALIZE

    
    *NeedSize = 0;
    //pBuf = (unsigned char*)Buffer;
    pBuf = Buffer;

    if(!GetPlaneNumWithHAC( pNewTB_Header, nSelTBNum, pTB_Header,  nFilePlaneNum,HeatPlanes, CVDPlane, ColorTableType ) )
    {
        bret = false;
        goto Release_Label;
    }

    HeaderSize = ( nSelTBNum * 9 + 1 );
//    MYTRACE((char*)m_JobProp.byTableRoot, "GetInsillicaHeatData HeaderSize:%d, DataSize:%d \n", HeaderSize, DataSize);
    for ( i = 0; i <  nSelTBNum; i++ )
    {
//        MYTRACE((char*)m_JobProp.byTableRoot, "GetInsillicaHeatData,  i:%d pNewTB_Header.nSizeOfWord:%d \n", i, pNewTB_Header[i].nSizeOfWord);
        DataSize += pNewTB_Header[i].nSizeOfWord * 2 ;
    }
    *NeedSize = HeaderSize + DataSize;
    PlaneOffset = HeaderSize; // for recode header plane offset.
//    MYTRACE((char*)m_JobProp.byTableRoot, "GetInsillicaHeatData HeaderSize:%d, DataSize:%d nSelTBNum:%d \n", HeaderSize, DataSize, nSelTBNum);

    //== Check Buffer size and pointer
    if ( Buffer == (unsigned char *)NULL || ( BufferSize < *NeedSize )  )
    {
        bret = true;
        goto Release_Label;
    }
    else
    {
        // == Write Tag number
        memcpy( pBuf + BufOffset, &nSelTBNum, 1 );
        BufOffset += 1;
    }


    // == Write header
    for ( i = 0; i <  nSelTBNum; i++ )
    {
        //write Plane
        memcpy( pBuf + BufOffset, &(pNewTB_Header[i].nTagCode), 1 );
        BufOffset += 1;
        //write size
        PlaneSize = pNewTB_Header[i].nSizeOfWord;
        tmpSize = PlaneSize;
        
        memcpy( pBuf + BufOffset, &tmpSize, 4 );
        BufOffset += 4;
        //write offset
        pNewTB_Header[i].nBufferOffset = PlaneOffset;
        tmpSize = PlaneOffset; // New file Offset
        
        memcpy( pBuf + BufOffset, &tmpSize, 4 );
        BufOffset += 4;

        PlaneOffset += (PlaneSize*2) ;
    }
    //==End of header


    //== Write Data
    for ( i = 0; i <  nSelTBNum; i++ )
    {
        PlaneOffset = pNewTB_Header[i].nOffset;
        BufOffset = pNewTB_Header[i].nBufferOffset;
        PlaneSize = pNewTB_Header[i].nSizeOfWord * 2 ;

        //file.Seek( PlaneOffset, CAFile::begin);
	fseek(file, PlaneOffset, SEEK_SET);
        //file.Read( (pBuf + BufOffset), PlaneSize);
	fread((pBuf + BufOffset), sizeof(unsigned char), PlaneSize, file);
    }
    //==End of write data

    // CLEAN
    Release_Label:
    //file.Close();
    fclose(file);
    if ( pTB_Header )
    {
        //2020.08.05 changed by Bill
        //delete [] pTB_Header;
        free(pTB_Header);

        //pTB_Header = (PHEATTB_HEADER) NULL;
    }
    if ( pNewTB_Header )
    {
        //2020.08.05 changed by Bill
        //delete [] pNewTB_Header;
        free(pNewTB_Header);
        //pNewTB_Header = (PHEATTB_HEADER) NULL;
    }
    return bret;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool COneJob::GetPlaneNumWithHAC( PHEATTB_HEADER pNewPlaneTag, unsigned char &nSelTagNum, PHEATTB_HEADER pTB_Header,  unsigned char nTagNum,unsigned short HeatPlanes, bool CVDPlane, unsigned short ColorTableType)
{
    unsigned char	nTargetTagCode;
    bool	bIfHAC_YMC =false;
    bool	bIfHAC_all =false;
    bool	bIfLS_YMC =false;//Long smear
    bool	bIfLS_all =false;//Long smear
    bool	bIfGL_YMC =false;//Ghost Line
    bool	bIfGL_all =false;//Ghost Line
    bool	bIfEG_YMC =false;//Energy
    bool	bIfEG_all =false;//Energy

    nSelTagNum = 0;
    if( (pNewPlaneTag == (PHEATTB_HEADER)NULL) || (pTB_Header == (PHEATTB_HEADER)NULL) )
        return false;

    // == Lode Version ==
    {
        // == Main Version ==
        nTargetTagCode = MAIN_VERSION;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            /*
            //Add a default version if no version find
            PlaneIndex[nSelTagNum].nTagCode = nTargetTagCode;
            PlaneIndex[nSelTagNum].nSizeOfWord = 2;
            PlaneIndex[nSelTagNum].nOffset = 0;
            nSelTagNum ++;
            */
            //return false;// not find table
        }


        // == Minor Version ==
        nTargetTagCode = MINOR_VERSION;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            /*
            //Add a default version if no version find
            PlaneIndex[nSelTagNum].nTagCode = nTargetTagCode;
            PlaneIndex[nSelTagNum].nSizeOfWord = 2;
            PlaneIndex[nSelTagNum].nOffset = 0;
            nSelTagNum ++;
            */
            //return false;// not find table
        }
    }

    // == Load All Y table======================================================================
    if (HeatPlanes & HT_Y)
    {
        // == Heating table
        nTargetTagCode = HT_D_Y;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            return false;// not find table
        }

        // == HAC table
        nTargetTagCode = HEAT_ACCUM_COMP_Y;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfHAC_YMC ) // If not write HAC YMC
            {
                nTargetTagCode = HEAT_ACCUM_COMP_YMC;
                if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    if (! bIfHAC_all ) // If already write HAC all
                    {
                        nTargetTagCode = HEAT_ACCUM_COMP_ALL;
                        if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                        {
                            bIfHAC_all = true;
                        }
                    }
                }
                else
                {
                    bIfHAC_YMC = true;
                }
            }
        }// == End of Load HAC table

        //== Long smear table
        nTargetTagCode = LONG_SMEAR_Y;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfLS_YMC ) // If not write Long smear YMC
            {
                nTargetTagCode = LONG_SMEAR_YMC;
                if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    if (! bIfLS_all ) // If already write Long smear all
                    {
                        nTargetTagCode = LONG_SMEAR_ALL;
                        if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                        {
                            bIfLS_all = true;
                        }
                    }
                }
                else
                {
                    bIfLS_YMC = true;
                }
            }
        }// == End of Load Long smear

        //== GHOST LINE table
        nTargetTagCode = GHOST_LINE_COMP_Y;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfGL_YMC ) // If not write HAC YMC
            {
                nTargetTagCode = GHOST_LINE_COMP_YMC;
                if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    if (! bIfGL_all ) // If already write HAC all
                    {
                        nTargetTagCode = GHOST_LINE_COMP_ALL;
                        if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                        {
                            bIfGL_all = true;
                        }
                    }
                }
                else
                {
                    bIfGL_YMC = true;
                }
            }
        }// == End of Load Ghost line

        //== ENERGY table
        nTargetTagCode = ENERGY_Y;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfEG_YMC ) // If not write HAC YMC
            {
                nTargetTagCode = ENERGY_YMC;
                if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    if (! bIfEG_all ) // If already write HAC all
                    {
                        nTargetTagCode = ENERGY_ALL;
                        if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                        {
                            bIfEG_all = true;
                        }
                    }
                }
                else
                {
                    bIfEG_YMC = true;
                }
            }
        }// == End of Load Energy

    }// == End of Load All Y table


    // == Load All M table======================================================================
    if (HeatPlanes & HT_M)
    {
        // == Heating table
        nTargetTagCode = HT_D_M;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            return false;// not find table
        }

        // == HAC table
        nTargetTagCode = HEAT_ACCUM_COMP_M;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfHAC_YMC ) // If not write HAC YMC
            {
                nTargetTagCode = HEAT_ACCUM_COMP_YMC;
                if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    if (! bIfHAC_all ) // If already write HAC all
                    {
                        nTargetTagCode = HEAT_ACCUM_COMP_ALL;
                        if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                        {
                            bIfHAC_all = true;
                        }
                    }
                }
                else
                {
                    bIfHAC_YMC = true;
                }
            }
        }// == End of Load HAC table

        //== Long smear table
        nTargetTagCode = LONG_SMEAR_M;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfLS_YMC ) // If not write Long smear YMC
            {
                nTargetTagCode = LONG_SMEAR_YMC;
                if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    if (! bIfLS_all ) // If already write Long smear all
                    {
                        nTargetTagCode = LONG_SMEAR_ALL;
                        if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                        {
                            bIfLS_all = true;
                        }
                    }
                }
                else
                {
                    bIfLS_YMC = true;
                }
            }
        }// == End of Load Long smear

        //== GHOST LINE table
        nTargetTagCode = GHOST_LINE_COMP_M;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfGL_YMC ) // If not write Ghost line YMC
            {
                nTargetTagCode = GHOST_LINE_COMP_YMC;
                if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    if (! bIfGL_all ) // If already write Ghost line all
                    {
                        nTargetTagCode = GHOST_LINE_COMP_ALL;
                        if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                        {
                            bIfGL_all = true;
                        }
                    }
                }
                else
                {
                    bIfGL_YMC = true;
                }
            }
        }// == End of Load Ghost line

        //== ENERGY table
        nTargetTagCode = ENERGY_M;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfEG_YMC ) // If not write Energy YMC
            {
                nTargetTagCode = ENERGY_YMC;
                if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    if (! bIfEG_all ) // If already write Energy all
                    {
                        nTargetTagCode = ENERGY_ALL;
                        if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                        {
                            bIfEG_all = true;
                        }
                    }
                }
                else
                {
                    bIfEG_YMC = true;
                }
            }
        }// == End of Load Energy

    }// == End of Load All M table

    // == Load All C table======================================================================
    if (HeatPlanes & HT_C)
    {
        // == Heating table
        nTargetTagCode = HT_D_C;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            return false;// not find table
        }

        // == HAC table
        nTargetTagCode = HEAT_ACCUM_COMP_C;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfHAC_YMC ) // If not write HAC YMC
            {
                nTargetTagCode = HEAT_ACCUM_COMP_YMC;
                if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    if (! bIfHAC_all ) // If already write HAC all
                    {
                        nTargetTagCode = HEAT_ACCUM_COMP_ALL;
                        if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                        {
                            bIfHAC_all = true;
                        }
                    }
                }
                else
                {
                    bIfHAC_YMC = true;
                }
            }
        }// == End of Load HAC table

        //== Long smear table
        nTargetTagCode = LONG_SMEAR_C;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfLS_YMC ) // If not write Long smear YMC
            {
                nTargetTagCode = LONG_SMEAR_YMC;
                if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    if (! bIfLS_all ) // If already write Long smear all
                    {
                        nTargetTagCode = LONG_SMEAR_ALL;
                        if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                        {
                            bIfLS_all = true;
                        }
                    }
                }
                else
                {
                    bIfLS_YMC = true;
                }
            }
        }// == End of Load Long smear

        //== GHOST LINE table
        nTargetTagCode = GHOST_LINE_COMP_C;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfGL_YMC ) // If not write Ghost line YMC
            {
                nTargetTagCode = GHOST_LINE_COMP_YMC;
                if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    if (! bIfGL_all ) // If already write Ghost line all
                    {
                        nTargetTagCode = GHOST_LINE_COMP_ALL;
                        if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                        {
                            bIfGL_all = true;
                        }
                    }
                }
                else
                {
                    bIfGL_YMC = true;
                }
            }
        }// == End of Load Ghost line

        //== ENERGY table
        nTargetTagCode = ENERGY_C;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfEG_YMC ) // If not write Energy YMC
            {
                nTargetTagCode = ENERGY_YMC;
                if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    if (! bIfEG_all ) // If already write Energy all
                    {
                        nTargetTagCode = ENERGY_ALL;
                        if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                        {
                            bIfEG_all = true;
                        }
                    }
                }
                else
                {
                    bIfEG_YMC = true;
                }
            }
        }// == End of Load Energy

    }// == End of Load All C table


    if (HeatPlanes & HT_K)
    {// == Load Dye K  All table======================================================================
        // == Heating table
        nTargetTagCode = HT_D_K;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            return false;// not find table
        }

        // == HAC table
        nTargetTagCode = HEAT_ACCUM_COMP_DK;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfHAC_all ) // If already write HAC all
            {
                nTargetTagCode = HEAT_ACCUM_COMP_ALL;
                if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    bIfHAC_all = true;
                }
            }
        }// == End of Load HAC table

        //== Long smear table
        nTargetTagCode = LONG_SMEAR_DK;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfLS_all ) // If already write Long smear all
            {
                nTargetTagCode = LONG_SMEAR_ALL;
                if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    bIfLS_all = true;
                }
            }
        }// == End of Load Long smear

        //== GHOST LINE table
        nTargetTagCode = GHOST_LINE_COMP_DK;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfGL_all ) // If already write ghost line all
            {
                nTargetTagCode = GHOST_LINE_COMP_ALL;
                if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    bIfGL_all = true;
                }
            }

        }// == End of Load Ghost line

        //== ENERGY table
        nTargetTagCode = ENERGY_DK;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfEG_all ) // If already write Energy all
            {
                nTargetTagCode = ENERGY_ALL;
                if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    bIfEG_all = true;
                }
            }
        }// == End of Load Energy

    }// == End of Load All Dye K table

    if (HeatPlanes & HT_RSN_TYPE)
    {
        if ( ( HeatPlanes & HT_RK ) == HT_RK )
        {
            // == Load K of YMCKO All table======================================================================

            // == Heating table
            nTargetTagCode = HT_R_K;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                return false;// not find table
            }

            // == HAC table
            nTargetTagCode = HEAT_ACCUM_COMP_RK;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfHAC_all ) // If already write HAC all
                {
                    nTargetTagCode = HEAT_ACCUM_COMP_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfHAC_all = true;
                    }
                }
            }// == End of Load HAC table

            //== Long smear table
            nTargetTagCode = LONG_SMEAR_RK;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfLS_all ) // If already write Long smear all
                {
                    nTargetTagCode = LONG_SMEAR_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfLS_all = true;
                    }
                }
            }// == End of Load Long smear

            //== GHOST LINE table
            nTargetTagCode = GHOST_LINE_COMP_RK;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfGL_all ) // If already write ghost line all
                {
                    nTargetTagCode = GHOST_LINE_COMP_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfGL_all = true;
                    }
                }

            }// == End of Load Ghost line

            //== ENERGY table
            nTargetTagCode = ENERGY_RK;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfEG_all ) // If already write Energy all
                {
                    nTargetTagCode = ENERGY_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfEG_all = true;
                    }
                }
            }// == End of Load Energy

        }// == End of Load All K of YMCKO table
        else if( ( HeatPlanes & HT_R ) == HT_R )
        {// == Load Resin K  All table======================================================================

            // == Heating table
            nTargetTagCode = HT_R_R;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                return false;// not find table
            }

            // == HAC table
            nTargetTagCode = HEAT_ACCUM_COMP_R;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfHAC_all ) // If already write HAC all
                {
                    nTargetTagCode = HEAT_ACCUM_COMP_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfHAC_all = true;
                    }
                }
            }// == End of Load HAC table

            //== Long smear table
            nTargetTagCode = LONG_SMEAR_R;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfLS_all ) // If already write Long smear all
                {
                    nTargetTagCode = LONG_SMEAR_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfLS_all = true;
                    }
                }
            }// == End of Load Long smear

            //== GHOST LINE table
            nTargetTagCode = GHOST_LINE_COMP_R;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfGL_all ) // If already write ghost line all
                {
                    nTargetTagCode = GHOST_LINE_COMP_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfGL_all = true;
                    }
                }

            }// == End of Load Ghost line

            //== ENERGY table
            nTargetTagCode = ENERGY_R;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfEG_all ) // If already write Energy all
                {
                    nTargetTagCode = ENERGY_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfEG_all = true;
                    }
                }
            }// == End of Load Energy

        }// == End of Load All Resin K table

    }
    // O Table 只須包一個
    if (HeatPlanes & HT_O)// 只須判斷 O Flag
    {
        if ( (HeatPlanes & HT_MO) == HT_MO )
        {
            // == Load MO  All table======================================================================

            // == Heating table
            nTargetTagCode = HT_D_MO;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                return false;// not find table
            }

            // == HAC table
            nTargetTagCode = HEAT_ACCUM_COMP_MO;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfHAC_all ) // If already write HAC all
                {
                    nTargetTagCode = HEAT_ACCUM_COMP_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfHAC_all = true;
                    }
                }
            }// == End of Load HAC table

            //== Long smear table
            nTargetTagCode = LONG_SMEAR_MO;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfLS_all ) // If already write Long smear all
                {
                    nTargetTagCode = LONG_SMEAR_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfLS_all = true;
                    }
                }
            }// == End of Load Long smear

            //== GHOST LINE table
            nTargetTagCode = GHOST_LINE_COMP_MO;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfGL_all ) // If already write ghost line all
                {
                    nTargetTagCode = GHOST_LINE_COMP_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfGL_all = true;
                    }
                }

            }// == End of Load Ghost line

            //== ENERGY table
            nTargetTagCode = ENERGY_MO;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfEG_all ) // If already write Energy all
                {
                    nTargetTagCode = ENERGY_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfEG_all = true;
                    }
                }
            }// == End of Load Energy


        }// == End of Load MO  All table======================================================================

        else
        {
            // == Load Dye O or KO  All table======================================================================

            // == Heating table
            if ( (HeatPlanes & HT_KO) == HT_KO )
            {
                nTargetTagCode = HT_D_KO; // Find KO First
                if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    nTargetTagCode = HT_D_O;// If not Find KO, Load O
                    if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        return false;// not find table
                    }
                }
            }
            else
            {
                nTargetTagCode = HT_D_O;// Load DyeO heating table
                if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    nTargetTagCode = HT_D_KO;// If not Find KO, Load O
                    if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        return false;// not find table
                    }
                }

            }

            // == HAC table
            nTargetTagCode = HEAT_ACCUM_COMP_O;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfHAC_all ) // If already write HAC all
                {
                    nTargetTagCode = HEAT_ACCUM_COMP_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfHAC_all = true;
                    }
                }
            }// == End of Load HAC table

            //== Long smear table
            nTargetTagCode = LONG_SMEAR_O;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfLS_all ) // If already write Long smear all
                {
                    nTargetTagCode = LONG_SMEAR_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfLS_all = true;
                    }
                }
            }// == End of Load Long smear

            //== GHOST LINE table
            nTargetTagCode = GHOST_LINE_COMP_O;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfGL_all ) // If already write ghost line all
                {
                    nTargetTagCode = GHOST_LINE_COMP_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfGL_all = true;
                    }
                }

            }// == End of Load Ghost line

            //== ENERGY table
            nTargetTagCode = ENERGY_O;
            if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
            {
                if (! bIfEG_all ) // If already write Energy all
                {
                    nTargetTagCode = ENERGY_ALL;
                    if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                    {
                        bIfEG_all = true;
                    }
                }
            }// == End of Load Energy


        }// == End of Load O or KO  All table======================================================================
    }

    if (HeatPlanes & HT_FO)
    {// == Load FO  All table======================================================================

        // == Heating table
        nTargetTagCode = HT_R_FO;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            return false;// not find table
        }

        // == HAC table
        nTargetTagCode = HEAT_ACCUM_COMP_FO;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfHAC_all ) // If already write HAC all
            {
                nTargetTagCode = HEAT_ACCUM_COMP_ALL;
                if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    bIfHAC_all = true;
                }
            }
        }// == End of Load HAC table

        //== Long smear table
        nTargetTagCode = LONG_SMEAR_FO;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfLS_all ) // If already write Long smear all
            {
                nTargetTagCode = LONG_SMEAR_ALL;
                if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    bIfLS_all = true;
                }
            }
        }// == End of Load Long smear

        //== GHOST LINE table
        nTargetTagCode = GHOST_LINE_COMP_FO;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfGL_all ) // If already write ghost line all
            {
                nTargetTagCode = GHOST_LINE_COMP_ALL;
                if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    bIfGL_all = true;
                }
            }

        }// == End of Load Ghost line

        //== ENERGY table
        nTargetTagCode = ENERGY_FO;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            if (! bIfEG_all ) // If already write Energy all
            {
                nTargetTagCode = ENERGY_ALL;
                if(	FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
                {
                    bIfEG_all = true;
                }
            }
        }// == End of Load Energy

    }// == End of Load All FO table

    if (HeatPlanes & HT_L)
    {
        // == Load L  Heating table(L only have heating table)======================================================================

        // == Heating table
        nTargetTagCode = HT_R_L;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            return false;// not find table
        }

    }// == End of Load L All table======================================================================

    if ( CVDPlane )
    {
        nTargetTagCode = HT_CVD;
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            return false;// not find table
        }
    }
    //Color table 目前只包一個
    if ( ColorTableType > 0 )
    {
        if( ColorTableType & CT_INVERT )
        {
            nTargetTagCode = CT_Invert;
        }
        else if ( ColorTableType & CT_HITI_CLASSIC )
        {
            nTargetTagCode = CT_Classic;
        }
        else if ( ColorTableType & CT_HITI_IDPASSP )
        {
            nTargetTagCode = CT_IDPass;
        }
        else
        {	// Default return Invert
            nTargetTagCode = CT_Invert;
        }
        if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
        {
            return false;// not find table
        }
    }
//Tone Curve Compensatino : For compatible, if not find TC table, must reure true.
    nTargetTagCode = TC_Compensation;
    if(	!FindPlaneIndex( nSelTagNum, pNewPlaneTag, pTB_Header, nTagNum,  nTargetTagCode))
    {
        return true;// not find table
    }


    return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool COneJob::FindPlaneIndex(unsigned char &nTagCount, PHEATTB_HEADER pNewPlaneTag, PHEATTB_HEADER pTB_Header, UINT32 nTagNum, unsigned char nTargetTagCode)
{
    //PHEATTB_HEADER *ptemp = NULL;
    bool bret = false;
    bool bNotFind = true;
    UINT32 i=0;
    //UINT32 index;

    //ptemp = pTB_Header;
    //index = 0;

//    MYTRACELOG((char*)m_JobProp.byTableRoot, "FindPlaneIndex, nTagCount = %d bret:%d i:%d Before while, nTagNum:%d ,\n", nTagCount, bret, i, nTagNum);
    while ( i < nTagNum && bNotFind )
    {
        if ( nTargetTagCode == pTB_Header[i].nTagCode )
        {
            //index = i;
            bNotFind = false;
            bret = true;

            pNewPlaneTag[nTagCount].nTagCode	= nTargetTagCode;
            pNewPlaneTag[nTagCount].nSizeOfWord = pTB_Header[i].nSizeOfWord;
            pNewPlaneTag[nTagCount].nOffset		= pTB_Header[i].nOffset;
            nTagCount ++;
        }
        i++;
    }

//    MYTRACELOG((char*)m_JobProp.byTableRoot, "FindPlaneIndex, nTagCount = %d bret:%d i:%d, nTagNum:%d ,\n", nTagCount, bret, i, nTagNum);
    return bret;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UINT8 COneJob::SendPlaneData(UINT8* lpSrc, UINT16 wWidth, UINT16 wHeight, short shStartLine, UINT16 wLinesOneTime)
{
	UINT8				byRet = 0;
	UINT8				*lpTemp = 0;

	UINT16				wStartLine = 0,
						wTotalRemainLine = 0,
						wSendLines = 0;
	UINT32				dwSize = 0;
/*
//-------------------
	FILE				*fp = 0;
	size_t				dwWritten = 0;
	char				szRawFile[256] = {0};

	sprintf(szRawFile, "/tmp/%dx%d.raw", wWidth, wHeight);

	fp = _tfopen(szRawFile, "a+b");
//-------------------
*/
	wTotalRemainLine = wHeight;

	wStartLine = (UINT16)shStartLine;
	lpTemp = lpSrc;

	while ( wTotalRemainLine > 0 )
	{
		if ( wTotalRemainLine < wLinesOneTime )
			wSendLines = wTotalRemainLine;
		else
			wSendLines = wLinesOneTime;

		dwSize = (UINT32)wWidth * (UINT32)wSendLines;

		byRet = m_Hpp->ExtSendExtPrintData(wStartLine, wSendLines, lpTemp, dwSize);
		MYTRACE(_T("ExtSendExtPrintData(), wStartLine = %4d, wSendLines = %d, dwSize = %d, byRet = 0x%02X,\n"), wStartLine, wSendLines, dwSize, byRet);
/*
//-------------------
		dwWritten = fwrite(lpTemp, 1, dwSize, fp);
//-------------------
*/
		wStartLine += wLinesOneTime;
		lpTemp += dwSize;
		wTotalRemainLine -= wSendLines;
	}
/*
//-------------------
	fclose(fp);
//-------------------
*/
	return byRet;
}
