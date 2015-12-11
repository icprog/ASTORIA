
#include "scada-protocol.h"


namespace ns3 {

ScadaProtocol::ScadaProtocol(){}

std::string ScadaProtocol::createRequest(uint16_t functionCode, uint32_t param1){ return ""; }
std::string ScadaProtocol::createReply(std::string message, std::vector<float> registers, uint32_t nodeId){ return ""; }
std::vector<std::string> ScadaProtocol::createReply(std::string message, std::vector<float> registers, uint32_t nodeId, std::vector<std::pair<uint32_t, std::pair<std::string, uint16_t>>> extraData){ std::vector<std::string> returnVec; return returnVec; }

uint16_t ScadaProtocol::getPacketLength(){ return 0;}

std::vector<std::pair<uint32_t,uint16_t>> ScadaProtocol::getMessageData(std::string message) { 
	std::vector<std::pair<uint32_t,uint16_t>> retVector;
	retVector.push_back(std::make_pair(-1,-1)); 
	return retVector;
}




}
