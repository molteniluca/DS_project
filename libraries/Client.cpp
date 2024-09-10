#include "Client.hpp"

#include <algorithm> // For find()
#include <stdexcept> // For invalid_argument exception

Client::Client(std::string userId) : userId(userId) { }

std::vector<std::string> Client::getNonDeletedRooms() {
    std::vector<std::string> roomIds;
    for (auto const& room : rooms) {
        if (!room.second.isDeleted())
            roomIds.push_back(room.first);
    }
    return roomIds;
}

std::vector<std::string> Client::getManagedNonDeletedRooms() {
    std::vector<std::string> roomIds;
    for (auto const& room : rooms) {
        if (room.second.amIAdmin() && !room.second.isDeleted())
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

std::vector<ChatMessage*> Client::handleAskMessage(AskMessage *amsg) {
    std::vector<ChatMessage*> cm = rooms[amsg->getRoomId()].resendMessage(amsg);
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
    try{
        rooms[msg->getRoomId()].deleteRoom(msg);
    } catch (Room::DeleteMeException e) {
        rooms.erase(msg->getRoomId());
    }
}

RoomDeletionMessage *  Client::getRoomDeletionMessage(std::string roomId){
    if(rooms.find(roomId) == rooms.end())
        throw std::invalid_argument("Room does not exist");
    return new RoomDeletionMessage(roomId, rooms[roomId].getVectorClock());
}

void Client::handleAckMessage(AckMessage *msg) {
    rooms[msg->getRoomId()].acked(msg->getUserId());
}

std::pair<ActionPerformed, std::vector<BaseMessage*>> * Client::handleMessage(Message *msg) {
    if (msg->getType() == MessageType::CREATE_ROOM) {
        RoomCreationMessage *roomMsg = dynamic_cast<RoomCreationMessage*>(msg);
        std::vector<std::string> participants = roomMsg->getParticipants();
        if(std::find(participants.begin(), participants.end(), userId) == participants.end()){
            return new std::pair(ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE, std::vector<BaseMessage*>());
        }
        if(rooms.find(roomMsg->getRoomId()) != rooms.end()) {
            std::vector<BaseMessage*> ack = std::vector<BaseMessage*>();
            Room room = rooms[roomMsg->getRoomId()];
            ack.push_back(room.getAckMessage());
            return new std::pair(ActionPerformed::DISCARDED_ALREADY_RECEIVED_ROOM_CREATION, ack);
        }
        std::vector<BaseMessage*> ack = std::vector<BaseMessage*>();
        ack.push_back(handleRoomCreation(roomMsg));
        return new std::pair(ActionPerformed::CREATED_ROOM,ack);
    }
    if (msg->getType() == MessageType::CHAT) {
        ChatMessage* chatMsg = dynamic_cast<ChatMessage*>(msg);
        if(rooms.find(chatMsg->getRoomId()) == rooms.end())
            return new std::pair(ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE, std::vector<BaseMessage*>());
        if (rooms[chatMsg->getRoomId()].checkReceived(chatMsg)) {
            return new std::pair(ActionPerformed::DISCARDED_ALREADY_RECEIVED_MESSAGE, std::vector<BaseMessage*>());
        }
        handleChatMessage(chatMsg);
        /// TODO: differentiate between displayed and queued messages
        return new std::pair(ActionPerformed::RECEIVED_CHAT_MESSAGE, std::vector<BaseMessage*>());
    }
    if (msg->getType() == MessageType::ASK) {
        AskMessage* askMsg = dynamic_cast<AskMessage*>(msg);
        if(rooms.find(askMsg->getRoomId()) == rooms.end())
            return new std::pair(ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE, std::vector<BaseMessage*>());
        
        std::vector<BaseMessage *> cm = std::vector<BaseMessage *>();
        std::vector<ChatMessage*> chatMessages = handleAskMessage(askMsg);
        cm.insert(cm.end(), chatMessages.begin(), chatMessages.end());

        Room room = rooms[askMsg->getRoomId()];
        if(room.isDeleted()) {
            cm.push_back(new RoomDeletionMessage(askMsg->getRoomId(), room.getDeletionVectorClock()));
        }
        return new std::pair(ActionPerformed::ANSWERED_ASK_FOR_MESSAGE, cm);
    }
    if(msg->getType() == MessageType::ACK) {
        AckMessage* ackMsg = dynamic_cast<AckMessage*>(msg);
        if(rooms.find(ackMsg->getRoomId()) == rooms.end()) {
            return new std::pair(ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE, std::vector<BaseMessage*>());
        }
        if(!rooms[ackMsg->getRoomId()].amIAdmin()) {
            return new std::pair(ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE, std::vector<BaseMessage*>());
        }
        handleAckMessage(ackMsg);
        return new std::pair(ActionPerformed::RECEIVED_ACK_FOR_ROOM_CREATION, std::vector<BaseMessage*>());
    }
    if(msg->getType() == MessageType::DELETE_ROOM) {
        RoomDeletionMessage* delMsg = dynamic_cast<RoomDeletionMessage*>(msg);
        if(rooms.find(delMsg->getRoomId()) == rooms.end())
            return new std::pair(ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE, std::vector<BaseMessage*>());
        else
            deleteRoom(delMsg);
        return new std::pair(ActionPerformed::ROOM_DELETED, std::vector<BaseMessage*>());
    }

    throw std::invalid_argument("Invalid message type");
}

std::list<AskMessage> Client::askMessages() {
    std::list<AskMessage> askMessages = std::list<AskMessage>();
    for (auto& room : rooms) {
        if (!room.second.isDeleted())
            askMessages.push_back(*room.second.askMessages());
    }
    return askMessages;
}