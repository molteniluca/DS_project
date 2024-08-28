#ifndef ROOM_HPP
#define ROOM_HPP

#include <iostream>
#include <vector>
#include <string>
#include <omnetpp.h>
#include "utils.hh"

class Room {
public:
    Room(std::vector<std::string> participants, std::string adminId, std::string roomId);

    std::string getRoomId();

    RoomCreationMessage getMessageCreation();

    ChatMessage getMessage(const std::string& message);

    Room(RoomCreationMessage msg);

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
