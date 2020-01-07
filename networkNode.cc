/*
 * networkNode.cc
 *
 *  Created on: 4 dic. 2019
 *      Author: Adrian
 */
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "custom_packet_m.h"

using namespace omnetpp;

class NetworkNode : public cSimpleModule
{
    private:
        cChannel *channel[2]; // one channel for each output
        cQueue *queue[2];  // one queue for each channel
        double probability;  // from 0 to 1
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void sendNew(CustomPacket *pkt);
        virtual void sendNext(int gateIndex);
        virtual void sendPacket(CustomPacket *pkt, int gateIndex);
};

Define_Module(NetworkNode);

void NetworkNode::initialize() {
    // Get cChannel pointers from gates
    channel[0] = gate("outPort", 0) -> getTransmissionChannel();
    channel[1] = gate("outPort", 1) -> getTransmissionChannel();

    // Create one queue for each channel
    queue[0] = new cQueue("queueZero");
    queue[1] = new cQueue("queueOne");

    // Initialize random number generator
    srand(time(NULL));

    // Get probability parameter
    probability = (double) par("probability");
}

void NetworkNode::handleMessage(cMessage *msg)
{
    CustomPacket *pkt = check_and_cast<CustomPacket *> (msg);
    cGate *arrivalGate = pkt -> getArrivalGate();
    int arrivalGateIndex = arrivalGate -> getIndex();
    EV << "Packet arrived from gate " + std::to_string(arrivalGateIndex) + "\n";

    if (pkt -> getFromSource()) {
        // Packet from source
        EV << "Forward packet from source\n";
        pkt -> setFromSource(false);
        sendNew(pkt);
        return;
    }
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
            sendNew(pkt);
        }
    }
    else if (pkt -> getKind() == 2) { // 2: ACK
        EV << "ACK from next node\n";
        if (queue[arrivalGateIndex] -> isEmpty())
            EV << "WARNING: there are not packets in queue, but ACK arrived\n";
        else {
            // pop() removes queue's first packet
            queue[arrivalGateIndex] -> pop();
            sendNext(arrivalGateIndex);
        }
    }
    else { // 3: NAK
        EV << "NAK from next node\n";
        sendNext(arrivalGateIndex);
    }
}

void NetworkNode::sendNew(CustomPacket *pkt) {
    int gateIndex;
    double randomNumber = ((double) rand() / (RAND_MAX));
    if (randomNumber < probability)
        gateIndex = 0;
    else
        gateIndex = 1;

    if (queue[gateIndex] -> isEmpty()) {
        EV << "Queue is empty, send packet and wait";
        // Insert in queue (it may have to be sent again)
        queue[gateIndex] -> insert(pkt);
        sendPacket(pkt, gateIndex);
    } else {
        EV << "Queue is not empty, add to back and wait";
        queue[gateIndex] -> insert(pkt);
    }
}

void NetworkNode::sendNext(int gateIndex) {
    if (queue[gateIndex] -> isEmpty())
        EV << "No more packets in queue\n";
    else {
        // front() gets the packet without removing it from queue
        CustomPacket *pkt = check_and_cast<CustomPacket *> (queue[gateIndex] -> front());
        sendPacket(pkt, gateIndex);
    }
}

void NetworkNode::sendPacket(CustomPacket *pkt, int gateIndex) {
    if (channel[gateIndex] -> isBusy()) {
        EV << "WARNING: channel is busy, check that everything is working fine\n";
    } else {
        // OMNeT++ can't send a packet while it is queued, must send a copy
        CustomPacket *newPkt = check_and_cast<CustomPacket *> (pkt -> dup());
        send(newPkt, "outPort", gateIndex);
    }
}
