#ifndef _BLKHHEAP_H
#define _BLKHHEAP_H

/* 
 * This header specifies the interface for the use of heaps. 
 */

#include "LKH.h"

void BLKHMakeHeap(int Size);
void BLKHHeapInsert(Node * N);
void BLKHHeapDelete(Node * N);
Node *BLKHHeapDeleteMin(void);
void BLKHHeapLazyInsert(Node * N);
void BLKHHeapify(void);
void BLKHHeapSiftUp(Node * N);
void BLKhHeapSiftDown(Node * N);

#endif
