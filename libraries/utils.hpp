#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

std::string vectorOfStrings_to_String(const std::vector<std::string>&);
std::vector<std::string> string_to_vectorOfStrings(const std::string&);

std::string vectorOfInts_to_String(const std::vector<int>&);
std::vector<int> string_to_vectorOfInts(const std::string&);

#endif // UTILS_HPP