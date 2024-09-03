#include <vector>
#include <algorithm> // For find

#include "ClientNetwork.hpp"
#include "libraries/Message.hpp"
#include "libraries/UserEvent.hpp"
#include "libraries/utils.hpp"

ClientNetwork::ClientNetwork() : cSimpleModule(), personalRoomId(0), personalMessageId(0), client(nullptr), timeToLive(-1) {}

void ClientNetwork::initialize()
{
    stopEventTime = getParentModule()->par("stopEventTime").doubleValue();
    client = new Client(std::string(this->getFullName()));
    timeToLive = getParentModule()->par("numClients").intValue() / 2;
    scheduleAt(simTime() + uniform(100, 300), new cMessage(ue_toString(UserEvent::CREATE_ROOM).c_str()));
    scheduleAt(simTime() + uniform(100, 300), new cMessage(ue_toString(UserEvent::SEND_MESSAGE).c_str()));
    scheduleAt(simTime() + 200, new cMessage(ue_toString(UserEvent::RESEND_CREATION).c_str()));
    scheduleAt(simTime() + 200, new cMessage(ue_toString(UserEvent::ASK_MESSAGES).c_str()));
}

void ClientNetwork::handleMessage(cMessage *msg)
{
    if(msg == nullptr) {
        return;
    }
    
    if(msg->isSelfMessage()) {
        handleUserEvent(msg);
        delete msg;
        return;
    }

    try{
        handleReceivedMessage(msg);
    } catch (Room::QueueTooLongException &e) {
        AskMessage *am = e.missingMessages;
        cMessage *cMsg = am->getCmessage();
        cMsg->addPar("timeToLive").setLongValue(this->timeToLive);
        sendToAll(cMsg);

        EV << this->getFullName() << " - Asked for message: " << vectorOfInts_to_String(am->getMissingVectorClock()) << " - Room: " << am->getRoomId() << endl;
        std::cout << this->getFullName() << " - Asked for message: " << vectorOfInts_to_String(am->getMissingVectorClock()) << " - Room: " << am->getRoomId() << std::endl;

        cancelAndDelete(cMsg);
    }

    forwardMessage(msg);

    cancelAndDelete(msg);
    return;
}

void ClientNetwork::handleUserEvent(cMessage *msg)
{
    UserEvent re = ue_fromString(msg->getName());
    switch(re) {

        case UserEvent::CREATE_ROOM:
            if(uniform(0, 1) < 0.2) {
                handleEvent_RoomCreation();
            }
            if(simTime() < this->stopEventTime) {
                scheduleAt(simTime() + uniform(1000, 5000), new cMessage(ue_toString(UserEvent::CREATE_ROOM).c_str()));
            }

            scheduleAt(simTime() + ROOM_LIVE_TIME, new cMessage(ue_toString(UserEvent::DELETE_ROOM).c_str()));
            break;

        case UserEvent::SEND_MESSAGE:
        
            if(uniform(0, 1) < 0.5) {
                handleEvent_SendMessage();
            }
            if(simTime() < this->stopEventTime) {
                scheduleAt(simTime() + uniform(100, 300), new cMessage(ue_toString(UserEvent::SEND_MESSAGE).c_str()));
            }
            break;
        
        case UserEvent::RESEND_CREATION:
            handleEvent_ResendCreation();
            scheduleAt(simTime() + 200, new cMessage(ue_toString(UserEvent::RESEND_CREATION).c_str()));
            break;
        
        case UserEvent::ASK_MESSAGES:
            handleEvent_AskMessages();
            scheduleAt(simTime() + 200, new cMessage(ue_toString(UserEvent::ASK_MESSAGES).c_str()));
            break;

        case UserEvent::DELETE_ROOM:
            handleEvent_deleteFirstRoom();
            break;

        default:
            break;
    }

    return;
}

void ClientNetwork::handleEvent_deleteFirstRoom()
{
    std::vector<std::string> rooms = client->getRooms();
    if(rooms.empty()) {
        EV << this->getFullName() << " - No rooms available" << endl;
        std::cout << this->getFullName() << " - No rooms available" << std::endl;
        return;
    }

    std::string roomId = rooms[0];
    RoomDeletionMessage *msg = client->getRoomDeletionMessage(roomId);
    client->deleteRoom(msg);
    cMessage *cMsg = msg->getCmessage();
    cMsg->addPar("timeToLive").setLongValue(this->timeToLive);
    sendToAll(cMsg);

    EV << this->getFullName() << " - Deleting room: " << roomId << endl;
    std::cout << this->getFullName() << " - Deleting room: " << roomId << std::endl;
    return;
}

void ClientNetwork::handleEvent_RoomCreation()
{
    int numClients = getParentModule()->par("numClients").intValue();
    int numMembers = intuniform(2, numClients);
    std::vector<std::string> members;
    members.push_back(this->getFullName());
    while(members.size() < numMembers) {
        std::string clientName = "client[" + std::to_string(intuniform(0, numClients-1)) + "]";
        if(std::find(members.begin(), members.end(), clientName) == members.end()) {
            members.push_back(clientName);
        }
    }

    std::string roomId = "stanza(" + std::string(this->getFullName()) + ", " + std::to_string(this->personalRoomId++) + ")";
    RoomCreationMessage *msg = client->createRoom(roomId, members);

    cMessage *cMsg = msg->getCmessage();
    cMsg->addPar("timeToLive").setLongValue(this->timeToLive);
    sendToAll(cMsg);

    EV << this->getFullName() << " - Sent room creation: " << roomId << " with: " << vectorOfStrings_to_String(members) << endl;
    std::cout << this->getFullName() << " - Sent room creation: " << roomId << " with: " << vectorOfStrings_to_String(members) << std::endl;

    cancelAndDelete(cMsg);
    delete msg;
    return;
}

void ClientNetwork::handleEvent_SendMessage()
{
    std::vector<std::string> rooms = client->getRooms();
    if(rooms.empty()) {
        EV << this->getFullName() << " - No rooms available" << endl;
        std::cout << this->getFullName() << " - No rooms available" << std::endl;
        return;
    }

    std::string roomId = rooms[intuniform(0, rooms.size()-1)];
    ChatMessage *msg = client->getMessage("msg("+std::to_string(this->personalMessageId++)+","+getFullName()+")", roomId);

    cMessage *cMsg = msg->getCmessage();
    cMsg->addPar("timeToLive").setLongValue(this->timeToLive);
    sendToAll(cMsg);

    EV << this->getFullName() << " - Sending message to room " << msg->getRoomId() << " - " << msg->getContent() << endl;
    std::cout << this->getFullName() << " - Sending message to room " << msg->getRoomId()  << " - " << msg->getContent() << std::endl;

    cancelAndDelete(cMsg);
    delete msg;
    return;
}

void ClientNetwork::handleEvent_ResendCreation() {
    std::set<RoomCreationMessage *> creationMsgs = client->creationToResend();
    for(RoomCreationMessage * msg : creationMsgs) {
        cMessage *cMsg = msg->getCmessage();
        cMsg->addPar("timeToLive").setLongValue(this->timeToLive);
        sendToAll(cMsg);
        EV << this->getFullName() << " - Resending room creation: " << cMsg->par("roomId").stringValue() << endl;
        std::cout << this->getFullName() << " - Resending room creation: " << cMsg->par("roomId").stringValue() << std::endl;
        cancelAndDelete(cMsg);
    }

}

void ClientNetwork::handleEvent_AskMessages() {
    std::list<AskMessage> messages = client->askMessages();
    for(AskMessage am : messages) {
        cMessage *cMsg = am.getCmessage();
        cMsg->addPar("timeToLive").setLongValue(this->timeToLive);
        sendToAll(cMsg);

        EV << this->getFullName() << " - Asked for message: " << vectorOfInts_to_String(am.getMissingVectorClock()) << " - Room: " << am.getRoomId() << endl;
        std::cout << this->getFullName() << " - Asked for message: " << vectorOfInts_to_String(am.getMissingVectorClock()) << " - Room: " << am.getRoomId() << std::endl;

        cancelAndDelete(cMsg);
    }
}

void ClientNetwork::handleReceivedMessage(cMessage *msg)
{
    std::pair<ActionPerformed, std::vector<BaseMessage*>>  *result = client->handleMessage(Message::createMessage(*msg));
    ActionPerformed ap = result->first;
    if(ap == ActionPerformed::CREATED_ROOM) {
        std::vector<BaseMessage*> ackList = (std::vector<BaseMessage *>)result->second;
        AckMessage *ack = (AckMessage *) ackList[0];
        cMessage *cMsg = ack->getCmessage();
        cMsg->addPar("timeToLive").setLongValue(this->timeToLive);
        sendToAll(cMsg);
        EV << this->getFullName() << " - Room created: " << msg->par("roomId").stringValue() << endl;
        std::cout << this->getFullName() << " - Room created: " << msg->par("roomId").stringValue() << std::endl;
        EV << this->getFullName() << " - Ack sent: " << cMsg->par("roomId").stringValue() << endl;
        std::cout << this->getFullName() << " - Ack sent: " << cMsg->par("roomId").stringValue() << std::endl;
        cancelAndDelete(cMsg);
    } else if(ap == ActionPerformed::RECEIVED_CHAT_MESSAGE) {
        EV << this->getFullName() << " - Room: " << msg->par("roomId").stringValue() << " - Message received: " << msg->par("message").stringValue() << endl;
        std::cout << this->getFullName() << " - Room: " << msg->par("roomId").stringValue() << " - Message received: " << msg->par("message").stringValue() << std::endl;
    } else if(ap == ActionPerformed::DISCARDED_ALREADY_RECEIVED_MESSAGE) {
        EV << this->getFullName() << " - Room: " << msg->par("roomId").stringValue() << " - Message already received: " << endl;
        std::cout << this->getFullName() << " - Room: " << msg->par("roomId").stringValue() << " - Message already received: " << std::endl;
    } else if(ap == ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE) {
        EV << this->getFullName() << " - Room: " << msg->par("roomId").stringValue() << " - Message discarded for I'm not a recipient: " << endl;
        std::cout << this->getFullName() << " - Room: " << msg->par("roomId").stringValue() << " - Message discarded for I'm not a recipient: " << std::endl;
    } else if(ap == ActionPerformed::ANSWERED_ASK_FOR_MESSAGE) {
        EV << this->getFullName() << " - Room: " << msg->par("roomId").stringValue() << " - Asked for message" << endl;
        std::cout << this->getFullName() << " - Room: " << msg->par("roomId").stringValue() << " - Asked for message" << std::endl;

        std::vector<BaseMessage *> messages = result->second;
        for (BaseMessage *bm : messages) {
            ChatMessage *cm = (ChatMessage *) bm;
            cMessage *cMess = cm->getCmessage();
            cMess->addPar("timeToLive").setLongValue(this->timeToLive);
            sendToAll(cMess);
            EV << this->getFullName() << " - Replayed message: " << cMess->par("message").stringValue() << " - Room: " << cMess->par("roomId").stringValue() << endl;
            std::cout << this->getFullName() << " - Replayed message: " << cMess->par("message").stringValue() << " - Room: " << cMess->par("roomId").stringValue() << std::endl;
            cancelAndDelete(cMess);
        }
    }
    return;
}

void ClientNetwork::forwardMessage(cMessage *msg)
{
    if(msg->par("timeToLive").longValue() > 1) {
        msg->par("timeToLive").setLongValue(msg->par("timeToLive").longValue() - 1);
        forward(msg);
        
        EV << this->getFullName() << " - Message forwarded: " << msg->getName() << " - Time to live: " << msg->par("timeToLive").longValue() << endl;
        std::cout << this->getFullName() << " - Message forwarded: " << msg->getName() << " - Time to live: " << msg->par("timeToLive").longValue() << std::endl;
    }
    else {
        EV << this->getFullName() << " - time to live = 0 for message: " << msg->getName() << endl;
        std::cout << this->getFullName() << " - time to live = 0 for message: " << msg->getName() << std::endl;
    }

    return;
}
