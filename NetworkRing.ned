// Network topology: Ring
network NetworkRing extends Network
{
    submodules:
        behavior: NetworkRingBehavior;
        client[numClients]: ClientRing;
    
    connections allowunconnected:
        for i = 0..numClients-1 {
            client[i].out_right --> { delay = parent.linkDelay; datarate = parent.linkDatarate; ber = parent.linkBER; } --> client[(i+1)%numClients].in_left;
            client[i].out_left --> { delay = parent.linkDelay; datarate = parent.linkDatarate; ber = parent.linkBER; } --> client[(i-1+numClients)%numClients].in_right;
        }
}

simple NetworkRingBehavior {}