#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <map>
#include <vector>
#include <string>
#include <set>

#include "Room.hpp"
#include "Message.hpp"

enum class ActionPerformed {
    CREATED_ROOM,
    DISCARDED_NON_RECIPIENT_MESSAGE,
    RECEIVED_CHAT_MESSAGE,
    DISCARDED_ALREADY_RECEIVED_MESSAGE,
    ANSWERED_ASK_FOR_MESSAGE,
    RECEIVED_ACK_FOR_ROOM_CREATION,
    RECEIVED_ACK_FOR_ROOM_CREATION,
    ROOM_DELETED
};

class Client {
private:
    std::string userId;
    std::map<std::string, Room> rooms;

    AckMessage* handleRoomCreation(RoomCreationMessage *msg);
    void handleChatMessage(ChatMessage *msg);
    ChatMessage* handleAskMessage(AskMessage *msg);
    void handleAckMessage(AckMessage *msg);

public:
    Client(std::string userId);

    std::vector<std::string> getRooms();
    RoomCreationMessage* createRoom(std::string roomId, std::vector<std::string> participants);
    std::set<RoomCreationMessage *> creationToResend();
    ChatMessage* getMessage(std::string text, std::string roomId);
    std::pair<ActionPerformed, BaseMessage *> handleMessage(Message *msg);
    std::list<AskMessage> askMessages();
    RoomDeletionMessage * getRoomDeletionMessage(std::string roomId);
    void deleteRoom(RoomDeletionMessage *msg);
};

#endif // CLIENT_HPP
