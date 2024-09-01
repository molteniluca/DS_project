#include "Room.hpp"

#include <iostream> // For cout

#include "utils.hpp"

Room::Room(std::vector<std::string> participants, std::string adminId, std::string roomId) {
    this->numParticipants = participants.size();
    this->adminId = adminId;
    this->roomId = roomId;
    this->userId = adminId;
    this->participants = participants;
    
    this->userIndex = lookupUserIndex(userId);
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

    this->userIndex = lookupUserIndex(userId);
    if(userIndex == -1) {
        throw std::runtime_error("User not found in participants");
    }

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

int Room::lookupUserIndex(const std::string& userId) {
    for(int i = 0; i < numParticipants; i++) {
        if(participants[i] == userId) {
            return i;
        }
    }

    return -1;
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

void Room::displayMessage(ChatMessage *msg){
    std::string message = msg->getContent();
    std::string senderId = msg->getSenderId();

    std::cout << this->userId << " - " << this->roomId << " - Displayed message: " << message << " from user " << senderId << std::endl;
}

void Room::processMessage(ChatMessage *msg) {
    std::vector<int> receivedVectorClock = msg->getVectorClock();
    std::string senderId = msg->getSenderId();
    messages.insert(std::make_pair(receivedVectorClock, *msg));
    if(canBeReceived(receivedVectorClock, vectorClock, lookupUserIndex(senderId))) {
        displayMessage(msg);
        vectorClock[lookupUserIndex(senderId)]++;
        flushMessages();
    }else{
        messagesQueue.insert(std::make_pair(receivedVectorClock, *msg));
        if (messagesQueue.size() > 10) {
            throw QueueTooLongException(getMissingMessages());
        }
    }
}

std::list<AskMessage> Room::getMissingMessages() {
    std::string missingSenderId;
    std::list<AskMessage> missingMessagesList;

    for(int i = 0; i < numParticipants; i++) {
        AskMessage missingMessage = *(new AskMessage(vectorClock[i]+1, missingSenderId, roomId));
        missingMessagesList.push_back(missingMessage);
    }

    return missingMessagesList;
}

ChatMessage* Room::resendMessage(AskMessage *amsg) {
    int missingMessageId = amsg->getMissingMessageId();
    std::string missingSenderId = amsg->getMissingSenderId();

    for(auto it = messages.begin(); it != messages.end(); it++) {
        std::vector<int> messageVectorClock = it->first;
        ChatMessage *message = &it->second;
        if(message->getSenderId() == missingSenderId && missingMessageId == messageVectorClock[lookupUserIndex(missingSenderId)]) {
            return message;
        }
    }

    return nullptr;
}


void Room::flushMessages() {
    bool found = true;
    while (found){
        found = false;
        for(auto it = messagesQueue.begin(); it != messagesQueue.end(); it++) {
            std::vector<int> receivedVectorClock = it->first;
            ChatMessage message = it->second;

            if (canBeReceived(receivedVectorClock, vectorClock, lookupUserIndex(message.getSenderId()))) {
                displayMessage(&message);
                vectorClock[lookupUserIndex(message.getSenderId())]++;
                messagesQueue.erase(it);
                found = true;
                break;
            }
        }
    }
}

bool Room::checkPrinted(ChatMessage *msg) {
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

bool Room::checkReceived(ChatMessage *msg) {
    if (checkPrinted(msg)) {
        return true;
    }
    std::vector<int> receivedVectorClock = msg->getVectorClock();

    return messagesQueue.find(receivedVectorClock) != messagesQueue.end();
}