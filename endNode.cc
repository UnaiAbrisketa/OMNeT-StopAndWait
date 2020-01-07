/*
 * endNode.cc
 *
 *  Created on: 4 dic. 2019
 *      Author: Adrian
 */
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "custom_packet_m.h"

using namespace omnetpp;

class EndNode : public cSimpleModule
{
    protected:
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(EndNode);

void EndNode::handleMessage(cMessage *msg)
{
    CustomPacket *pkt = (CustomPacket*) msg;
    cGate *arrivalGate = pkt -> getArrivalGate();
    int arrivalGateIndex = arrivalGate -> getIndex();
    EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";

    if (pkt -> getKind() == 1) { // 1: Packet
        if (pkt -> hasBitError()) {
            EV << "Packet arrived with error, send NAK\n";
            CustomPacket *nak = new CustomPacket("NAK");
            nak -> setKind(3);
            send(nak, "outPort", arrivalGateIndex);
        }
        else {
            EV << "Packet arrived without error, send ACK\n";
            CustomPacket *ack = new CustomPacket("ACK");
            ack -> setKind(2);
            send(ack, "outPort", arrivalGateIndex);
            EV << "Packet it's okay!";
        }
    }
}
