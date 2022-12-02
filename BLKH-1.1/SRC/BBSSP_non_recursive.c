#include "LKH.h"

static int Delta;
static int *Vidx, *Widx, *MinDepth;

static int Biconnected();

int BBSSP()
{
    int Low = INT_MAX, High = INT_MIN, d, i, j;
    Node *From;

    assert(Vidx = (int *) malloc(Dimension * sizeof(int)));
    assert(Widx = (int *) malloc(Dimension * sizeof(int)));
    assert(MinDepth = (int *) malloc(Dimension * sizeof(int)));
    for (From = &NodeSet[i = 1]; i <= DimensionSaved; i++, From++) {
        for (j = ProblemType != ATSP ? i + 1 : 1; j <= DimensionSaved; j++) {
            if (j == i)
                continue;
            d = ProblemType != ATSP ? Distance(From,
                                               &NodeSet[j]) : From->C[j];
            if (d < Low)
                Low = d;
            if (d > High)
                High = d;
        }
    }
    while (Low < High) {
        Delta = Low + (High - Low) / 2;
        if (TraceLevel > 0)
            printff("BBSSP: Low = %d, High = %d, Delta = %d\n",
                    Low, High, Delta);
        if (Biconnected())
            High = Delta;
        else
            Low = Delta + 1;
    }
    free(Vidx);
    free(Widx);
    free(MinDepth);
    return Low;
}

static int DFS();

#define Depth LastV
static int RootChildren;

static int Biconnected()
{
    int i;

    for (i = 1; i <= DimensionSaved; i++) {
        NodeSet[i].Depth = -1;
        NodeSet[i].Dad = 0;
    }
    if (!DFS())
        return 0;
    for (i = 1; i <= DimensionSaved; i++)
        if (NodeSet[i].Depth == -1)
            return 0;
    return 1;
}

static int min(int a, int b)
{
    return a < b ? a : b;
}

static int DFS()
{
    int M, Top = 0;
    Node *Root = FirstNode, *V, *W;

    RootChildren = 0;

    Vidx[0] = Root->Id;
    Widx[0] = 0;
    MinDepth[0] = Root->Depth = 0;
    while (Top >= 0) {
        V = &NodeSet[Vidx[Top]];
        M = MinDepth[Top];
        if (++Widx[Top] > DimensionSaved) {
            if (--Top >= 0) {
                V = &NodeSet[Vidx[Top]];
                if (V != Root ? M >= V->Depth : ++RootChildren > 1)
                    return 0;
                if (M < MinDepth[Top])
                    MinDepth[Top] = M;
            }
        } else {
            W = &NodeSet[Widx[Top]];
            if (W != V && W->Depth < M && W != V->Dad &&
                (ProblemType != ATSP ?
                 (!c || c(V, W) <= Delta) && Distance(V, W) <= Delta :
                 min(V->C[V->Id], W->C[V->Id]) <= Delta)) {
                if (W->Depth == -1) {
                    W->Depth = V->Depth + 1;
                    W->Dad = V;
                    Vidx[++Top] = W->Id;
                    Widx[Top] = 0;
                }
                MinDepth[Top] = W->Depth;
            }
        }
    }
    return 1;
}
