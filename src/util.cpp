#include "util.h"
#include <ctime>
#include <iomanip>
#include <iostream>


namespace pslr {

std::vector<std::string> Split(std::string line, char sparator) {
    std::vector<std::string> ret;

    int start = 0;
    std::size_t pos = line.find(sparator, start);
    while (pos != std::string::npos) {
        ret.push_back(line.substr(start, pos));
        start = pos + 1;
        pos = line.find(sparator, start);
    }
    ret.push_back(line.substr(start));
    return ret;
}

int ToInt(const char* str) {
    int flag = 1, ret = 0;
    const char* p = str;

    if (*p == '-') {
        ++p;
        flag = -1;
    } else if (*p == '+') {
        ++p;
    }

    while (*p) {
        ret = ret * 10 + (*p - '0');
        ++p;
    }
    return flag * ret;
}

int ToInt(const std::string& str) {
    return ToInt(str.c_str());
}

float ToFloat(const char* str) {
    float integer = 0, decimal = 0;
    float base = 1;
    const char* p = str;

    while (*p) {
        if (*p == '.') {
            base = 0.1;
            ++p;
            continue;
        }
        if (base >= 1.0) {
            integer = integer * 10 + (*p - '0');
        } else {
            decimal += base * (*p - '0');
            base *= 0.1;
        }
        ++p;
    }

    return integer + decimal;
}

float ToFloat(const std::string& str) {
    return ToFloat(str.c_str());
}

uint64_t ReverseBytes(uint64_t x) {
    x = x << 32 | x >> 32;
    x = (x & 0x0000FFFF0000FFFFULL) << 16 | (x & 0xFFFF0000FFFF0000ULL) >> 16;
    x = (x & 0x00FF00FF00FF00FFULL) << 8  | (x & 0xFF00FF00FF00FF00ULL) >> 8;
    x = (x & 0x0F0F0F0F0F0F0F0FULL) << 4  | (x & 0xF0F0F0F0F0F0F0F0ULL) >> 4;
    return x;
}

void Time() {
    time_t rawtime;
    time(&rawtime);
    struct tm* curr_time = localtime(&rawtime);
    std::cout<<std::setw(2) <<curr_time->tm_hour <<':'
             <<std::setw(2) <<curr_time->tm_min <<':'
             <<std::setw(2) <<curr_time->tm_sec;
}


} // namespace pslr

