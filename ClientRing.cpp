#include <string>

#include "ClientNetwork.hpp"

using namespace omnetpp;

class ClientRing : public ClientNetwork
{
    public:
        ClientRing();

    protected:
        virtual void sendToAll(cMessage *msg) override;
        virtual void forward(cMessage *msg) override;
};

Define_Module(ClientRing);

ClientRing::ClientRing() : ClientNetwork() {}

void ClientRing::sendToAll(cMessage *msg)
{
    try{
        send(msg, "out_left");
    } catch(cRuntimeError e) {}

    try{
        send(msg->dup(), "out_right");
    } catch(cRuntimeError e) {}
}

void ClientRing::forward(cMessage *msg)
{
    if(std::string(msg->getArrivalGate()->getName()) == "in_left")
        try{
            send(msg, "out_right");
        } catch(cRuntimeError e) {}
    else if(std::string(msg->getArrivalGate()->getName()) == "in_right")
        try{
            send(msg, "out_left");
        } catch(cRuntimeError e) {}
}
