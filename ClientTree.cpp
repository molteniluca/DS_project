#include <string>

#include "ClientNetwork.hpp"

using namespace omnetpp;

class ClientTree : public ClientNetwork
{
    public:
        ClientTree();

    protected:
        virtual void sendToAll(cMessage *msg) override;
        virtual void forward(cMessage *msg) override;
};

Define_Module(ClientTree);

ClientTree::ClientTree() : ClientNetwork() {}

void ClientTree::sendToAll(cMessage *msg)
{
    cMessage *cMsg = msg->dup();
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

void ClientTree::forward(cMessage *msg)
{
    cMessage *cMsg = nullptr;
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
