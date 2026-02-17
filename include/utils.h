#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace utils {
    std::string readFile(const std::string& filePath);
    void checkErrors(unsigned int ID, std::string type);
}

#endif