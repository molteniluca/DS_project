#include "Client.hpp"

#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <algorithm> // For find()
#include <stdexcept> // For invalid_argument exception

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
    Room room(*msg, userId);
    rooms[room.getRoomId()] = room;
}

void Client::handleChatMessage(ChatMessage *msg) {
    rooms[msg->getRoomId()].processMessage(msg);
}

ActionPerformed Client::handleMessage(Message *msg) {
    if (msg->getType() == MessageType::CREATE_ROOM) {
        RoomCreationMessage *roomMsg = dynamic_cast<RoomCreationMessage*>(msg);
        std::vector<std::string> participants = roomMsg->getParticipants();
        if(std::find(participants.begin(), participants.end(), userId) == participants.end()){
            return ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE;
        }
        if(rooms.find(roomMsg->getRoomId()) != rooms.end())
            return ActionPerformed::DISCARDED_ALREADY_RECEIVED_MESSAGE;
        handleRoomCreation(roomMsg);
        return ActionPerformed::CREATED_ROOM;
    }
    if (msg->getType() == MessageType::CHAT) {
        ChatMessage* chatMsg = dynamic_cast<ChatMessage*>(msg);
        if(rooms.find(chatMsg->getRoomId()) == rooms.end())
            return ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE;
        if (rooms[chatMsg->getRoomId()].checkReceived(chatMsg)) {
            return ActionPerformed::DISCARDED_ALREADY_RECEIVED_MESSAGE;
        }
        handleChatMessage(chatMsg);
        /// TODO: differentiate between displayed and queued messages
        return ActionPerformed::RECEIVED_CHAT_MESSAGE;
    }

    throw std::invalid_argument("Invalid message type");
}
