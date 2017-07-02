#ifndef PSLR_UTIL_H_
#define PSLR_UTIL_H_

#include <string>
#include <vector>


namespace pslr {

    std::vector<std::string> Split(std::string line, char sparator);

    int ToInt(const char* str);
    int ToInt(const std::string& str);

    float ToFloat(const char* str);
    float ToFloat(const std::string& str);

    uint64_t ReverseBytes(uint64_t x);

    void Time();

} // namespace pslr

#endif  // PSLR_UTIL_H_

