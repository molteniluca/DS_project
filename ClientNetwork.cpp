#include <vector>
#include <algorithm> // For find

#include "ClientNetwork.hpp"
#include "libraries/Message.hpp"
#include "libraries/UserEvent.hpp"
#include "libraries/utils.hpp"

ClientNetwork::ClientNetwork() : cSimpleModule(), personalRoomId(0), personalMessageId(0), client(nullptr), timeToLive(-1) {}

void ClientNetwork::initialize()
{
    this->createRoomMinTime = getParentModule()->par("createRoomMinTime").doubleValue();
    this->createRoomMaxTime = getParentModule()->par("createRoomMaxTime").doubleValue();
    this->createRoomProbability = getParentModule()->par("createRoomProbability").doubleValue();
    this->sendMessageMinTime = getParentModule()->par("sendMessageMinTime").doubleValue();
    this->sendMessageMaxTime = getParentModule()->par("sendMessageMaxTime").doubleValue();
    this->sendMessageProbability = getParentModule()->par("sendMessageProbability").doubleValue();
    this->resendCreationTime = getParentModule()->par("resendCreationTime").doubleValue();
    this->askMessagesTime = getParentModule()->par("askMessagesTime").doubleValue();
    this->deleteRoomMinTime = getParentModule()->par("deleteRoomMinTime").doubleValue();
    this->deleteRoomMaxTime = getParentModule()->par("deleteRoomMaxTime").doubleValue();
    this->deleteRoomProbability = getParentModule()->par("deleteRoomProbability").doubleValue();

    this->stopEventTime = getParentModule()->par("stopEventTime").doubleValue();

    this->client = new Client(std::string(this->getFullName()));
    this->timeToLive = getParentModule()->par("numClients").intValue();
    this->controlPacketSize = getParentModule()->par("controlPacketSize").intValue();
    this->messagePacketSize = getParentModule()->par("messagePacketSize").intValue();

    scheduleAt(simTime() + uniform(createRoomMinTime, createRoomMaxTime), new cMessage(ue_toString(UserEvent::CREATE_ROOM).c_str()));
    scheduleAt(simTime() + uniform(sendMessageMinTime, sendMessageMaxTime), new cMessage(ue_toString(UserEvent::SEND_MESSAGE).c_str()));
    scheduleAt(simTime() + resendCreationTime, new cMessage(ue_toString(UserEvent::RESEND_CREATION).c_str()));
    scheduleAt(simTime() + askMessagesTime, new cMessage(ue_toString(UserEvent::ASK_MESSAGES).c_str()));
}

void ClientNetwork::handleMessage(cMessage *cmsg)
{

    if(cmsg == nullptr) {
        return;
    }

    if(cmsg->isSelfMessage()) {
        handleUserEvent(cmsg);
        delete cmsg;
        return;
    }
    cPacket *msg = check_and_cast<cPacket *>(cmsg);

    if(msg->hasBitError()) {
        EV << this->getFullName() << " - Message with bit error: " << msg->getName() << endl;
        std::cout << this->getFullName() << " - Message with bit error: " << msg->getName() << std::endl;
        delete msg;
        return;
    }

    handleReceivedMessage(msg);

    forwardMessage(msg);

    cancelAndDelete(msg);
    return;
}

void ClientNetwork::handleUserEvent(cMessage *msg)
{
    UserEvent re = ue_fromString(msg->getName());
    switch(re) {

        case UserEvent::CREATE_ROOM:
            if(uniform(0, 1) < createRoomProbability) {
                handleEvent_RoomCreation();
            }
            if(simTime() < this->stopEventTime) {
                scheduleAt(simTime() + uniform(createRoomMinTime, createRoomMaxTime), new cMessage(ue_toString(UserEvent::CREATE_ROOM).c_str()));
                scheduleAt(simTime() + uniform(deleteRoomMinTime, deleteRoomMaxTime), new cMessage(ue_toString(UserEvent::DELETE_ROOM).c_str()));
            }

            break;

        case UserEvent::SEND_MESSAGE:

            if(uniform(0, 1) < sendMessageProbability) {
                handleEvent_SendMessage();
            }
            if(simTime() < this->stopEventTime) {
                scheduleAt(simTime() + uniform(sendMessageMinTime, sendMessageMaxTime), new cMessage(ue_toString(UserEvent::SEND_MESSAGE).c_str()));
            }
            break;

        case UserEvent::RESEND_CREATION:
            handleEvent_ResendCreation();
            scheduleAt(simTime() + resendCreationTime, new cMessage(ue_toString(UserEvent::RESEND_CREATION).c_str()));
            break;

        case UserEvent::ASK_MESSAGES:
            scheduleAt(simTime() + askMessagesTime, new cMessage(ue_toString(UserEvent::ASK_MESSAGES).c_str()));
            handleEvent_AskMessages();
            break;

        case UserEvent::DELETE_ROOM:
            if(uniform(0, 1) < deleteRoomProbability) {
                handleEvent_deleteFirstRoom();
            }
            break;

        default:
            break;
    }

    return;
}

void ClientNetwork::handleEvent_deleteFirstRoom()
{
    std::vector<std::string> rooms = client->getManagedNonDeletedRooms();
    if(rooms.empty()) {
        EV << this->getFullName() << " - No rooms available" << endl;
        std::cout << this->getFullName() << " - No rooms available" << std::endl;
        return;
    }

    std::string roomId = rooms[0];
    RoomDeletionMessage *msg = client->getRoomDeletionMessage(roomId);
    client->deleteRoom(msg);
    cPacket *cMsg = msg->getcPacket();
    cMsg->addPar("timeToLive").setLongValue(this->timeToLive);
    cMsg->setBitLength(this->controlPacketSize);
    sendToAll(cMsg);

    EV << this->getFullName() << " - Deleting room: " << roomId << endl;
    std::cout << this->getFullName() << " - Deleting room: " << roomId << std::endl;

    cancelAndDelete(cMsg);
    delete msg;
    
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

    cPacket *cMsg = msg->getcPacket();
    cMsg->addPar("timeToLive").setLongValue(this->timeToLive);
    cMsg->setBitLength(this->controlPacketSize);
    sendToAll(cMsg);

    EV << this->getFullName() << " - Sent room creation: " << roomId << " with: " << vectorOfStrings_to_String(members) << endl;
    std::cout << this->getFullName() << " - Sent room creation: " << roomId << " with: " << vectorOfStrings_to_String(members) << std::endl;

    cancelAndDelete(cMsg);
    delete msg;
    return;
}

void ClientNetwork::handleEvent_SendMessage()
{
    std::vector<std::string> rooms = client->getNonDeletedRooms();
    if(rooms.empty()) {
        EV << this->getFullName() << " - No rooms available" << endl;
        std::cout << this->getFullName() << " - No rooms available" << std::endl;
        return;
    }

    std::string roomId = rooms[intuniform(0, rooms.size()-1)];
    ChatMessage *msg = client->getMessage("msg("+std::to_string(this->personalMessageId++)+","+getFullName()+")", roomId);

    cPacket *cMsg = msg->getcPacket();
    cMsg->addPar("timeToLive").setLongValue(this->timeToLive);
    cMsg->setBitLength(this->messagePacketSize);
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
        cPacket *cMsg = msg->getcPacket();
        cMsg->addPar("timeToLive").setLongValue(this->timeToLive);
        cMsg->setBitLength(this->controlPacketSize);
        sendToAll(cMsg);
        EV << this->getFullName() << " - Resending room creation: " << cMsg->par("roomId").stringValue() << endl;
        std::cout << this->getFullName() << " - Resending room creation: " << cMsg->par("roomId").stringValue() << std::endl;
        cancelAndDelete(cMsg);
    }

}

void ClientNetwork::handleEvent_AskMessages() {
    std::list<AskMessage> roomsAskMessages = client->askMessages();
    EV << this->getFullName() << " - Start asking for messages" << endl;
    std::cout << this->getFullName() << " - Start asking for messages" << std::endl;
    for(AskMessage am : roomsAskMessages) {
        cPacket *cMsg = am.getcPacket();
        cMsg->addPar("timeToLive").setLongValue(this->timeToLive);
        cMsg->setBitLength(this->controlPacketSize);
        sendToAll(cMsg);

        EV << this->getFullName() << " - Asked for message: " << vectorOfInts_to_String(am.getMissingVectorClock()) << " - Room: " << am.getRoomId() << endl;
        std::cout << this->getFullName() << " - Asked for message: " << vectorOfInts_to_String(am.getMissingVectorClock()) << " - Room: " << am.getRoomId() << std::endl;

        cancelAndDelete(cMsg);
    }
}

void ClientNetwork::handleReceivedMessage(cPacket *msg)
{
    std::pair<ActionPerformed, std::vector<BaseMessage*>>  *result = client->handleMessage(Message::createMessage(*msg));
    ActionPerformed ap = result->first;
    if(ap == ActionPerformed::CREATED_ROOM) {
        std::vector<BaseMessage*> ackList = (std::vector<BaseMessage *>)result->second;
        AckMessage *ack = (AckMessage *) ackList[0];
        cPacket *cMsg = ack->getcPacket();
        cMsg->addPar("timeToLive").setLongValue(this->timeToLive);
        cMsg->setBitLength(this->controlPacketSize);
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
            Message *msg = (Message *) bm;
            if (msg->getType() == MessageType::CHAT) {
                ChatMessage *cm = (ChatMessage *) bm;
                cPacket *cMess = cm->getcPacket();
                cMess->addPar("timeToLive").setLongValue(this->timeToLive);
                cMess->setBitLength(this->messagePacketSize);
                sendToAll(cMess);
                EV << this->getFullName() << " - Replayed message: " << cMess->par("message").stringValue() << " - Room: " << cMess->par("roomId").stringValue() << endl;
                std::cout << this->getFullName() << " - Replayed message: " << cMess->par("message").stringValue() << " - Room: " << cMess->par("roomId").stringValue() << std::endl;
                cancelAndDelete(cMess);
            } else if (msg->getType() == MessageType::DELETE_ROOM) {
                RoomDeletionMessage *rdm = (RoomDeletionMessage *) bm;
                cPacket *cMess = rdm->getcPacket();
                cMess->addPar("timeToLive").setLongValue(this->timeToLive);
                cMess->setBitLength(this->controlPacketSize);
                sendToAll(cMess);
                EV << this->getFullName() << " - Resending deletion: " << cMess->par("roomId").stringValue() << endl;
                std::cout << this->getFullName() << " - Resending deletion: " << cMess->par("roomId").stringValue() << std::endl;
                cancelAndDelete(cMess);
            }
        }
    } else if(ap == ActionPerformed::ROOM_DELETED) {
        EV << this->getFullName() << " - Room scheduled for deletion: " << msg->par("roomId").stringValue() << endl;
        std::cout << this->getFullName() << " - Room scheduled for deletion: " << msg->par("roomId").stringValue() << std::endl;
    }
    return;
}

void ClientNetwork::forwardMessage(cPacket *msg)
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
