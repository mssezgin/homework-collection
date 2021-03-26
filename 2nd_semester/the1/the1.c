/* I used "loop_continue" function, not "loop_continues". */

#include "loop.h"
#include <stdio.h>

int i = 0;
char vars[128];
int starts[128], ends[128], steps[128], currs[128], conts[128];
int indexes[256]; /* e.g. indexes['x'] ---> 0, starts[indexes['r']] --> -3 */

void loop_function(int index, char var, int start, int end, int step);
void conts_reset(void);

int main() {

    while ((vars[i] = getchar()) != EOF) {
        scanf("%d %d %d ", &starts[i], &ends[i], &steps[i]);
        indexes[vars[i]] = i;
        i++;
    }
    
    loop_function(0, vars[0], starts[0], ends[0], steps[0]);
    
    return 0;
}

void loop_function(int index, char var, int start, int end, int step) {
    
    if (step >= 0) {
        if (index == i - 1) {
            for (start; start <= end; start += step) {
                if (conts[index]) {
                    if (indexes[conts[index]] == index) {
                        conts_reset();
                    }
                    else {
                        continue;
                    }
                }
                currs[index] = start;
                loop_execute();
            }
        }
        for (start; start <= end; start += step) {
            if (conts[index]) {
                if (indexes[conts[index]] == index) {
                    conts_reset();
                }
                else {
                    continue;
                }
            }
            currs[index] = start;
            loop_function(index + 1, vars[index + 1], starts[index + 1], ends[index + 1], steps[index + 1]);
        }
    }
    else {
        if (index == i - 1) {
            for (start; start >= end; start += step) {
                if (conts[index]) {
                    if (indexes[conts[index]] == index) {
                        conts_reset();
                    }
                    else {
                        continue;
                    }
                }
                currs[index] = start;
                loop_execute();
            }
        }
        for (start; start >= end; start += step) {
            if (conts[index]) {
                if (indexes[conts[index]] == index) {
                    conts_reset();
                }
                else {
                    continue;
                }
            }
            currs[index] = start;
            loop_function(index + 1, vars[index + 1], starts[index + 1], ends[index + 1], steps[index + 1]);
        }
    }

    return;
}

int loop_variable_value(char c) {
    return currs[indexes[c]];
}

void loop_continue(char c) {
    int j;
    for (j = indexes[c]; j < 128; j++) {
        conts[j] = c;
    }

    return;
}

void conts_reset() {
    int k;
    for (k = 0; k < 128; k++) {
        conts[k] = 0;
    }

    return;
}
