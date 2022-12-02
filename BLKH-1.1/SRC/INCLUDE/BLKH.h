#ifndef _BLKH_H
#define _BLKH_H

#include "LKH.h"
#include <sys/time.h>
#include <sys/resource.h>

int TwoMax(), BBSSP(int Low), BBSSPA(int Low), BSCSSP(int Low), BAP(int Low);
int SolveBTSP(int LowerBound, const char* lkhExecPath);
GainType SolveTSP(int Dimension, char *ParFileName,
                  char *TourFileName, int *Tour, const char* lkhExecPath);
int SolveTransformedTSP(int Low, int High, int *Tour, GainType *Cost, const char* lkhExecPath);
double GetTimeUsage(int who);

#endif
