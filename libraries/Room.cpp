#include "Room.hpp"

#include <iostream> // For cout
#include <algorithm> // For find

#include "utils.hpp"

Room::Room(std::vector<std::string> participants, std::string adminId, std::string roomId) {
    this->numParticipants = participants.size();
    this->adminId = adminId;
    this->roomId = roomId;
    this->userId = adminId;
    this->participants = participants;

    // setting notAcked to all participants except the admin
    this->notAcked = participants;
    auto it = std::find(notAcked.begin(), notAcked.end(), adminId);
    if (it != notAcked.end()) {
        notAcked.erase(it);
    }
    
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
    // Not setting notAcked here, as it is not the admin
    
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

bool Room::amIAdmin() const {
    return userId == adminId;
}

int Room::lookupUserIndex(const std::string& userId) const {
    for(int i = 0; i < numParticipants; i++) {
        if(participants[i] == userId) {
            return i;
        }
    }

    return -1;
}

void Room::deleteRoom(RoomDeletionMessage *msg)
{
    if (msg->getRoomId() != roomId) {
        throw std::runtime_error("Room deletion message for different room");
    }
    if (msg->getVectorClock().size() != numParticipants) {
        throw std::runtime_error("Room deletion message with wrong vector clock size");
    }
    deletionVectorClock = msg->getVectorClock();
    scheduledForDeletion = true;
    for (int i = 0; i < numParticipants; i++) {
        if (msg->getVectorClock()[i] > vectorClock[i]) {
            return;
        }
    }
    throw DeleteMeException(roomId);
}

std::string Room::getRoomId() const {
    return roomId;
}

std::vector<std::string> Room::getParticipants() const {
    return participants;
}

RoomCreationMessage* Room::getMessageCreation() const {
    return new RoomCreationMessage(adminId, roomId, participants);
}

RoomCreationMessage* Room::getMessageCreationIfNotAcked() const {
    if (notAcked.empty()) {
        return nullptr;
    }
    return new RoomCreationMessage(adminId, roomId, participants);
}

void Room::acked(std::string userId) {
    auto it = std::find(notAcked.begin(), notAcked.end(), userId);
    if (it != notAcked.end()) {
        notAcked.erase(it);
    }
}

ChatMessage* Room::getMessage(const std::string& message) {
    vectorClock[userIndex]++;
    ChatMessage *msg = new ChatMessage(userId, roomId, message, vectorClock);

    messages.insert(std::make_pair(vectorClock, *msg));
    
    return msg;
}

AckMessage* Room::getAckMessage() const {
    return new AckMessage(roomId, userId);
}

void Room::displayMessage(ChatMessage *msg){
    std::string message = msg->getContent();
    std::string senderId = msg->getSenderId();

    std::cout << this->userId << " - " << this->roomId << " - Displayed message: " << message << " from user " << senderId << std::endl;

    if (scheduledForDeletion){
        for (int i = 0; i < numParticipants; i++) {
            if (deletionVectorClock[i] > vectorClock[i]) {
                return;
            }
        }
        throw DeleteMeException(roomId);
    }
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
    }
    if (messagesQueue.size() >= 5) {
        throw QueueTooLongException(getMissingMessages());
    }
}

std::list<AskMessage> Room::askMessages() {
    return getMissingMessages();
}

std::list<AskMessage> Room::getMissingMessages() {
    std::string missingSenderId;
    std::list<AskMessage> missingMessagesList;

    for(int i = 0; i < numParticipants; i++) {
        AskMessage missingMessage = *(new AskMessage(vectorClock[i]+1, participants[i], roomId));
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

bool Room::checkPrinted(ChatMessage *msg) const {
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

bool Room::checkReceived(ChatMessage *msg) const {
    if (checkPrinted(msg)) {
        return true;
    }
    std::vector<int> receivedVectorClock = msg->getVectorClock();

    return messagesQueue.find(receivedVectorClock) != messagesQueue.end();
}