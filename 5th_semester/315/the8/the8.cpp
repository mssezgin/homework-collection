#include "the8.h"

//DO NOT ADD OTHER LIBRARIES

using namespace std;


int modPow(int base, int exponent, int modulo) {
    int result = 1;
    for (int i = 0; i < exponent; i++) {
        result = (result * base) % modulo;
    }
    return result;
}


int hashStr(const string& str, int start, int end, int b, int modulo) {
    int x = 0;
    for (int i = start; i < end; i++) {
        x = (str[i] + x * b) % modulo;
    }
    return x;
}


int updateHashStr(int x, char c_old, char c_new, int b, int bToM1, int modulo) {
    x = (x - (c_old * bToM1 % modulo) + modulo) % modulo;
    x = (c_new + x * b) % modulo;
    return x;
}


bool checkPattern(const string& text, int start, int M, const string& pattern) {
    for (int i = 0; i < M; i++) {
        if (text[start + i] != pattern[i]) {
            return false;
        }
    }
    return true;
}


vector<int> the8(const string& text, const vector<string>& patterns) {

    vector<int> shifts; // DO NOT CHANGE THIS
    
    int N = text.length();
    int K = patterns.size();
    int M = patterns[0].length();
    int b = 26;
    int mod = 1 << 24; // INT_MAX >> 5;
    int bToM1 = modPow(b, M - 1, mod);
    
    vector<int> patternHashes;
    for (int i = 0; i < K; i++) {
        patternHashes.push_back(hashStr(patterns[i], 0, M, b, mod));
    }
    
    int t = hashStr(text, 0, M, b, mod);
    int i;
    for (i = 0; i < N - M; i++) {
        for (int j = 0; j < K; j++) {
            if (patternHashes[j] == t && checkPattern(text, i, M, patterns[j])) {
                shifts.push_back(i);
                break;
            }
        }
        t = updateHashStr(t, text[i], text[i + M], b, bToM1, mod);
    }
    for (int j = 0; j < K; j++) {
        if (patternHashes[j] == t && checkPattern(text, i, M, patterns[j])) {
            shifts.push_back(i);
            break;
        }
    }
    
    return shifts; // DO NOT CHANGE THIS
}
