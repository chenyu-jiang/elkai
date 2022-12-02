#include "BLKH.h"

/*
 * The BAP function computes a lower bound for the asymmetric BTSP by 
 * solving the linear bottleneck assignment problem. 
 * The function uses the Ford-Fulkerson algorithm for searching for a 
 * maximal flow in a network.
 * The search for optimum is governed by a threshold algorithm.
 *  
 * Time complexity: O(n^2.5), where n is the number of nodes.
 */
static int InitializeResiduals(int **Res, Node * S, Node * T, int MaxCost);
static int MaxFlow(int **Res, Node * S, Node * T, int Stop);
static int FordFulkersonLabeling(int **Res, Node * S, Node * T);

int BAP(int Low)
{
    int Delta, Flow, m, n, Stop, i, High = INT_MAX;
    int **Res;
    Node *S = &NodeSet[0], *T = &NodeSet[Dimension + 1];

    n = Dimension + 2;
    assert(Res = (int **) malloc(n * sizeof(int *)));
    for (i = 0; i < n; i++)
        assert(Res[i] = (int *) malloc(n * sizeof(int *)));
    Delta = Low;
    while (Low < High) {
        m = InitializeResiduals(Res, S, T, Delta);
        Stop = (int) (2 * pow(n, 2.0 / 3.0) + 0.5);
        m = (int) (sqrt(m * 1.0) + 0.5);
        if (m < Stop)
            Stop = m;
        Flow = MaxFlow(Res, S, T, Stop);
        if (Flow < DimensionSaved)
            Flow += FordFulkersonLabeling(Res, S, T);
        if (Flow == DimensionSaved) 
            High = Delta;
        else
            Low = Delta + 1;
        Delta = Low + (High - Low) / 2;
    }
    for (i = 0; i < n; i++)
        free(Res[i]);
    free(Res);
    return Low;
}

#define Dist LastV

static int InitializeResiduals(int **Res, Node * S, Node * T, int MaxCost)
{
    int Edges = Dimension, s = S->Id, t = T->Id, n, Cost, i, j;

    n = Dimension + 2;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++)
            Res[i][j] = 0;
        NodeSet[i].Dad = 0;
    }
    n = DimensionSaved;
    for (i = 1; i <= n; i++) {
        Node *From = &NodeSet[i];
        for (j = 1; j <= n; j++) {
            if (i != j && (Cost = From->C[j]) <= MaxCost) {
                Res[i][j + n] = 1;
                Edges++;
            }
        }
        Res[s][i] = Res[i + n][t] = 1;
        From->Dist = 2;
        NodeSet[i + n].Dist = 1;
    }
    S->Dist = 3;
    T->Dist = 0;
    return Edges;
}

static int MaxFlow(int **Res, Node * S, Node * T, int Stop)
{
    Node *From, *To, *U, *V;
    int n = Dimension + 2, Flow, MinD, Admissable, j;

    Flow = 0;
    From = S;
    while (S->Dist < Stop) {
        MinD = n + 1;
        Admissable = 0;
        for (j = 0; j < n; j++) {
            To = &NodeSet[j];
            if (Res[From->Id][j] > 0) {
                if (From->Dist == To->Dist + 1) {
                    Admissable = 1;
                    To->Dad = From;
                    From = To;
                    if (To == T) {
                        Flow++;
                        U = From;
                        V = U->Dad;
                        while (U != S) {
                            Res[U->Id][V->Id] = 1;
                            Res[V->Id][U->Id] = -1;
                            U = V;
                            V = U->Dad;
                        }
                        From = S;
                    }
                    break;
                } else if (To->Dist + 1 < MinD)
                    MinD = To->Dist + 1;
            }
        }
        if (!Admissable) {
            From->Dist = MinD;
            if (From != S)
                From = From->Dad;
        }
    }
    return Flow;
}

#define Label V

static int FordFulkersonLabeling(int **Res, Node * S, Node * T)
{
    int i, j, n = Dimension + 2, Flow = 0;
    Node *From, *To, *Top = 0, *U, *V;

    T->Label = 1;
    while (T->Label) {
        for (i = 0; i < n; i++) {
            NodeSet[i].Label = 0;
            NodeSet[i].Dad = 0;
        }
        S->Label = 1;
        S->Prev = Top;
        Top = S;
        while (Top) {
            From = Top;
            Top = Top->Prev;
            for (j = 0; j < n; j++) {
                To = &NodeSet[j];
                if (!To->Label && Res[From->Id][j] > 0) {
                    To->Label = 1;
                    To->Dad = From;
                    To->Prev = Top;
                    Top = To;
                }
            }
        }
        if (T->Label) {
            Flow++;
            U = T;
            V = U->Dad;
            while (U != S) {
                Res[U->Id][V->Id] = 1;
                Res[V->Id][U->Id] = -1;
                U = V;
                V = U->Dad;
            }
            From = S;
        }
    }
    return Flow;
}
