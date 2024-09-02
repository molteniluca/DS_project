#include <string>

#include "ClientNetwork.hpp"

using namespace omnetpp;

class ClientFully : public ClientNetwork
{
    public:
        ClientFully();

    protected:
        virtual void initialize() override;
        virtual void sendToAll(cMessage *msg) override;
        virtual void forward(cMessage *msg) override;
    
    private:
        int myIndex;
};

Define_Module(ClientFully);

ClientFully::ClientFully() : ClientNetwork(), myIndex(-1) {}

void ClientFully::initialize()
{
    ClientNetwork::initialize();

    std::string fullName = this->getFullName();
    std::string index = fullName.substr(fullName.find("[") + 1, fullName.find("]") - fullName.find("[") - 1);
    std::cout << "Index: " << index << "name: " << fullName << std::endl;
    this->myIndex = std::stoi(index);
}

void ClientFully::sendToAll(cMessage *msg)
{
    for(int i = 0; i < gateSize("out"); i++) {
        if(i != this->myIndex) {
            cMessage *cMsg = msg->dup();
            try{
                send(cMsg, "out", i);
            } catch(cRuntimeError e) {
                cancelAndDelete(cMsg);
            }
        }
    }
}

void ClientFully::forward(cMessage *msg)
{
    for(int i = 0; i < gateSize("out"); i++) {
        if(std::string(msg->getArrivalGate()->getFullName()) != "in" + std::to_string(i)) {
            if(i != this->myIndex) {
                cMessage *cMsg = msg->dup();
                try{
                    send(cMsg, "out", i);
                } catch(cRuntimeError e) {
                    cancelAndDelete(cMsg);
                }
            }
        }
    }
}
