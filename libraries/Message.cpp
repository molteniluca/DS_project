#include "Message.hpp"

// #include <iostream>

Message* Message::createMessage(const omnetpp::cMessage& msg) {
    std::string type = std::string(msg.par("type").stringValue());
    if (type == "message") {
        std::string senderId = msg.par("idMsg").stringValue();
        std::string roomId = msg.par("roomId").stringValue();
        std::string content = std::string(msg.par("message").stringValue());
        std::vector<int> vectorClock = msg.par("vectorClock").objectValue();
        return new ChatMessage(senderId, roomId, content, vectorClock);
    } else if (type == "create_room") {
        std::string adminId = msg.par("idMsg").stringValue();
        std::string roomId = msg.par("roomId").stringValue();
        std::vector<std::string> participants = msg.par("participants").objectValue();
        return new RoomCreationMessage(adminId, roomId, participants);
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
    omnetpp::cMessage *msg = new omnetpp::cMessage("cosaèquesto");
    msg->addPar("idMsg").setStringValue(adminId);
    msg->addPar("type").setStringValue("create_room");
    msg->addPar("roomId").setStringValue(roomId);
    msg->addPar("participants").setObjectValue(participants);
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
    omnetpp::cMessage *msg = new omnetpp::cMessage("cosaèquesto");
    msg->addPar("idMsg").setStringValue(senderId);
    msg->addPar("type").setStringValue("message");
    msg->addPar("roomId").setStringValue(roomId);
    msg->addPar("vectorClock").setObjectValue(vectorClock);
    msg->addPar("message").setStringValue(content.c_str());
    return msg;
}

MessageType ChatMessage::getType() const {
    return MessageType::CHAT;
}
