/*********************************************************
* Written and developed by HiTi Digital, Inc.
* Copyright HiTi Digital, Inc. 2013-2015 All Rights Reserved
**********************************************************/

//#ifdef WIN32
//#include "stdafx.h"
//#endif

#include <stdio.h>
#include "CSaturation.h"
#include <memory.h>
CSaturation::CSaturation():m_nMul255TB(NULL),m_fInvSumTB(NULL),m_fTableS(NULL),m_bStop(false)
{

}

CSaturation::~CSaturation()
{
    MemoryFree();
}

void CSaturation::StopProcess()
{
    m_bStop = true;
}

void CSaturation::MemoryFree()
{
    if ( m_nMul255TB )
    {
        delete [] m_nMul255TB;
        m_nMul255TB = NULL;
    }

    if ( m_fInvSumTB )
    {
        delete [] m_fInvSumTB;
        m_fInvSumTB = NULL;
    }

    if ( m_fTableS )
    {
        delete [] m_fTableS;
        m_fTableS = NULL;
    }

}

long CSaturation::InitialParameter()
{
    int i = 0;
    int j = 0;
    if( m_nMul255TB == NULL)
    {
        m_nMul255TB = new long [256];
        if ( m_nMul255TB ==(long*)NULL )
        {
            return 0;//HT_RETURN_MEMORY_ERROR;
        }

        m_nMul255TB[0] = 0;
        for ( i = 1; i < 256; i++, j+= 255 )
        {
                m_nMul255TB[i] = j;
        }
    }

    if( m_fInvSumTB == NULL)
    {
        m_fInvSumTB = new double [511];
        if ( m_fInvSumTB ==(double*)NULL )
        {
            return 0;//HT_RETURN_MEMORY_ERROR;
        }

        m_fInvSumTB[0] = 1.0;
        m_fInvSumTB[510] = 1.0;
        for ( i = 1; i < 256; i++)
        {
                m_fInvSumTB[i] = 1.0/i;
        }
        for ( i = 256; i < 510; i++ )
        {
                m_fInvSumTB[i] = 1.0/(510-i);
        }
    }

    if( m_fTableS == NULL)
    {
        m_fTableS = new double [256];
        if ( m_fTableS ==(double*)NULL )
        {
            return 0;//HT_RETURN_MEMORY_ERROR;
        }
    }


    return 1;//HT_RETURN_SUCCESS;
}

long CSaturation::MakeSaturationTable( unsigned char RealV )
{
    m_bStop = false;

    long nret = 1;//HT_RETURN_SUCCESS;

    nret = InitialParameter();

    //if ( nret != HT_RETURN_SUCCESS )
	if ( nret != 1 )
    {
        return 0;
    }

    //Make Saturation table
    long        i;
    long	Band = 0;
    double	dbBand = 0;
    double	dbslope = 0;

    if ( RealV == 255 )
    {
            m_fTableS[0] = 0.0;
            for( i=1; i<256; i++ )
            {
                    m_fTableS[i] = 1.0;
            }
    }
    else if ( RealV == 0 )
    {
            for( i=0; i<256; i++ )
            {
                    m_fTableS[i] = 0.0;
            }
    }
    else if	( RealV > 128 )
    {
            Band = 255 - (RealV - 128)*2;
            for ( i = 0; i < Band; i++ )
            {
                    m_fTableS[i] = ( double)i/(double)Band;
            }
            for ( i = Band; i < 256; i++ )
            {
                    m_fTableS[i] = 1.0;
            }
    }
    else if	( RealV < 128 )
    {
            dbBand = (255.0 - ( 128.0 - RealV ) * 2.0 ) / 65536.0;
            for ( i = 0; i < 256; i++ )
            {
                    m_fTableS[i] = (float)i*dbBand;
            }
    }
    else
    {
            dbslope = 1.0f/255.0f;
            dbBand = 0.0f;
            for ( i = 0; i < 256; i++, dbBand += dbslope )
            {
                    m_fTableS[i] = dbBand;
            }
    }
    return 1;
}

long CSaturation::DoImageSaturation(unsigned char *pSrcImg,
                                    unsigned char *pDstImg,
                                    unsigned long Height,
                                    unsigned long Width,
                                    unsigned long BytePerLine,
                                    unsigned char JmpP,
                                    unsigned char ShiftR,
                                    unsigned char ShiftG,
                                    unsigned char ShiftB)
{
        unsigned long		i;
        unsigned long		j;
        unsigned long		k;

        unsigned char	*pOrgImg = NULL;
        unsigned char	*pResImg = NULL;
        unsigned char	R = 0;
        unsigned char	G = 0;
        unsigned char	B = 0;

        //Initial Image Parameter
        pOrgImg	= pSrcImg;
        pResImg	= pDstImg;



        unsigned char MaxCh;
        unsigned char MinCh;
        unsigned char MidCh;

        long    sum;
        double  delta;
        double  delta_mid_min;
        double  S;
        double  NewS;
        double  max;
        double  min;
        double  mid;


        for ( j = 0; j < Height; j++ )
        {
                k=0;
                for ( i = 0; i < Width; i++, k += JmpP )
                {

                        R = pOrgImg[k+ShiftR];
                        G = pOrgImg[k+ShiftG];
                        B = pOrgImg[k+ShiftB];

                        if ( R > G )  // find max of R,G,B
                        {
                                if ( R > B )
                                {
                                        max = R;
                                        MaxCh = ShiftR;
                                        if ( G>B) // R > G > B
                                        {
                                                mid = G;
                                                MidCh = ShiftG;
                                                min = B;
                                                MinCh = ShiftB;
                                        }
                                        else // R > B > G
                                        {
                                                mid = B;
                                                MidCh = ShiftB;
                                                min = G;
                                                MinCh = ShiftG;
                                        }
                                }
                                else // B > R > G
                                {
                                        max = B;
                                        MaxCh = ShiftB;
                                        mid = R;
                                        MidCh = ShiftR;
                                        min = G;
                                        MinCh = ShiftG;
                                }
                        }
                        else //G > R
                        {
                                if ( G>B )
                                {
                                        max = G;
                                        MaxCh = ShiftG;
                                        if ( R>B ) // G > R > B
                                        {
                                                mid = R;
                                                MidCh = ShiftR;
                                                min = B;
                                                MinCh = ShiftB;
                                        }
                                        else	// G > B > R
                                        {
                                                min = R;
                                                MinCh = ShiftR;
                                                mid = B;
                                                MidCh = ShiftB;
                                        }
                                }
                                else // B > G > R
                                {
                                        max = B;
                                        MaxCh = ShiftB;
                                        mid = G;
                                        MidCh = ShiftG;
                                        min = R;
                                        MinCh = ShiftR;
                                }
                        }
                        sum = (double)(max + min);
                        delta = max - min;
                        delta_mid_min = mid - min;

                        S = m_fInvSumTB[ sum ] * m_nMul255TB[ (long)delta ];

                        NewS = m_fTableS[(int)S];

                        if ( NewS < 0.00001)
                        {
                                max = mid = min = 0.5*sum;
                                goto Label_END;
                        }
                        if ( sum < 255)
                        {
                                max = 0.5 * sum * ( 1.0 + NewS );
                        }
                        else {
                                //S /= ((1.0-L)*2);
                                max = NewS * 255 + (1.0-NewS) * 0.5 * sum;
                        }                        
                        min = sum - max;
                        mid = min + delta_mid_min / delta * ( max - min );

Label_END:
                        pResImg[ k + MaxCh ] = (unsigned char)( max + 0.5);
                        pResImg[ k + MidCh ] = (unsigned char)( mid + 0.5);
                        pResImg[ k + MinCh ] = (unsigned char)( min + 0.5);
                }
                if (m_bStop)
                {
                    return 0;//HT_RETURN_GENERAL_ERROR;
                }
                pOrgImg += BytePerLine;
                pResImg += BytePerLine;
        }
        return 1;//HT_RETURN_SUCCESS;//NO_ERROR;
}
