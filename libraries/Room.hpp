#ifndef ROOM_HPP
#define ROOM_HPP

#include <vector>
#include <string>

#include "Message.hpp"

class Room {
public:
    Room(std::vector<std::string> participants, std::string adminId, std::string roomId);

    Room(RoomCreationMessage msg, std::string userId);

    // necessary for using Room in std::map
    Room();

    bool amIAdmin() const;

    std::string getRoomId() const;

    std::vector<std::string> getParticipants() const;

    RoomCreationMessage* getMessageCreation() const;

    RoomCreationMessage* getMessageCreationIfNotAcked() const;

    ChatMessage* getMessage(const std::string& message);

    AckMessage* getAckMessage() const;

    void acked(std::string userId);

    void processMessage(ChatMessage *msg);

    bool checkReceived(ChatMessage *msg) const;

    bool checkPrinted(ChatMessage *msg) const;

    void flushMessages();

    void displayMessage(ChatMessage *msg);

    int lookupUserIndex(const std::string& userId) const;

    AskMessage *askMessages();
    
    void deleteRoom(RoomDeletionMessage *msg);

    std::vector<int> getVectorClock() { return vectorClock; }    

    AskMessage *getMissingMessages();

    std::vector<ChatMessage *> resendMessage(AskMessage *amsg);


    class QueueTooLongException : public std::exception {
        virtual const char* what() const throw() {
            return "Queue too long";
        }

        public:
            QueueTooLongException(AskMessage *missingMessages) : missingMessages(missingMessages) {}
            AskMessage *missingMessages;        
    };

    class DeleteMeException : public std::exception {
        virtual const char* what() const throw() {
            return "Delete me";
        }

        public:
            DeleteMeException(std::string roomId) : roomId(roomId) {}
            std::string getRoomId() const { return roomId; }
            std::string roomId;
    };


private:
    int numParticipants;
    int userIndex;
    bool scheduledForDeletion = false;
    std::string userId;
    std::string adminId;
    std::string roomId;
    std::vector<std::string> participants;
    std::vector<int> vectorClock;   
    std::vector<int> deletionVectorClock;
    std::map<std::vector<int>, ChatMessage> messages;
    std::map<std::vector<int>, ChatMessage> messagesQueue;
    std::vector<std::string> notAcked;
    std::vector<std::string> notDeleteAcked;
};

#endif // ROOM_HPP
