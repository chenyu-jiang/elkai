#include "BLKH.h"

/*
 * The SolveBTSP function solves the given BTSP instance. The lower bound
 * is passed as argument. The return value is the cost of the bottleneck edge.
 *
 * At return the corresponding tour and its cost are stored in the two
 * global variables BestTour and BestCost.
 */

int SolveBTSP(int LowerBound, const char* lkhExecPath)
{
    int Low = LowerBound, High = INT_MAX, Bottleneck, B, *Tour;
    GainType Cost;

    Bottleneck = High = SolveTransformedTSP(Low, High, BestTour, &BestCost, lkhExecPath);
    assert(Tour = (int *) malloc((DimensionSaved + 1) * sizeof(int)));
    while (Low < High && Bottleneck != LowerBound && Bottleneck != Optimum) {
        if (TraceLevel > 0)
            printff("Low = %d, High = %d\n", M - Low, M - High);
        B = SolveTransformedTSP(Low, High, Tour, &Cost, lkhExecPath);
        if (B < Bottleneck) {
            Bottleneck = High = B;
            if (TraceLevel > 0)
                printff("Bottleneck = %d\n", M - Bottleneck);
            memcpy(BestTour, Tour, (DimensionSaved + 1) * sizeof(int));
            BestCost = Cost;
            if (High <= Low)
                Low = LowerBound + (High - LowerBound) / 2;
            else if (MSTSP)
                High = Low + (High - Low + 1) / 2;
        } else {
            High = Bottleneck;
            Low += (High - Low + 1) / 2;
        }
    }
    free(Tour);
    if (TraceLevel > 0)
        printff("*** Bottleneck = %d\n", M - Bottleneck);
    return Bottleneck;
}
