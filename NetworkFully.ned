// Network topology: Fully Connected
network NetworkFully
{
    parameters:
        int numClients = default(5); // Parameter to set the number of clients
        double stopEventTime = default(100000); // Parameter to set the time to stop tsending room creation and chat messages before the simulation ends

    submodules:
        behavior: NetworkFullyBehavior;
        client[numClients]: ClientFully;

    connections allowunconnected:
        for i = 0..numClients-1,
            for j = 0..numClients-1,
                if (i != j) {
                    client[i].out[j] --> client[j].in[i];
                }
}

simple NetworkFullyBehavior
{
    parameters:
        /// TODO: Hide
}
