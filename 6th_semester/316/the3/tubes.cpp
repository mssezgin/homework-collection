#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#define LEFT_EVT 1
#define RIGHT_EVT 2



struct event {
    size_t type, x, y;
};



std::vector<event> events;
std::map<size_t, size_t> buildings;
size_t T = 0;



void construct() {

    size_t N;

    // dummy event at (0,0)
    events.push_back({ LEFT_EVT, 0, 0 });

    std::cin >> N;
    for (size_t i = 0; i < N; i++) {
        size_t L, H, R;
        std::cin >> L >> H >> R;
        events.push_back({ LEFT_EVT, L, H });
        events.push_back({ RIGHT_EVT, R, H });
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

    auto prev_evt = events.begin();
    auto evt      = events.begin() + 1;
    for (; evt != events.end(); ++evt, ++prev_evt) {

        if (buildings.size() > 0) {
            T += (evt->x - prev_evt->x) * buildings.rbegin()->first;
        }

        switch (evt->type) {
            case LEFT_EVT:
                buildings[evt->y]++;
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

    std::cout << T << std::endl;

    return 0;
}
