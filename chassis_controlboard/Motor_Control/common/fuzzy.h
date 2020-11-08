#ifndef __fuzzy_h__
#define __fuzzy_h__

#pragma pack(4)
typedef struct FUZTAB
{
    int cntrow;
    int cntcolume;
    float ptab[10][10];
    float itab[10][10];
    float dtab[10][10];
    float Edot[10];
    float ECdot[10];
} FUZTAB;
extern FUZTAB SpeedTAB;
void fuzzyout(FUZTAB *l_tab,float l_EE,float l_EEC,float *pout,float *iout,float *dout);
#endif
