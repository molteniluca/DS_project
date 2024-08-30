// Netword topology: Ring
network NetworkRing
{
    parameters:
        int numClients = default(5); // Parameter to set the number of clients

    @display("bgb=800,800");
    submodules:
        client[numClients]: ClientRing;
    
    connections allowunconnected:
        for i = 0..numClients-1 {
            client[i].out_right --> client[(i+1)%numClients].in_left;
            client[i].out_left --> client[(i-1+numClients)%numClients].in_right;
        }
}
