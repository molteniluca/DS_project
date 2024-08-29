#include <omnetpp.h>
#include <vector>
#include <string>
#include <algorithm>

#include "libraries/Client.hpp"
#include "libraries/Message.hpp"
#include "libraries/UserEvent.hpp"
#include "libraries/utils.hpp"

using namespace omnetpp;

class ClientRing : public cSimpleModule
{
    public:
        ClientRing();

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;

    private:
        Client * client;
        int timeToLive;
        int personalRoomId; // Used to create unique room ids

        void handleUserEvent(cMessage *msg);
        void handleEvent_RoomCreation();
        void handleEvent_SendMessage();

        void handleReceivedMessage(cMessage *msg);
        void forwardMessage(cMessage *msg);
};

Define_Module(ClientRing);

ClientRing::ClientRing() : cSimpleModule(), personalRoomId(0), client(nullptr), timeToLive(-1) {}

void ClientRing::initialize()
{
    client = new Client(std::string(this->getFullName()));
    timeToLive = getParentModule()->par("numClients").intValue() / 2;
    scheduleAt(simTime() + uniform(100, 300), new cMessage(ue_toString(UserEvent::CREATE_ROOM).c_str()));
    scheduleAt(simTime() + uniform(100, 300), new cMessage(ue_toString(UserEvent::SEND_MESSAGE).c_str()));
}

void ClientRing::handleMessage(cMessage *msg)
{
    if(msg == nullptr) {
        return;
    }
    
    if(msg->isSelfMessage()) {
        handleUserEvent(msg);
        return;
    }

    handleReceivedMessage(msg);

    forwardMessage(msg);

    return;
}

void ClientRing::handleUserEvent(cMessage *msg)
{
    UserEvent re = ue_fromString(msg->getName());
    switch(re) {

        case UserEvent::CREATE_ROOM:
            if(uniform(0, 1) < 0.2) {
                handleEvent_RoomCreation();
            }
            scheduleAt(simTime() + uniform(1000, 5000), new cMessage(ue_toString(UserEvent::CREATE_ROOM).c_str()));
            break;

        case UserEvent::SEND_MESSAGE:
        
            if(uniform(0, 1) < 0.5) {
                handleEvent_SendMessage();
            }
            scheduleAt(simTime() + uniform(100, 300), new cMessage(ue_toString(UserEvent::SEND_MESSAGE).c_str()));
            break;

        default:
            break;
    }

    delete msg;
    return;
}

void ClientRing::handleEvent_RoomCreation()
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
    send(cMsg, "out_left");
    send(cMsg->dup(), "out_right");

    EV << this->getFullName() << " - Sent room creation: " << roomId << " with: " << vectorOfStrings_to_String(members) << endl;
    std::cout << this->getFullName() << " - Sent room creation: " << roomId << " with: " << vectorOfStrings_to_String(members) << std::endl;

    delete msg;
    return;
}

void ClientRing::handleEvent_SendMessage()
{
    ChatMessage *msg = client->getRandomMessage("msg text");

    if(msg == nullptr) {
        EV << this->getFullName() << " - No rooms available" << endl;
        std::cout << this->getFullName() << " - No rooms available" << std::endl;
        return;
    }

    cMessage *cMsg = msg->getCmessage();
    cMsg->addPar("timeToLive").setLongValue(this->timeToLive);
    send(cMsg, "out_left");
    send(cMsg->dup(), "out_right");

    EV << this->getFullName() << " - Sending message to room " << msg->getRoomId() << endl;
    std::cout << this->getFullName() << " - Sending message to room " << msg->getRoomId() << std::endl;

    delete msg;
    return;
}

void ClientRing::handleReceivedMessage(cMessage *msg)
{
    ActionPerformed ap = client->handleMessage(Message::createMessage(*msg));
    if(ap == ActionPerformed::CREATED_ROOM) {
        EV << this->getFullName() << " - Room created: " << msg->par("roomId").stringValue() << endl;
        std::cout << this->getFullName() << " - Room created: " << msg->par("roomId").stringValue() << std::endl;
    } else if(ap == ActionPerformed::RECEIVED_CHAT_MESSAGE) {
        EV << this->getFullName() << " - Room: " << msg->par("roomId").stringValue() << " - Message received: " << msg->par("message").stringValue() << endl;
        std::cout << this->getFullName() << " - Room: " << msg->par("roomId").stringValue() << " - Message received: " << msg->par("message").stringValue() << std::endl;
    } else if(ap == ActionPerformed::DISCARDED_ALREADY_RECEIVED_MESSAGE) {
        EV << this->getFullName() << " - Room: " << msg->par("roomId").stringValue() << " - Message already received: " << endl;
        std::cout << this->getFullName() << " - Room: " << msg->par("roomId").stringValue() << " - Message already received: " << std::endl;
    } else if(ap == ActionPerformed::DISCARDED_NON_RECIPIENT_MESSAGE) {
        EV << this->getFullName() << " - Room: " << msg->par("roomId").stringValue() << " - Message discarded for I'm not a recipient: " << endl;
        std::cout << this->getFullName() << " - Room: " << msg->par("roomId").stringValue() << " - Message discarded for I'm not a recipient: " << std::endl;
    }

    return;
}

void ClientRing::forwardMessage(cMessage *msg)
{
    if(msg->par("timeToLive").longValue() > 1) {
        msg->par("timeToLive").setLongValue(msg->par("timeToLive").longValue() - 1);
        if(std::string(msg->getArrivalGate()->getName()) == "in_left")
            send(msg, "out_right");
        else if(std::string(msg->getArrivalGate()->getName()) == "in_right")
            send(msg, "out_left");
        
        EV << this->getFullName() << " - Message forwarded: " << msg->getName() << " - Time to live: " << msg->par("timeToLive").longValue() << endl;
        std::cout << this->getFullName() << " - Message forwarded: " << msg->getName() << " - Time to live: " << msg->par("timeToLive").longValue() << std::endl;

        return;
    }
    else {
        EV << this->getFullName() << " - time to live = 0 for message: " << msg->getName() << endl;
        std::cout << this->getFullName() << " - time to live = 0 for message: " << msg->getName() << std::endl;
        delete msg;
        return;
    }
}
