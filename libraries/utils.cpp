#include "utils.hpp"

#include <iterator> // For ostream_iterator
#include <algorithm> // For copy
#include <sstream> // For istringstream, ostringstream

#include <iostream>

std::string vectorOfStrings_to_String(const std::vector<std::string>& vec) {
    std::ostringstream oss;
    if (!vec.empty())
    {
        std::copy(vec.begin(), vec.end()-1, std::ostream_iterator<std::string>(oss, ","));
        oss << vec.back();
    }
    return oss.str();
}

std::vector<std::string> string_to_vectorOfStrings(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> vec;
    std::string item;
    while (std::getline(iss, item, ','))
    {
        vec.push_back(item);
    }
    return vec;
}

std::string vectorOfInts_to_String(const std::vector<int>& vec) {
    std::ostringstream oss;
    if (!vec.empty())
    {
        std::transform(vec.begin(), vec.end(), std::ostream_iterator<std::string>(oss, ","), [](int num) {
            return std::to_string(num);
        });
        oss << std::to_string(vec.back());
    }
    // std::cout << "vettore" << vec[0] << ',' << vec[1] << ',' << vec[2] << "convertito in: " << oss.str() << std::endl;
    return oss.str();
}

std::vector<int> string_to_vectorOfInts(const std::string& str) {
    std::istringstream iss(str);
    std::vector<int> vec;
    std::string item;
    while (std::getline(iss, item, ','))
    {
        vec.push_back(std::stoi(item));
    }
    return vec;
}
