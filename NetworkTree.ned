// Network topology: Tree
network NetworkTree extends Network
{
    submodules:
        behavior: NetworkTreeBehavior;
        client[numClients]: ClientTree;
    
    connections allowunconnected:
        for i = 0..numClients-1,
        if (i > 0) {
            client[i].out_parent --> { delay = parent.linkDelay; } --> client[int((i - 1) / 2)].in_left if i % 2 == 1;
            client[i].out_parent --> { delay = parent.linkDelay; } --> client[int((i - 1) / 2)].in_right if i % 2 == 0;
            client[int((i - 1) / 2)].out_left --> { delay = parent.linkDelay; } --> client[i].in_parent if i % 2 == 1;
            client[int((i - 1) / 2)].out_right --> { delay = parent.linkDelay; } --> client[i].in_parent if i % 2 == 0;
        }

}

simple NetworkTreeBehavior {}
