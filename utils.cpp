#include "utils.h"

unsigned int StringToInt(string s) {
    int length = (int)s.length();
    unsigned int answer = 0;

    if(length % 2 == 1) {
        answer = s.at(length-1);
        length--;
    }

    for(int i = 0; i < length; i += 2) {
        answer += s.at(i);
        answer += ((int)s.at(i+1)) << 8;
    }

    return answer;
}