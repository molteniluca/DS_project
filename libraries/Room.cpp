#include "Room.hpp"

#include <iostream> // For cout

Room::Room(std::vector<std::string> participants, std::string adminId, std::string roomId) {
    this->numParticipants = participants.size();
    this->adminId = adminId;
    this->roomId = roomId;
    this->userId = adminId;
    this->participants = participants;
    
    this->userIndex = -1;
    for(int i = 0; i < numParticipants; i++) {
        if(participants[i] == userId) {
            this->userIndex = i;
            break;
        }
    }
    if(userIndex == -1) {
        throw std::runtime_error("User not found in participants");
    }

    vectorClock.resize(numParticipants, 0);
}

Room::Room(RoomCreationMessage msg, std::string userId) {
    this->numParticipants = msg.getParticipants().size();
    this->adminId = msg.getAdminId();
    this->roomId = msg.getRoomId();
    this->participants = msg.getParticipants();
    this->userId = userId;
    vectorClock.resize(numParticipants, 0);
}

Room::Room() {
    numParticipants = -1;
    adminId = "";
    roomId = "";
    userId = "";
    userIndex = -1;
    vectorClock = {};
}

std::string Room::getRoomId() {
    return roomId;
}

std::vector<std::string> Room::getParticipants() {
    return participants;
}

RoomCreationMessage* Room::getMessageCreation() {
    return new RoomCreationMessage(adminId, roomId, participants);
}

ChatMessage* Room::getMessage(const std::string& message) {
    vectorClock[userIndex]++;

    return new ChatMessage(userId, roomId, message, vectorClock);
}

void Room::processMessage(ChatMessage *msg) {
    std::vector<int> receivedVectorClock = msg->getVectorClock();
    std::string message = msg->getContent();
    std::string senderId = msg->getSenderId();

    /// TODO: manca logica per mettere messaggi in coda e processarli in ordine in base ai vector clock

    /// TODO: stampa sempre c0 come sender???
    std::cout << this->userId << " - Received message: " << message << " from user " << senderId << std::endl;
}

bool Room::checkReceived(ChatMessage *msg) {
    std::vector<int> receivedVectorClock = msg->getVectorClock();
    std::string senderId = msg->getSenderId();

    for(int i = 0; i < numParticipants; i++) {
        if(participants[i] == senderId) {
            if(receivedVectorClock[i] <= vectorClock[i]) {
                return true;
            }
        }
    }

    return false;
}
