#include <omnetpp.h>

using namespace omnetpp;

class Client : public cSimpleModule
{
  protected:
    virtual void handleMessage(cMessage *msg) override;
    virtual void initialize() override;
    virtual void sendBroadcastMessage(std::string message);
};

Define_Module(Client);

void Client::initialize()
{
    std::string schedName = "broadcast" + std::to_string(getIndex());
    scheduleAt((double)getIndex(), new cMessage(schedName.c_str()));
}

void Client::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        // Send a broadcast message
        sendBroadcastMessage(msg->getName());
        delete msg;
    } else {
        // Received a message from access point, process it if needed
        EV << "Client " << std::to_string(getIndex()) << " received message: " << msg->getName() << "\n";
        delete msg;
    }
}

void Client::sendBroadcastMessage(std::string message)
{
    // Create a new message to be broadcasted
    cMessage *msg = new cMessage(message.c_str());
    send(msg, "out");  // Send the message to access point
}