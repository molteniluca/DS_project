class Client
{
    private:
        std::string name;
        std::map<std::string, Room> rooms;
    public:
        createRoom(std::string room, std::vector<std::string> participants);
        manageRoomCreation(RoomCreationMessage *msg);
        manageMessage(SimpleMessage *msg);
}

Client::createRoom(std::string room, std::string type, std::vector<std::string> participants)
{
    Room room(room, participants);
    rooms[room] = room;

    return room.getMessageCreation();
}

Client::manageMessage(SimpleMessage *msg)
{
    if(rooms[msg->room].checkReceived(msg->vc)) {
        EV << this->getName() << " - Message already received: " << msg->getName() << endl;
        delete msg;
        return;
    }
    rooms[msg->room].addReceived(msg->vc);
    EV << this->getName() << " - Message received: " << msg->getName() << endl;
    delete msg;
}

Client::manageRoomCreation(RoomCreationMessage *msg)
{
    Room room(msg->room, msg->type, msg->participants);
    rooms[room.name] = room;

    SimpleMessage *new_msg = room.getMessageCreation();
    send(new_msg, "out_left");
    send(new_msg->dup(), "out_right");

    EV << this->getName() << " - Sending message for room " << room << " creation" << endl;
}

