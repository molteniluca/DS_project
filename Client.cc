#include <map>
#include <vector>
#include <string>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

enum class ActionPerformed {
    CREATED_ROOM,
    RECIVED_CHAT_MESSAGE,
    DISCARDED_ALREADY_RECIVED_MESSAGE
};

class Client
{
    private:
        std::string userId;
        std::map<std::string, Room> rooms;
        void manageRoomCreation(RoomCreationMessage *msg);
        void manageChatMessage(ChatMessage *msg);
    public:
        Client(std::string userId) : userId(userId) {
            // Seed the random number generator
            std::srand(std::time(nullptr));
        }
        RoomCreationMessage createRoom(std::string room, std::vector<std::string> participants);
        SimpleMessage* getRandomMessage(std::string text);
        ActionPerformed manageMessage(Message *msg);
}

RoomCreationMessage Client::createRoom(std::string roomId, std::vector<std::string> participants)
{
    /// TODO:
    Room room(participants, userId, roomId);
    rooms[room] = room;

    return room.getMessageCreation();
}

SimpleMessage* Client::getRandomMessage(std::string text) {
    if (rooms.empty()) {
        return nullptr;
    }

    // Generate a random index
    int randomIndex = std::rand() % rooms.size();

    // Get the room at the random index
    auto it = rooms.begin();
    std::advance(it, randomIndex);
    Room& randomRoom = it->second;

    // Create a new message for the selected room
    randomRoom.getMessage(text);

    EV << this->getName() << " - Random message created for room: " << randomRoom.name << endl;

    return msg;
}

void Client::manageRoomCreation(RoomCreationMessage *msg)
{
    Room room(msg);
    rooms[room.getRoomId()] = room;
    return;
}

void Client::manageChatMessage(ChatMessage *msg)
{
    /// TODO: change to check if userId is in participants in case message is recived before room creation ???
    if(rooms.find(msg->getRoomId()) == rooms.end())
        return;
    rooms[msg->getRoomId()].processMessage(msg);
}


ActionPerformed Client::manageMessage(Message *msg)
{
    if(alreadyReceived(msg))
        return ActionPerformed::ALREADY_RECIVED_MESSAGE;
    if(msg->getType() == MessageType::CREATE_ROOM) {
        manageRoomCreation(dynamic_cast<RoomCreationMessage*>(msg));
        return ActionPerformed::CREATED_ROOM;
    }
    if(msg->getType() == MessageType::CHAT) {
        manageChatMessage(dynamic_cast<ChatMessage*>(msg));
        return ActionPerformed::RECIVED_CHAT_MESSAGE;
    }
}
