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
    send(msg, "out_left");
    send(msg->dup(), "out_right");
}

void ClientRing::forward(cMessage *msg)
{
    if(std::string(msg->getArrivalGate()->getName()) == "in_left")
        send(msg, "out_right");
    else if(std::string(msg->getArrivalGate()->getName()) == "in_right")
        send(msg, "out_left");
}
