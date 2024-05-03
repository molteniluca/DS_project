network Network
{
    @display("bgb=800,800");
    submodules:
        accessPoint[2]: AccessPoint {
            gates:
                inClients[2];
                outClients[2];
                outAccessPoints[1];
                inAccessPoints[1];
        }

        client[4]: Client {
            gates:
                in;
                out;
        }

    connections allowunconnected:
        accessPoint[0].outClients[0] --> client[0].in;
        accessPoint[0].outClients[1] --> client[1].in;

        accessPoint[1].outClients[0] --> client[2].in;
        accessPoint[1].outClients[1] --> client[3].in;

        client[0].out --> accessPoint[0].inClients[0];
        client[1].out --> accessPoint[0].inClients[1];

        client[2].out --> accessPoint[1].inClients[0];
        client[3].out --> accessPoint[1].inClients[1];

        accessPoint[0].outAccessPoints[0] --> accessPoint[1].inAccessPoints[0];
        accessPoint[1].outAccessPoints[0] --> accessPoint[0].inAccessPoints[0];
}