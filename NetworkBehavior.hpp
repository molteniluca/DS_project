#ifndef DYNAMICNETWORK_HPP
#define DYNAMICNETWORK_HPP

#include <omnetpp.h>
#include <set>
#include <tuple>
#include <string>

using namespace omnetpp;

class NetworkBehavior : public cSimpleModule
{
    public:
        NetworkBehavior();

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;

        void handleNetworkEvent(cMessage *msg);

        virtual std::set<std::tuple<std::string,std::string>> handleEvent_partition() = 0;
        virtual void handleEvent_endPartition(std::set<std::tuple<std::string,std::string>> gates) = 0;

    private:
        enum class NetworkEvent {
            PARTITION,
            END_PARTITION
        };

        std::string ne_toString(NetworkEvent ne);
        NetworkEvent ne_fromString(std::string ne);
};

#endif /* DYNAMICNETWORK_HPP */
