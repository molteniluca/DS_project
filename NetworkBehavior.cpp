#include "NetworkBehavior.hpp"

#include <vector>

#include "libraries/utils.hpp"

NetworkBehavior::NetworkBehavior() : cSimpleModule() {}

void NetworkBehavior::initialize() {
    cMessage *partitionEvent = new cMessage(ne_toString(NetworkEvent::PARTITION).c_str());
    scheduleAt(simTime() + uniform(100, 200), partitionEvent);
}

void NetworkBehavior::handleMessage(cMessage *msg) {
    if(msg == nullptr) {
        return;
    }

    if(!msg->isSelfMessage()) {
        cancelAndDelete(msg);
        return;
    }

    handleNetworkEvent(msg);

    cancelAndDelete(msg);
    return;
}

void NetworkBehavior::handleNetworkEvent(cMessage *msg) {
    NetworkEvent ne = ne_fromString(msg->getName());
    std::set<std::tuple<std::string,std::string>> gates;
    switch(ne) {
        case NetworkEvent::PARTITION: {
            gates = handleEvent_partition();
            cMessage *endPartitionEvent = new cMessage(ne_toString(NetworkEvent::END_PARTITION).c_str());
            endPartitionEvent->addPar("gates").setStringValue(setOfTuplesOfStrings_to_String(gates).c_str());
            scheduleAt(simTime() + uniform(1000, 2000), endPartitionEvent);
            EV << "Partitioning network" << endl;
            std::cout << "Partitioning network" << std::endl;
            break;
        }
        case NetworkEvent::END_PARTITION: {
            gates = string_to_setOfTuplesOfStrings(msg->par("gates").stringValue());
            handleEvent_endPartition(gates);
            cMessage *partitionEvent = new cMessage(ne_toString(NetworkEvent::PARTITION).c_str());
            scheduleAt(simTime() + uniform(1000, 2000), partitionEvent);
            EV << "Ending partition" << endl;
            std::cout << "Ending partition" << std::endl;
            break;
        }
    }

    return;
}

std::string NetworkBehavior::ne_toString(NetworkEvent ne) {
    switch (ne) {
    case NetworkEvent::PARTITION:
        return "PARTITION";
    case NetworkEvent::END_PARTITION:
        return "END_PARTITION";
    default:
        throw std::invalid_argument("Invalid NetworkEvent");
    }
}

NetworkBehavior::NetworkEvent NetworkBehavior::ne_fromString(std::string ne) {
    if(ne == "PARTITION")
        return NetworkEvent::PARTITION;
    else if(ne == "END_PARTITION")
        return NetworkEvent::END_PARTITION;

    throw std::invalid_argument("Invalid NetworkEvent string");
}
