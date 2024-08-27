#include <omnetpp.h>
#include <vector>
#include <string>
#include <algorithm>

#include "utils.hh"

using namespace omnetpp;

class ClientRingNet : public cSimpleModule
{
    private:
        Client client(this->getName());
    protected:
        virtual void handleMessage(cMessage *msg) override;
        virtual void initialize() override;
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
        if(std::string(msg->getName()) == "create_room") {
            cMessage *new_msg = client.createRoom("stanza", {"c2", "c4"});
            send(new_msg, "out_left");
            send(new_msg->dup(), "out_right");

            EV << this->getName() << " - Sending room creation: stanza with: " << this->getName() << ", c2, c4" << endl;
        }
        if(std::string(msg->getName()) == "send_msg") {
            if(uniform(0, 1) < 0.5) {
                cMessage *new_msg = client.getMessage("msg text");
                send(new_msg, "out_left");
                send(new_msg->dup(), "out_right");

                EV << this->getName() << " - Sending message to room " << room << endl;
            }
            scheduleAt(simTime()+100, new cMessage("send_msg"));
        }
        delete msg;
        return;
    }

/// TODO: sostituire con hop??
    if(client.checkReceived(msg-)) {
        EV << this->getName() << " - Message already received: " << msg->getName() << endl;
        delete msg;
        return;
    }

    if(std::string(msg->getArrivalGate()->getName()) == "in_left")
        send(msg, "out_right");
    else if(std::string(msg->getArrivalGate()->getName()) == "in_right")
        send(msg, "out_left");

    EV << this->getName() << " - Message forwarded: " << msg->getName() << endl;    
    
    if(std::string(msg->par("type").stringValue()) == "room_creation") {        
        std::vector<std::string> members = stringToVector(msg->par("members").stringValue());
        if(std::find(members.begin(), members.end(), std::string(this->getName())) != members.end()) {
            Room room(this->getName(), msg);
            rooms[room.name] = room;
            EV << this->getName() << "- Joining room: " << msg->par("roomName").stringValue() << endl;
        }
    }
    
    if(std::string(msg->par("type").stringValue()) == "msg" && std::find(rooms.begin(), rooms.end(), std::string(msg->par("room").stringValue())) != rooms.end()) {
        EV << this->getName() << " - Message received: " << msg->getName() << endl;
    }

}
