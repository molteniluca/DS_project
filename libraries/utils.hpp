#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

std::string vectorOfStrings_to_String(const std::vector<std::string>&);
std::vector<std::string> string_to_vectorOfStrings(const std::string&);

std::string vectorOfInts_to_String(const std::vector<int>&);
std::vector<int> string_to_vectorOfInts(const std::string&);

bool canBeReceived(const std::vector<int>& received, const std::vector<int>& localVector, int senderId);
std::string genRandomString();

#endif // UTILS_HPP