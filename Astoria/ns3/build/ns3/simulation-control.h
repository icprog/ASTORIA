#ifndef SIMULATION_CONTROL_H
#define SIMULATION_CONTROL_H

#include "ns3-simulation.h"
#include "mosaik-call.h"


#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <iostream>
#include <ctime>


using namespace std;

namespace ns3{

const string SIM_META = "{\"api_version\": 2.0,\"models\": {\"Network\": {\"public\": true,\"params\": [\"topology_file\"],\"attrs\": []}, \"MTU\": {\"public\": false, \"params\": [], \"attrs\": [\"P_list\"]}, \"RTU\": {\"public\": false, \"params\": [], \"attrs\": [\"P\"]}}}";

class SimulationControl {

	public:
		SimulationControl(Ns3Simulation *sim);
		void addSimulation();
		int createSocket();
		void run();

	private:

		void simulationInit();
		void simulationCreate();
		void simulationStep();
		void simulationStop();

		string getParamValue(string paramName,vector<pair<string,string> > initParameters);
		void generateOutputFiles(std::vector<TransmissionData*> outputData, std::string fileType);
		TransmissionData* getReferenceData(std::string nodeName);

		MosaikCall *currentMessage;
		Ns3Simulation *simulation;
		int mosaikSocket;
		std::string reply;
		
		std::vector<TransmissionData*> referenceData;
		
		uint32_t stepSize;
		std::chrono::time_point<std::chrono::system_clock> beginTime;
		 std::chrono::time_point<std::chrono::system_clock> endTime;
		
};

}

#endif
