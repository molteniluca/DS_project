#include "NetworkBehavior.hpp"

using namespace omnetpp;

class NetworkRingBehavior : public NetworkBehavior
{
    public:
        NetworkRingBehavior();

    protected:
        virtual std::set<std::tuple<std::string,std::string>> handleEvent_partition() override;
        virtual void handleEvent_endPartition(std::set<std::tuple<std::string,std::string>> gates) override;
};

Define_Module(NetworkRingBehavior);

NetworkRingBehavior::NetworkRingBehavior() : NetworkBehavior() {}

std::set<std::tuple<std::string,std::string>> NetworkRingBehavior::handleEvent_partition() {
    std::set<std::tuple<std::string, std::string>> gates;
    int partitionA, partitionB;
    int numClients = this->getParentModule()->par("numClients");

    do {
        partitionA = uniform(0, numClients - 1);
        partitionB = uniform(0, numClients - 1);
    } while(partitionA == partitionB);
    
    cGate *a_in = this->getParentModule()->getModuleByPath(("client[" + std::to_string(partitionA) + "]").c_str())->gate("in_right");
    cGate *a_out = this->getParentModule()->getModuleByPath(("client[" + std::to_string(partitionA) + "]").c_str())->gate("out_right");
    
    cGate *b_in = this->getParentModule()->getModuleByPath(("client[" + std::to_string(partitionB) + "]").c_str())->gate("in_right");
    cGate *b_out = this->getParentModule()->getModuleByPath(("client[" + std::to_string(partitionB) + "]").c_str())->gate("out_right");

    gates.insert(std::make_tuple(a_out->getFullPath(), a_out->getNextGate()->getFullPath()));
    gates.insert(std::make_tuple(a_in->getPreviousGate()->getFullPath(), a_in->getFullPath()));
    gates.insert(std::make_tuple(b_out->getFullPath(), b_out->getNextGate()->getFullPath()));
    gates.insert(std::make_tuple(b_in->getPreviousGate()->getFullPath(), b_in->getFullPath()));   

    for(std::tuple<std::string,std::string> gate : gates) {
        std::string modulePath0 = std::get<0>(gate).substr(0, std::get<0>(gate).rfind("."));
        std::string gateName0 = std::get<0>(gate).substr(std::get<0>(gate).rfind(".") + 1);

        cGate *gate0 = this->getParentModule()->getModuleByPath(modulePath0.c_str())->gate(gateName0.c_str());
        gate0->disconnect();
    }

    return gates;
}

void NetworkRingBehavior::handleEvent_endPartition(std::set<std::tuple<std::string, std::string>> gates) {
    for(std::tuple<std::string,std::string> gate : gates) {
        std::string modulePath0 = std::get<0>(gate).substr(0, std::get<0>(gate).rfind("."));
        std::string gateName0 = std::get<0>(gate).substr(std::get<0>(gate).rfind(".") + 1);

        std::string modulePath1 = std::get<1>(gate).substr(0, std::get<1>(gate).rfind("."));
        std::string gateName1 = std::get<1>(gate).substr(std::get<1>(gate).rfind(".") + 1);

        cChannel *channel = cDatarateChannel::create("channel");
        channel->par("delay").setDoubleValue(this->linkDelay);
        channel->par("datarate").setDoubleValue(this->linkDatarate);

        cGate *gate0 = this->getParentModule()->getModuleByPath(modulePath0.c_str())->gate(gateName0.c_str());
        cGate *gate1 = this->getParentModule()->getModuleByPath(modulePath1.c_str())->gate(gateName1.c_str());

        gate0->connectTo(gate1, channel);
    }

    return;
}
