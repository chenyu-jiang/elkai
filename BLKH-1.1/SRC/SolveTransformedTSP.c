#include "BLKH.h"
#include "Heap.h"
#include <unistd.h>

extern int BLKHHeapCount, BLKHHeapCapacity;

/* 
 * The SolveTransformedTSP function transforms the BTSP instance into a 
 * TSP/ATSP instance specified by a cost matrix whose entries are
 *  
 *    zero, if C[i][j] <= Low
 *    unchanged, if Low < C[i][j] < High
 *    C[i][j] + INT_MAX / 4, if C[i][i] > High
 *
 * where C[i][j] is the original cost of edge (i, j).
 *
 * The transformed TSP/ATSP is solved by LKH (by calling SolveTSP).
 * The parameters given to LKH are taken from the parameters to BLKH.
 * However, if High = INT_MAX, the following parameters are used
 *
 *       CANDIDATE_FILE = xxx.cand
 *       GAIN23 = NO
 *       MAX_CANDIDATES = 0
 *       MOVE_TYPE = 3
 *       PATCHING_A = 0;
 *       PATCHING_C = 1
 *       SUBGRADIENT = NO
 *       SUBSEQUENT_MOVE_TYPE = 3
 *
 * where xxx.cand is a generated candidate file that contains all edges 
 * whose transformed costs are zero. By setting MAX_CANDIDATES to 0, these
 * edges will be the only candidate edges used by LKH.
 *
 * The bottleneck cost is returned by the function, at the corresponding
 * tour and its cost are stored in *Tour and *Cost, respectively.
 */

int SolveTransformedTSP(int Low, int High, int *Tour, GainType * Cost, const char* lkhExecPath)
{
    FILE *ParFile, *ProblemFile, *CandFile;
    char ParFileName[256], ProblemFileName[256], CandFileName[256],
        NewTourFileName[256], Prefix[256];
    Node *From, *To, *Top;
    int Bottleneck, d, i, j;
    const int MaxCands = ProblemType != ATSP ? 100 : 1000;

    sprintf(Prefix, "%s.pid%d", Name, getpid());

    /* Create the problem file */
    sprintf(ProblemFileName, "%s.btsp", Prefix);
    assert(ProblemFile = fopen(ProblemFileName, "w"));
    fprintf(ProblemFile, "NAME : %s.btsp\n", Prefix);
    if (Type)
        fprintf(ProblemFile, "TYPE : %s\n", Type);
    if (ProblemType != ATSP)
        fprintf(ProblemFile, "DIMENSION : %d\n", Dimension);
    else
        fprintf(ProblemFile, "DIMENSION : %d\n", DimensionSaved);
    fprintf(ProblemFile, "EDGE_WEIGHT_TYPE : EXPLICIT\n");
    if (ProblemType != ATSP)
        fprintf(ProblemFile, "EDGE_WEIGHT_FORMAT : UPPER_ROW\n");
    else
        fprintf(ProblemFile, "EDGE_WEIGHT_FORMAT : FULL_MATRIX\n");
    fprintf(ProblemFile, "EDGE_WEIGHT_SECTION\n");
    for (i = 1; i <= DimensionSaved; i++) {
        From = &NodeSet[i];
        for (j = ProblemType != ATSP ? i + 1 : 1; j <= DimensionSaved; j++) {
            if (i == j)
                fprintf(ProblemFile, "9999999 ");
            else {
                // printff("Row %d column %d: cost: %d\n", i, j, From->C[j]);
                d = ProblemType != ATSP ? Distance(From, &NodeSet[j])
                    : From->C[j];
                if (d <= Low)
                    d = 0;
                else if (d >= High)
                    d += INT_MAX / 4 / Precision;
                fprintf(ProblemFile, "%d ", d);
                while (d * Precision / Precision != d) {
                    printff("*** PRECISION (= %d) is too large. ",
                            Precision);
                    if ((Precision /= 10) < 1)
                        Precision = 1;
                    printff("Changed to %d.\n", Precision);
                }
            }
        }
    }
    fprintf(ProblemFile, "EOF\n");
    fclose(ProblemFile);

    if (High == INT_MAX) {
        /* Create candidate file */
        BLKHMakeHeap(MaxCands);
        sprintf(CandFileName, "%s.cand", Prefix);
        assert(CandFile = fopen(CandFileName, "w"));
        fprintf(CandFile, "%d\n", Dimension);
        for (From = &NodeSet[i = 1]; i <= Dimension; i++, From++) {
            int Count = 0;
            Top = 0;
            for (To = &NodeSet[j = 1]; j <= Dimension; j++, To++) {
                if (i != j && (To->Rank = Distance(From, To)) <= Low) {
                    To->Prev = Top;
                    Top = To;
                    Count++;
                }
            }
            if (Count <= MaxCands) {
                fprintf(CandFile, "%d 0 %d ", i, Count);
                for (To = Top; To; To = To->Prev)
                    fprintf(CandFile, "%d 0 ", To->Id);
            } else {
                fprintf(CandFile, "%d 0 %d ", i, MaxCands);
                BLKHHeapCount = 0;
                BLKHHeapCapacity = MaxCands;
                for (To = Top; To; To = To->Prev)
                    HeapInsert(To);
                for (j = 1; j <= MaxCands; j++)
                    fprintf(CandFile, "%d 0 ", Heap[j]->Id);
            }
            fprintf(CandFile, "\n");
        }
        free(BLKHHeap);
        fclose(CandFile);
    }

    /* Create the parameter file */
    sprintf(ParFileName, "%s.par", Prefix);
    assert(ParFile = fopen(ParFileName, "w"));
    fprintf(ParFile, "PROBLEM_FILE = %s\n", ProblemFileName);
    fprintf(ParFile, "ASCENT_CANDIDATES = %d\n", AscentCandidates);
    fprintf(ParFile, "BACKBONE_TRIALS = %d\n", BackboneTrials);
    if (Backtracking)
        fprintf(ParFile, "BACKTRACKING  = YES\n");
    for (i = 0; i < CandidateFiles; i++)
        fprintf(ParFile, "CANDIDATE_FILE = %s\n", CandidateFileName[i]);
    fprintf(ParFile, "CANDIDATE_SET_TYPE = ALPHA\n");
    if (Excess > 0)
        fprintf(ParFile, "EXCESS = %g\n", Excess);
    if (!Gain23Used)
        fprintf(ParFile, "GAIN23 = NO\n");
    if (!GainCriterionUsed)
        fprintf(ParFile, "GAIN_CRITERION = NO\n");
    fprintf(ParFile, "INITIAL_PERIOD = %d\n", InitialPeriod);
    if (InitialTourAlgorithm != WALK)
        fprintf(ParFile, "INITIAL_TOUR_ALGORITHM = %s\n",
                InitialTourAlgorithm ==
                NEAREST_NEIGHBOR ? "NEAREST-NEIGHBOR" :
                InitialTourAlgorithm == GREEDY ? "GREEDY" : "");
    fprintf(ParFile, "INITIAL_STEP_SIZE = %d\n", InitialStepSize);
    if (InitialTourFileName)
        fprintf(ParFile, "INITIAL_TOUR_FILE = %s\n", InitialTourFileName);
    fprintf(ParFile, "INITIAL_TOUR_FRACTION = %0.3f\n",
            InitialTourFraction);
    if (InputTourFileName)
        fprintf(ParFile, "INPUT_TOUR_FILE = %s\n", InputTourFileName);
    fprintf(ParFile, "KICK_TYPE = %d\n", KickType);
    fprintf(ParFile, "MAX_BREADTH = %d\n", MaxBreadth);
    fprintf(ParFile, "MAX_CANDIDATES = %d%s\n", MaxCandidates,
            CandidateSetSymmetric ? " SYMMETRIC" : "");
    fprintf(ParFile, "MAX_SWAPS = %d\n", MaxSwaps);
    fprintf(ParFile, "MAX_TRIALS = %d\n", MaxTrials);
    for (i = 0; i < MergeTourFiles; i++)
        fprintf(ParFile, "MERGE_TOUR_FILE = %s\n", MergeTourFileName[i]);
    fprintf(ParFile, "MOVE_TYPE = %d\n", MoveType);
    if (NonsequentialMoveType >= 4)
        fprintf(ParFile, "NONSEQUENTIAL_MOVE_TYPE = %d\n",
                NonsequentialMoveType);
    fprintf(ParFile, "OPTIMUM = 0\n");
    fprintf(ParFile, "PATCHING_A = %d %s\n", PatchingA,
            PatchingARestricted ? "RESTRICTED" :
            PatchingAExtended ? "EXTENDED" : "");
    fprintf(ParFile, "PATCHING_C = %d %s\n", PatchingC,
            PatchingCRestricted ? "RESTRICTED" :
            PatchingCExtended ? "EXTENDED" : "");
    if (PiFileName)
        fprintf(ParFile, "PI_FILE = %s\n", PiFileName);
    fprintf(ParFile, "POPMUSIC_INITIAL_TOUR = %s\n",
            POPMUSIC_InitialTour ? "YES" : "NO");
    fprintf(ParFile, "POPMUSIC_MAX_NEIGHBORS = %d\n", POPMUSIC_MaxNeighbors);
    fprintf(ParFile, "POPMUSIC_SAMPLE_SIZE = %d\n", POPMUSIC_SampleSize);
    fprintf(ParFile, "POPMUSIC_SOLUTIONS = %d\n", POPMUSIC_Solutions);
    fprintf(ParFile, "POPMUSIC_TRIALS = %d\n", POPMUSIC_Trials);
    fprintf(ParFile, "RECOMBINATION = %s\n",
                     Recombination == GPX2 ? "GPX2" : "IPT");
    fprintf(ParFile, "POPULATION_SIZE = %d\n", MaxPopulationSize);
    fprintf(ParFile, "PRECISION = %d\n", Precision);
    if (!RestrictedSearch)
        fprintf(ParFile, "RESTRICTED_SEARCH = NO\n");
    fprintf(ParFile, "RUNS = %d\n", Runs);
    fprintf(ParFile, "SEED = %d\n", Seed);
    if (!StopAtOptimum)
        fprintf(ParFile, "STOP_AT_OPTIMUM = NO\n");
    if (!Subgradient)
        fprintf(ParFile, "SUBGRADIENT = NO\n");
    if (SubproblemSize > 0)
        fprintf(ParFile, "SUBPROBLEM_SIZE = %d\n", SubproblemSize);
    if (SubproblemTourFileName)
        fprintf(ParFile, "SUBPROBLEM_TOUR_FILE = %s\n",
                SubproblemTourFileName);
    fprintf(ParFile, "SUBSEQUENT_MOVE_TYPE = %d\n", SubsequentMoveType);
    if (!SubsequentPatching)
        fprintf(ParFile, "SUBSEQUENT_PATCHING = NO\n");
    if (TimeLimit != DBL_MAX)
        fprintf(ParFile, "TIME_LIMIT = %0.1f\n", TimeLimit);
    sprintf(NewTourFileName, "%s.temp.tour", Prefix);
    fprintf(ParFile, "TOUR_FILE = %s\n", NewTourFileName);
    if (TraceLevel == 0)
        fprintf(ParFile, "TRACE_LEVEL = 1\n");

    if (High == INT_MAX) {
        fprintf(ParFile, "CANDIDATE_FILE = %s\n", CandFileName);
        fprintf(ParFile, "GAIN23 = NO\n");
        fprintf(ParFile, "MAX_CANDIDATES = 0\n");
        fprintf(ParFile, "MOVE_TYPE = 3\n");
        fprintf(ParFile, "PATCHING_A = 0\n");
        fprintf(ParFile, "PATCHING_C = 1\n");
        fprintf(ParFile, "RESTRICTED_SEARCH = NO\n");
        fprintf(ParFile, "SUBGRADIENT = NO\n");
        fprintf(ParFile, "SUBSEQUENT_MOVE_TYPE = %d\n",
                ProblemType == ATSP ? 3 : 2);
    }

    fclose(ParFile);

    /* Solve the TSP/ATSP */
    SolveTSP(DimensionSaved, ParFileName, NewTourFileName, Tour, lkhExecPath);
    unlink(ProblemFileName);
    unlink(ParFileName);
    if (High == INT_MAX)
        unlink(CandFileName);

    /* Compute the bottleneck cost of the tour */
    for (i = 1; i <= DimensionSaved; i++)
        NodeSet[Tour[i - 1]].Suc = &NodeSet[Tour[i]];
    Bottleneck = INT_MIN;
    *Cost = 0;
    From = FirstNode;
    do {
        To = From->Suc;
        d = ProblemType != ATSP ? Distance(From, To) : From->C[To->Id];
        if (d > Bottleneck)
            Bottleneck = d;
        *Cost += d;
    } while ((From = To) != FirstNode);
    return Bottleneck;
}
