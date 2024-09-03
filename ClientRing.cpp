#include <string>

#include "ClientNetwork.hpp"

using namespace omnetpp;

class ClientRing : public ClientNetwork
{
    public:
        ClientRing();

    protected:
        virtual void initialize() override;
        virtual void sendToAll(cMessage *msg) override;
        virtual void forward(cMessage *msg) override;
};

Define_Module(ClientRing);

ClientRing::ClientRing() : ClientNetwork() {}

void ClientRing::initialize()
{
    ClientNetwork::initialize();
    this->timeToLive = getParentModule()->par("numClients").intValue() / 2;
}

void ClientRing::sendToAll(cMessage *msg)
{
    cMessage *cMsg = msg->dup();
    try{
        send(cMsg, "out_left");
    } catch(cRuntimeError e) {
        cancelAndDelete(cMsg);
    }

    cMsg = msg->dup();
    try{
        send(cMsg, "out_right");
    } catch(cRuntimeError e) {
        cancelAndDelete(cMsg);
    }
}

void ClientRing::forward(cMessage *msg)
{
    if(std::string(msg->getArrivalGate()->getName()) == "in_left") {
        cMessage *cMsg = msg->dup();
        try{
            send(cMsg, "out_right");
        } catch(cRuntimeError e) {
            cancelAndDelete(cMsg);
        }
    }
    else if(std::string(msg->getArrivalGate()->getName()) == "in_right") {
        cMessage *cMsg = msg->dup();
        try{
            send(cMsg, "out_left");
        } catch(cRuntimeError e) {
            cancelAndDelete(cMsg);
        }
    }
}
