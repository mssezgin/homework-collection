#include "the5.h"
// do not add extra libraries here

/*
W: the width of the field
L: the length of the field
partitions: 2D Boolean array for partitions, if a partition of w x l exists then partitions[x][y] is true.
numberOfCalls/numberOfIterations: method specific control parameter
*/

int recursiveMethod(int W, int L, bool** partitions, int* numberOfCalls) {
    (*numberOfCalls)++;
    if (partitions[W][L]) {
        return 0;
    }
    int minWasted = W * L;
    for (int M = 1; M <= W/2; M++) {
        int currWasted = recursiveMethod(M, L, partitions, numberOfCalls) + recursiveMethod(W-M, L, partitions, numberOfCalls);
        if (currWasted < minWasted) {
            minWasted = currWasted;
        }
    }
    for (int N = 1; N <= L/2; N++) {
        int currWasted = recursiveMethod(W, N, partitions, numberOfCalls) + recursiveMethod(W, L-N, partitions, numberOfCalls);
        if (currWasted < minWasted) {
            minWasted = currWasted;
        }
    }
    return minWasted;
}

int memoizationMethodH(int W, int L, bool** partitions, int* numberOfCalls, int** memo) {
    (*numberOfCalls)++;
    if (memo[W][L] == -1) {
        if (partitions[W][L]) {
            memo[W][L] = 0;
        } else {
            int minWasted = W * L;
            for (int M = 1; M <= W/2; M++) {
                int currWasted = memoizationMethodH(M, L, partitions, numberOfCalls, memo) + memoizationMethodH(W-M, L, partitions, numberOfCalls, memo);
                if (currWasted < minWasted) {
                    minWasted = currWasted;
                }
            }
            for (int N = 1; N <= L/2; N++) {
                int currWasted = memoizationMethodH(W, N, partitions, numberOfCalls, memo) + memoizationMethodH(W, L-N, partitions, numberOfCalls, memo);
                if (currWasted < minWasted) {
                    minWasted = currWasted;
                }
            }
            memo[W][L] = minWasted;
        }
    }
    return memo[W][L];
}

int memoizationMethod(int W, int L, bool** partitions, int* numberOfCalls) {
    (*numberOfCalls)++;
    int** memo = new int*[W+1];
    for (int i = 0; i <= W; i++) {
        memo[i] = new int[L+1];
        for (int j = 0; j <= L; j++) {
            memo[i][j] = -1;
        }
    }
    int result = memoizationMethodH(W, L, partitions, numberOfCalls, memo);
    for (int i = 0; i <= W; i++) {
        delete[] memo[i];
    }
    delete[] memo;
    return result;
}

int bottomUpMethod(int W, int L, bool** partitions, int* numberOfIterations) {

    int** memo = new int*[W+1];
    for (int i = 0; i <= W; i++) {
        memo[i] = new int[L+1];
        for (int j = 0; j <= L; j++) {
            memo[i][j] = W*L;
        }
    }

    for (int i = 1; i <= W; i++) {
        for (int j = 1; j <= L; j++) {

            if (partitions[i][j]) {
                memo[i][j] = 0;
            } else {
                int minWasted = i * j;
                for (int M = 1; M <= i; M++) {
                    (*numberOfIterations)++;
                    int currWasted = memo[M][j] + memo[i-M][j];
                    if (currWasted < minWasted) {
                        minWasted = currWasted;
                    }
                }
                for (int N = 1; N <= j; N++) {
                    (*numberOfIterations)++;
                    int currWasted = memo[i][N] + memo[i][j-N];
                    if (currWasted < minWasted) {
                        minWasted = currWasted;
                    }
                }
                memo[i][j] = minWasted;
            }
        }
    }

    int result = memo[W][L];
    for (int i = 0; i <= W; i++) {
        delete[] memo[i];
    }
    delete[] memo;
    return result;
}
