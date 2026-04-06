/*
 * toc.h
 *
 *  Created on: Aug 24, 2025
 *      Author: siddhantaggarwal
 */

// Toc.h

#ifndef _GBN_TOC_H
#define _GBN_TOC_H

#include <omnetpp.h>
#include "GbnPacket_m.h"

using namespace omnetpp;

class Toc : public cSimpleModule
{
  private:
    // Parameters
    int windowSize;      // W
    int ackFrequency;    // N
    int bufferSize;      // Max buffer capacity

    // State variables
    unsigned int expectedSeqNum;
    int packetsReceivedSinceAck;
    unsigned int maxSeqNum;
    bool rnrState;       // true = currently not ready

    cQueue receiveBuffer; // Receiver buffer

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void sendAck();
    void sendRnr();      // NEW: send Receive Not Ready
};

#endif
