log_file_path = "test.log"

with open(log_file_path, "r") as log_file:
    for line in log_file:
        line = line.strip()
        # parse line this client[7] - stanza(client[7], 1) - Displayed message: KipY38k0Eb from user client[8]
        if "Displayed message" in line:
            message = line.split("Displayed message: ")[1].split(" from user")[0]
            client = line.split("-")[0]
            room = line.split("stanza(client[")[1].split("]")[0]
            print(f"Client {client} in room {room} displayed message: {message}")
        # parse line client[4] - Sending message to room stanza(client[4], 13) - msg(534,client[4])
        if "Sending message to room" in line:
            message = line.split("-")[2].strip()
            client = line.split("-")[0].strip()
            room = line.split("stanza(client[")[1].split("]")[0]
            print(f"Client {client} sent message to room {room}: {message}")
        # parse line client[4] - Sent room creation: stanza(client[4], 0) with: client[4],client[3],client[0],client[5],client[2],client[8]
        if "Sent room creation" in line:
            clients = line.split("with: ")[1].split(",")
            client = line.split("client[")[1].split("]")[0]
            print(f"Client {client} created room with clients: {clients}")