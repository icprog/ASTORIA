#include "simulation-control.h"

#ifndef NS3_SIMULATION_H
#define NS3_SIMULATION_H

#include "topology.h"
#include "scada-application.h"
#include "MTU.h"
#include "RTU.h"
#include "Sensor.h"
#include "MSIAttacker.h"
#include "DOSAttacker.h"

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <pthread.h>
#include <mutex>

namespace ns3 {

class Ns3Simulation {

	public:
		Ns3Simulation();
		void setSimInfo(uint32_t nNodesPerSubServer, uint32_t simEnd, uint32_t step, std::string protocolName);
		void createSimulation();
		void scheduleServerGet(uint32_t id, uint32_t time);
		void addNodeData(std::string id, float data);
		float simulationStep();
		std::vector<node_ns3*> setTopology(std::string);
		std::vector<TransmissionData*> retrieveRTUData();
		std::vector<TransmissionData*> retrieveSensorData();
		std::vector<TransmissionData*> retrieveMTUData();
		bool finished;

		virtual ~Ns3Simulation();

	private:
	
		static void* SimReceiver(void *threadarg);

		std::vector<ScadaApplication*> 	serverList;
		std::vector<ScadaApplication*>	stationList;
		std::vector<ScadaApplication*>	sensorList;
		uint32_t stepSize;

		uint32_t nodesPerSubServer;
		uint32_t simEndTime;
		std::string protocol;
		pthread_t recvThread;

		std::vector<node_ns3*> topologyNodes;

		Topology* ns3Topology;

		std::vector<std::string> getElementList();

		std::fstream simInfoFile;


};


}

#endif
