// Tic.cc
#include "Tic.h"

Define_Module(Tic);

void Tic::initialize()
{
    dataRate = par("dataRate").doubleValue();
    packetLossRate = par("packetLossRate").doubleValue();
    seqNumBits = par("seqNumBits");
    maxSeqNum = 1 << seqNumBits;

    windowSize = 0;
    sendBase = 0;
    nextSeqNum = 0;
    readyToSend = false;

    sendEvent = new cMessage("sendEvent");
    timeoutEvent = new cMessage("timeoutEvent");

    EV << "Sending window size query to Toc.\n";
    GbnPacket *query = new GbnPacket("query_request");
    query->setKind(0); // QUERY_REQ
    send(query, "port$o");
}

void Tic::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        if (msg == sendEvent) {
            if (nextSeqNum < (sendBase + windowSize) % maxSeqNum ||
                (sendBase + windowSize >= maxSeqNum &&
                (nextSeqNum >= sendBase || nextSeqNum < (sendBase + windowSize) % maxSeqNum)))
            {
                sendDataPacket();
            }
        } else if (msg == timeoutEvent) {
            EV << "TIMEOUT! Retransmitting from sequence number " << sendBase << "\n";
            retransmitPackets();
        }
    } else {
        GbnPacket *pkt = check_and_cast<GbnPacket*>(msg);
            if (pkt->getKind() == 1) { // QUERY_REPLY
            windowSize = pkt->getWindowSize();
            EV << "Received window size W=" << windowSize << " from Toc. Starting data transmission.\n";
            readyToSend = true;
            scheduleAt(simTime(), sendEvent);
        }
        else if (pkt->getKind() == 3) { // RR (ACK)
            EV << "Received ACK (RR) for packet " << pkt->getAckNumber() << "\n";
            unsigned int ackedNum = pkt->getAckNumber();

            while(sendBase != (ackedNum + 1) % maxSeqNum) {
                if (!unackedPackets.isEmpty()) {
                    delete (GbnPacket*)unackedPackets.pop();
                    sendBase = (sendBase + 1) % maxSeqNum;
                } else {
                    break;
                }
            }

            stopTimer();
            if (sendBase != nextSeqNum) {
                startTimer();
            }
            if (!sendEvent->isScheduled() && readyToSend) {
                 scheduleAt(simTime(), sendEvent);
            }
        }
        else if (pkt->getKind() == 4) { // RNR
            EV << "Received RNR from Toc. Pausing transmission.\n";
            readyToSend = false;
        }
        delete pkt;
    }
}

void Tic::sendDataPacket() {
    if (!readyToSend) return;

    GbnPacket *pkt = new GbnPacket("data");
    pkt->setKind(2);
    pkt->setSequenceNumber(nextSeqNum);
    pkt->setBitLength(1024);

    if (uniform(0, 1) < packetLossRate) {
        pkt->setHasError(true);
        EV << "Packet " << nextSeqNum << " is marked as lost/corrupted.\n";
    } else {
        pkt->setHasError(false);
        EV << "Sending packet with seq: " << nextSeqNum << "\n";
    }

    unackedPackets.insert(pkt->dup());
    send(pkt, "port$o");

    if (sendBase == nextSeqNum) {
        startTimer();
    }

    nextSeqNum = (nextSeqNum + 1) % maxSeqNum;

    simtime_t delay = pkt->getBitLength() / dataRate;
    scheduleAt(simTime() + delay, sendEvent);
}

void Tic::retransmitPackets() {
    stopTimer();
    cQueue tempQueue;
    while(!unackedPackets.isEmpty()){
        GbnPacket* pkt = (GbnPacket*)unackedPackets.pop();
        tempQueue.insert(pkt);
    }

    while(!tempQueue.isEmpty()){
        GbnPacket* pkt = (GbnPacket*)tempQueue.pop();
        unackedPackets.insert(pkt->dup());
        EV << "Retransmitting packet " << pkt->getSequenceNumber() << "\n";
        send(pkt->dup(), "port$o");
    }

    if (!unackedPackets.isEmpty()) {
        startTimer();
    }
}

void Tic::startTimer() {
    if (!timeoutEvent->isScheduled()) {
        scheduleAt(simTime() + 2.0, timeoutEvent);
    }
}

void Tic::stopTimer() {
    if (timeoutEvent->isScheduled()) {
        cancelEvent(timeoutEvent);
    }
}
