#include <iostream>



// maximum points
int maxPoints;
// indices of last two balloons
size_t I, J;
// number of balloons
size_t N;
// array of values of balloons
int *balloons;
// array of binary state of ballons
// 1: balloon is alive, 0: balloon was popped
size_t *remainingBalloons;
// solution matrix
// S[i][j] holds max points obtained by popping the balloons between i and j
int **S;
// intermediate matrix
// K[i][j] holds 1 + the index of the balloon that is popped last among the ones between i and j
size_t **K;


void construct() {

    std::cin >> N;

    balloons = new int[N];
    remainingBalloons = new size_t[N];
    S = new int*[N];
    K = new size_t*[N];

    for (size_t i = 0; i < N; i++) {
        std::cin >> balloons[i];
        remainingBalloons[i] = 1;
        S[i] = new int[N];
        K[i] = new size_t[N];
    }
}


void destruct() {

    delete[] balloons;
    delete[] remainingBalloons;
    for (size_t i = 0; i < N; i++) {
        delete[] S[i];
        delete[] K[i];
    }
    delete[] S;
    delete[] K;
}


void solveDynamically() {

    for (size_t d = 1; d < N; d++) {
        for (size_t i = 0; i < N; i++) {
            size_t _j = i + d;
            size_t j = (_j < N) ? _j : (_j - N);
            if (d == 1) {
                S[i][j] = 0;
            } else {
                int maxPointsSij = 0;
                for (size_t _k = i + 1; _k < _j; _k++) {
                    size_t k = (_k < N) ? _k : (_k - N);
                    int points = S[i][k] + S[k][j] + balloons[i] * balloons[k] * balloons[j];
                    if (points > maxPointsSij) {
                        maxPointsSij = points;
                        K[i][j] = k + 1;
                    }
                    // TODO: this should have been outside the loop
                    S[i][j] = maxPointsSij;
                }
            }
        }
    }

    maxPoints = 0;
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++) {
            int points = S[i][j] + S[j][i] + balloons[i] * balloons[i] * (balloons[j] + balloons[i]);
            if (points > maxPoints) {
                maxPoints = points;
                I = i;
                J = j;
            }
        }
    }
}


void popAll(size_t i, size_t j) {
    if (K[i][j] == 0) {
        return;
    }
    size_t k = K[i][j] - 1;
    popAll(i, k);
    popAll(k, j);
    size_t p = 0;
    for (size_t l = 0; l <= k; l++) {
        p += remainingBalloons[l];
    }
    std::cout << p << " ";
    remainingBalloons[k] = 0;
}


void printOutput() {
    std::cout << maxPoints << std::endl;
    popAll(I, J);
    popAll(J, I);
    size_t big, small;
    if (balloons[I] > balloons[J]) {
        big = I;
        small = J;
    } else {
        big = J;
        small = I;
    }
    size_t p = 0;
    for (size_t l = 0; l <= small; l++) {
        p += remainingBalloons[l];
    }
    std::cout << p << " ";
    remainingBalloons[small] = 0;
    p = 0;
    for (size_t l = 0; l <= big; l++) {
        p += remainingBalloons[l];
    }
    std::cout << p << std::endl;
    remainingBalloons[big] = 0;
}


int main() {
    construct();
    solveDynamically();
    printOutput();
    destruct();
    return 0;
}
