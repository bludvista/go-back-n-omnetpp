cplusplus {{
#include <omnetpp.h>
}}

class GbnPacket extends cPacket
{
	int kind;
	int sequenceNumber;
	int ackNumber;
	int windowSize;
	bool hasError;
}