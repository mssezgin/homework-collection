#include "the7.h"


void createPath(std::vector<int>& path, int**& midpts, int i, int j) {
    if (midpts[i][j] == -1) {
        path.push_back(i);
    } else {
        createPath(path, midpts, i, midpts[i][j]);
        createPath(path, midpts, midpts[i][j], j);
    }
}


void FindRoute(int n, std::vector<Road> roads, int s, int d, int x, int y) {
    // You can change these variables. These are here for demo only.
    std::vector<int> path;
    int cost = INT_MAX;
    // int** adj = new int*[n];
    int** costs = new int*[n];
    int** midpts = new int*[n];
    
    // create and initialize matrices for all pairs shortest path
    for (int i = 0; i < n; i++) {
        costs[i] = new int[n];
        midpts[i] = new int[n];
        for (int j = 0; j < n; j++) {
            if (i == j) {
                costs[i][j] = 0;
            } else {
                costs[i][j] = INT_MAX;
            }
            midpts[i][j] = -1;
        }
    }
    
    // set costs^0 to road weights
    int m = roads.size();
    for (int k = 0; k < m; k++) {
        int i = roads[k].endpoints.first;
        int j = roads[k].endpoints.second;
        costs[i][j] = costs[j][i] = roads[k].time;
    }
    
    // relaxate and calculate costs^1, costs^2, ..., costs^n
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            if (costs[i][k] == INT_MAX)
                continue;
            for (int j = 0; j < n; j++) {
                int ik = costs[i][k];
                int kj = costs[k][j];
                if (ik == INT_MAX || kj == INT_MAX)
                    continue;
                if (ik + kj < costs[i][j]) {
                    costs[i][j] = ik + kj;
                    midpts[i][j] = k;
                }
            }
        }
    }
    
    // pick the shortest of two possible paths: s->x->y->d and s->y->x->d
    int SX = costs[s][x];
    int XY = costs[x][y];
    int YD = costs[y][d];
    int SXYD = SX + XY + YD;
    int SY = costs[s][y];
    int YX = costs[y][x];
    int XD = costs[x][d];
    int SYXD = SY + YX + XD;
    
    if (SXYD < SYXD) {
        cost = SXYD;
        createPath(path, midpts, s, x);
        createPath(path, midpts, x, y);
        createPath(path, midpts, y, d);
        path.push_back(d);
    } else {
        cost = SYXD;
        createPath(path, midpts, s, y);
        createPath(path, midpts, y, x);
        createPath(path, midpts, x, d);
        path.push_back(d);
    }
    
    // deallocate matrices
    for (int i = 0; i < n; i++) {
        delete[] costs[i];
        delete[] midpts[i];
    }
    delete[] costs;
    delete[] midpts;
    
    // Your output should be like this. You can change this as long as you keep
    // the output format. PrintRange function helps you print elements of
    // containers with iteratos (e.g., std::vector).
    std::cout << cost << " ";
    PrintRange(path.begin(), path.end());
    std::cout << std::endl;
}