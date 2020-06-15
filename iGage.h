#ifndef _KSOFT_IGAGE_H
#define _KSOFT_IGAGE_H

enum GAGE_MODE{GAGE_MODE_LSC, GAGE_MODE_MIC, GAGE_MODE_MCC};

bool _stdcall KS_GageCircle(GAGE_MODE iGageMode, 
				   const double *pDataX, const double *pDataY, long nDataSize,
				   double *pXc, double *pYc, double *pRc, double *pRoundness=0, 
				   int nFilterTimes=0, double dFilerWidth=10, bool bUseKS=false);

#endif