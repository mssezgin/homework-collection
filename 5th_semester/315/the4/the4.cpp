#include "the4.h"



int recursive_sln(int i, int*& arr, int &number_of_calls) { // direct recursive
    number_of_calls+=1;

    switch (i) {
        case 0:
            return arr[0];
        case 1:
            return (arr[1] > arr[0]) ? arr[1] : arr[0];
        case 2:
            return (arr[1] > arr[0]) ?
                ((arr[2] > arr[1]) ? arr[2] : arr[1]) :
                ((arr[2] > arr[0]) ? arr[2] : arr[0]);
        default:
            int fst = arr[i] + recursive_sln(i-3, arr, number_of_calls);
            int snd = recursive_sln(i-1, arr, number_of_calls);
            return (snd > fst) ? snd : fst;
    }
}



int memoization_sln(int i, int*& arr, int*& mem) { // memoization

    if (mem[i] == -1) {

        switch (i) {
            case 0:
                mem[0] = arr[0];
                break;
            case 1:
                mem[1] = (arr[1] > arr[0]) ? arr[1] : arr[0];
                break;
            case 2:
                mem[2] = (arr[1] > arr[0]) ?
                    ((arr[2] > arr[1]) ? arr[2] : arr[1]) :
                    ((arr[2] > arr[0]) ? arr[2] : arr[0]);
                break;
            default:
                int fst = arr[i] + memoization_sln(i-3, arr, mem);
                int snd = memoization_sln(i-1, arr, mem);
                mem[i] = (snd > fst) ? snd : fst;
                break;
        }
    }

    return mem[i];
}



int dp_sln(int size, int*& arr, int*& mem) { // dynamic programming

    for (int i = 0; i < size; i++) {

        switch (i) {
            case 0:
                mem[0] = arr[0];
                break;
            case 1:
                mem[1] = (arr[1] > arr[0]) ? arr[1] : arr[0];
                break;
            case 2:
                mem[2] = (arr[1] > arr[0]) ?
                    ((arr[2] > arr[1]) ? arr[2] : arr[1]) :
                    ((arr[2] > arr[0]) ? arr[2] : arr[0]);
                break;
            default:
                int fst = arr[i] + mem[i-3];
                int snd = mem[i-1];
                mem[i] = (snd > fst) ? snd : fst;
                break;
        }
    }

    return mem[size - 1];
}

