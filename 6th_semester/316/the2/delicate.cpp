#include <iostream>


size_t N;
int *balloons;
int **S;
size_t **K;


void construct() {

    std::cin >> N;

    balloons = new int[N];
    S = new int*[N];
    K = new size_t*[N];

    for (size_t i = 0; i < N; i++) {
        std::cin >> balloons[i];
        S[i] = new int[N]();
        K[i] = new size_t[N]();
    }
}


void destruct() {

    delete[] balloons;
    for (size_t i = 0; i < N; i++) {
        delete[] S[i];
        delete[] K[i];
    }
    delete[] S;
    delete[] K;
}


void solveDynamically() {

    for (size_t d = 2; d < N; d += 3) {
        for (size_t i = 0; i < N - d; i++) {
            size_t j = i + d;
            int maxPointsSij = 0;
            size_t maxPointsK = 0;
            for (size_t k = i + 1; k < j; k += 3) {
                int points = S[i + 1][k - 1] + S[k + 1][j - 1] + balloons[i] * balloons[k] * balloons[j];
                if (points > maxPointsSij) {
                    maxPointsSij = points;
                    maxPointsK = k;
                }
            }
            if (d > 2) {
                int points = S[i][i + 2] + S[i + 3][j];
                if (points > maxPointsSij) {
                    maxPointsSij = points;
                    maxPointsK = N;
                }
                points = S[i][j - 3] + S[j - 2][j];
                if (points > maxPointsSij) {
                    maxPointsSij = points;
                    maxPointsK = N + 1;
                }
            }
            S[i][j] = maxPointsSij;
            K[i][j] = maxPointsK;
        }
    }
}


void popAll(size_t i, size_t j) {
    if (i >= j) {
        return;
    }
    size_t k = K[i][j];
    if (k == N) {
        popAll(i, i + 2);
        popAll(i + 3, j);
    } else if (k == N + 1) {
        popAll(i, j - 3);
        popAll(j - 2, j);
    } else if (k > 0) {
        popAll(i + 1, k - 1);
        popAll(k + 1, j - 1);
        std::cout << k + 1 << " ";
    }
}

void printOutput() {
    std::cout << S[0][N - 1] << std::endl;
    popAll(0, N - 1);
    std::cout << std::endl;
}


int main() {
    construct();
    solveDynamically();
    printOutput();
    destruct();
    return 0;
}
