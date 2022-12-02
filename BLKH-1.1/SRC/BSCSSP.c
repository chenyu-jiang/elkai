#include "BLKH.h"

/*
 * The BSCSSPA function computes a lower bound for the asymmetric BTSP by 
 * solving the bottleneck strongly connected spanning subgraph problem. 
 * The function uses Tarjan's algorithm for checking strongly connectivity.
 * The search for optimum is governed by a threshold algorithm.
 * 
 * Time complexity: O(n^2), where n is the number of nodes.
 */

static void Recurse(Node * V);
static int StronglyConnected();

static int Delta;

int BSCSSP(int Low)
{
    int High = INT_MAX;
    Delta = Low;
    if (StronglyConnected())
        return Low;
    Low++;
    while (Low < High) {
        Delta = (Low + High) / 2;
        if (StronglyConnected())
            High = Delta;
        else
            Low = Delta + 1;
    }
    return Low;
}

#define pre Loc
#define low Rank
#define sc LastV

static Node *Top;
static int cnt0, cnt1;

static int StronglyConnected()
{
    Node *V;
    int i;

    for (i = 1; i <= DimensionSaved; i++) {
        V = &NodeSet[i];
        V->pre = V->sc = -1;
    }
    cnt0 = cnt1 = 0;
    Top = 0;
    Recurse(FirstNode);
    if (cnt1 > 1)
        return 0;
    for (i = 1; i <= DimensionSaved; i++) {
        V = &NodeSet[i];
        if (V->pre == -1)
            return 0;
    }
    return 1;
}

static void Recurse(Node * V)
{
    int min, i;
    Node *W = FirstNode;

    V->pre = V->low = min = cnt0++;
    V->Prev = Top;
    Top = V;
    for (i = 1; i <= DimensionSaved; i++) {
        W = &NodeSet[i];
        if (W != V && V->C[W->Id] <= Delta) {
            if (W->pre == -1)
                Recurse(W);
            if (W->low < min)
                min = W->low;
        }
    }
    if (min < V->low) {
        V->low = min;
        return;
    }
    do {
        (W = Top)->sc = cnt1;
        Top = Top->Prev;
        W->low = V->Id - 1;
    } while (W != V);
    cnt1++;
}
