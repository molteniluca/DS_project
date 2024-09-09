#include "NetworkBehavior.hpp"

using namespace omnetpp;

class NetworkTreeBehavior : public NetworkBehavior
{
    public:
        NetworkTreeBehavior();

    protected:
        virtual std::set<std::tuple<std::string,std::string>> handleEvent_partition() override;
        virtual void handleEvent_endPartition(std::set<std::tuple<std::string,std::string>> gates) override;
};

Define_Module(NetworkTreeBehavior);

NetworkTreeBehavior::NetworkTreeBehavior() : NetworkBehavior() {}

std::set<std::tuple<std::string,std::string>> NetworkTreeBehavior::handleEvent_partition() {
    std::set<std::tuple<std::string, std::string>> gates;
    int numClients = this->getParentModule()->par("numClients");
    int client = uniform(1, numClients - 1);

    cGate *in = this->getParentModule()->getModuleByPath(("client[" + std::to_string(client) + "]").c_str())->gate("in_parent");
    cGate *out = this->getParentModule()->getModuleByPath(("client[" + std::to_string(client) + "]").c_str())->gate("out_parent");

    gates.insert(std::make_tuple(out->getFullPath(), out->getNextGate()->getFullPath()));
    gates.insert(std::make_tuple(in->getPreviousGate()->getFullPath(), in->getFullPath()));

    for(std::tuple<std::string,std::string> gate : gates) {
        std::string modulePath0 = std::get<0>(gate).substr(0, std::get<0>(gate).rfind("."));
        std::string gateName0 = std::get<0>(gate).substr(std::get<0>(gate).rfind(".") + 1);

        cGate *gate0 = this->getParentModule()->getModuleByPath(modulePath0.c_str())->gate(gateName0.c_str());
        gate0->disconnect();
    }   

    return gates;
}

void NetworkTreeBehavior::handleEvent_endPartition(std::set<std::tuple<std::string, std::string>> gates) {
    for(std::tuple<std::string,std::string> gate : gates) {
        std::string modulePath0 = std::get<0>(gate).substr(0, std::get<0>(gate).rfind("."));
        std::string gateName0 = std::get<0>(gate).substr(std::get<0>(gate).rfind(".") + 1);

        std::string modulePath1 = std::get<1>(gate).substr(0, std::get<1>(gate).rfind("."));
        std::string gateName1 = std::get<1>(gate).substr(std::get<1>(gate).rfind(".") + 1);

        cChannel *channel = cDelayChannel::create("channel");
        channel->par("delay").setDoubleValue(this->linkDelay);

        cGate *gate0 = this->getParentModule()->getModuleByPath(modulePath0.c_str())->gate(gateName0.c_str());
        cGate *gate1 = this->getParentModule()->getModuleByPath(modulePath1.c_str())->gate(gateName1.c_str());

        gate0->connectTo(gate1, channel);
    }

    return;
}
