#include "the3.h"

// do not add extra libraries here


int toIndex(std::string s, int length, int i, int l) {

    int index = 0;
    int lower = length - (i + 1) * l;
    int upper = length - i * l;
    for (int m = lower; m < upper; m++) {
        if (m < 0) continue;
        index = index * 26 + s[m] - 'A';
    }

    return index;
}

/*
    arr       : array to be sorted, in order to get points this array should contain be in sorted state before returning
    ascending : true for ascending, false for descending 
    n         : number of elements in the array
    l         : the number of characters used in counting sort at each time
    
    you can use ceil function from cmath
    
*/
int radixSort(std::string arr[], bool ascending, int n, int l){
    
    if (n <= 0)
        return 0;

    int k = pow(26, l);
    int iterations = 0;
    int wordLength = arr[0].size();
    int numberOfSubstrings = ceil((float) wordLength / l);
    
    for (int nos = 0; nos < numberOfSubstrings ; nos++) {
        
        int *C = new int[k]();

        std::string *brr = new std::string[n];

        for (int i = 0; i < n; i++) {
            C[toIndex(arr[i], wordLength, nos, l)]++;
            iterations++;
        }

        if (ascending) {
            iterations++;
            for (int j = 1; j < k; j++) {
                C[j] += C[j - 1];
                iterations++;
            }
        } else {
            iterations++;
            for (int j = k - 2; j >= 0; j--) {
                C[j] += C[j + 1];
                iterations++;
            }
        }

        for (int i = n - 1; i >= 0; i--) {
            int index = toIndex(arr[i], wordLength, nos, l);
            brr[C[index]-1] = arr[i];
            C[index]--;
            iterations++;
        }

        for (int i = 0; i < n; i++) {
            arr[i] = brr[i];
            iterations++;
        }

        delete[] brr;
        delete[] C;
    }
    
    return iterations;
}
