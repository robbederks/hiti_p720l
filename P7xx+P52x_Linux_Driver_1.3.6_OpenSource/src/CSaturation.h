#ifndef CSATURATION_20090721154811_H
#define CSATURATION_20090721154811_H


#ifdef WIN32
class CSaturation
#else
class __attribute__((__visibility__("hidden"))) CSaturation
#endif
{
public:
    CSaturation();
    virtual ~CSaturation();

    void StopProcess();
    long MakeSaturationTable( unsigned char RealV );
    long DoImageSaturation( unsigned char *pSrcImg,
                            unsigned char *pDstImg,
                            unsigned long Height,
                            unsigned long Width,
                            unsigned long BytePerLine,
                            unsigned char JmpP,
                            unsigned char ShiftR,
                            unsigned char ShiftG,
                            unsigned char ShiftB);


private:
    long    *m_nMul255TB;
    double  *m_fInvSumTB;
    double  *m_fTableS;
    bool    m_bStop;

    void    MemoryFree();
    long    InitialParameter();
};

#endif // CSATURATION_20090721154811_H
