/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright 2014-2015 HiTi Digital, Inc.  All Rights Reserved
**********************************************************/

#ifndef __MAPTABLE_H__
#define __MAPTABLE_H__


#define MEM_ALLOC_ERROR		3

#ifdef WIN32
class CMapingTable
#else
class __attribute__((__visibility__("hidden"))) CMapingTable
#endif
{
public:
	CMapingTable();
	virtual ~CMapingTable();
	unsigned char CreateTableData(unsigned char Bright=128,unsigned char Contrast=128,unsigned char Gamma=128,unsigned char CB_RC=128,unsigned char CB_GM=128,unsigned char CB_BY=128,unsigned char TableBit=8);
	void ModifyBrightTable(unsigned char Bright);
	void ModifyContrastTable(unsigned char Contrast);
	void ModifyGammaTable(unsigned char Gamma);
	void ModifyGammaTable(double fGamma);
	void ModifyColorBalanceTable(unsigned char CB,unsigned char Channel);
	void MapTableToImage(unsigned char *pSrcImg,unsigned char *pDstImg,unsigned long Height,unsigned long Width,unsigned long BytePerLine,unsigned char BitsPerColor=8);
	void ResetAll();
	void Release();

protected:
	unsigned short	m_wTableBit;				//Table bit n
	unsigned short	m_wTableLen_1;				//Table Boundary = 2^n - 1;
	unsigned long	m_dwTableLen;				//Table Length = 2^n
	unsigned short	*m_pMemory;					//Allocate total memory
	unsigned short	*m_pColorBalanceTable[3];	//Color-Balance Table
	unsigned short	*m_pBrightnessTable;		//Brightness Table
	unsigned short	*m_pContrastTable;			//Contrast Table
	unsigned short	*m_pGammaTable;				//Gamma Table
	unsigned short	*m_pCombinedTable[3];			//Combined Table

	unsigned char CreatTable(unsigned short TableBit);
	void CombineTable();
	void MakeGammaTable(unsigned short *pTable,unsigned char Level);
	void MakeGammaTable(unsigned short *pTable, double fGamma);

	void MakeBrightnessTable(unsigned short *pTable,unsigned char Level);
	void MakeContrastTable(unsigned short *pTable,unsigned char Level);
};

#endif // !defined(__MAPTABLE_H__)
