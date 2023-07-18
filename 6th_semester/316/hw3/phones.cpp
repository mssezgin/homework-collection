#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#define LEFT_EVT 1
#define PHONE_EVT 2
#define RIGHT_EVT 3



struct event {
    size_t type, x, y;
};



std::vector<event> events;
std::map<size_t, size_t> buildings;
size_t F = 0;



void construct() {

    size_t N, M;

    std::cin >> N;
    for (size_t i = 0; i < N; i++) {
        size_t L, H, R;
        std::cin >> L >> H >> R;
        events.push_back({ LEFT_EVT, L, H });
        events.push_back({ RIGHT_EVT, R, H });
    }

    std::cin >> M;
    for (size_t i = 0; i < M; i++) {
        size_t X, Y;
        std::cin >> X >> Y;
        events.push_back({ PHONE_EVT, X, Y });
    }
}



bool compareEvents(const event& evt1, const event& evt2) {
    if (evt1.x < evt2.x)
        return true;

    if (evt1.x > evt2.x)
        return false;

    if (evt1.type < evt2.type)
        return true;

    if (evt1.type > evt2.type)
        return false;

    if (evt1.y <= evt2.y)
        return true;

    return false;
}


int main() {

    construct();

    std::sort(events.begin(), events.end(), compareEvents);

    for (auto evt = events.begin(); evt != events.end(); ++evt) {
        switch (evt->type) {
            case LEFT_EVT:
                buildings[evt->y]++;
                break;

            case PHONE_EVT:
                if (buildings.size() == 0 || evt->y > buildings.rbegin()->first) {
                    F++;
                }
                break;

            case RIGHT_EVT:
                buildings[evt->y]--;
                if (buildings[evt->y] == 0) {
                    buildings.erase(evt->y);
                }
                break;
            
            default:
                break;
        }
    }

    std::cout << F << std::endl;

    return 0;
}
