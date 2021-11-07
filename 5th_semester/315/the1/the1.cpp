#include "the1.h"


// You can add your own helper functions

int sillySort(int* arr, long &comparison, long &swap, int size) {

    int num_of_calls = 1;
    
    if (size == 2) {
        comparison++;
        if (arr[0] > arr[1]) {
            swap++;
            int temp = arr[0];
            arr[0] = arr[1];
            arr[1] = temp;
        }
    } else if (size >= 4) {
        num_of_calls += sillySort(arr, comparison, swap, size / 2);
        num_of_calls += sillySort(arr + size / 4, comparison, swap, size / 2);
        num_of_calls += sillySort(arr + size / 2, comparison, swap, size / 2);
        num_of_calls += sillySort(arr, comparison, swap, size / 2);
        num_of_calls += sillySort(arr + size / 4, comparison, swap, size / 2);
        num_of_calls += sillySort(arr, comparison, swap, size / 2);
    }
    
    return num_of_calls;
}


void merge(long &comparison, int *targetArr, int *arr1, int size1, int *arr2, int size2) {
    
    int i = 0, j = 0, k = 0;
    
    while (j < size1 && k < size2) {
        comparison++;
        if (arr2[k] < arr1[j]) {
            targetArr[i++] = arr2[k++];
        } else {
            targetArr[i++] = arr1[j++];
        }
    }
    
    while (j < size1) {
        targetArr[i++] = arr1[j++];
    }
    
    while (k < size2) {
        targetArr[i++] = arr2[k++];
    }
}


int crossMergeSort(int *arr, long &comparison, int size) {
    
    int num_of_calls = 1;
    
    if (size == 2) {
        comparison++;
        if (arr[0] > arr[1]) {
            int temp = arr[0];
            arr[0] = arr[1];
            arr[1] = temp;
        }
    } else if (size >= 4) {
        
        int quarterSize = size / 4;
        num_of_calls += crossMergeSort(arr, comparison, quarterSize);
        num_of_calls += crossMergeSort(arr + quarterSize, comparison, quarterSize);
        num_of_calls += crossMergeSort(arr + 2 * quarterSize, comparison, quarterSize);
        num_of_calls += crossMergeSort(arr + 3 * quarterSize, comparison, quarterSize);
        
        int *h1 = new int[quarterSize * 2];
        merge(comparison, h1, arr, quarterSize, arr + 2 * quarterSize, quarterSize);
        int *h2 = new int[quarterSize * 2];
        merge(comparison, h2, arr + quarterSize, quarterSize, arr + 3 * quarterSize, quarterSize);
        
        merge(comparison, arr, h1, 2 * quarterSize, h2, 2 * quarterSize);
        
        delete[] h1;
        delete[] h2;
        h1 = h2 = nullptr;
    }
    
    return num_of_calls;
}
