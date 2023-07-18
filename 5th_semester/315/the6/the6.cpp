#include "the6.h"
// Don't add any additional includes

/*
  N: Number of racers
  updates: Updates you have heard
*/
std::pair<bool, std::vector<int>>
RaceResult(int N, std::vector<std::pair<int, int>> updates) {
    
    // allocate
    int** edges = new int*[N];
    for (int i = 0; i < N; i++) {
        edges[i] = new int[N]();
    }
    
    for (auto itr = updates.begin(); itr != updates.end(); ++itr) {
        edges[itr->first][itr->second] = 1;
    }
    
    // second is reversed
    std::pair<bool, std::vector<int>> result = {true, {}};
    std::vector<int> status; // 0: white, 1: gray, 2: black
    for (int i = 0; i < N; i++) {
        status.push_back(0);
    }
    std::vector<int> stack;
    
    // topological sort
    while (true) {
        
        if (stack.empty()) {
            // get next unvisited (white) vertex
            int id = -1;
            for (int i = 0; i < N; i++) {
                if (status[i] == 0) {
                    id = i;
                    break;
                }
            }
            if (id == -1) { // not found, every vertex has been visited
                break;
            }
            
            status[id] = 1; // mark as visiting (gray)
            stack.push_back(id);
            continue;
        }
        
        // get next unvisited (white) adjacent vertex
        int adj = -1;
        for (int j = 0; j < N; j++) {
            if (edges[stack.back()][j] == 1) {
                if (status[j] == 0) {
                    // found
                    adj = j;
                    break;
                } else if (status[j] == 1) {
                    // cycle
                    adj = j;
                    result.first = false;
                    break;
                }
                // else: visited (black) adjacent, continue searching
            }
        }
        if (result.first == false) {
            // found a cycle
            result.second.clear();
            result.second.push_back(adj);
            while (stack.back() != adj) {
                result.second.push_back(stack.back());
                stack.pop_back();
            }
            result.second.push_back(adj);
            break;
        }
        
        // no cycles
        if (adj == -1) {
            // no unvisited adjacent vertex
            // mark as visited (black)
            status[stack.back()] = 2;
            result.second.push_back(stack.back());
            stack.pop_back();
        } else {
            status[adj] = 1; // mark as visiting (gray)
            stack.push_back(adj);
        }
    }
    
    // reverse result
    std::vector<int> temp;
    while (!result.second.empty()) {
        temp.push_back(result.second.back());
        result.second.pop_back();
    }
    result.second.swap(temp);
    
    // deallocate
    for (int i = 0; i < N; i++) {
        delete[] edges[i];
        edges[i] = nullptr;
    }
    delete[] edges;
    edges = nullptr;
    
    return result;
}
