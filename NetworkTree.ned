// Network topology: Tree
network NetworkTree
{
    parameters:
        int numClients = default(5); // Parameter to set the number of clients
        double stopEventTime = default(100000); // Parameter to set the time to stop tsending room creation and chat messages before the simulation ends

    submodules:
        behavior: NetworkTreeBehavior;
        client[numClients]: ClientTree;
    
    connections allowunconnected:
        for i = 0..numClients-1,
        if (i > 0) {
            client[i].out_parent --> client[int((i - 1) / 2)].in_left if i % 2 == 1;
            client[i].out_parent --> client[int((i - 1) / 2)].in_right if i % 2 == 0;
            client[int((i - 1) / 2)].out_left --> client[i].in_parent if i % 2 == 1;
            client[int((i - 1) / 2)].out_right --> client[i].in_parent if i % 2 == 0;
        }

}

simple NetworkTreeBehavior
{
    parameters:
        /// TODO: Hide
}
