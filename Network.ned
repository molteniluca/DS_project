network Network
{
    parameters:
        int numClients = default(5);
        double linkDelay @unit(s) = default(10ms);
        double linkDatarate @unit(bps) = default(10Mbps);

        double stopEventTime @unit(s) = default(2d); // Parameter to set the time to stop tsending room creation and chat messages before the simulation ends

        double createRoomMinTime @unit(s) = default(10min);
        double createRoomMaxTime @unit(s) = default(2h);
        double createRoomProbability = default(0.2);

        double sendMessageMinTime @unit(s) = default(10s);
        double sendMessageMaxTime @unit(s) = default(30min);
        double sendMessageProbability = default(0.5);

        double resendCreationTime @unit(s) = default(1min);

        double askMessagesTime @unit(s) = default(10min);

        double deleteRoomMinTime @unit(s) = default(1h);
        double deleteRoomMaxTime @unit(s) = default(12h);
        double deleteRoomProbability = default(0.2);

        double partitionMinTime @unit(s) = default(5min);
        double partitionMaxTime @unit(s) = default(1h);

        double endPartitionMinTime @unit(s) = default(5min);
        double endPartitionMaxTime @unit(s) = default(1h);
}
