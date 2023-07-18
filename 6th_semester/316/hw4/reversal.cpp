#include <iostream>
#include <vector>

std::vector<size_t> numbers;
std::vector<std::pair<size_t, size_t>> reversals;

void construct() {
    size_t a;
    while (scanf("%lu ", &a) == 1) {
        numbers.push_back(a);
    }
}

void reverseRange(size_t i, size_t j) {
    for (; i < j; i++, j--) {
        size_t temp = numbers[i];
        numbers[i] = numbers[j];
        numbers[j] = temp;
    }
}

size_t h() {
    size_t t = 0;

    if (numbers.front() != 1)
        t++;

    for (size_t i = 1; i < numbers.size(); i++) {
        if ((numbers[i-1] + 1 == numbers[i] || numbers[i] + 1 == numbers[i-1]) == false)
            t++;
    }

    if (numbers.back() != numbers.size())
        t++;

    return (t + 1) / 2;
}

bool dfid(size_t limit) {

    size_t n = reversals.size();
    size_t heur = h();

    if (n + heur > limit) {
        return false;
    }

    if (n == limit) {
        if (heur == 0)
            return true;
        else
            return false;
    }

    n = numbers.size();
    for (size_t i = 0; i < n - 1; i++) {
        for (size_t j = i + 1; j < n; j++) {
            reversals.push_back({i, j});
            reverseRange(i, j);
            if (dfid(limit) == true)
                return true;
            reverseRange(i, j);
            reversals.pop_back();
        }
    }

    return false;
}

void print() {
    std::cout << reversals.size() << std::endl;
    for (auto& reversal : reversals) {
        std::cout << reversal.first + 1 << " " << reversal.second + 1 << std::endl;
    }
}

int main() {
    construct();
    for (size_t limit = 0; dfid(limit) == false; limit++);
    print();
    return 0;
}
