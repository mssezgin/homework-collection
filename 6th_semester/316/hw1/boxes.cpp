#include <iostream>
#include <vector>
#include <algorithm>


class Box {
public:
    int width, height;
    int wrank, hrank;
    int capacity;

    Box(int _width = 0, int _height = 0, int _wrank = 0, int _hrank = 0, int _capacity = 0) :
        width(_width), height(_height), wrank(_wrank), hrank(_hrank), capacity(_capacity) { }
};


class MaxFenwickTree {
private:
    std::vector<int> arr;

    size_t lsb(size_t x) const {
        return x & (x ^ (x - 1));
    }

public:
    MaxFenwickTree(size_t N) :
        arr(N + 1, 0) { }

    size_t size() const {
        return arr.size() - 1;
    }

    void update(size_t k, int v) {
        for (size_t i = k; i <= size() && arr[i] < v; i += lsb(i)) {
            arr[i] = v;
        }
    }

    int query(size_t k) const {
        int maxValue = arr[k];
        for (size_t i = k; i > 0; i -= lsb(i)) {
            if (arr[i] > maxValue) {
                maxValue = arr[i];
            }
        }
        return maxValue;
    }

    void print() const {
        for (size_t i = 1; i <= size(); i++) {
            std::cout << i << ": " << arr[i] << "\n";
        }
        std::cout << std::endl;
    }
};


size_t N;
size_t nW, nH;
Box* boxes;
std::vector<std::vector<Box*>> widthRanks;
std::vector<std::vector<Box*>> heightRanks;


void parseInput() {
    std::cin >> N;
    boxes = new Box[N];
    for (size_t i = 0; i < N; i++) {
        std::cin >> boxes[i].width >> boxes[i].height;
    }
}


bool compareByWidth(const Box& b1, const Box& b2) {
    return b1.width < b2.width;
}


bool compareByHeight(const Box& b1, const Box& b2) {
    return b1.height < b2.height;
}


void rankByWidth() {

    std::sort(boxes, boxes + N, compareByWidth);
    widthRanks.push_back(std::vector<Box*>()); // dummy vector for rank 0

    int previousWidth = 0;
    int r = 0; // current rank

    for (size_t i = 0; i < N; i++) {
        if (previousWidth != boxes[i].width) {
            previousWidth = boxes[i].width;
            widthRanks.push_back(std::vector<Box*>());
            r++;
        }
        boxes[i].wrank = r;
        widthRanks[r].push_back(&boxes[i]);
    }

    nW = r;
}


void rankByHeight() {

    std::sort(boxes, boxes + N, compareByHeight);
    heightRanks.push_back(std::vector<Box*>()); // dummy vector for rank 0

    int previousHeight = 0;
    int r = 0; // current rank

    for (size_t i = 0; i < N; i++) {
        if (previousHeight != boxes[i].height) {
            previousHeight = boxes[i].height;
            heightRanks.push_back(std::vector<Box*>());
            r++;
        }
        boxes[i].hrank = r;
        heightRanks[r].push_back(&boxes[i]);
    }

    nH = r;
}


int main() {

    parseInput();
    rankByHeight();
    rankByWidth();

    MaxFenwickTree maxBoxChain(nH);

    for (size_t i = 1; i <= nW; i++) {
        for (size_t j = 0; j < widthRanks[i].size(); j++) {
            Box& b = *widthRanks[i][j];
            b.capacity = maxBoxChain.query(b.hrank - 1) + 1;
        }
        for (size_t j = 0; j < widthRanks[i].size(); j++) {
            Box& b = *widthRanks[i][j];
            maxBoxChain.update(b.hrank, b.capacity);
        }
    }

    std::cout << maxBoxChain.query(nH);

    delete boxes;
    return 0;
}
