#include <iostream>
#include <vector>
#include <omnetpp.h>
#include "utils.hh"


enum class MessageType {
    CREATE_ROOM,
    CHAT,
    UNKNOWN
};


class Message {
public:
    static Message createMessage(const cMessage& msg) {
        std::string type = std::string(msg->par("type").stringValue());
        if (type == "message") {
            std::string senderId = msg->par("idMsg")..stringValue();
            std::string roomId = msg->par("roomId")..stringValue();
            std::string content = std::string(msg->par("message").stringValue());
            std::vector<int> vectorClock = msg->par("vectorClock").objectValue();
            return ChatMessage(senderId, roomId, content, vectorClock);
        } else if (type == "create_room") {
            std::string adminId = msg->par("idMsg").stringValue();
            std::string roomId = msg->par("roomId").stringValue();
            std::vector<std::string> participants = msg->par("participants").objectValue();
            return RoomCreationMessage(adminId, roomId, participants);
        } else {
            throw std::runtime_error("Unknown message type");
        }
    }

    virtual MessageType getType() const = 0;
private:
};

class RoomCreationMessage : public Message {
public:
    RoomCreationMessage(std::string adminId, std::string roomId, std::vector<std::string> participants) : adminId(adminId), roomId(roomId), participants(participants) {}

    std::string getAdminId() const {
        return adminId;
    }

    std::string getRoomId() const {
        return roomId;
    }

    std::vector<std::string> getParticipants() const {
        return participants;
    }

    void cMessage getCmessage() {
        cMessage *msg = new cMessage("cosaèquesto");
        msg->addPar("idMsg").setStringValue(adminId);
        msg->addPar("type").setStringValue("create_room");
        msg->addPar("roomId").setStringValue(roomId);
        msg->addPar("participants").setObjectValue(participants);
        
        return msg;
    }

    MessageType getType() const override {
        return MessageType::CREATE_ROOM;
    }

private:
    std::string adminId;
    std::string roomId;
    std::vector<std::string> participants;
};
    

class ChatMessage : public Message {
public:
    ChatMessage(std::string senderId, std::string roomId, const std::string& content, const std::vector<int>& vectorClock) : senderId(senderId), roomId(roomId), content(content), vectorClock(vectorClock) {}

    std::string getSenderId() const {
        return senderId;
    }

    std::string getRoomId() const {
        return roomId;
    }

    const std::string& getContent() const {
        return content;
    }

    const std::vector<int>& getVectorClock() const {
        return vectorClock;
    }

    void cMessage getCmessage() {
        cMessage *msg = new cMessage("cosaèquesto");
        msg->addPar("idMsg").setStringValue(userId);
        msg->addPar("type").setStringValue("message");
        msg->addPar("roomId").setStringValue(roomId);
        msg->addPar("vectorClock").setObjectValue(vectorClock);
        msg->addPar("message").setStringValue(message.c_str());
        
        return msg;
    }

    MessageType getType() const override {
        return MessageType::CHAT;
    }
    
private:
    std::string senderId;
    std::string roomId;
    std::string content;
    std::vector<int> vectorClock;
};
