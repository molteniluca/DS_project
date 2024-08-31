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
    std::string stringa;
    for (int i = 0; i < vec.size(); i++) {
        stringa += std::to_string(vec[i]);
        if (i != vec.size() - 1) {
            stringa += ",";
        }
    }
    return stringa;
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

bool canBeReceived(const std::vector<int>& received, const std::vector<int>& localVector, int senderId){
    if(received.size()!=localVector.size())
        throw std::runtime_error("Size mismatch!");
    
    if (received[senderId]==localVector[senderId]+1){
        for(int i=0; i<received.size(); i++){
            if(senderId!=i){
                if(received[i]>localVector[i]){
                    return false;
                }
            }
        }
        return true;
    } else{
        return false;
    }
}

std::string genRandomString() {
    std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    std::string newstr;
    for(int i = 0; i < 10; ++i) {
        int pos = std::rand() % str.size();
        newstr += str[pos];
    }
    return newstr;
}