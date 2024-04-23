#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;
class Txc1 : public cSimpleModule
{
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Txc1);
void Txc1::initialize()
{
    if (strcmp("tic", getName()) == 0)
    {
        cMessage *msg = new cMessage("tictocMsg");
        send(msg, "out");
    }
}
void Txc1::handleMessage(cMessage *msg)
{
    send(msg, "out");
}