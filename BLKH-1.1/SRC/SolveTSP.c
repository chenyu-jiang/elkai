#include "BLKH.h"
#include <unistd.h>

/*
 * The SolveTSP function solves a TSP instance using LKH.
 *
 * Parameters
 *   Dimension:    The number of nodes in the instance.
 *   ParFileName:  Name of the parameter file.
 *   TourFileName: Name of a temporary tour file.
 *   Tour:         The solution tour. 
 *   
 * The return value is the cost of the solution tour.
 */
GainType SolveTSP(int Dimension, char *ParFileName,
                  char *TourFileName, int *Tour, const char* lkhExecPath)
{
    GainType Cost;
    FILE *p, *TourFile;
    int i;
    char Command[256], Key[256], Buffer[256], *Line, *Keyword;
    char Delimiters[] = " :=\n\t\r\f\v\xef\xbb\xbf";

    sprintf(Command, "%s %s", lkhExecPath, ParFileName);
    assert(p = popen(Command, "r"));
    Cost = PLUS_INFINITY;
    while (fgets(Buffer, sizeof(Buffer), p)) {
        if (TraceLevel > 1)
            printff("%s", Buffer);
        if (sscanf(Buffer, "%s", Key) > 0) {
            GainType LocalCost;
            int LocalTrial;
            double LocalTime;
            if (!strcmp(Key, "Cost.min")) {
                char *cp = strchr(Buffer + strlen(Key), '=');
                sscanf(cp + 1, GainInputFormat, &Cost);
            } else if (TraceLevel > 0) {
                if (!strcmp(Key, "Run")) {
                    char *cp = Buffer + strlen(Key);
                    sscanf(cp + 1, "%d", &Run);
                    sscanf(cp + 1, "%d", &LocalTrial);
                    cp = strchr(cp + 1, '=');
                    sscanf(cp + 1, GainInputFormat, &LocalCost);
                    cp = strchr(cp + 1, 'T');
                    cp = strchr(cp + 1, '=');
                    sscanf(cp + 1, "%lf", &LocalTime);
                    printff("Run %d: Cost = " GainFormat ", ",
                            Run, LocalCost);
                    printff("Time = %0.2f sec.\n\n", LocalTime);
                } else if (!strcmp(Key, "*")) {
                    char *cp = Buffer + strlen(Key);
                    if (sscanf(cp + 1, "%d", &LocalTrial) > 0) {
                        cp = strchr(cp + 1, '=');
                        sscanf(cp + 1, GainInputFormat, &LocalCost);
                        cp = strchr(cp + 1, 'T');
                        cp = strchr(cp + 1, '=');
                        sscanf(cp + 1, "%lf", &LocalTime);
                        printff("# %d: Cost = " GainFormat ", ",
                                LocalTrial, LocalCost);
                        printff("Time = %0.2f sec.\n", LocalTime);
                    }
                }
            } else if (!strcmp(Key, "Run")) {
                char *cp = Buffer + strlen(Key);
                sscanf(cp + 1, "%d", &Run);
            }
        }
    }
    pclose(p);
    if (!(TourFile = fopen(TourFileName, "r")))
        return Cost;
    while ((Line = ReadLine(TourFile))) {
        if (!(Keyword = strtok(Line, Delimiters)))
            continue;
        for (i = 0; i < strlen(Keyword); i++)
            Keyword[i] = (char) toupper(Keyword[i]);
        if (!strcmp(Keyword, "TOUR_SECTION"))
            break;
    }
    for (i = 1; i <= Dimension; i++)
        fscanf(TourFile, "%d", &Tour[i]);
    Tour[0] = Tour[Dimension];
    fclose(TourFile);
    unlink(TourFileName);
    return Cost;
}
