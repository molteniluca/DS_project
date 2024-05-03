simple AccessPoint
{
    parameters:
        double timePartition = default(-1.0);  // Time partition for the access point
    gates:
        input inClients[];  // Gate for receiving messages from clients
        output outClients[];  // Gate for broadcasting messages to clients
        
        input inAccessPoints[];  // Gate for receiving messages from other access points
        output outAccessPoints[];  // Gate for broadcasting messages to other access points
}