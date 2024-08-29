#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <map>
#include <vector>
#include <string>

#include "Room.hpp"
#include "Message.hpp"

enum class ActionPerformed {
    CREATED_ROOM,
    DISCARDED_NON_RECIVER_MESSAGE,
    RECIVED_CHAT_MESSAGE,
    DISCARDED_ALREADY_RECIVED_MESSAGE
};

class Client {
private:
    std::string userId;
    std::map<std::string, Room> rooms;

    void handleRoomCreation(RoomCreationMessage *msg);
    void handleChatMessage(ChatMessage *msg);

public:
    Client(std::string userId);

    RoomCreationMessage* createRoom(std::string roomId, std::vector<std::string> participants);
    ChatMessage* getRandomMessage(std::string text);
    ActionPerformed handleMessage(Message *msg);
};

#endif // CLIENT_HPP
