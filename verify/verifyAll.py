import configparser
import os
import threading

def verifyFile(log_file_path):
    rooms = {}
    messagesSent = {}
    messagesDisplayed = {}
    messagesAtRoomDeletion = {}
    messagesCausality = {} # Keeps track of the messages that the sender has seen before sending the message
    
    # Check that if a message is displayed, it has already seen all the messages the sender has seen
    def testCausality(clientDisplaying, room, message):
        sender = message.split(",")[1].split(")")[0]
        if sender not in messagesDisplayed:
            return # Sender has not displayed any messages in this room
        if room not in messagesDisplayed[sender]:
            return # Sender has not displayed any messages in this room
        
        for message_saw_by_sender in messagesCausality[message]:
            sender_of_message_saw_by_sender = message_saw_by_sender.split(",")[1].split(")")[0]
            
            # If the sender is the same as the client displaying the message ignore
            if sender_of_message_saw_by_sender != clientDisplaying:
                if message_saw_by_sender not in messagesDisplayed[clientDisplaying][room]:
                    print("Message: " + message + " in room: " + str(room) + " was displayed to " + 
                          clientDisplaying + " before " + message_saw_by_sender + " was displayed to " 
                          + sender_of_message_saw_by_sender)

    def saveMessagesCausality(client, room, message):
        if client not in messagesDisplayed:
            messagesCausality[message] = []
            return
        if room not in messagesDisplayed[client]:
            messagesCausality[message] = []
            return
        messagesCausality[message] = messagesDisplayed[client][room].copy()

    def messageDisplayed(client, room, message):
        if client not in messagesDisplayed:
            messagesDisplayed[client] = {}
        if room not in messagesDisplayed[client]:
            messagesDisplayed[client][room] = []
        messagesDisplayed[client][room].append(message)
        
        testCausality(client, room, message)
        
    def messageSent(client, room, message):
        if room not in messagesSent:
            messagesSent[room] = {}
        if client not in messagesSent[room]:
            messagesSent[room][client] = []
        messagesSent[room][client].append(message)
        saveMessagesCausality(client, room, message)
        
    def roomCreated(roomName, client, clients):
        rooms[roomName]=(client,clients)
        
    def roomDeleted(roomName, admin):
        try:
            msg1 = messagesDisplayed[admin][roomName]
        except KeyError:
            msg1 = []
        try:
            msg2 = messagesSent[roomName][admin]
        except KeyError:
            msg2 = []
        
        messagesAtRoomDeletion[roomName] = msg1+msg2

    # Check that all messages sent are received
    def testAllMessagesReceived():
        for room in messagesSent:
            for send_client in messagesSent[room]:
                for message in messagesSent[room][send_client]:
                    for rec_client in rooms[room][1]:
                        # Check that the message was received by all clients in the room except the sender
                        if rec_client != send_client:
                            try:
                                # Check that the message was received by the client
                                if message not in messagesDisplayed[rec_client][room]:
                                    # Check that the room was not deleted before the message was sent
                                    if room in messagesAtRoomDeletion:
                                        if message in messagesAtRoomDeletion[room]:
                                            print("Message: " + message + " in room: " + str(room) + " was not received by " + rec_client)
                                    else:
                                        print("Message: " + message + " in room: " + str(room) + " was not received by " + rec_client + " but was not deleted")
                            
                            # rec_client has not displayed any messages in this room
                            except KeyError: 
                                # Check that the room was not deleted before the message was sent
                                if room in messagesAtRoomDeletion:
                                    if message in messagesAtRoomDeletion[room]:
                                        print("Message: " + message + " in room: " + str(room) + " was not received by " + rec_client)
                                else:
                                    print("Message: " + message + " in room: " + str(room) + " was not received by " + rec_client + " but was not deleted")

    # Messages dont get displayed if the client is not in the room
    def messagesOnlyToRecipients():
        for client in messagesDisplayed:
            for room in messagesDisplayed[client]:
                for message in messagesDisplayed[client][room]:
                    if client not in rooms[room][1]:
                        print("Message: " + message + " in room: " + str(room) + " was displayed to " +
                              client + " who is not in the room")
    

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
            # parse line client[4] - Deleting room: stanza(client[1], 0)
            if "Deleting room" in line:
                roomName = line.split("Deleting room: ")[1].strip()
                admin = line.split("-")[0].strip()
                roomDeleted(roomName, admin)

    testAllMessagesReceived()
    
    messagesOnlyToRecipients()
    

def runTest(config_name, r):
    outFile = os.path.join(logPath, config_name + "_" + str(r) + ".log")
    execFile = os.path.join(projectPath, "DS_project")
    print()
    print(f"Running configuration {config_name} with repetition {r}.")
    os.system(f'{execFile} -u Cmdenv -f omnetpp.ini -c {config_name} -r {r} > {outFile}')
    print(f"Checking log of configuration {config_name} with repetition {r}.")
    verifyFile(outFile)
    print()


dirPath = os.path.dirname(os.path.abspath(__file__))
projectPath = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
logPath = os.path.join(dirPath, "log")

if not os.path.exists(logPath):
    os.makedirs(logPath)


# Load the INI file
config = configparser.ConfigParser(allow_no_value=True)
config.read(os.path.join(projectPath, 'omnetpp.ini'))

# Initialize a dictionary to store configurations and their repeat values
config_repeats = {}

# Iterate through all sections in the INI file
for section in config.sections():
    repeat_value = config[section].getint('repeat', 1)  # Get the repeat value, default is 1 if not specified
    config_repeats[section] = repeat_value

# Print all configurations and their repeat values
for config_name, repeat in config_repeats.items():
    for r in range(repeat):
        t = threading.Thread(target=runTest, args=(config_name, r))
        t.start()
        