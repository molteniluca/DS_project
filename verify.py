log_file_path = "test.log"

rooms = {}
messagesSent = {}
messagesReceived = {}



messagesCausality = {}
# Test 3: Check that if a message is displayed, it has already seen all the messages the sender has seen
def test3(client, room, message):
    sender = message.split(",")[1].split(")")[0]
    if sender not in messagesReceived:
        return
    if room not in messagesReceived[sender]:
        return
    for message_saw_by_sender in messagesCausality[message]:
        sender_of_message_saw_by_sender = message_saw_by_sender.split(",")[1].split(")")[0]
        if message_saw_by_sender not in messagesReceived[client][room] and sender_of_message_saw_by_sender != client:
            print("Message: " + message + " in room: " + str(room) + " was displayed to " + client + " before " + message_saw_by_sender + " was displayed to " + sender_of_message_saw_by_sender)


def test3save(client, room, message):
    if client not in messagesReceived:
        messagesCausality[message] = []
        return
    if room not in messagesReceived[client]:
        messagesCausality[message] = []
        return
    messagesCausality[message] = messagesReceived[client][room].copy()


def messageDisplayed(client, room, message):
    if client not in messagesReceived:
        messagesReceived[client] = {}
    if room not in messagesReceived[client]:
        messagesReceived[client][room] = []
    messagesReceived[client][room].append(message)
    
    test3(client, room, message)
    
def messageSent(client, room, message):
    if room not in messagesSent:
        messagesSent[room] = {}
    if client not in messagesSent[room]:
        messagesSent[room][client] = []
    messagesSent[room][client].append(message)
    test3save(client, room, message)
    
def roomCreated(roomName, client, clients):
    rooms[roomName]=(client,clients)


with open(log_file_path, "r") as log_file:
    for line in log_file:
        line = line.strip()
        # parse line this client[7] - stanza(client[7], 1) - Displayed message: KipY38k0Eb from user client[8]
        if "Displayed message" in line:
            message = line.split("Displayed message: ")[1].split(" from user")[0]
            client = line.split("-")[0].strip()
            room = line.split("-")[1].strip()
            messageDisplayed(client, room, message)
        # parse line client[4] - Sending message to room stanza(client[4], 13) - msg(534,client[4])
        if "Sending message to room" in line:
            message = line.split("-")[2].strip()
            client = line.split("-")[0].strip()
            room = line.split("Sending message to room ")[1].split(" - ")[0]
            messageSent(client, room, message)
        # parse line client[4] - Sent room creation: stanza(client[4], 0) with: client[4],client[3],client[0],client[5],client[2],client[8]
        if "Sent room creation" in line:
            roomName = line.split("Sent room creation: ")[1].split(" with:")[0]
            clients = line.split("with: ")[1].split(",")
            client = line.split("-")[0].strip()
            roomCreated(roomName, client, clients)


## Test 1: Check that all messages sent are received
for room in messagesSent:
    for client in messagesSent[room]:
        for message in messagesSent[room][client]:
            for rec_client in rooms[room][1]:
                try:
                    if message not in messagesReceived[rec_client][room] and rec_client != client:
                        print("Message: " + message + " in room: " + str(room) + " was not received by " + rec_client)
                except KeyError:
                    print("Message: " + message + " in room: " + str(room) + " was not received by " + rec_client)

# Test 2: Messages dont get displayed if the client is not in the room
for client in messagesReceived:
    for room in messagesReceived[client]:
        for message in messagesReceived[client][room]:
            if client not in rooms[room][1]:
                print("Message: " + message + " in room: " + str(room) + " was displayed to " + client + " who is not in the room")

