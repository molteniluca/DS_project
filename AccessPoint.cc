#include <omnetpp.h>

using namespace omnetpp;

class AccessPoint : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void notifyPartition();


  private:
    double timePartition = -1;
};

Define_Module(AccessPoint);

void AccessPoint::initialize()
{
    timePartition = par("timePartition");
    
    if(timePartition>=0)
        scheduleAt(timePartition, new cMessage("notifyPartition"));
}

void AccessPoint::notifyPartition()
{
    EV << "Node detaching from network:" << std::to_string(getIndex());
}

void AccessPoint::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage() && strcmp(msg->getName(), "notifyPartition") == 0) {
        notifyPartition();
    }else {
        if (msg->getArrivalGate()->isName("inClients")) {
            // Broadcast the message to all clients
            for (int i = 0; i < gateSize("outClients"); i++) {
                if (i == msg->getArrivalGate()->getIndex()) {
                    continue;
                }
                send(msg->dup(), "outClients", i);
            }

            // Broadcast it also to other access points if not partitioned
            if (timePartition==-1 || simTime() < timePartition) {
                for (int i = 0; i < gateSize("outAccessPoints"); i++) {
                    send(msg->dup(), "outAccessPoints", i);
                }
            }
        } else {
            // if message is from another access point, process it

            if (timePartition==-1 || simTime() < timePartition) {
                EV << "AP: " << std::to_string(getIndex()) << " relaying message: " << msg->getName() << "\n";
                // Broadcast the message to all clients
                for (int i = 0; i < gateSize("outClients"); i++) {
                    send(msg->dup(), "outClients", i);
                }
            }
        }
    }
    delete msg;
}