// Network topology: Fully Connected
network NetworkFully
{
    parameters:
        int numClients = default(5); // Parameter to set the number of clients

    @display("bgb=800,800");
    submodules:
        client[numClients]: ClientFully;

    connections allowunconnected:
        for i = 0..numClients-1,
            for j = 0..numClients-1,
                if (i != j) {
                    client[i].out[j] --> client[j].in[i];
                }
}
