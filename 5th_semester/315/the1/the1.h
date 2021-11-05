#ifndef __THE1_H__
#define __THE1_H__

/**
 * sillySort() should count the number of comparison and swap executed during
 * sorting proccess (Comparisons are only between the values to be sorted only,
 * not your auxiliary comparisons) and return the number of calls of
 * sillySort() (which is 1 in minimum).
 * 
 * Silly sorting algortihm (sillySort()) is as follows:
 * 
 *  - assume the input array A[1..N] is divided into 4 quarters as q1=A[1..N/4],
 *    q2=A[N/4+1..N/2], q3=A[N/2+1..3N/4], q4=A[3N/4+1..N]
 * 
 *  - do 6 recursive calls as follows when N>=4 otherwise sort the list with
 *    N<4 elements directly.
 *    1. sillysort: q1 and q2 (sillysort A[1..N/2])
 *    2. sillysort: q2 and q3 (sillysort A[N/4+1..3N/4])
 *    3. sillysort: q3 and q4 (sillysort A[N/2+1..N])
 *    4. sillysort: q1 and q2 (sillysort A[1..N/2])
 *    5. sillysort: q2 and q3 (sillysort A[N/4+1..3N/4])
 *    6. sillysort: q1 and q2 (sillysort A[1..N/2])
 * 
 *  - when the input size  N<=2 no recursion. (do nothing for N=0 or 1 and
 *    just apply swap when N =2)
 *  - to make things simpler we will only use N as a power of 2 on our tests
 *    (although not necessary with non-rec termination conditions).
 *  - It is an in-place algorithm, so no merging is needed. Nothing else is
 *    needed after the recursive calls.
 *  - count the swap between any 2 elements of the array A, such as swapping A[i] and A[j].
 *  - count the comparison between any 2 elements of the array A, such as A[i]>A[j]
 *  - return the total number of calls to sillySort()
 */
int sillySort(int* arr, long &comparison, long &swap, int size);

/**
 * crossMergeSort() should count the number of comparison executed during
 * sorting proccess (Comparisons are only between the values to be sorted only,
 * not your auxiliary comparisons) and return the number of calls of
 * crossMergeSort() (which is 1 in minimum).
 * 
 * Cross merge sort (crossMergeSort()) is a variation of k-way merge sort,
 * where k is 4 and the partitions are merged in a different order:
 * 
 *  - Assume the input array has N elements which is a power of 2. If the
 *    input array A[1..N] has more than or equal to 4 elements it is divied
 *    into 4 quarters as q1=A[1..N/4], q2=A[N/4+1..N/2], q3=A[N/2+1..3N/4],
 *    q4=A[3N/4+1..N]
 * 
 *  - do 4 recursive calls as follows:
 *    1. cross merge sort q1
 *    2. cross merge sort q2
 *    3. cross merge sort q3
 *    4. cross merge sort q4
 * 
 * Then,
 *  - merge q1 and q3 into h1
 *  - merge q2 and q4 into h2
 *  - merge h1 and h2 into resulting array
 * 
 *  - If the input array has exactly 2 elements, just compare these elements
 *    and swap if necessary.
 *  - If the input array has exactly 1 element, do nothing.
 */
int crossMergeSort(int *arr, long &comparison, int size);

#endif // __THE1_H__