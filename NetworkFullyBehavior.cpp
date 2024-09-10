#include "NetworkBehavior.hpp"

using namespace omnetpp;

class NetworkFullyBehavior : public NetworkBehavior
{
    public:
        NetworkFullyBehavior();

    protected:
        virtual std::set<std::tuple<std::string,std::string>> handleEvent_partition() override;
        virtual void handleEvent_endPartition(std::set<std::tuple<std::string,std::string>> gates) override;
};

Define_Module(NetworkFullyBehavior);

NetworkFullyBehavior::NetworkFullyBehavior() : NetworkBehavior() {}

std::set<std::tuple<std::string,std::string>> NetworkFullyBehavior::handleEvent_partition() {
    std::set<std::tuple<std::string, std::string>> gates;
    std::set<int> partitionA;
    std::set<int> partitionB;

    int numClients = this->getParentModule()->par("numClients");

    for(int i = 0; i < numClients; i++) {
        if(uniform(0, 1) < 0.5) {
            partitionA.insert(i);
        } else {
            partitionB.insert(i);
        }
    }

    for(int i : partitionA) {
        for(int j : partitionB) {
            cGate *a_in = this->getParentModule()->getSubmodule("client", i)->gate("in", j);
            cGate *a_out = this->getParentModule()->getSubmodule("client", i)->gate("out", j);

            gates.insert(std::make_tuple(a_out->getFullPath(), a_out->getNextGate()->getFullPath()));
            gates.insert(std::make_tuple(a_in->getPreviousGate()->getFullPath(), a_in->getFullPath()));
        }
    }

    for(std::tuple<std::string,std::string> gate : gates) {
        std::string modulePath0 = std::get<0>(gate).substr(0, std::get<0>(gate).rfind("."));
        std::string gateName0 = std::get<0>(gate).substr(std::get<0>(gate).rfind(".") + 1, std::get<0>(gate).rfind("[") - std::get<0>(gate).rfind(".") - 1);
        int index0 = std::stoi(std::get<0>(gate).substr(std::get<0>(gate).rfind("[") + 1, std::get<0>(gate).rfind("]") - std::get<0>(gate).rfind("[") - 1));

        cGate *gate0 = this->getParentModule()->getModuleByPath(modulePath0.c_str())->gate(gateName0.c_str(), index0);
        gate0->disconnect();
    }

    return gates;
}

void NetworkFullyBehavior::handleEvent_endPartition(std::set<std::tuple<std::string, std::string>> gates) {
    for(std::tuple<std::string,std::string> gate : gates) {
        std::string modulePath0 = std::get<0>(gate).substr(0, std::get<0>(gate).rfind("."));
        std::string gateName0 = std::get<0>(gate).substr(std::get<0>(gate).rfind(".") + 1, std::get<0>(gate).rfind("[") - std::get<0>(gate).rfind(".") - 1);
        int index0 = std::stoi(std::get<0>(gate).substr(std::get<0>(gate).rfind("[") + 1, std::get<0>(gate).rfind("]") - std::get<0>(gate).rfind("[") - 1));

        std::string modulePath1 = std::get<1>(gate).substr(0, std::get<1>(gate).rfind("."));
        std::string gateName1 = std::get<1>(gate).substr(std::get<1>(gate).rfind(".") + 1, std::get<1>(gate).rfind("[") - std::get<1>(gate).rfind(".") - 1);
        int index1 = std::stoi(std::get<1>(gate).substr(std::get<1>(gate).rfind("[") + 1, std::get<1>(gate).rfind("]") - std::get<1>(gate).rfind("[") - 1));

        cChannel *channel = cDatarateChannel::create("channel");
        channel->par("delay").setDoubleValue(this->linkDelay);
        channel->par("datarate").setDoubleValue(this->linkDatarate);
        channel->setMode(cDatarateChannel::Mode::MULTI);
        
        cGate *gate0 = this->getParentModule()->getModuleByPath(modulePath0.c_str())->gate(gateName0.c_str(), index0);
        cGate *gate1 = this->getParentModule()->getModuleByPath(modulePath1.c_str())->gate(gateName1.c_str(), index1);

        gate0->connectTo(gate1, channel);
    }

    return;
}
