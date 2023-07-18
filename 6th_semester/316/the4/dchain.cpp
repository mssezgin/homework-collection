#include <iostream>
#include <vector>

size_t h(const std::vector<size_t> &chain, size_t target) {
    size_t v = *chain.rbegin();
    size_t u = *(chain.rbegin() + 1);
    size_t h = 0;
    while (v < target) {
        size_t uv = u + v;
        u = v;
        v = uv;
        h++;
    }
    return h;
}

bool dfs(std::vector<size_t> &chain, size_t limit, size_t target) {

    if (chain.size() + h(chain, target) > limit) {
        return false;
    }

    if (chain.back() == target) {
        return true;
    }

    for (int i = chain.size() - 1; i > 0; i--) {
        for (int j = i - 1; j >= 0; j--) {
            size_t v = chain[i] + chain[j];
            if (chain.back() < v && v <= target) {
                chain.push_back(v);
                if (dfs(chain, limit, target)) {
                    return true;
                }
                chain.pop_back();
            }
        }
    }

    return false;
}

std::vector<size_t> dChain(size_t target) {
    std::vector<size_t> chain = {1, 2};
    for(size_t limit = 1; dfs(chain, limit, target) == false; limit++);
    return chain;
}

int main() {
    size_t N;
    std::cin >> N;
    auto chain = dChain(N);
    for (auto v : chain) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    return 0;
}
