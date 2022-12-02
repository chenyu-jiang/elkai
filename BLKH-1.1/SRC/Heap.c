#include "LKH.h"
#include "BLKHHeap.h"

/*
 * A binary heap is used to implement a priority queue. 
 *
 * A heap is useful in order to speed up the computations of minimum 
 * spanning trees. The elements of the heap are the nodes, and the
 * priorities (ranks) are their associated costs (their minimum distance 
 * to the current tree). 
 */

int BLKHHeapCount = -1;    /* Its current number of elements */
int BLKHHeapCapacity = -1; /* Its capacity */

/*
 * The MakeHeap function creates an empty heap. 
 */

void BLKHMakeHeap(int Size)
{
    assert(BLKHHeap = (Node **) malloc((Size + 2) * sizeof(Node *)));
    BLKHHeapCapacity = Size;
    BLKHHeapCount = 0;
}

/*
 * The HeapSiftUp function is called when the rank of a node is decreased, 
 * or when a node is inserted into the heap.
 * The function moves the node forward in the heap (the foremost node
 * of the heap has the lowest rank).
 * When calling HeapSiftUp(N), node N must belong to the heap.              
 */

void BLKHHeapSiftUp(Node * N)
{
    int Loc = N->Loc, Parent = Loc / 2;

    while (Parent && N->Rank < BLKHHeap[Parent]->Rank) {
        BLKHHeap[Loc] = BLKHHeap[Parent];
        BLKHHeap[Loc]->Loc = Loc;
        Loc = Parent;
        Parent /= 2;
    }
    BLKHHeap[Loc] = N;
    N->Loc = Loc;
}

/*
 * The HeapSiftDown function is called by the Heapify and HeapDeleteMin 
 * functions. The function moves the node backwards in the heap 
 * (the foremost node of the heap has the lowest rank).
 * When calling HeapSiftDown(N), node N must belong to the heap.              
 */

void BLKHHeapSiftDown(Node * N)
{
    int Loc = N->Loc, Child;

    while (Loc <= BLKHHeapCount / 2) {
        Child = 2 * Loc;
        if (Child < BLKHHeapCount && BLKHHeap[Child + 1]->Rank < BLKHHeap[Child]->Rank)
            Child++;
        if (N->Rank <= BLKHHeap[Child]->Rank)
            break;
        BLKHHeap[Loc] = BLKHHeap[Child];
        BLKHHeap[Loc]->Loc = Loc;
        Loc = Child;
    }
    BLKHHeap[Loc] = N;
    N->Loc = Loc;
}

/*       
 * The HeapDeleteMin function deletes the foremost node from the heap. 
 * The function returns a pointer to the deleted node (0, if the heap
 * is empty).
 */

Node *BLKHHeapDeleteMin()
{
    Node *Remove;

    if (!BLKHHeapCount)
        return 0;
    Remove = BLKHHeap[1];
    BLKHHeap[1] = BLKHHeap[BLKHHeapCount--];
    BLKHHeap[1]->Loc = 1;
    BLKHHeapSiftDown(BLKHHeap[1]);
    Remove->Loc = 0;
    return Remove;
}

/*       
 * The HeapInsert function inserts a node N into the heap.
 * When calling HeapInsert(N), node N must not belong to the heap.
 */

void BLKHHeapInsert(Node * N)
{
    BLKHHeapLazyInsert(N);
    BLKHHeapSiftUp(N);
    if (BLKHHeapCount > BLKHHeapCapacity) {
        BLKHHeapCount = BLKHHeapCapacity;
        N->Loc %= BLKHHeapCapacity;
    }
}

/*
 * The HeapDelete function deletes a node N from the heap.
 */

void BLKHHeapDelete(Node * N)
{
    int Loc = N->Loc;
    if (!Loc)
        return;
    BLKHHeap[Loc] = BLKHHeap[BLKHHeapCount--];
    BLKHHeap[Loc]->Loc = Loc;
    if (BLKHHeap[Loc]->Rank > N->Rank)
        BLKHHeapSiftDown(BLKHHeap[Loc]);
    else
        BLKHHeapSiftUp(BLKHHeap[Loc]);
    N->Loc = 0;
}

/*       
 * The HeapLazyInsert function inserts a node as the last node of the heap.
 * This may destroy the heap condition, but it can later be restored by 
 * calling the Heapify function.
 * When calling HeapLazyInsert(N), node N must not belong to the heap.
 */

void BLKHHeapLazyInsert(Node * N)
{
    assert(BLKHHeapCount <= BLKHHeapCapacity);
    BLKHHeap[++BLKHHeapCount] = N;
    N->Loc = BLKHHeapCount;
}

/*       
 * The Heapify function constructs a heap from its nodes.
 */

void BLKHHeapify()
{
    int Loc;
    for (Loc = BLKHHeapCount / 2; Loc >= 1; Loc--)
        BLKHHeapSiftDown(BLKHHeap[Loc]);
}
