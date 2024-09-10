#ifndef CLIENTNETWORK_HPP
#define CLIENTNETWORK_HPP

#define ROOM_LIVE_TIME 50000

#include <omnetpp.h>
#include <map>
#include <string>

#include "libraries/Client.hpp"

using namespace omnetpp;

class ClientNetwork : public cSimpleModule
{
    public:
        ClientNetwork();

    protected:
        Client * client;
        int timeToLive;
        int controlPacketSize;
        int messagePacketSize;
        int personalRoomId; // Used to create unique room ids
        int personalMessageId; // Used to create unique message ids
        double stopEventTime;

        double createRoomMinTime, createRoomMaxTime, createRoomProbability,
            sendMessageMinTime, sendMessageMaxTime, sendMessageProbability,
            resendCreationTime,
            askMessagesTime,
            deleteRoomMinTime, deleteRoomMaxTime, deleteRoomProbability;

        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;

        virtual void sendToAll(cPacket *msg) = 0;
        virtual void forward(cPacket *msg) = 0;

        void handleUserEvent(cMessage *msg);
        void handleEvent_RoomCreation();
        void handleEvent_SendMessage();
        void handleEvent_ResendCreation();
        void handleEvent_AskMessages();
        void handleEvent_deleteFirstRoom();

        void handleReceivedMessage(cPacket *msg);
        void forwardMessage(cPacket *msg);
};

#endif // CLIENTNETWORK_HPP
