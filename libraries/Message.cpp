#include "Message.hpp"

#include <stdexcept>

#include "utils.hpp"

Message* Message::createMessage(omnetpp::cMessage& msg) {
    std::string type = std::string(msg.par("type").stringValue());
    if (type == "message") {
        std::string senderId = std::string(msg.par("senderId").stringValue());
        std::string roomId = std::string(msg.par("roomId").stringValue());
        std::string content = std::string(msg.par("message").stringValue());
        std::vector<int> vectorClock = string_to_vectorOfInts(std::string(msg.par("vectorClock").stringValue()));
        return new ChatMessage(senderId, roomId, content, vectorClock);
    } else if (type == "create_room") {
        std::string adminId = std::string(msg.par("senderId").stringValue());
        std::string roomId = std::string(msg.par("roomId").stringValue());
        std::vector<std::string> participants = string_to_vectorOfStrings(std::string(msg.par("participants").stringValue()));
        return new RoomCreationMessage(adminId, roomId, participants);
    } else if (type == "ask") {
        int missingMessageId = msg.par("missingMessageId").longValue();
        std::string missingSenderId = std::string(msg.par("missingSenderId").stringValue());
        std::string roomId = std::string(msg.par("roomId").stringValue());
        return new AskMessage(missingMessageId, missingSenderId, roomId);
    } else {
        throw std::runtime_error("Unknown message type");
    }
}

RoomCreationMessage::RoomCreationMessage(std::string adminId, std::string roomId, std::vector<std::string> participants)
    : adminId(adminId), roomId(roomId), participants(participants) {}

std::string RoomCreationMessage::getAdminId() const {
    return adminId;
}

std::string RoomCreationMessage::getRoomId() const {
    return roomId;
}

std::vector<std::string> RoomCreationMessage::getParticipants() const {
    return participants;
}

omnetpp::cMessage* RoomCreationMessage::getCmessage() const {
    omnetpp::cMessage *msg = new omnetpp::cMessage(("creation of room " + roomId).c_str());
    msg->addPar("senderId").setStringValue(adminId.c_str());
    msg->addPar("type").setStringValue("create_room");
    msg->addPar("roomId").setStringValue(roomId.c_str());
    msg->addPar("participants").setStringValue(vectorOfStrings_to_String(participants).c_str());
    return msg;
}

MessageType RoomCreationMessage::getType() const {
    return MessageType::CREATE_ROOM;
}

ChatMessage::ChatMessage(std::string senderId, std::string roomId, const std::string& content, const std::vector<int>& vectorClock)
    : senderId(senderId), roomId(roomId), content(content), vectorClock(vectorClock) {}

std::string ChatMessage::getSenderId() const {
    return senderId;
}

std::string ChatMessage::getRoomId() const {
    return roomId;
}

const std::string& ChatMessage::getContent() const {
    return content;
}

const std::vector<int>& ChatMessage::getVectorClock() const {
    return vectorClock;
}

omnetpp::cMessage* ChatMessage::getCmessage() const {
    omnetpp::cMessage *msg = new omnetpp::cMessage(content.c_str());
    msg->addPar("senderId").setStringValue(senderId.c_str());
    msg->addPar("type").setStringValue("message");
    msg->addPar("roomId").setStringValue(roomId.c_str());
    msg->addPar("vectorClock").setStringValue(vectorOfInts_to_String(vectorClock).c_str());
    msg->addPar("message").setStringValue(content.c_str());
    return msg;
}

MessageType ChatMessage::getType() const {
    return MessageType::CHAT;
}

AskMessage::AskMessage(int missingMessageId, std::string missingSenderId, std::string roomId)
    : missingMessageId(missingMessageId), missingSenderId(missingSenderId), roomId(roomId) {}

int AskMessage::getMissingMessageId() const {
    return missingMessageId;
}

std::string AskMessage::getMissingSenderId() const {
    return missingSenderId;
}

std::string AskMessage::getRoomId() const {
    return roomId;
}

omnetpp::cMessage* AskMessage::getCmessage() const {
    omnetpp::cMessage* msg = new omnetpp::cMessage(("ask message for missing message " + std::to_string(missingMessageId)).c_str());
    msg->addPar("type").setStringValue("ask");
    msg->addPar("missingMessageId").setLongValue(missingMessageId);
    msg->addPar("missingSenderId").setStringValue(missingSenderId.c_str());
    msg->addPar("roomId").setStringValue(roomId.c_str());
    return msg;
}

MessageType AskMessage::getType() const {
    return MessageType::ASK;
}