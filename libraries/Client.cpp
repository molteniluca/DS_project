#include "Client.hpp"

#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

Client::Client(std::string userId) : userId(userId) {
    // Seed the random number generator
    std::srand(std::time(nullptr));
}

RoomCreationMessage* Client::createRoom(std::string roomId, std::vector<std::string> participants) {
    Room room(participants, userId, roomId);
    rooms[roomId] = room;

    return room.getMessageCreation();
}

ChatMessage* Client::getRandomMessage(std::string text) {
    if (rooms.empty()) {
        return nullptr;
    }

    // Generate a random index
    int randomIndex = std::rand() % rooms.size();

    // Get the room at the random index
    auto it = rooms.begin();
    std::advance(it, randomIndex);
    Room& randomRoom = it->second;

    // Create a new message for the selected room
    ChatMessage* msg = randomRoom.getMessage(text);

    return msg;
}

void Client::handleRoomCreation(RoomCreationMessage *msg) {
    Room room(*msg);
    rooms[room.getRoomId()] = room;
}

void Client::handleChatMessage(ChatMessage *msg) {
    if (rooms.find(msg->getRoomId()) == rooms.end())
        return;
    rooms[msg->getRoomId()].processMessage(msg);
}

ActionPerformed Client::handleMessage(Message *msg) {
    if (msg->getType() == MessageType::CREATE_ROOM) {
        RoomCreationMessage *roomMsg = dynamic_cast<RoomCreationMessage*>(msg);
        if(rooms.find(roomMsg->getRoomId()) != rooms.end())
            return ActionPerformed::DISCARDED_ALREADY_RECIVED_MESSAGE;
        handleRoomCreation(roomMsg);
        return ActionPerformed::CREATED_ROOM;
    }
    if (msg->getType() == MessageType::CHAT) {
        ChatMessage* chatMsg = dynamic_cast<ChatMessage*>(msg);
        if (rooms[chatMsg->getRoomId()].checkReceived(chatMsg))
            return ActionPerformed::DISCARDED_ALREADY_RECIVED_MESSAGE;
        handleChatMessage(chatMsg);
        return ActionPerformed::RECIVED_CHAT_MESSAGE;
    }

    return ActionPerformed::DISCARDED_ALREADY_RECIVED_MESSAGE;
}
