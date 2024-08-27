#include <iostream>
#include <vector>
#include <omnetpp.h>
#include "utils.hh"

class Message {
public:
    static Message createMessage(const cMessage& msg) {
        std::string type = std::string(msg->par("type").stringValue());
        if (type == "message") {
            int senderId = msg->par("idMsg").longValue();
            int roomId = msg->par("roomId").longValue();
            std::string content = std::string(msg->par("message").stringValue());
            std::vector<int> vectorClock = msg->par("vectorClock").objectValue();
            return ChatMessage(senderId, roomId, content, vectorClock);
        } else if (type == "create_room") {
            int adminId = msg->par("idMsg").longValue();
            int roomId = msg->par("roomId").longValue();
            return RoomCreationMessage(adminId, roomId);
        } else {
            throw std::runtime_error("Unknown message type");
        }
    }
private:
};

class RoomCreationMessage : public Message {
public:
    RoomCreationMessage(int adminId, int roomId) : adminId(adminId), roomId(roomId) {}

    int getAdminId() const {
        return adminId;
    }

    int getRoomId() const {
        return roomId;
    }

    void cMessage getCmessage() {
        cMessage *msg = new cMessage("cosaèquesto");
        msg->addPar("idMsg").setLongValue(adminId);
        msg->addPar("type").setStringValue("create_room");
        msg->addPar("roomId").setLongValue(roomId);
        
        return msg;
    }

private:
    int adminId;
    int roomId;
};
    

class ChatMessage : public Message {
public:
    ChatMessage(int senderId, int roomId, const std::string& content, const std::vector<int>& vectorClock) : senderId(senderId), roomId(roomId), content(content), vectorClock(vectorClock) {}

    int getSenderId() const {
        return senderId;
    }

    int getRoomId() const {
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
        msg->addPar("idMsg").setLongValue(userId);
        msg->addPar("type").setStringValue("message");
        msg->addPar("roomId").setLongValue(roomId);
        msg->addPar("vectorClock").setObjectValue(vectorClock);
        msg->addPar("message").setStringValue(message.c_str());
        
        return msg;
    }
    
private:
    int senderId;
    int roomId;
    std::string content;
    std::vector<int> vectorClock;
};
