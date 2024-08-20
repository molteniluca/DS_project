#include <omnetpp.h>
#include <vector>
#include <string>
#include <algorithm>

#include "utils.hh"

using namespace omnetpp;

class ClientRing : public cSimpleModule
{
  protected:
    int nextID;
    std::vector<std::string> rooms;
    std::vector<std::string> ids;
    virtual void handleMessage(cMessage *msg) override;
    virtual void initialize() override;

};

Define_Module(ClientRing);

void ClientRing::initialize()
{
    this->nextID = 0;
    if(std::string(this->getName()) == "c0") {
        scheduleAt(simTime()+1, new cMessage("create_room"));
    }
    scheduleAt(simTime()+100, new cMessage("send_msg"));
}

void ClientRing::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()) {
        if(std::string(msg->getName()) == "create_room") {
            EV << this->getName() << "- Creating room: stanza; with: " << this->getName() << ", c2, c4" << endl;
            std::string id = (std::string(this->getName()) + ":" + std::to_string(this->nextID++));
            cMessage *msg = new cMessage("stanza:creation");
            msg->addPar("id").setStringValue(id.c_str());
            msg->addPar("type").setStringValue("room_creation");
            msg->addPar("roomName").setStringValue("stanza");
            msg->addPar("members").setStringValue(vectorToString({this->getName(), "c2", "c4"}).c_str());
            send(msg, "out_left");
            send(msg->dup(), "out_right");
            rooms.push_back("stanza");
            ids.push_back(id);
            if(ids.size() > 100) {
                ids.erase(ids.begin());
            }
        }
        if(std::string(msg->getName()) == "send_msg") {
            if(rooms.size() > 0 && uniform(0, 1) < 0.5) {
                std::string room = rooms[(int)uniform(0, rooms.size())];
                cMessage *new_msg = new cMessage((room + ":msg").c_str());
                std::string id = (std::string(this->getName()) + ":" + std::to_string(this->nextID++));
                new_msg->addPar("id").setStringValue(id.c_str());
                new_msg->addPar("type").setStringValue("msg");
                new_msg->addPar("room").setStringValue(room.c_str());
                EV << this->getName() << "- Sending message to room " << room << endl;
                send(new_msg, "out_left");
                send(new_msg->dup(), "out_right");
                ids.push_back(id);
                if(ids.size() > 100) {
                    ids.erase(ids.begin());
                }
            }
            scheduleAt(simTime()+100, new cMessage("send_msg"));
        }
        delete msg;
        return;
    }


    if(std::find(ids.begin(), ids.end(), std::string(msg->par("id").stringValue())) != ids.end()) {
        EV << this->getName() << "- Message already received: " << msg->getName() << endl;
        delete msg;
        return;
    }

    if(std::string(msg->getArrivalGate()->getName()) == "in_left")
        send(msg, "out_right");
    else if(std::string(msg->getArrivalGate()->getName()) == "in_right")
        send(msg, "out_left");
    
    ids.push_back(std::string(msg->par("id").stringValue()));
    if(ids.size() > 100) {
        ids.erase(ids.begin());
    }
    
    
    if(std::string(msg->par("type").stringValue()) == "room_creation") {
        std::vector<std::string> members = stringToVector(msg->par("members").stringValue());
        if(std::find(members.begin(), members.end(), std::string(this->getName())) != members.end()) {
            rooms.push_back(std::string(msg->par("roomName").stringValue()));
            EV << this->getName() << "- Joining room: " << msg->par("roomName").stringValue() << endl;
        }
    }
    
    if(std::string(msg->par("type").stringValue()) == "msg" && std::find(rooms.begin(), rooms.end(), std::string(msg->par("room").stringValue())) != rooms.end()) {
        EV << this->getName() << "- Message received: " << msg->getName() << endl;
    }

}
