#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <vector>
#include <string>
#include <omnetpp.h>

enum class MessageType {
    DELETE_ROOM,
    CREATE_ROOM,
    CHAT,
    UNKNOWN,
    ASK,
    ACK,
};

class BaseMessage {
};

class Message : public BaseMessage {
public:
    static Message* createMessage(omnetpp::cPacket& msg);

    virtual MessageType getType() const = 0;
    virtual omnetpp::cPacket* getcPacket() const = 0;
};

class RoomCreationMessage : public Message {
public:
    RoomCreationMessage(std::string adminId, std::string roomId, std::vector<std::string> participants);

    std::string getAdminId() const;
    std::string getRoomId() const;
    std::vector<std::string> getParticipants() const;

    omnetpp::cPacket* getcPacket() const override;

    MessageType getType() const override;

private:
    std::string adminId;
    std::string roomId;
    std::vector<std::string> participants;
};

class RoomDeletionMessage : public Message {
public:
    RoomDeletionMessage(std::string roomId, std::vector<int> vectorClock);

    std::string getRoomId() const;

    omnetpp::cPacket* getcPacket() const override;

    MessageType getType() const override;

    const std::vector<int>& getVectorClock() const;
private:
    std::string roomId;
    std::vector<int> vectorClock;
};

class ChatMessage : public Message {
public:
    ChatMessage(std::string senderId, std::string roomId, const std::string& content, const std::vector<int>& vectorClock);

    std::string getSenderId() const;
    std::string getRoomId() const;
    const std::string& getContent() const;
    const std::vector<int>& getVectorClock() const;

    omnetpp::cPacket* getcPacket() const override;

    MessageType getType() const override;

private:
    std::string senderId;
    std::string roomId;
    std::string content;
    std::vector<int> vectorClock;
};

class AskMessage : public Message {
public:
    AskMessage(std::string roomId, std::vector<int> &missingVectorClock);
    
    std::vector<int> getMissingVectorClock() const;
    std::string getRoomId() const;

    omnetpp::cPacket* getcPacket() const override;

    MessageType getType() const override;

private:
    std::vector<int> missingVectorClock;
    std::string roomId;
};

class AckMessage : public Message {
public:
    AckMessage(std::string userId, std::string roomId);

    std::string getUserId() const;
    std::string getRoomId() const;

    omnetpp::cPacket* getcPacket() const override;

    MessageType getType() const override;

private:
    std::string userId;
    std::string roomId;
};

#endif // MESSAGE_HPP
