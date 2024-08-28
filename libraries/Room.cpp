#include "Room.hpp"

Room::Room(std::vector<std::string> participants, std::string adminId, std::string roomId) {
    this->numParticipants = participants.size();
    this->adminId = adminId;
    this->roomId = roomId;
    this->userId = adminId;
    this->participants = participants;
    vectorClock.resize(numParticipants, 0);
}

Room::Room(RoomCreationMessage msg, std::string userId) {
    this->numParticipants = msg.getParticipants().size();
    this->adminId = msg.getAdminId();
    this->roomId = msg.getRoomId();
    this->userId = userId;
    vectorClock.resize(numParticipants, 0);
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
    vectorClock[userId]++;

    return new ChatMessage(userId, roomId, message, vectorClock);
}

void Room::processMessage(ChatMessage *msg) {
    std::vector<int> receivedVectorClock = msg->getVectorClock();
    std::string message = msg->getContent();
    std::string senderId = msg->getSenderId();

    std::cout << "Received message: " << message << " from user " << senderId << std::endl;
}

bool Room::checkReceived(ChatMessage *msg) {
    std::vector<int> receivedVectorClock = msg->getVectorClock();
    int senderId = msg->getSenderId();

    for(int i = 0; i < numParticipants; i++) {
        if(participants[i] != senderId) {
            if(receivedVectorClock[i] > vectorClock[i]) {
                return false;
            }
        }
    }

    return true;
}
