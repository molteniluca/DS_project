// Network topology: Fully Connected
network NetworkFully extends Network
{
    submodules:
        behavior: NetworkFullyBehavior;
        client[numClients]: ClientFully;

    connections allowunconnected:
        for i = 0..numClients-1,
            for j = 0..numClients-1,
                if (i != j) {
                    client[i].out[j] --> { delay = parent.linkDelay; datarate = parent.linkDatarate; ber = parent.linkBER; } --> client[j].in[i];
                }
}

simple NetworkFullyBehavior {}
