#include <omnetpp.h>
#include <vector>
#include <string>
#include <algorithm>

#include "libraries/Client.hpp"
#include "libraries/Message.hpp"
#include "libraries/Room.hpp"

using namespace omnetpp;

class ClientRing : public cSimpleModule
{
    private:
        Client client;
    protected:
        virtual void handleMessage(cMessage *msg) override;
        virtual void initialize() override;
    public:
        ClientRing() : cSimpleModule(), client(this->getName()) {}
};

Define_Module(ClientRing);

void ClientRing::initialize()
{
    if(std::string(this->getName()) == "c0") {
        scheduleAt(simTime()+1, new cMessage("create_room"));
    }
    scheduleAt(simTime()+100, new cMessage("send_msg"));
}

void ClientRing::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()) {
        // Random event
        if(std::string(msg->getName()) == "create_room") {
            // Create Room
            cMessage *new_msg = client.createRoom("stanza", {this->getName(), "c2", "c4"})->getCmessage();
            new_msg->addPar("timeToLive").setLongValue(10);
            send(new_msg, "out_left");
            send(new_msg->dup(), "out_right");
            delete new_msg;

            EV << this->getName() << " - Sending room creation: stanza with: " << this->getName() << ", c2, c4" << endl;
        }
        if(std::string(msg->getName()) == "send_msg") {
            if(uniform(0, 1) < 0.5) {
                // Create Message
                ChatMessage *msgToSend = client.getRandomMessage("msg text");
                if(msgToSend == nullptr) {
                    EV << this->getName() << " - No rooms available" << endl;
                    scheduleAt(simTime()+100, new cMessage("send_msg"));
                    delete msg;
                    return;
                }
                
                cMessage *new_msg = msgToSend->getCmessage();
                new_msg->addPar("timeToLive").setLongValue(10);
                send(new_msg, "out_left");
                send(new_msg->dup(), "out_right");
                delete msgToSend;
                delete new_msg;

                EV << this->getName() << " - Sending message to room " << msgToSend->getRoomId() << endl;
            }
            scheduleAt(simTime()+100, new cMessage("send_msg"));
        }
        delete msg;
        return;
    }

    ActionPerformed ap = client.handleMessage(Message::createMessage(*msg));
    if(ap == ActionPerformed::CREATED_ROOM) {
        EV << this->getName() << " - Room created: " << msg->par("roomId").stringValue() << endl;
    } else if(ap == ActionPerformed::RECIVED_CHAT_MESSAGE) {
        EV << this->getName() << " - Room: " << msg->par("roomId").stringValue() << " - Message received: " << msg->par("message").stringValue() << endl;
    } else if(ap == ActionPerformed::DISCARDED_ALREADY_RECIVED_MESSAGE) {
        EV << this->getName() << " - Room: " << msg->par("roomId").stringValue() << " - Message already received: " << msg->par("message").stringValue() << endl;
    }

    if(msg->par("timeToLive").longValue() > 1) {
        msg->par("timeToLive").setLongValue(msg->par("timeToLive").longValue() - 1);
        if(std::string(msg->getArrivalGate()->getName()) == "in_left")
            send(msg, "out_right");
        else if(std::string(msg->getArrivalGate()->getName()) == "in_right")
            send(msg, "out_left");
    }

    EV << this->getName() << " - Message forwarded: " << msg->getName() << endl;

    delete msg;
    return;
}
