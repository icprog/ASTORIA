#include "ns3-simulation.h"

namespace ns3 {

#define MTU_TYPE "MTU"
#define NODE_TYPE "node"
#define RTU_TYPE "RTU"
#define PV_TYPE "PV"
#define SENSOR_TYPE "SENSOR"
#define TR_TYPE "tr"



void* Ns3Simulation::SimReceiver(void *threadarg) {

	Ns3Simulation *simulation;
	simulation = (Ns3Simulation *) threadarg;

	SimulationControl *simControl = new SimulationControl(simulation);
	simControl->run();

}


Ns3Simulation::Ns3Simulation() {

	finished = false;
	simInfoFile.open("simulation.info", std::fstream::out);
	pthread_create(&recvThread, NULL, SimReceiver,(void*) this);

}

Ns3Simulation::~Ns3Simulation() {

	simInfoFile.close();

}


void Ns3Simulation::setSimInfo(uint32_t nNodesPerSubServer, uint32_t simEnd, uint32_t step, std::string protocolName){

	nodesPerSubServer = nNodesPerSubServer; //por enquanto ainda não utilizado
	simEndTime = simEnd;
	protocol = protocolName;
	stepSize = step;

}

void Ns3Simulation::scheduleServerGet(uint32_t id,uint32_t time){ //Por enquanto tempo é fixo e só um server

	serverList[id]->ScheduleNextTx();

}

void Ns3Simulation::addNodeData (string name, float data) {

	uint32_t nodeId = ns3Topology->getNodeId(name);
	std::string nodeType = ns3Topology->getNodeTypeById(nodeId);

	if(nodeType == RTU_TYPE) {

		stationList[nodeId]->AddData(data);

	} else {

		sensorList[nodeId]->AddData(data);
	
	}
	
}

std::vector<TransmissionData*> Ns3Simulation::retrieveRTUData(){
	
	std::vector<TransmissionData*> rtuData;

	for(node_ns3* node : ns3Topology->getNodeList()) {

		
		if(node->type == RTU_TYPE){
			
			TransmissionData* tData = stationList[node->nodeId]->getTransmissionData();
			rtuData.push_back(tData);
			
			std::vector<TransmissionData*> sensorData = stationList[node->nodeId]->getSensorData();

			//std::cout << "TAMANHO SENSORDATA " << sensorData[0]->nodeId << " " << sensorData.size() << std::endl;

			if(sensorData.size() != 0)
			for(TransmissionData* sensor : sensorData) {
				//std::cout << "tamanho TransmissionData vector (" << sensor->nodeId << ") : " <<
				//	sensor->getTransmissionDataSize() << std::endl; 
				rtuData.push_back(sensor);
			}

		}
	}

	return rtuData;
}


std::vector<TransmissionData*> Ns3Simulation::retrieveSensorData() {
	
	std::vector<TransmissionData*> sensorData;

	for(node_ns3* node : ns3Topology->getNodeList()){
		if(node->type == SENSOR_TYPE){
			TransmissionData* tData = sensorList[node->nodeId]->getTransmissionData();
			sensorData.push_back(tData);
		}
	}
	
	return sensorData;

}


std::vector<TransmissionData*> Ns3Simulation::retrieveMTUData()
{
	std::vector<TransmissionData*> mtuData;
	std::vector<TransmissionData*> stations;
	std::vector<TransmissionData*> sensors;

	stations = serverList.at(0)->getStationData();
	sensors = serverList.at(0)->getSensorData();

	for(int i = 0; i < stations.size(); i++)
		mtuData.push_back(stations[i]);

	std::cout << "station size: " << stations.size() << std::endl;
	std::cout << "sensors size: " << sensors.size() << std::endl;

	for(int i = 0; i < sensors.size(); i++)
		mtuData.push_back(sensors[i]);

	return mtuData;

}


float Ns3Simulation::simulationStep(){

	Simulator::Schedule(Seconds(stepSize),&Simulator::Stop);
	Simulator::Run();

	return Simulator::Now().GetSeconds();

}


std::vector<node_ns3*> Ns3Simulation::setTopology(string topoFile){
	ns3Topology = new Topology();
	ns3Topology->generateTopology(topoFile);
	return ns3Topology->getNodeList();
}


void Ns3Simulation::createSimulation(){

	NodeContainer ns3RTUNodes;
	NodeContainer ns3MTUNodes;
	
	std::vector<std::string> exceptTypes;
	exceptTypes.push_back(TR_TYPE);
	exceptTypes.push_back(PV_TYPE);
	exceptTypes.push_back(MTU_TYPE);
	std::vector<node_ns3*> simulationNodes = ns3Topology->getNodeList();

	exceptTypes.pop_back();
	exceptTypes.push_back(NODE_TYPE);
	exceptTypes.push_back(SENSOR_TYPE);
	exceptTypes.push_back(RTU_TYPE);
	std::vector<node_ns3*> mtuNode = ns3Topology->getNodeList2(exceptTypes);

	std::vector<edge_ns3*> simulationEdges = ns3Topology->getEdgeVectorOne();
	std::vector<edge_ns3*> simulationEdgesDouble = ns3Topology->getEdgeList();

	//ns3RTUNodes.Create(simulationNodes.size()-1);
	ns3RTUNodes.Create(simulationNodes.size()-1);
	ns3MTUNodes.Create(1);

	NodeContainer allNodes = NodeContainer(ns3MTUNodes,ns3RTUNodes);

	InternetStackHelper internet;
	internet.Install(allNodes);

	/***** BEGIN TEST PRINTS ***/
	if(mtuNode.size() != 1) {
		std::cout << "The Smart Grid system needs to have one Master Terminal Unit." << std::endl;
		return;
	}

	if(simulationNodes[0]->type != MTU_TYPE)
		std::cout << "MTU INDEX NEED TO BE ZERO!" << std::cout;

	int houses = 0;
	int rtus = 0;

	for(int i = 0; i < simulationNodes.size(); i++) {
		if(simulationNodes[i]->type == SENSOR_TYPE)
			houses++;
		if(simulationNodes[i]->type == RTU_TYPE)
			rtus++;
	}

	simInfoFile << "Houses: " << houses << std::endl;	
	simInfoFile << "RTUs: " << rtus << std::endl;
	simInfoFile << "MTUs: " << ns3MTUNodes.GetN() << std::endl;
	simInfoFile << "simulationNodes:" << std::endl;
	for(int i = 0; i < simulationNodes.size(); i++) {
		simInfoFile << "src (" << simulationNodes[i]->nodeId << " " << 
			ns3Topology->getNodeNameById(simulationNodes[i]->nodeId) << " " <<
				simulationNodes[i]->type << ")" << std::endl;
	}
/*
	std::cout << "mtuNode:" << std::endl;
	for(int i = 0; i < mtuNode.size(); i++) {
		std::cout << "src (" << mtuNode[i]->nodeId << " " << 
			ns3Topology->getNodeNameById(mtuNode[i]->nodeId) << " " <<
				mtuNode[i]->type << ")" << std::endl;
	}
	*/
	simInfoFile << std::endl << std::endl << "simulationEdges:" << std::endl;
	for(int i = 0; i < simulationEdges.size(); i++) {
		simInfoFile << "(" << i << ") src (" << simulationEdges[i]->srcId << " " << ns3Topology->getNodeNameById(simulationEdges[i]->srcId) << " " << ns3Topology->getNodeTypeById(simulationEdges[i]->srcId) 
			<< ") target: (" << simulationEdges[i]->targetId << " " << ns3Topology->getNodeNameById(simulationEdges[i]->targetId) << " " << ns3Topology->getNodeTypeById(simulationEdges[i]->targetId) << ")" << std::endl;
	}


	/***** END TEST PRINTS ***/

	//Caso P2P, ainda não existe o outro
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute("DataRate", StringValue("100Mbps")); //TODO mudar esses rates de acordo com tipo conexões
	std::vector<NetDeviceContainer> deviceAdjacencyList;

	for(edge_ns3* edge : simulationEdges) {
		int r = rand()%100+80;
		p2p.SetChannelAttribute("Delay", TimeValue(MilliSeconds(r)));
		deviceAdjacencyList.push_back(p2p.Install(NodeContainer(allNodes.Get(edge->srcId),allNodes.Get(edge->targetId))));
	}

	//NS_LOG_INFO ("Assign IP Addresses.");
	Ipv4AddressHelper ipv4;
	std::vector<Ipv4InterfaceContainer> interfaceAdjacencyList(simulationEdges.size());
	
	uint16_t byteA = 10;
	uint16_t byteB = 1;
	uint16_t byteC = 0;
	uint16_t byteD = 0;
	
	// Generate IP addresses for each p2p connection
	for(int i = 0; i < simulationEdges.size() ; i++) {

			byteD = (++byteD)%64;

			if(byteD == 0) {

				byteC = (++byteC)%256;

				if(byteC == 0) {
					byteB = (++byteB)%256;

				if(byteB == 0)
					byteA = (++byteA)%256;
				}
			}

			std::ostringstream subnet;
			subnet<<std::dec<<byteA<<".";
			subnet<<std::dec<<byteB<<".";
			subnet<<std::dec<<byteC<<".";
			uint16_t byteDShifted = byteD*4;	// shift left 2
			subnet<<std::dec<<byteDShifted;

			//std::cout << "subnet: " << subnet.str() << std::endl; 
			ipv4.SetBase (subnet.str ().c_str (), "255.255.255.252");
			interfaceAdjacencyList[i] = ipv4.Assign(deviceAdjacencyList[i]);

			//std::cout << " src (" << simulationEdges[i]->srcId << "-" << ns3Topology->getNodeNameById(simulationEdges[i]->srcId) << ") " << InetSocketAddress::ConvertFrom(InetSocketAddress(interfaceAdjacencyList[i].GetAddress(0),502)).GetIpv4();
			//std::cout << " target (" << simulationEdges[i]->targetId << "-" << ns3Topology->getNodeNameById(simulationEdges[i]->targetId) << ") " << InetSocketAddress::ConvertFrom(InetSocketAddress(interfaceAdjacencyList[i].GetAddress(1),502)).GetIpv4() << std::endl;
	}

	uint16_t portServer = 502;	//Modbus port (hard mode)
	//Address serverAddress;
	Address localServerAddress = InetSocketAddress(Ipv4Address::GetAny (),portServer);

	std::vector<std::pair<uint32_t, std::pair<std::string,Address>>> stationAddressList;
	std::vector<Address> serverAddressOfEachStation(simulationNodes.size());

	std::vector<bool> processedNodes(simulationNodes.size(),false);

	std::queue<node_ns3*> auxQueue;
	std::list<edge_ns3*> edgeList = ns3Topology->getEdgeListOne();
	auxQueue.push(simulationNodes[0]);
	Address serverAddress;
	Address stationAddress;
	Address sensorAddress;

	stationList.resize(simulationNodes.size());
	sensorList.resize(simulationNodes.size());
	int edgesProcessed = 0;


	/**
	 *   Previously we have installed the structure of the communication network.
	 *   Now we need to install applications on each node and start a socket connection between them.
	 *   We also install Attacker applications in the required nodes.
	 *   Therefore, we performs a breadth-first search in the nodes graph.
	**/

	/**** BFS (Breadth-first search) ****/
	while(!auxQueue.empty()) {
	
		node_ns3* currentNode = auxQueue.front();
		auxQueue.pop();

		int i = 0;

		if(edgesProcessed < simulationEdges.size()) {
		for(std::list<edge_ns3*>::iterator it = edgeList.begin(); it != edgeList.end(); it++) {

			edge_ns3* currentEdge = *it;

		if(currentNode->nodeId == currentEdge->srcId && processedNodes[currentEdge->targetId] == false) {
			if(processedNodes[currentEdge->targetId] == false)
				auxQueue.push(simulationNodes[currentEdge->targetId]);
			
				if(ns3Topology->getNodeTypeById(currentNode->nodeId) == MTU_TYPE) {
					serverAddress = InetSocketAddress(interfaceAdjacencyList[i].GetAddress(0),portServer);
					stationAddress = InetSocketAddress(interfaceAdjacencyList[i].GetAddress(1),portServer);
					std::pair<uint32_t, std::pair<std::string,Address>> nodeInfo = std::make_pair(currentEdge->targetId, std::make_pair(ns3Topology->getNodeNameById(currentEdge->targetId), stationAddress));
					
					if(ns3Topology->getNodeTypeById(currentEdge->targetId) == RTU_TYPE)
						stationAddressList.push_back(nodeInfo);
				
					if(serverAddressOfEachStation[currentEdge->targetId].IsInvalid())
						serverAddressOfEachStation[currentEdge->targetId] = serverAddress;

					//std::cout << "(Substation1) " << ns3Topology->getNodeNameById(currentEdge->targetId) << " " << InetSocketAddress::ConvertFrom(stationAddress).GetIpv4();
					//std::cout << " (SERVER) " << "MTU" << " " << InetSocketAddress::ConvertFrom(serverAddress).GetIpv4() << std::endl;

				
				} else {

					serverAddress = serverAddressOfEachStation[currentEdge->srcId];
					serverAddressOfEachStation[currentEdge->targetId] = serverAddress;

					if(ns3Topology->getNodeTypeById(currentEdge->targetId) == RTU_TYPE) {
						stationAddress = InetSocketAddress(interfaceAdjacencyList[i].GetAddress(1),portServer);
						sensorAddress = InetSocketAddress(interfaceAdjacencyList[i].GetAddress(0),portServer);
					}
					else if(ns3Topology->getNodeTypeById(currentEdge->targetId) == SENSOR_TYPE) {
						stationAddress = InetSocketAddress(interfaceAdjacencyList[i].GetAddress(0),portServer);
						sensorAddress = InetSocketAddress(interfaceAdjacencyList[i].GetAddress(1),portServer);
					}

					if(ns3Topology->getNodeTypeById(currentEdge->targetId) == RTU_TYPE) {
						std::pair<uint32_t,std::pair<std::string,Address>> nodeInfo = std::make_pair(currentEdge->targetId, std::make_pair(ns3Topology->getNodeNameById(currentEdge->targetId), stationAddress));
						stationAddressList.push_back(nodeInfo);
					}
					
					//std::cout << "(Substation2) " << ns3Topology->getNodeNameById(currentEdge->targetId) << " type:" << ns3Topology->getNodeTypeById(currentEdge->targetId) << " " << InetSocketAddress::ConvertFrom(stationAddress).GetIpv4();
					//std::cout << " (SERVER) " << "MTU" << " " << InetSocketAddress::ConvertFrom(serverAddress).GetIpv4() << std::endl;

				}

				ScadaApplication * app;

				if(ns3Topology->getNodeTypeById(currentEdge->targetId) == RTU_TYPE) {

					Ptr<Socket> tcpSocket = Socket::CreateSocket(allNodes.Get(currentEdge->targetId), TcpSocketFactory::GetTypeId());
					app = new Station(tcpSocket,InetSocketAddress(Ipv4Address::GetAny (),portServer),serverAddress,currentEdge->targetId,ns3Topology->getNodeNameById(currentEdge->targetId),stepSize,protocol);
					

					// TAMPERING VIRUS ATTACK
					/*if(ns3Topology->getNodeNameById(currentEdge->targetId) == "RTU2") {
						std::cout << "RTU2!!! TAMPERING ATTACK!" << std::endl;
						// socket, local addr, target addr, estação que faz de conta ser, stepSize, protocol, target name
						ScadaApplication * attackerApp = new MSIAttacker(tcpSocket,InetSocketAddress(Ipv4Address::GetAny (),portServer),serverAddress,currentEdge->targetId,"RTU2",stepSize,protocol,"MTU");
					
						allNodes.Get(currentEdge->targetId)->AddApplication((MSIAttacker*) attackerApp);
						stationList[currentEdge->targetId] = (MSIAttacker*) attackerApp;
						attackerApp->SetStartTime(Seconds(0.0));
						attackerApp->SetStopTime(Seconds(simEndTime));
					}else {
					*/
					stationList[currentEdge->targetId] = (Station*) app;

					allNodes.Get(currentEdge->targetId)->AddApplication((Station*) app);
					//}

				} else {

					Ptr<Socket> tcpSocket = Socket::CreateSocket(allNodes.Get(currentEdge->targetId), TcpSocketFactory::GetTypeId());

					app = new Sensor(tcpSocket,sensorAddress,stationAddress,currentEdge->targetId,ns3Topology->getNodeNameById(currentEdge->targetId),stepSize,protocol,ns3Topology->getNodeNameById(currentEdge->srcId));
				
					if(stationList[currentEdge->srcId] != NULL && ns3Topology->getNodeTypeById(currentEdge->targetId) == SENSOR_TYPE)
						stationList[currentEdge->srcId]->AddSensorAddress(currentEdge->targetId, ns3Topology->getNodeNameById(currentEdge->targetId), sensorAddress);

					// DOS ATTACKER (RTU 9, 5, 4)
/*
					if(ns3Topology->getNodeNameById(currentEdge->targetId) == "House_50" || ns3Topology->getNodeNameById(currentEdge->targetId) == "House_32" || ns3Topology->getNodeNameById(currentEdge->targetId) == "House_157") {
						
						int attacktime;

						if(ns3Topology->getNodeNameById(currentEdge->targetId) == "House_50")
							attacktime = 0;
						if(ns3Topology->getNodeNameById(currentEdge->targetId) == "House_32")
							attacktime = 0;
						if(ns3Topology->getNodeNameById(currentEdge->targetId) == "House_157")
							attacktime = 0;


						std::cout << std::endl << std::endl << std::endl << "DOS ATTACK!  " << ns3Topology->getNodeNameById(currentEdge->targetId) << " attacking " << ns3Topology->getNodeNameById(currentEdge->srcId)  << "  attackTime: " << attacktime  << std::endl << std::endl;
						// socket, local addr, target addr, estação que faz de conta ser, stepSize, protocol, target name
						ScadaApplication * attackerApp = new DOSAttacker(tcpSocket,InetSocketAddress(Ipv4Address::GetAny (),portServer),stationAddress,currentEdge->targetId,ns3Topology->getNodeNameById(currentEdge->targetId),stepSize,protocol,ns3Topology->getNodeNameById(currentEdge->targetId),attacktime);

						sensorList[currentEdge->targetId] = (DOSAttacker*) attackerApp;

						allNodes.Get(currentEdge->targetId)->AddApplication((DOSAttacker*) attackerApp);

						attackerApp->SetStartTime(Seconds(0.0));
						attackerApp->SetStopTime(Seconds(simEndTime));
						
					}
					else
					{
*/
						sensorList[currentEdge->targetId] = (Sensor*) app;
						allNodes.Get(currentEdge->targetId)->AddApplication((Sensor*) app);

					//}
					
				}
				
				app->SetStartTime(Seconds(0.0));
				app->SetStopTime(Seconds(simEndTime));

				edgesProcessed++;
				processedNodes[currentEdge->targetId] = true;
				currentEdge->srcId = -1;
				currentEdge->targetId = -1;

			}

			if(currentNode->nodeId == currentEdge->targetId && processedNodes[currentEdge->srcId] == false) {
				if(processedNodes[currentEdge->srcId] == false)
					auxQueue.push(simulationNodes[currentEdge->srcId]);

				if(ns3Topology->getNodeTypeById(currentNode->nodeId) == MTU_TYPE) {
					serverAddress = InetSocketAddress(interfaceAdjacencyList[i].GetAddress(1),portServer);
					stationAddress = InetSocketAddress(interfaceAdjacencyList[i].GetAddress(0),portServer);
					std::pair<uint32_t,std::pair<std::string,Address>> nodeInfo = std::make_pair(currentEdge->srcId, std::make_pair(ns3Topology->getNodeNameById(currentEdge->srcId), stationAddress));
					
					if(ns3Topology->getNodeTypeById(currentEdge->srcId) == RTU_TYPE)
						stationAddressList.push_back(nodeInfo);

					if(serverAddressOfEachStation[currentEdge->srcId].IsInvalid())
						serverAddressOfEachStation[currentEdge->srcId] = serverAddress;

					//std::cout << "(Substation3) " << ns3Topology->getNodeNameById(currentEdge->srcId) << " " << InetSocketAddress::ConvertFrom(stationAddress).GetIpv4();
					//std::cout << " (SERVER) " << "MTU" << " " << InetSocketAddress::ConvertFrom(serverAddress).GetIpv4() << std::endl;

				} else {

					serverAddress = serverAddressOfEachStation[currentEdge->targetId];
					serverAddressOfEachStation[currentEdge->srcId] = serverAddress;

					if(ns3Topology->getNodeTypeById(currentEdge->srcId) == RTU_TYPE) {
						stationAddress = InetSocketAddress(interfaceAdjacencyList[i].GetAddress(0),portServer);
						sensorAddress = InetSocketAddress(interfaceAdjacencyList[i].GetAddress(1),portServer);
					} else if(ns3Topology->getNodeTypeById(currentEdge->srcId) == SENSOR_TYPE) {
						stationAddress = InetSocketAddress(interfaceAdjacencyList[i].GetAddress(1),portServer);
						sensorAddress = InetSocketAddress(interfaceAdjacencyList[i].GetAddress(0),portServer);					
					}
					
					if(ns3Topology->getNodeTypeById(currentEdge->srcId) == RTU_TYPE) {
						std::pair<uint32_t,std::pair<std::string,Address>> nodeInfo = std::make_pair(currentEdge->srcId, std::make_pair(ns3Topology->getNodeNameById(currentEdge->srcId), stationAddress));
						stationAddressList.push_back(nodeInfo);
					}

					//std::cout << "(Substation4) " << ns3Topology->getNodeNameById(currentEdge->srcId) << " type:" << ns3Topology->getNodeTypeById(currentEdge->srcId) << " " << InetSocketAddress::ConvertFrom(stationAddress).GetIpv4();
					//std::cout << " (SERVER) " << "MTU" << " " << InetSocketAddress::ConvertFrom(serverAddress).GetIpv4() << std::endl;
				
				}

				ScadaApplication * app;

				if(ns3Topology->getNodeTypeById(currentEdge->srcId) == RTU_TYPE) {

					Ptr<Socket> tcpSocket = Socket::CreateSocket(allNodes.Get(currentEdge->srcId), TcpSocketFactory::GetTypeId());
					app = new Station(tcpSocket,InetSocketAddress(Ipv4Address::GetAny (),portServer),serverAddress,currentEdge->srcId,ns3Topology->getNodeNameById(currentEdge->srcId),stepSize,protocol);

					stationList[currentEdge->srcId] = (Station*) app;

					allNodes.Get(currentEdge->srcId)->AddApplication((Station*) app);

				} else {

					Ptr<Socket> tcpSocket = Socket::CreateSocket(allNodes.Get(currentEdge->srcId), TcpSocketFactory::GetTypeId());

					app = new Sensor(tcpSocket,sensorAddress,stationAddress,currentEdge->srcId,ns3Topology->getNodeNameById(currentEdge->srcId),stepSize,protocol,ns3Topology->getNodeNameById(currentEdge->targetId));

					if(stationList[currentEdge->targetId] != NULL && ns3Topology->getNodeTypeById(currentEdge->srcId) == SENSOR_TYPE)
						stationList[currentEdge->targetId]->AddSensorAddress(currentEdge->srcId, ns3Topology->getNodeNameById(currentEdge->srcId), sensorAddress);

					// DOS ATTACKER (RTU 9, 5, 4)
/*
					if(ns3Topology->getNodeNameById(currentEdge->srcId) == "House_50" || ns3Topology->getNodeNameById(currentEdge->srcId) == "House_242" || ns3Topology->getNodeNameById(currentEdge->srcId) == "House_409") {

						int attacktime;

						if(ns3Topology->getNodeNameById(currentEdge->srcId) == "House_50")
							attacktime = 0;
						if(ns3Topology->getNodeNameById(currentEdge->srcId) == "House_242")
							attacktime = 0;
						if(ns3Topology->getNodeNameById(currentEdge->srcId) == "House_409")
							attacktime = 0;

						std::cout << std::endl << std::endl << std::endl << "DOS ATTACK!  " << ns3Topology->getNodeNameById(currentEdge->srcId) << " attacking " << ns3Topology->getNodeNameById(currentEdge->targetId)  << "  attackTime: " << attacktime << std::endl << std::endl;
						// socket, local addr, target addr, estação que faz de conta ser, stepSize, protocol, target name
						ScadaApplication * attackerApp = new DOSAttacker(tcpSocket,InetSocketAddress(Ipv4Address::GetAny (),portServer),stationAddress,currentEdge->srcId,ns3Topology->getNodeNameById(currentEdge->srcId),stepSize,protocol,ns3Topology->getNodeNameById(currentEdge->targetId),attacktime);

						sensorList[currentEdge->srcId] = (DOSAttacker*) attackerApp;

						allNodes.Get(currentEdge->srcId)->AddApplication((DOSAttacker*) attackerApp);

						attackerApp->SetStartTime(Seconds(0.0));
						attackerApp->SetStopTime(Seconds(simEndTime));
						
					}
					else
					{
*/
						sensorList[currentEdge->srcId] = (Sensor*) app;

						allNodes.Get(currentEdge->srcId)->AddApplication((Sensor*) app);
					//}

				}

				
				app->SetStartTime(Seconds(0.0));
				app->SetStopTime(Seconds(simEndTime));

				edgesProcessed++;
				processedNodes[currentEdge->srcId] = true;
				currentEdge->srcId = -1;
				currentEdge->targetId = -1;

			}

			i++;
		}			

		}

	}

	
	// Create server application
	Server * serverApp;
	serverApp = new Server(stationAddressList,localServerAddress,0,MTU_TYPE,stepSize,protocol);
	allNodes.Get(0)->AddApplication(serverApp);
	serverApp->SetStartTime(Seconds(0.0));
	serverApp->SetStopTime(Seconds(simEndTime));
	serverList.push_back(serverApp);
	
	// Populate routing tables of all devices
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();



	// Here we create Wireshark traffic files of SCADA components in simOutput/wireshark/
	AsciiTraceHelper ascii;
	std::stringstream createFolder,folder;
	folder << "simOutput" << "/" << "wireshark/";
	createFolder<< "mkdir -p " << folder.str();
	
	system(createFolder.str().c_str());
	//p2p.EnableAsciiAll(ascii.CreateFileStream ("simOutput/wireshark/tcp-server.tr"));

	//folder << "Traffic";
	p2p.EnablePcap (folder.str(), allNodes);

	// Since our network has more than 500 components, we select only the nodes we want to analyse (House_50, House_242 and House_409)
	/*int i = 0;
	for(edge_ns3* edge : simulationEdges) {
		std::stringstream fileName;

		if(ns3Topology->getNodeNameById(edge->srcId) == "House_50" || ns3Topology->getNodeNameById(edge->targetId) == "House_50" ||
			ns3Topology->getNodeNameById(edge->srcId) == "House_242" || ns3Topology->getNodeNameById(edge->targetId) == "House_242" ||
			ns3Topology->getNodeNameById(edge->srcId) == "House_409" || ns3Topology->getNodeNameById(edge->targetId) == "House_409") {

			fileName.str(std::string());
			fileName << folder.str();
			fileName << ns3Topology->getNodeNameById(edge->srcId);
			p2p.EnablePcap (fileName.str(), (NodeContainer) allNodes.Get(edge->srcId));

			fileName.str(std::string());
			fileName << folder.str();
			fileName << ns3Topology->getNodeNameById(edge->targetId);
			p2p.EnablePcap (fileName.str(), (NodeContainer) allNodes.Get(edge->targetId));
		}
		
		i++;
	}

	*/

}

}


