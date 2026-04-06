# 📡 Go-Back-N Protocol Simulation – OMNeT++

Simulation of the **Go-Back-N (GBN) sliding window protocol** for reliable data transfer, 
implemented in C++ using the OMNeT++ discrete event simulation framework.

## 🔍 Features
- Sliding window with configurable window size (W)
- Sequence number bits configurable (2^n sequence space)
- Two simulation scenarios:
  - **NoLoss** – clean channel, 0% packet loss
  - **WithLoss** – 20% packet loss with retransmission
- Receiver Not Ready (RNR) flow control
- Cumulative ACK with configurable ACK frequency

## 🛠️ Tech Stack
- C++ (OMNeT++ framework)
- OMNeT++ 6.x
- NED network description language

## 🚀 How to Run
1. Install [OMNeT++](https://omnetpp.org/)
2. Import project into OMNeT++ IDE
3. Build the project
4. Run either configuration:
   - `NoLoss` – simulation without packet loss
   - `WithLoss` – simulation with 20% packet loss

## ⚠️ Note
Completed as part of a Computer Networks course at SRH University of Heidelberg.
Shared for portfolio purposes only.
