#include <stdio.h>
#include "loop.h"

int which_one = 0;

void loop_execute(void) {
    printf("a: %d | b: %d | c: %d | d: %d | e: %d | f: %d\n",
            loop_variable_value('a'),
            loop_variable_value('b'),
            loop_variable_value('c'),
            loop_variable_value('d'),
            loop_variable_value('e'),
            loop_variable_value('f'));
    
    /*printf("%d%d%d%d%d\n", loop_variable_value('+'), loop_variable_value('-'), loop_variable_value('*'), loop_variable_value('/'), loop_variable_value('Q'));*/

    /*if ((loop_variable_value('a') + loop_variable_value('b') + loop_variable_value('c') + loop_variable_value('d') + loop_variable_value('e')) % 5 == 0) {
        printf(" | Magic of five happened...");
    
        switch (which_one++ % 4) {
        
            case 0 : {
                printf(" | loop_continue('d');\n");
                loop_continue('d');
                break;
            }
            case 1: {
                printf(" | loop_continue('c');\n");
                loop_continue('c');
                break;
            }
            case 2 : {
                printf(" | loop_continue('b');\n");
                loop_continue('b');
                break;
            }
            case 3 : {
                printf(" | loop_continue('a');\n");
                loop_continue('a');
                break;
            }
        }
    }
    else printf("\n");*/

    return;
}