#include "Message.hpp"

#include <stdexcept>

#include "utils.hpp"

Message* Message::createMessage(omnetpp::cPacket& msg) {
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
        std::string roomId = std::string(msg.par("roomId").stringValue());
        std::vector<int> vectorClock = string_to_vectorOfInts(std::string(msg.par("vectorClock").stringValue()));
        return new AskMessage(roomId, vectorClock);
    } else if (type == "ack") {
        std::string userId = std::string(msg.par("userId").stringValue());
        std::string roomId = std::string(msg.par("roomId").stringValue());
        return new AckMessage(roomId, userId);
    } else if (type == "delete_room") {
        std::string roomId = std::string(msg.par("roomId").stringValue());
        std::vector<int> vectorClock = string_to_vectorOfInts(std::string(msg.par("vectorClock").stringValue()));
        return new RoomDeletionMessage(roomId, vectorClock);
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

omnetpp::cPacket* RoomCreationMessage::getcPacket() const {
    omnetpp::cPacket *msg = new omnetpp::cPacket(("creation of room " + roomId).c_str());
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

omnetpp::cPacket* ChatMessage::getcPacket() const {
    omnetpp::cPacket *msg = new omnetpp::cPacket(content.c_str());
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

AskMessage::AskMessage(std::string roomId, std::vector<int> &missingVectorClock)
    : missingVectorClock(missingVectorClock), roomId(roomId) {}

std::vector<int> AskMessage::getMissingVectorClock() const {
    return missingVectorClock;
}

std::string AskMessage::getRoomId() const {
    return roomId;
}

omnetpp::cPacket* AskMessage::getcPacket() const {
    omnetpp::cPacket* msg = new omnetpp::cPacket(("ask message for room " + roomId).c_str());
    msg->addPar("type").setStringValue("ask");
    msg->addPar("roomId").setStringValue(roomId.c_str());
    msg->addPar("vectorClock").setStringValue(vectorOfInts_to_String(missingVectorClock).c_str());
    return msg;
}

MessageType AskMessage::getType() const {
    return MessageType::ASK;
}

AckMessage::AckMessage(std::string roomId, std::string userId) : userId(userId), roomId(roomId) {}

std::string AckMessage::getUserId() const {
    return userId;
}

std::string AckMessage::getRoomId() const {
    return roomId;
}

omnetpp::cPacket* AckMessage::getcPacket() const {
    omnetpp::cPacket* msg = new omnetpp::cPacket(("ack message for room " + roomId).c_str());
    msg->addPar("type").setStringValue("ack");
    msg->addPar("userId").setStringValue(userId.c_str());
    msg->addPar("roomId").setStringValue(roomId.c_str());
    return msg;
}

MessageType AckMessage::getType() const {
    return MessageType::ACK;
}

RoomDeletionMessage::RoomDeletionMessage(std::string roomId, std::vector<int> vectorClock)
    : roomId(roomId), vectorClock(vectorClock) {}

MessageType RoomDeletionMessage::getType() const {
    return MessageType::DELETE_ROOM;
}

std::string RoomDeletionMessage::getRoomId() const {
    return roomId;
}

const std::vector<int>& RoomDeletionMessage::getVectorClock() const {
    return vectorClock;
}

omnetpp::cPacket* RoomDeletionMessage::getcPacket() const {
    omnetpp::cPacket* msg = new omnetpp::cPacket(("deletion of room " + roomId).c_str());
    msg->addPar("type").setStringValue("delete_room");
    msg->addPar("roomId").setStringValue(roomId.c_str());
    msg->addPar("vectorClock").setStringValue(vectorOfInts_to_String(vectorClock).c_str());
    return msg;
}