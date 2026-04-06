// Toc.cc
#include "Toc.h"
#include "GbnPacket_m.h"

Define_Module(Toc);

void Toc::initialize()
{
    windowSize = par("windowSize");
    ackFrequency = par("ackFrequency");
    expectedSeqNum = 0;
    packetsReceivedSinceAck = 0;
    maxSeqNum = 1 << getParentModule()->getSubmodule("tic")->par("seqNumBits").intValue();
    rnrState = false; // initially ready
}

void Toc::handleMessage(cMessage *msg)
{
    GbnPacket *pkt = check_and_cast<GbnPacket*>(msg); // @suppress("Function cannot be instantiated")
    int kind = pkt->getKind();

    if (kind == 0) { // QUERY_REQ
        EV << "Received window size query. Replying with W=" << windowSize << "\n";
        GbnPacket *reply = new GbnPacket("query_reply");
        reply->setKind(1);
        reply->setWindowSize(windowSize);
        send(reply, "port$o");
    }
    else if (kind == 2) { // DATA
        if (pkt->getHasError()) {
            EV << "Received a corrupted packet. Discarding.\n";
            delete pkt;
            return;
        }

        // Check buffer capacity
        if (receiveBuffer.getLength() >= par("bufferSize").intValue()) {
            EV << "Buffer full. Sending RNR.\n";
            sendRnr();
            delete pkt;
            return;
        }

        // Normal packet handling
        if (pkt->getSequenceNumber() == expectedSeqNum) {
            EV << "Received expected packet with seq: " << expectedSeqNum << "\n";
            receiveBuffer.insert(pkt->dup());
            expectedSeqNum = (expectedSeqNum + 1) % maxSeqNum;
            packetsReceivedSinceAck++;

            if (packetsReceivedSinceAck >= ackFrequency) {
                sendAck();
            }
        } else {
            EV << "Received out-of-order packet. Expected: " << expectedSeqNum
               << ", got: " << pkt->getSequenceNumber() << ". Discarding.\n";
            sendAck();
        }
    }
    delete pkt;
}

void Toc::sendAck() {
    if (expectedSeqNum == 0 && receiveBuffer.isEmpty()) return;

    GbnPacket *ack = new GbnPacket("ack");
    ack->setKind(3); // RR = Receive Ready
    ack->setAckNumber((expectedSeqNum + maxSeqNum - 1) % maxSeqNum);
    EV << "Sending RR (ACK) for packet " << ack->getAckNumber() << "\n";
    send(ack, "port$o");
    packetsReceivedSinceAck = 0;

    // Resume from RNR state if it was active
    if (rnrState) {
        EV << "Buffer has space again. Switching from RNR to RR.\n";
        rnrState = false;
    }
}

void Toc::sendRnr() {
    GbnPacket *rnr = new GbnPacket("rnr");
    rnr->setKind(4); // 4 = RNR
    EV << "Sending RNR (Receive Not Ready).\n";
    send(rnr, "port$o");
    rnrState = true;
}
