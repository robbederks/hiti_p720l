/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright HiTi Digital, Inc. 2013-2015 All Rights Reserved
**********************************************************/

/*
 * Include necessary headers...
 */

#include <cups/cups.h>
#include <cups/ppd.h>
#include <cups/raster.h>
#include <stdlib.h>
#include <fcntl.h>

#define CUPSRASTEROPEN			cupsRasterOpen
#define CUPSRASTERREADHEADER2	cupsRasterReadHeader2
#define CUPSRASTERREADPIXELS	cupsRasterReadPixels
#define CUPSRASTERCLOSE			cupsRasterClose

#define PPDCLOSE				ppdClose
#define PPDFINDCHOICE			ppdFindChoice
#define PPDFINDMARKEDCHOICE		ppdFindMarkedChoice
#define PPDFINDOPTION			ppdFindOption
#define PPDMARKDEFAULTS			ppdMarkDefaults
#define PPDOPENFILE				ppdOpenFile

#define CUPSGETOPTION			cupsGetOption
#define CUPSFREEOPTIONS			cupsFreeOptions
#define CUPSPARSEOPTIONS		cupsParseOptions
#define CUPSMARKOPTIONS			cupsMarkOptions


#define MAX(a,b) ( ((a) > (b)) ? (a) : (b) )

//--------------------------------------
#include "HTRTApi.h"

#include "Util0.h"

#include "OneJob.h"

bool GetUISettings(char * commandLineOptionSettings, HITI_JOB_PROPERTY_RT *lpMyJobProp);

inline int myGetOptionValueInt(const char *name, int nDefValue, int num_options, cups_option_t *options)
{
	const char			*chOption = NULL;
	int					nMyValue = 0;

	chOption = CUPSGETOPTION(name, num_options, options);

	nMyValue = nDefValue;
	if ( chOption )
		nMyValue = atoi(chOption);

	return nMyValue;
}
//--------------------------------------



int main(int argc, char *argv[])
{
	int                 fd = 0;         /* File descriptor providing CUPS raster data                                           */
	cups_raster_t *     ras = NULL;     /* Raster stream for printing                                                           */
	cups_page_header2_t header = {0};   /* CUPS Page header                                                                     */
	int                 page = 0;       /* Current page                                                                         */

	//-------------------------
	BITMAP				PageBmp = {0};
	UINT8				*lpRasterPage = 0;
	UINT32				dwRasterPage = 0;
	UINT32				dwRead = 0;
	UINT32				dwRet = 0;
	bool				bRet = false;

	HITI_JOB_PROPERTY_RT		MyJobProp = {0};
	COneJob				*cpJob = 0;
	char				szErrorMsg[256] = {0};
	//-------------------------


	//Validate the command-line arguments...
	if (argc < 6 || argc > 7)
	{
		fprintf(stderr, "Usage: %s job user title copies options [filename]\n", argv[0]);
		return 1;
	}


	/* disable buffering on all streams */
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	setbuf(stdin, NULL);

	//
	bRet = GetUISettings(argv[5], &MyJobProp);
	MYTRACE("After GetUISettings(), bRet = %d,\n", bRet);
	if ( !bRet )
	{
		return 1;
	}


	//
    if (argc == 7)
    {
        if ((fd = open(argv[6], O_RDONLY)) == -1)
        {
            perror("ERROR: Unable to open raster file - ");
            sleep(1);

            return EXIT_FAILURE;
        }
    }
    else
    {
        fd = 0;
    }

	ras = CUPSRASTEROPEN(fd, CUPS_RASTER_READ);
	MYTRACE("after cupsRasterOpen(), fd = %d, ras = 0x%x,\n", fd, ras);
	if ( !ras )
	{
		perror("ERROR: Unable to open raster file - ");

		if ( fd != 0 )
			close(fd);

		return (1);
	}

	cpJob = new COneJob();

	//Process pages as needed...
	while (CUPSRASTERREADHEADER2(ras, &header))
	{
		page ++;
		MYTRACE("after cupsRasterReadHeader2(), page = %d, cupsWidth = %d, cupsHeight = %d, cupsBitsPerPixel = %d, NumCopies = %d,\n", page, header.cupsWidth, header.cupsHeight, header.cupsBitsPerPixel, header.NumCopies);

		//Check for canceled jobs...
		//if (CancelJob)
		//	break;

		//Allocate memory to read one page raster data
		if ( lpRasterPage )
		{
			if ( PageBmp.bmHeight != header.cupsHeight
				|| PageBmp.bmWidthBytes != header.cupsBytesPerLine
				|| PageBmp.bmBitsPixel != header.cupsBitsPerPixel
				)
			{
				delete [] lpRasterPage;
				lpRasterPage = 0;
			}
		}

		dwRasterPage = header.cupsBytesPerLine * header.cupsHeight;

		if ( !lpRasterPage )
		{
			lpRasterPage = new UINT8[dwRasterPage];

			PageBmp.bmType			= 0x5250;
			PageBmp.bmHeight		= header.cupsHeight;
			PageBmp.bmWidthBytes	= header.cupsBytesPerLine;
			PageBmp.bmWidth			= header.cupsWidth;
			PageBmp.bmPlanes		= 1;
			PageBmp.bmBitsPixel		= header.cupsBitsPerPixel;
			PageBmp.bmBits			= lpRasterPage;
		}

		if ( lpRasterPage )
			memset(lpRasterPage, 0, dwRasterPage);

		fprintf(stderr, "PAGE: %d %d\n", page, header.NumCopies);
		//fprintf(stderr, "INFO: Starting page %d...", page);

//		if (!StartPage(ppd, &job, &header))
//			break;

		dwRead = CUPSRASTERREADPIXELS(ras, lpRasterPage, dwRasterPage);
		MYTRACE("after cupsRasterReadPixels(), dwRasterPage = %d, dwRead = %d,\n", dwRasterPage, dwRead);

		dwRet = 0;
		dwRet = cpJob->SendOnePageData(&MyJobProp, &PageBmp, header.NumCopies, page);
		MYTRACE("after SendOnePageData(), dwRet = %d,\n", dwRet);
		if ( dwRet )
		{
			GetErrorString(dwRet, szErrorMsg);
			fprintf(stderr, "ERROR: %s\n", szErrorMsg);
			break;
		}

//		if (!EndPage(ppd, &job, &header))
//			break;
	}

	if ( dwRet == 0 )
	{
		dwRet = cpJob->EndJob();
		MYTRACE("after EndJob(), dwRet = %d,\n", dwRet);
		if ( dwRet )
		{
			GetErrorString(dwRet, szErrorMsg);
			fprintf(stderr, "ERROR: %s\n", szErrorMsg);
		}
	}

	//-----------------------
	if ( ras )
		CUPSRASTERCLOSE(ras);

	if ( lpRasterPage )
		delete [] lpRasterPage;

	if ( cpJob )
		delete cpJob;

	if ( fd != 0 )
		close(fd);

//	if ( !bRet || CancelJob )
//		return 1;

	if ( dwRet )
		return 1;

    return 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool GetUISettings(char * commandLineOptionSettings, HITI_JOB_PROPERTY_RT *lpMyJobProp)
{
	ppd_file_t			*ppd = NULL;
	cups_option_t		*pCupsOptions = NULL;
	int					nOptions  = 0;
	int					modelNumber = 0;

	ppd_option_t		*pPpdOption = NULL;
	ppd_choice_t		*pPpdChoice = NULL;
	const char			*chGetOption = NULL;
	int					nValue = 0;
	double				dbValue = 0;
	int					nModelSN = 0;
	int					nModelFamily = 0;

    ppd = PPDOPENFILE(getenv("PPD"));

    PPDMARKDEFAULTS(ppd);

    nOptions = CUPSPARSEOPTIONS(commandLineOptionSettings, 0, &pCupsOptions);
    if ((nOptions == 0) || (pCupsOptions == NULL))
		return false;

	CUPSMARKOPTIONS(ppd, nOptions, pCupsOptions);

	MYTRACE("=============================================================\n");
	MYTRACE("GetUISettings(), nOptions = %d, pCupsOptions = 0x%08X,\n", nOptions, pCupsOptions);

	//ppd info
	MYTRACE("PPD information:\n");
	MYTRACE("color_device = %d,\n", ppd->color_device);
	MYTRACE("manufacturer = \"%s\",\n", ppd->manufacturer);
	MYTRACE("model_number = %d,\n", ppd->model_number);
	MYTRACE("modelname = \"%s\",\n", ppd->modelname);
	MYTRACE("nickname = \"%s\",\n", ppd->nickname);
	MYTRACE("shortnickname = \"%s\",\n", ppd->shortnickname);
	MYTRACE("pcfilename = \"%s\",\n", ppd->pcfilename);
	MYTRACE("product = \"%s\",\n\n", ppd->product);


	nModelSN = ppd->model_number;
	lpMyJobProp->dwModelSN = nModelSN;

	nModelFamily = 2;//MODEL_FAMILY_ROLLTYPE;

	MYTRACE("nModelSN = %d, nModelFamily = %d,\n\n", nModelSN, nModelFamily);

	//default values
	lpMyJobProp->shGamma			= 100;//gamma
	lpMyJobProp->byApplyMatte		= 0;
	lpMyJobProp->wColorTableMode	= 2;//classic
	lpMyJobProp->bySharpenMode		= 1;//whole image
	lpMyJobProp->shSharpness		= 50;//sharpness
	lpMyJobProp->byPrintMode		= 0;//standard

	chGetOption = CUPSGETOPTION("PageSize", nOptions, pCupsOptions);
	MYTRACE("PageSize = \"%s\",\n", chGetOption);
	
	//2017.02.16 added by Bill
	if(chGetOption == NULL)
	{
		chGetOption = CUPSGETOPTION("page-size", nOptions, pCupsOptions);
		MYTRACE("page-size = \"%s\",\n", chGetOption);
	}
	//2017.02.24 added by Bill
	if(chGetOption == NULL)
	{
		chGetOption = CUPSGETOPTION("media", nOptions, pCupsOptions);
		MYTRACE("media = \"%s\",\n", chGetOption);
	}
	if(chGetOption == NULL)
	{
		chGetOption = CUPSGETOPTION("MediaSize", nOptions, pCupsOptions);
		MYTRACE("MediaSize = \"%s\",\n", chGetOption);
	}
	if(chGetOption == NULL)
	{
		chGetOption = CUPSGETOPTION("media-size", nOptions, pCupsOptions);
		MYTRACE("media-size = \"%s\",\n", chGetOption);
	}
	

	/*if(chGetOption == NULL)
	{
		cups_dest_t *dests;
		int num_dests = cupsGetDests(&dests);
		cups_dest_t *dest;
		int i;
		const char *value;

		for (i = num_dests, dest = dests; i > 0; i --, dest ++)
		{
			if (dest->instance == NULL)
			{
				chGetOption = cupsGetOption("PageSize", dest->num_options, dest->options);
				MYTRACE("PageSize from dest = \"%s\" , nOptions = %d, pCupsOptions = 0x%08X,\n", chGetOption, dest->num_options, dest->options);
			}
		}

		MYTRACE("...PageSize from dest");
		cupsFreeDests(num_dests, dests);
	}*/

	lpMyJobProp->byPaperSize = 0;

	if ( chGetOption )
	{
		if ( strcmp(chGetOption, "P5x7") == 0 )
		{
			lpMyJobProp->byRibbonType	= 0x02;
			lpMyJobProp->byPaperSize	= HITI_PAPER_SIZE_5X7_PHOTO;
			lpMyJobProp->wPageWidth		= 1548;
			lpMyJobProp->wPageHeight	= 2140;
		}
		else if ( strcmp(chGetOption, "P6x8") == 0 )
		{
			lpMyJobProp->byRibbonType	= 0x04;
			lpMyJobProp->byPaperSize	= HITI_PAPER_SIZE_6X8_PHOTO;
			lpMyJobProp->wPageWidth		= 1844;
			lpMyJobProp->wPageHeight	= 2434;
		}
		else if ( strcmp(chGetOption, "P6x9") == 0 )
		{
			lpMyJobProp->byRibbonType	= 0x03;
			lpMyJobProp->byPaperSize	= HITI_PAPER_SIZE_6X9_PHOTO;
			lpMyJobProp->wPageWidth		= 1844;
			lpMyJobProp->wPageHeight	= 2740;
		}
		else if ( strcmp(chGetOption, "P6x8_split") == 0 
				|| strcmp(chGetOption, "P6x9_split") == 0 )
		{
			lpMyJobProp->byRibbonType	= 0x03;
			lpMyJobProp->byPaperSize	= HITI_PAPER_SIZE_6X8_PHOTO_SPLIT;
			lpMyJobProp->wPageWidth		= 1844;
			lpMyJobProp->wPageHeight	= 2492;
			lpMyJobProp->bySplit2up		= 1;
		}
		else if ( strcmp(chGetOption, "P6x4_split") == 0 )
		{
			lpMyJobProp->byRibbonType	= 0x01;
			lpMyJobProp->byPaperSize	= HITI_PAPER_SIZE_6X4_PHOTO_SPLIT;
			lpMyJobProp->wPageWidth		= 1844;
			lpMyJobProp->wPageHeight	= 1248;//1240; //2017.01.18 changed by Bill
			lpMyJobProp->bySplit2up		= 1;
		}
		else if ( strcmp(chGetOption, "P5x7_2up") == 0 )
		{
			lpMyJobProp->byRibbonType	= 0x02;
			lpMyJobProp->byPaperSize	= HITI_PAPER_SIZE_5X7_PHOTO_2UP;
			lpMyJobProp->wPageWidth		= 1548;
			lpMyJobProp->wPageHeight	= 1072;
			lpMyJobProp->bySplit2up		= 1;
		}
		else if ( strcmp(chGetOption, "P6x8_2up") == 0 )
		{
			lpMyJobProp->byRibbonType	= 0x03;
			lpMyJobProp->byPaperSize	= HITI_PAPER_SIZE_6X8_PHOTO_2UP;
			lpMyJobProp->wPageWidth		= 1844;
			lpMyJobProp->wPageHeight	= 1240;
			lpMyJobProp->bySplit2up		= 1;
		}
		/*else if ( strcmp(chGetOption, "P6x4") == 0 )
		{
			lpMyJobProp->byRibbonType	= 0x01;
			lpMyJobProp->byPaperSize	= HITI_PAPER_SIZE_6X4_PHOTO;
			lpMyJobProp->wPageWidth		= 1844;
			lpMyJobProp->wPageHeight	= 1240;
		}*/
		else if ( strcmp(chGetOption, "P6x3.94") == 0 )
		{
			lpMyJobProp->byRibbonType	= 0x01;
			lpMyJobProp->byPaperSize	= HITI_PAPER_SIZE_6X4B_PHOTO;
			lpMyJobProp->wPageWidth		= 1844;
			lpMyJobProp->wPageHeight	= 1216;
		}
		else //if ( strcmp(chGetOption, "P6x4") == 0 )
		{
			lpMyJobProp->byRibbonType	= 0x01;
			lpMyJobProp->byPaperSize	= HITI_PAPER_SIZE_6X4_PHOTO;
			lpMyJobProp->wPageWidth		= 1844;
			lpMyJobProp->wPageHeight	= 1240;
		}
	}
	else//use default paper size "P6x4"
	{
		lpMyJobProp->byRibbonType	= 0x01;
		lpMyJobProp->byPaperSize	= HITI_PAPER_SIZE_6X4_PHOTO;
		lpMyJobProp->wPageWidth		= 1844;
		lpMyJobProp->wPageHeight	= 1240;
	}

	if ( lpMyJobProp->byPaperSize == 0 )
	{
		fprintf(stderr, "ERROR: Unsupported paper size.\n");
		PPDCLOSE(ppd);
		return false;
	}

	//Print quality
	lpMyJobProp->byPrintMode = myGetOptionValueInt("PrintQuality", 0, nOptions, pCupsOptions);
	/*if ( chGetOption )
	{
		if ( strcmp(chGetOption, "Standard") == 0 )
			lpMyJobProp->byPrintMode = 0;
		else if ( strcmp(chGetOption, "Fine") == 0 )
			lpMyJobProp->byPrintMode = 1;
	}*/
	MYTRACE("PrintMode = \"%d\", wPageHeight = %d, \n", lpMyJobProp->byPrintMode, lpMyJobProp->wPageHeight);

	//Enable matte
	lpMyJobProp->byApplyMatte = myGetOptionValueInt("EnableMatte", 0, nOptions, pCupsOptions);
	MYTRACE("EnableMatte = \"%d\",\n", lpMyJobProp->byApplyMatte);

	//color settings
	lpMyJobProp->shBrightness = myGetOptionValueInt("Brightness", 0, nOptions, pCupsOptions);
	lpMyJobProp->shContrast   = myGetOptionValueInt("Contrast", 0, nOptions, pCupsOptions);
	MYTRACE("Brightness = %d, Contrast = %d,\n", lpMyJobProp->shBrightness, lpMyJobProp->shContrast);

	lpMyJobProp->shSharpness = myGetOptionValueInt("Sharpness", lpMyJobProp->shSharpness, nOptions, pCupsOptions);
	lpMyJobProp->bySharpenMode = myGetOptionValueInt("SharpenMode", lpMyJobProp->bySharpenMode, nOptions, pCupsOptions);
	MYTRACE("Sharpness = %d, bySharpenMode = %d,\n", lpMyJobProp->shSharpness, lpMyJobProp->bySharpenMode);

	lpMyJobProp->shGamma = myGetOptionValueInt("Gamma", 100, nOptions, pCupsOptions);
	MYTRACE("Gamma = %d,\n", lpMyJobProp->shGamma);

	lpMyJobProp->shRed   = myGetOptionValueInt("Red", 0, nOptions, pCupsOptions);
	lpMyJobProp->shGreen = myGetOptionValueInt("Green", 0, nOptions, pCupsOptions);
	lpMyJobProp->shBlue  = myGetOptionValueInt("Blue", 0, nOptions, pCupsOptions);
	MYTRACE("Red = %d, Green = %d, Blue = %d,\n", lpMyJobProp->shRed, lpMyJobProp->shGreen, lpMyJobProp->shBlue);

	lpMyJobProp->shSaturation = myGetOptionValueInt("Saturation", 0, nOptions, pCupsOptions);
	MYTRACE("Saturation = %d,\n", lpMyJobProp->shSaturation);

	PPDCLOSE(ppd);

	CUPSFREEOPTIONS(nOptions, pCupsOptions);

	return true;
}

// end of rastertohtrt.c
