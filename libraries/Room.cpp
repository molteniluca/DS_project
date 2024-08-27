#include <iostream>
#include <vector>
#include <omnetpp.h>
#include "utils.hh"

class Room {
public:
    Room(int numParticipants) : numParticipants(numParticipants), vectorClock(numParticipants, 0) {}

    ChatMessage getMessage(const std::string& message) {
        vectorClock[userId]++;

        return ChatMessage(userId, roomId, message, vectorClock);
    }

    Room Room(RoomCrestionMessage msg) {
        adminId = msg.getAdminId();
        roomId = msg.getRoomId();
    }

    void processMessage(ChatMessage *msg) {
        std::vector<int> receivedVectorClock = msg.getVectorClock();
        std::string message = msg.getContent();
        int senderId = msg.getSenderId();

        std::cout << "Received message: " << message << " from user " << senderId << std::endl;
    }

    bool checkReceived(ChatMessage *msg) {
        std::vector<int> receivedVectorClock = msg.getVectorClock();
        int senderId = msg.getSenderId();

        for(int i = 0; i < numParticipants; i++) {
            if(i != senderId) {
                if(receivedVectorClock[i] > vectorClock[i]) {
                    return false;
                }
            }
        }

        return true;
    }


private:
    int numParticipants;
    int userId;
    int adminId;
    int roomId;
    std::vector<int> vectorClock;   
    std::vector<std::string> messages;
};
