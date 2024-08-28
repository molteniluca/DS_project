#include "utils.hpp"

#include <iterator>
#include <algorithm>
#include <sstream>

std::string vectorToString(const std::vector<std::string>& vec) {
    std::ostringstream oss;
    if (!vec.empty())
    {
        std::copy(vec.begin(), vec.end()-1, std::ostream_iterator<std::string>(oss, ","));
        oss << vec.back();
    }
    return oss.str();
}

// Function to convert string back to vector
std::vector<std::string> stringToVector(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> vec;
    std::string item;
    while (std::getline(iss, item, ','))
    {
        vec.push_back(item);
    }
    return vec;
}
