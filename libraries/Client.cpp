#include "Client.hpp"

#include <algorithm> // For find()
#include <stdexcept> // For invalid_argument exception

Client::Client(std::string userId) : userId(userId) { }

std::vector<std::string> Client::getRooms() {
    std::vector<std::string> roomIds;
    for (auto const& room : rooms) {
        roomIds.push_back(room.first);
    }
    return roomIds;
}

RoomCreationMessage* Client::createRoom(std::string roomId, std::vector<std::string> participants) {
    Room room(participants, userId, roomId);
    rooms[roomId] = room;

    return room.getMessageCreation();
}

std::set<RoomCreationMessage *> Client::creationToResend() {
    std::set<RoomCreationMessage *> creationMessages;
    for (auto& room : rooms) {
        RoomCreationMessage* msg = room.second.getMessageCreationIfNotAcked();
        if (msg != nullptr) {
            creationMessages.insert(msg);
        }
    }
    return creationMessages;
}

ChatMessage* Client::getMessage(std::string text, std::string roomId) {
    if(rooms.find(roomId) == rooms.end())
        throw std::invalid_argument("Room does not exist");

    ChatMessage* msg = rooms[roomId].getMessage(text);

    return msg;
}

ChatMessage *Client::handleAskMessage(AskMessage *amsg) {
    ChatMessage *cm = rooms[amsg->getRoomId()].resendMessage(amsg);
    return cm;
}

AckMessage* Client::handleRoomCreation(RoomCreationMessage *msg) {
    Room room(*msg, userId);
    rooms[room.getRoomId()] = room;
    return room.getAckMessage();
}

void Client::handleChatMessage(ChatMessage *msg) {
    try{
        rooms[msg->getRoomId()].processMessage(msg);
    } catch (Room::DeleteMeException e) {
        rooms.erase(msg->getRoomId());
        std::cout << "Room " << e.getRoomId() << " deleted" << std::endl;
    }
}

void Client::deleteRoom(RoomDeletionMessage *msg) {
    if(rooms.find(msg->getRoomId()) == rooms.end())
        throw std::invalid_argument("Room does not exist");
    rooms[msg->getRoomId()].deleteRoom(msg);
}

RoomDeletionMessage *  Client::getRoomDeletionMessage(std::string roomId){
    if(rooms.find(roomId) == rooms.end())
        throw std::invalid_argument("Room does not exist");
    return new RoomDeletionMessage(roomId, rooms[roomId].getVectorClock());
}

void Client::handleAckMessage(AckMessage *msg) {
    rooms[msg->getRoomId()].acked(msg->getUserId());
}

std::pair<ActionPerformed, BaseMessage*> Client::handleMessage(Message *msg) {
    if (msg->getType() == MessageType::CREATE_ROOM) {
        RoomCreationMessage *roomMsg = dynamic_cast<RoomCreationMessage*>(msg);
        std::vector<std::string> participants = roomMsg->getParticipants();
        if(std::find(participants.begin(), participants.end(), userId) == participants.end()){
            return std::pair(ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE, (BaseMessage*)nullptr);
        }
        if(rooms.find(roomMsg->getRoomId()) != rooms.end())
            return std::pair(ActionPerformed::DISCARDED_ALREADY_RECEIVED_MESSAGE, (BaseMessage*)nullptr);
        AckMessage* ack = handleRoomCreation(roomMsg);
        return std::pair(ActionPerformed::CREATED_ROOM, (BaseMessage*) ack);
    }
    if (msg->getType() == MessageType::CHAT) {
        ChatMessage* chatMsg = dynamic_cast<ChatMessage*>(msg);
        if(rooms.find(chatMsg->getRoomId()) == rooms.end())
            return std::pair(ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE, (BaseMessage*)nullptr);
        if (rooms[chatMsg->getRoomId()].checkReceived(chatMsg)) {
            return std::pair(ActionPerformed::DISCARDED_ALREADY_RECEIVED_MESSAGE, (BaseMessage*)nullptr);
        }
        handleChatMessage(chatMsg);
        /// TODO: differentiate between displayed and queued messages
        return std::pair(ActionPerformed::RECEIVED_CHAT_MESSAGE, (BaseMessage*)nullptr);
    }
    if (msg->getType() == MessageType::ASK) {
        AskMessage* askMsg = dynamic_cast<AskMessage*>(msg);
        if(rooms.find(askMsg->getRoomId()) == rooms.end())
            return std::pair(ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE, (BaseMessage*)nullptr);
        
        return std::pair(ActionPerformed::ASKED_FOR_MESSAGE, handleAskMessage(askMsg));
    }
    if(msg->getType() == MessageType::ACK) {
        AckMessage* ackMsg = dynamic_cast<AckMessage*>(msg);
        if(rooms.find(ackMsg->getRoomId()) == rooms.end())
            return std::pair(ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE, (BaseMessage*)nullptr);
        if(!rooms[ackMsg->getRoomId()].amIAdmin())
            return std::pair(ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE, (BaseMessage*)nullptr);
        handleAckMessage(ackMsg);
        return std::pair(ActionPerformed::RECEIVED_ACK_FOR_ROOM_CREATION, (BaseMessage*)nullptr);
    }
    if(msg->getType() == MessageType::DELETE_ROOM) {
        RoomDeletionMessage* delMsg = dynamic_cast<RoomDeletionMessage*>(msg);
        if(rooms.find(delMsg->getRoomId()) == rooms.end())
            return std::pair(ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE, (BaseMessage*)nullptr);
        try{
            rooms.find(delMsg->getRoomId())->second.deleteRoom(delMsg);
        } catch (Room::DeleteMeException e) {
            rooms.erase(delMsg->getRoomId());
        }
        return std::pair(ActionPerformed::ROOM_DELETED, (BaseMessage*)nullptr);
    }

    throw std::invalid_argument("Invalid message type");
}
