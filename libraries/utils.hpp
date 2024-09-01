#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <set>

std::string vectorOfStrings_to_String(const std::vector<std::string>&);
std::vector<std::string> string_to_vectorOfStrings(const std::string&);

std::string vectorOfInts_to_String(const std::vector<int>&);
std::vector<int> string_to_vectorOfInts(const std::string&);

std::string setOfTuplesOfStrings_to_String(const std::set<std::tuple<std::string, std::string>>&);
std::set<std::tuple<std::string, std::string>> string_to_setOfTuplesOfStrings(const std::string&);

bool canBeReceived(const std::vector<int>& received, const std::vector<int>& localVector, int senderId);

#endif // UTILS_HPP