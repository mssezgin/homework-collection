#include "the2.h"

// 2, 8, 5, 9, 1, 3, 7, 1, 3, 7
// 
// 8, 9, 7, 7, 1, 3, 5, 1, 3, 2
//          i                 j

int partition(unsigned short *arr, long &swap, double &avg_dist, double &max_dist, int size) {
    
    unsigned short pivot = arr[size - 1];
    unsigned short temp;
    int i = -1, j = 0;
    
    while (j < size - 1) {
        
        if (arr[j] < pivot) {
            ++j;
        } else { // arr[j] >= pivot
            
            int dist = j - ++i;
            if (dist > max_dist) {
                max_dist = dist;
            }
            avg_dist = (avg_dist * swap + dist) / ++swap; // change this
            
            temp = arr[i];
            arr[i] = arr[j];
            arr[j++] = temp;
        }
    }
    
    int dist = size - 1 - ++i;
    if (dist > max_dist) {
        max_dist = dist;
    }
    avg_dist = (avg_dist * swap + dist) / ++swap; // change this
    
    arr[size - 1] = arr[i];
    arr[i] = pivot;
    
    return i;
}

// 2, 8, 5, 9, 3, 1, 7, 1, 3, 7
// 
// 7, 8, 5, 9, 3, 7, 1, 1, 3, 2
//                j  i
// 9, 8, 7, 7, 5, 3, 3, 2 1, 1
//                            j   i

int partitionHoare(unsigned short *arr, long &swap, double &avg_dist, double &max_dist, int size) {
    
    unsigned short pivot = arr[(size - 1) / 2];
    unsigned short temp;
    int i = -1, j = size;
    
    /* while (i < j) {
        
        while (arr[--j] <= pivot);
        while (arr[++i] >= pivot);
        
        int dist = j - i;
        if (dist > max_dist) {
            max_dist = dist;
        }
        avg_dist = (avg_dist * swap + dist) / ++swap; // change this
        
        temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    } */
    
    while (true) {
        
        while (arr[--j] < pivot);
        while (arr[++i] > pivot);
        
        if (i < j) {
            int dist = j - i;
            if (dist > max_dist) {
                max_dist = dist;
            }
            avg_dist = (avg_dist * swap + dist) / ++swap; // change this
            
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        } else {
            return j;
        }
    }
}

void quickSort(unsigned short *arr, long &swap, double &avg_dist, double &max_dist, bool hoare, int size) {
    
    if (size > 1) {
        /* int pivotIndex = hoare ? partitionHoare(arr, swap, avg_dist, max_dist, size) : partition(arr, swap, avg_dist, max_dist, size);
        quickSort(arr, swap, avg_dist, max_dist, hoare, pivotIndex);
        quickSort(arr + pivotIndex + 1, swap, avg_dist, max_dist, hoare, size - pivotIndex - 1); */
        
        if (hoare) {
            int pivotIndex = partitionHoare(arr, swap, avg_dist, max_dist, size);
            quickSort(arr, swap, avg_dist, max_dist, hoare, pivotIndex + 1);
            quickSort(arr + pivotIndex + 1, swap, avg_dist, max_dist, hoare, size - pivotIndex - 1);
        } else {
            int pivotIndex = partition(arr, swap, avg_dist, max_dist, size);
            quickSort(arr, swap, avg_dist, max_dist, hoare, pivotIndex);
            quickSort(arr + pivotIndex + 1, swap, avg_dist, max_dist, hoare, size - pivotIndex - 1);
        }
    }

}

//       l     r
// 8, 9, 7, 7, 3, 1, 3, 1, 5, 2
//             i           j     k

// l  r
// 9, 8, 7, 7, 3, 1, 3, 1, 5, 2
//   ij  k

//                      l  r
// 9, 8, 7, 7, 3, 3, 5, 2, 1, 1
//                         i  j  k

//                l  r
// 9, 8, 7, 7, 5, 3, 3, 2, 1, 1
//                j    ik

struct pair {
    int l, r; // arr[l] = arr[l+1] = ... = arr[r-2] = arr[r-1] = pivot
};

//       l     r
// 8, 9, 7, 7, 3, 1, 3, 1, 5, 2
//             i           j     k


// pivot = 3
// comp = 1
// swap = 1
// l  r
// 3, 0
//    ij k

pair partition3(unsigned short *arr, long &swap, long &comparison, int size) {
    
    /* unsigned short pivot = arr[size - 1];
    unsigned short temp;
    int i = -1, j = 0, k = size - 1;
    
    while (j < k) {
        
        if (++comparison, arr[j] < pivot) {
            ++j;
        } else if (++comparison, arr[j] > pivot) {
            ++swap;
            temp = arr[++i];
            arr[i] = arr[j];
            arr[j++] = temp;
        } else {
            ++swap;
            temp = arr[j];
            arr[j] = arr[--k];
            arr[k] = temp;
        }
    }
    
    pair pivotLimits;
    pivotLimits.l = ++i;
    while (k < size) {
        ++swap;
        arr[k++] = arr[i];
        arr[i++] = pivot;
    }
    pivotLimits.r = i;
    
    return pivotLimits; */

    /* unsigned short pivot = arr[size - 1];
    unsigned short temp;
    int i = 0, j = 0, k = size - 1;
    
    while (j < k) {
        
        ++comparison;
        if (arr[j] < pivot) {
            ++j;
        } else if (arr[j] > pivot) {
            ++swap;
            temp = arr[i];
            arr[i++] = arr[j];
            arr[j++] = temp;
        } else {
            ++swap;
            temp = arr[j];
            arr[j] = arr[--k];
            arr[k] = temp;
        }
    }
    
    pair pivotLimits;
    pivotLimits.l = i;
    while (k < size) {
        ++swap;
        arr[k++] = arr[i];
        arr[i++] = pivot;
    }
    pivotLimits.r = i;
    
    return pivotLimits; */

    /* unsigned short pivot = arr[size - 1];
    unsigned short temp;
    int i = 0, j = 0, k = size - 1;
    
    while (j < k) {
        
        if (++comparison, arr[j] > pivot) {
            ++swap;
            temp = arr[i];
            arr[i++] = arr[j];
            arr[j++] = temp;
        } else if (++comparison, arr[j] == pivot) {
            ++swap;
            temp = arr[j];
            arr[j] = arr[--k];
            arr[k] = temp;
        } else {
            ++j;
        }
    }

    // pivot = 18
    // swap = 3
    // comp = 6
    // 
    // 18 18 18 18
    // ijk

    // size = 10
    // 4 5  7 8 7 9 7  6 6 6
    //      i         jk
    // 4 5  7 8 7  6 6 6 6 6
    //      i     jk
    
    pair pivotLimits;
    pivotLimits.l = i;
    pivotLimits.r = i + size - k;

    int m = size - k;
    if (k - i < m) {
        m = k - i;
    }
    k = size - m;
    swap += m;
    while (m--) {
        arr[k++] = arr[i];
        arr[i++] = pivot;
    }
    
    return pivotLimits; */

    unsigned short temp;
    int i = 0, j = 0, p = size - 1;
    
    while (i < p) {
        
        if (++comparison, arr[i] > arr[size - 1]) {
            ++swap;
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            ++i, ++j;
        } else if (++comparison, arr[i] == arr[size - 1]) {
            --p;
            ++swap;
            temp = arr[i];
            arr[i] = arr[p];
            arr[p] = temp;
        } else {
            ++i;
        }
    }
    
    pair pivotLimits;
    pivotLimits.l = j;
    pivotLimits.r = j + size - p;

    int m = size - p;
    if (p - j < m) {
        m = p - j;
    }
    p = size - m;
    swap += m;
    while (m) {
        temp = arr[j];
        arr[j] = arr[p];
        arr[p] = temp;
        ++j, ++p, --m;
    }
    
    return pivotLimits;
}

void quickSort3(unsigned short *arr, long &swap, long &comparison, int size) {
    
    if (size > 1) {
        pair pivotLimits = partition3(arr, swap, comparison, size);
        quickSort3(arr, swap, comparison, pivotLimits.l);
        quickSort3(arr + pivotLimits.r, swap, comparison, size - pivotLimits.r);
    }
}
