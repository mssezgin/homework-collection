#include <iostream>
#include "path_tracker.h"

int main() {

    /*
    int existing_path1[2] = {25, 30};
    int existing_path2[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    PathTracker pt1 = PathTracker(existing_path1, 1);
    PathTracker pt2 = PathTracker(existing_path2, 5);

    pt1.summary();
    pt2.summary();

    std::cout << "pt1 == pt2: " << (pt1 == pt2) << "\n";
    std::cout << "pt1 > pt2: " << (pt1 > pt2) << "\n";
    std::cout << "pt1 < pt2: " << (pt1 < pt2) << "\n";

    // notice that distance can be compared with a floating-point
    // not another PathTracker object.
    std::cout << "distance comparison: " << (pt1 == pt2.distance) << "\n";
    */
    
    PathTracker pt;
    int dot[2] = {0, 0};
    
    pt.summary();
    pt += dot;
    pt.summary();
    /* dot[0] = 3; dot[1] = 4;
    pt += dot;
    pt.summary();
    dot[0] = -3; dot[1] = -4;
    pt += dot;
    pt.summary(); */
    dot[0] = 1; dot[1] = 1;
    pt += dot;
    pt.summary();
    dot[0] = 2; dot[1] = 3;
    pt += dot;
    pt.summary();
    
    
    int arr[4] = {1, 1, -1, -1};
    PathTracker ptpt(arr, 2);
    ptpt.summary();
    

    /*
    std::cout << pt.calculate_l2(0, -3, -4, 0) << "\n";
    std::cout << pt.calculate_l2(0, 0, 0, 1) << "\n";
    std::cout << pt.calculate_l2(0, 0, 1, 0) << "\n";
    std::cout << pt.calculate_l2(0, 0, 0, -1) << "\n";
    */

    return 0;
}
