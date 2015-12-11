
#include "transmission-data.h"

namespace ns3 {

	TransmissionData::TransmissionData(uint32_t transmissionStep, std::string name,uint32_t id){
		m_transmissionStep = transmissionStep;
		nodeName = name;
		nodeId = id;
	}
	
	void TransmissionData::addTransmissionData(float transmissionTime, uint16_t data){
		transmissionData.push_back(std::make_pair(transmissionTime,data));	
	}
	
	std::pair<float, uint16_t> TransmissionData::getTransmissionData(uint32_t index) {

		if(index < transmissionData.size()-1)
			return transmissionData[index];
		else		
			return std::make_pair(-1,-1);

	}
	
	uint32_t TransmissionData::getTransmissionDataSize(){ return transmissionData.size(); }
	uint32_t TransmissionData::getTransmissionStep(){ return m_transmissionStep; }
	uint32_t TransmissionData::getNodeId(){ return nodeId; }
	
	
}
