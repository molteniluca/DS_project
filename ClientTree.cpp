#include <string>
#include <cmath>

#include "ClientNetwork.hpp"

using namespace omnetpp;

class ClientTree : public ClientNetwork
{
    public:
        ClientTree();

    protected:
        virtual void initialize() override;
        virtual void sendToAll(cPacket *msg) override;
        virtual void forward(cPacket *msg) override;
};

Define_Module(ClientTree);

ClientTree::ClientTree() : ClientNetwork() {}

void ClientTree::initialize()
{
    ClientNetwork::initialize();
    this->timeToLive = 2*(ceil(log(getParentModule()->par("numClients").intValue()+1)/log(2)));
}

void ClientTree::sendToAll(cPacket *msg)
{
    cPacket *cMsg = msg->dup();
    try{
        send(cMsg, "out_parent");
    } catch(cRuntimeError e) {
        cancelAndDelete(cMsg);
    }
    cMsg = msg->dup();
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

    return;
}

void ClientTree::forward(cPacket *msg)
{
    cPacket *cMsg = nullptr;
    if(std::string(msg->getArrivalGate()->getName()) == "in_parent") {
        cMsg = msg->dup();
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
    } else if(std::string(msg->getArrivalGate()->getName()) == "in_left") {
        cMsg = msg->dup();
        try{
            send(cMsg, "out_parent");
        } catch(cRuntimeError e) {
            cancelAndDelete(cMsg);
        }
        cMsg = msg->dup();
        try{
            send(cMsg, "out_right");
        } catch(cRuntimeError e) {
            cancelAndDelete(cMsg);
        }
    } else if(std::string(msg->getArrivalGate()->getName()) == "in_right") {
        cMsg = msg->dup();
        try{
            send(cMsg, "out_parent");
        } catch(cRuntimeError e) {
            cancelAndDelete(cMsg);
        }
        cMsg = msg->dup();
        try{
            send(cMsg, "out_left");
        } catch(cRuntimeError e) {
            cancelAndDelete(cMsg);
        }
    }

    return;
}
