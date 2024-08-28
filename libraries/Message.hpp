#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <omnetpp.h>
#include "utils.hh"

enum class MessageType {
    CREATE_ROOM,
    CHAT,
    UNKNOWN
};

class Message {
public:
    static Message createMessage(const cMessage& msg);

    virtual MessageType getType() const = 0;
};

class RoomCreationMessage : public Message {
public:
    RoomCreationMessage(std::string adminId, std::string roomId, std::vector<std::string> participants);

    std::string getAdminId() const;
    std::string getRoomId() const;
    std::vector<std::string> getParticipants() const;

    cMessage getCmessage() const;

    MessageType getType() const override;

private:
    std::string adminId;
    std::string roomId;
    std::vector<std::string> participants;
};

class ChatMessage : public Message {
public:
    ChatMessage(std::string senderId, std::string roomId, const std::string& content, const std::vector<int>& vectorClock);

    std::string getSenderId() const;
    std::string getRoomId() const;
    const std::string& getContent() const;
    const std::vector<int>& getVectorClock() const;

    cMessage getCmessage() const;

    MessageType getType() const override;

private:
    std::string senderId;
    std::string roomId;
    std::string content;
    std::vector<int> vectorClock;
};

#endif // MESSAGE_HPP
