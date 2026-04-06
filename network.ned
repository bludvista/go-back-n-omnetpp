// network.ned

simple Tic
{
    parameters:
        double dataRate @unit(bps);      // <-- FIX: Add this line
        double packetLossRate;           // <-- FIX: Add this line
        int seqNumBits;                  // <-- FIX: Add this line
        @display("i=block/routing");
    gates:
        inout port;
}
// In your .ned file (e.g., bnetwork.ned)

simple Toc
{
    parameters:
        int windowSize;
        int ackFrequency;
        int bufferSize;       // NEW
        @display("i=block/routing");
    gates:
        inout port;
}


// Define the network
network BNetwork // Assuming your network is named BNetwork
{
    // 1. CREATE THE INSTANCES from the blueprints
    submodules:
        tic: Tic {
            @display("p=100,200");
        }
        toc: Toc { // <-- Instance of Toc is created here, named 'toc'
            @display("p=400,200");
        }

    // 2. CONNECT THE INSTANCES using their lowercase names
    connections:
        tic.port <--> { delay = 100ms; } <--> toc.port; // <-- Use 'toc', not 'Toc'
}