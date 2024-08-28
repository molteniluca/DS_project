#ifndef ROOM_HPP
#define ROOM_HPP

#include <iostream>
#include <vector>
#include <string>

#include "Message.hpp"

class Room {
public:
    Room(std::vector<std::string> participants, std::string adminId, std::string roomId);

    Room(RoomCreationMessage msg);

    std::string getRoomId();

    std::vector<std::string> getParticipants();

    RoomCreationMessage* getMessageCreation();

    ChatMessage* getMessage(const std::string& message);

    void processMessage(ChatMessage *msg);

    bool checkReceived(ChatMessage *msg);

private:
    int numParticipants;
    std::string userId;
    std::string adminId;
    std::string roomId;
    std::vector<std::string> participants;
    std::vector<int> vectorClock;   
    std::vector<std::string> messages;
};

#endif // ROOM_HPP
