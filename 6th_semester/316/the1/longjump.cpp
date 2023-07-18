#include <iostream>
#include <vector>


typedef struct {
    size_t rank;
    int score;
} element;


class SumFenwickTree {
private:
    std::vector<int> arr;

    size_t lsb(size_t x) const {
        return x & (x ^ (x - 1));
    }

public:
    SumFenwickTree(size_t n) :
        arr(n + 1, 0) { }
    
    size_t size() const {
        return arr.size() - 1;
    }

    void update(size_t k, size_t v) {
        for (size_t i = k; i <= size(); i += lsb(i)) {
            arr[i] += v;
        }
    }

    int query(size_t k) const {
        int sum = 0;
        for (size_t i = k; i > 0; i -= lsb(i)) {
            sum += arr[i];
        }
        return sum;
    }
    
    void print() const {
        for (size_t i = 1; i <= size(); i++) {
            std::cout << i << ": " << arr[i] << "\n";
        }
        std::cout << std::endl;
    }
};


size_t N;
element* years;
element** sortedYearPtrs;


void parseInput() {
    std::cin >> N;
    years = new element[N];
    sortedYearPtrs = new element*[N];
    for (size_t i = 0; i < N; i++) {
        std::cin >> years[i].score;
        sortedYearPtrs[i] = &years[i];
    }
}


void mergeSort(size_t n, element** arr) {

    if (n == 1) {
        return;
    }

    size_t nHalves = n / 2;
    mergeSort(nHalves, arr);
    mergeSort(n - nHalves, arr + nHalves);

    size_t i = 0;
    size_t j = nHalves;
    size_t k = 0;
    element** aux = new element*[n];

    while (i < nHalves && j < n) {
        if (arr[i]->score < arr[j]->score) {
            aux[k++] = arr[j++];
        } else {
            aux[k++] = arr[i++];
        }
    }

    while (i < nHalves) {
        aux[k++] = arr[i++];
    }

    while (j < n) {
        aux[k++] = arr[j++];
    }

    for (i = 0, k = 0; i < n; i++, k++) {
        arr[i] = aux[k];
    }

    delete[] aux;
}


void sortDescAndRank() {

    mergeSort(N, sortedYearPtrs);
    int previousScore = 0;
    size_t rank = 0;
    for (size_t i = 0; i < N; i++) {
        if (sortedYearPtrs[i]->score != previousScore) {
            previousScore = sortedYearPtrs[i]->score;
            rank++;
        }
        sortedYearPtrs[i]->rank = rank;
    }
}


int main() {

    parseInput();
    sortDescAndRank();

    SumFenwickTree sft(N);

    for (size_t i = 0; i < N; i++) {
        sft.update(years[i].rank, 1);
        std::cout << sft.query(years[i].rank) << " ";
    }

    delete[] years;
    delete[] sortedYearPtrs;
    return 0;
}
