
#ifndef TRANSMISSION_DATA_H
#define TRANSMISSION_DATA_H

#include <vector>
#include <unistd.h> 
#include <string>
#include <fstream>


namespace ns3{

class TransmissionData{

	public:
		TransmissionData(uint32_t transmissionStep,std::string name,uint32_t id);
		void addTransmissionData(float transmissionTime, uint16_t transmissionData);
		std::pair<float,uint16_t> getTransmissionData(uint32_t index);
		uint32_t getTransmissionDataSize();
		uint32_t getTransmissionStep();
		uint32_t getNodeId();
				
		std::string nodeName;
		
	private:
		
		uint32_t nodeId;
		std::vector<std::pair<float,uint16_t> > transmissionData;
		uint32_t m_transmissionStep;

		
};
	
}

#endif
