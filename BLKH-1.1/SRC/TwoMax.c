#include "BLKH.h"

/*
 * The TwoMax function computes and returns the 2-Max lower bound for the BTSP.
 * This simple bound sobtained as follows:
 *
 * Symmetric BTSP:
 *    For each node N, find the second smallest of an edge incident to N, 
 *    counting multiplicities. Let Min2(N) be the cost of this edge. i
 *    The 2-max bound is defined as the maximum of Min2 for all nodes.
 *
 * Asymmetric BTSP:
 *    For each node N, find the smallest in-edge and out-edge costs and
 *    select the largest of all these values over all nodes.
 *
 * Time complexity: O(n^2), where n is the number of nodes.
 */

int TwoMax()
{
    int Min1, Min2, Max = INT_MIN, d, i, j;
    Node *N;

    for (N = &NodeSet[i = 1]; i <= DimensionSaved; i++, N++) {
        Min1 = Min2 = INT_MAX;
        for (j = 1; j <= DimensionSaved; j++) {
            if (j == i)
                continue;
            if (ProblemType != ATSP) {
                if (c && c(N, &NodeSet[j]) >= Min2)
                    continue;
                if ((d = Distance(N, &NodeSet[j])) < Min1) {
                    Min2 = Min1;
                    Min1 = d;
                } else if (d < Min2)
                    Min2 = d;
                /* Speed-up: */
                if (Min2 <= Max) 
                    break; 
            } else {
                /* In-edge: */
                if ((d = NodeSet[j].C[i]) < Min1) {
                    Min1 = d;
                    /* Speed-up: */
                    if (Min1 <= Max && Min2 <= Max)
                        break;
                }
                /* Out-edge: */
                if ((d = N->C[j]) < Min2) {
                    Min2 = d;
                    /* Speed-up: */
                    if (Min1 <= Max && Min2 <= Max)
                        break;
                }
            }
        }
        if (Min2 > Max)
            Max = Min2;
        if (Min1 > Max)
            Max = Min1;
    }
    return Max;
}
