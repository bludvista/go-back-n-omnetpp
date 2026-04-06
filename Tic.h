// Tic.h

#ifndef _GBN_TIC_H
#define _GBN_TIC_H

#include <omnetpp.h>
#include "GbnPacket_m.h"

using namespace omnetpp;

class Tic : public cSimpleModule
{
  private:
    // Parameters
    double dataRate;
    double packetLossRate;
    int seqNumBits;

    // State variables
    int windowSize;          // W (learned from Toc)
    unsigned int maxSeqNum;  // 2^seqNumBits
    unsigned int sendBase;
    unsigned int nextSeqNum;
    bool readyToSend;        // Flag: only true when Toc ready (RR)

    // Timers and queues
    cMessage *sendEvent;
    cMessage *timeoutEvent;
    cQueue unackedPackets;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void startTimer();
    void stopTimer();
    void sendDataPacket();
    void retransmitPackets();
};

#endif
