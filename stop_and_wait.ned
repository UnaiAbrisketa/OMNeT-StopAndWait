simple NetworkNode
{
    parameters:
        @display("i=block/departure");
        double probability;
    gates:
        input inPort[];
        output outPort[];
}

simple EndNode
{
    parameters:
        @display("i=block/arrival");
    gates:
        input inPort[];
        output outPort[];
}

simple Source
{
    parameters:
        @display("i=block/source");
        double meanPacketLength;
    gates:
        output outPort;
}

network StopAndWait
{
    @display("bgb=285.6,182.28");
    types:
        channel Channel extends ned.DatarateChannel
        {
            datarate = 1000bps;
            delay = 100us;
            per = 0.25;
        }
    submodules:
        node1: NetworkNode {
            @display("p=102.48,78.96");
            probability = 1/4;
            gates:
                inPort[3];
                outPort[2];
        }
        node2: NetworkNode {
            @display("p=166.32,33.6");
            probability = 1/3;
            gates:
                inPort[5];
                outPort[4];
        }
        node3: EndNode {
            @display("p=256.2,33.6");
            gates:
                inPort[1];
                outPort[1];
        }
        node4: EndNode {
            @display("p=256.2,127.68");
            gates:
                inPort[2];
                outPort[2];
        }
        node5: NetworkNode {
            @display("p=166.32,127.68");
            probability = 1/2;
            gates:
                inPort[4];
                outPort[3];
        }
        source1: Source {
            @display("p=29.4,78.96");
            meanPacketLength = 1000/3;
        }
        source2: Source {
            @display("p=29.4,33.6");
            meanPacketLength = 1000/3;
        }
        source3: Source {
            @display("p=29.4,127.68");
            meanPacketLength = 1000/3;
        }
    connections:
        node1.outPort[0] --> Channel --> node2.inPort[2];
        node1.inPort[0] <-- Channel <-- node2.outPort[2];

        node1.outPort[1] --> Channel --> node5.inPort[2];
        node1.inPort[1] <-- Channel <-- node5.outPort[2];

        node2.outPort[0] --> Channel --> node3.inPort[0];
        node2.inPort[0] <-- Channel <-- node3.outPort[0];

        node2.outPort[1] --> Channel --> node4.inPort[0];
        node2.inPort[1] <-- Channel <-- node4.outPort[0];

        node5.outPort[0] --> Channel --> node2.inPort[3];
        node5.inPort[0] <-- Channel <-- node2.outPort[3];

        node5.outPort[1] --> Channel --> node4.inPort[1];
        node5.inPort[1] <-- Channel <-- node4.outPort[1];

        source1.outPort --> node1.inPort[2];
        source2.outPort --> node2.inPort[4];
        source3.outPort --> node5.inPort[3];
}
