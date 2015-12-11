#include "simulation-control.h"

using namespace std;

namespace ns3 {

SimulationControl::SimulationControl(Ns3Simulation *sim){

	simulation = sim;
	mosaikSocket = createSocket();
}

int SimulationControl::createSocket(){

	int socket_desc , new_socket , c;
	struct sockaddr_in server , client;

	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 5678 );


	int yes = 1;
	if ( setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 )
	{
	    perror("setsockopt");
	}

	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("bind failed");
		return 1;
	}
	puts("bind done");

	listen(socket_desc , 3);

	puts("Waiting for Mosaik connection...");
	c = sizeof(struct sockaddr_in);
	if( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		puts("Connection accepted");
		return new_socket;	
	}

	return -1;

}

string SimulationControl::getParamValue(string paramName,vector<pair<string,string> > initParameters){

	for(pair<string,string> p : initParameters)
			if(paramName.compare(p.first)==0)
				return p.second;
	return "";

}

void SimulationControl::simulationInit(){//ainda não há verificação de falha para init (somente verificação da msg)

	std::vector<std::pair<std::string,std::string> > initParameters = currentMessage->getInitData();

	std::string protocol;
	uint32_t nNodesPerSubServer,simEndTime;

	std::istringstream(getParamValue("subserver",initParameters)) >> nNodesPerSubServer;
	std::istringstream (getParamValue("sim_end",initParameters))  >> simEndTime;
	std::istringstream (getParamValue("step_size",initParameters))  >> stepSize;
	protocol = getParamValue("protocol",initParameters);

	simulation->setSimInfo(nNodesPerSubServer,simEndTime,stepSize,protocol);

	reply=currentMessage->createReply(SIM_META);
	send(mosaikSocket, reply.c_str(), reply.length(), 0);
}

void SimulationControl::simulationCreate(){

	beginTime = std::chrono::high_resolution_clock::now();

	std::pair<std::string,std::string> topoFile = currentMessage->getInitData()[0]; //Get model type and quantity
	//std::cout << "sim1" << std::endl;

	//std::cout << "pair 1: " << topoFile.first << " pair 2: " << topoFile.second << std::endl; 

	std::vector<node_ns3*> creationReport = simulation->setTopology(topoFile.second);
	//std::cout << "sim2" << std::endl;
	for(node_ns3* node : creationReport){
		if(node->nodeName != "MTU"){
			referenceData.resize(node->nodeId + 1);
			referenceData[node->nodeId] = new TransmissionData(stepSize,node->nodeName,node->nodeId);
		}
	}
	//std::cout << "sim3" << std::endl;
	reply=currentMessage->createReply(creationReport);
	//std::cout << "sim4" << std::endl;
	send(mosaikSocket, reply.c_str(), reply.length(), 0);
	//std::cout << "sim5" << std::endl;
	simulation->createSimulation();
}

void SimulationControl::simulationStep(){
	std::vector<std::pair<std::string,float> > nodesData = currentMessage->getStepData();

	for(std::pair<std::string,float> d : nodesData){

		simulation->addNodeData(d.first,d.second);

		TransmissionData* data = getReferenceData(d.first);
		data->addTransmissionData(Simulator::Now().GetSeconds(),d.second);
	}
	

	float currentTime = simulation->simulationStep();

	std::stringstream nextStep;
	nextStep << currentTime;
	reply = currentMessage->createReply(nextStep.str());
	send(mosaikSocket, reply.c_str(), reply.length(), 0);

}

void SimulationControl::simulationStop(){

	std::cout<<"Stop"<<std::endl;

	close(mosaikSocket);

	endTime = std::chrono::high_resolution_clock::now();

	std::cout << "Running time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime-beginTime).count() << std::endl;
	
	std::vector<TransmissionData*> rtuData = simulation->retrieveRTUData();
	generateOutputFiles(rtuData, "RTU");
	std::vector<TransmissionData*> mtuData = simulation->retrieveMTUData();
	generateOutputFiles(mtuData, "MTU");
	std::vector<TransmissionData*> sensorData = simulation->retrieveSensorData();
	generateOutputFiles(sensorData, "SENSOR");
	simulation->finished = true;

}

TransmissionData* SimulationControl::getReferenceData(std::string nodeName){
	for(TransmissionData* data : referenceData){
		//std::cout<<data->nodeId<<"---"<<nodeName<<std::endl;
		if(data != NULL) {
			if(data->nodeName.compare(nodeName) == 0){
				return data;
			}
		}
	}
}


void SimulationControl::generateOutputFiles(std::vector<TransmissionData*> outputData, std::string fileType){

	if(outputData.size() == 0)
		return;


	std::stringstream createFolder,folder;
	folder << "simOutput" << "/" << "power" << "/" << fileType;
	createFolder<< "mkdir -p " << folder.str();
	//std::cout << "a!" << std::endl;
	system(createFolder.str().c_str());

	int maxTime = 0;
	//std::cout << "outputData size: !" << outputData.size() << std::endl;
	for(TransmissionData* tData : outputData){
		if(tData != NULL && tData->getTransmissionDataSize() != 0){
			std::ofstream nodeFile;
			std::stringstream fileName;
			fileName<< folder.str()<<"/"<<tData->nodeName<<"-id="<<tData->getNodeId()<<".txt";
			nodeFile.open(fileName.str());
			nodeFile<<"#\tX\tY\n";
			//std::cout << "b!" << std::endl;
			for(int i = 0; i<tData->getTransmissionDataSize()-1; i++) {
				//std::cout << "c!" << std::endl;
				std::pair<float,uint16_t> dataInfo = tData->getTransmissionData(i);
				//std::cout << "d!" << std::endl;
				nodeFile<<dataInfo.first << ";" << dataInfo.second<<"\n";
				//std::cout << "e!" << std::endl;
				if(maxTime < dataInfo.first)
					maxTime = dataInfo.first;

				//std::cout << "f!" << std::endl;
			}
			nodeFile.close();
		}

	}

/*
	folder.clear();
	folder.str(std::string());
	createFolder.clear();
	createFolder.str(std::string());

	folder << "simOutput" << "/" << "control" << "/" << fileType;
	createFolder<< "mkdir -p " << folder.str();
	
	system(createFolder.str().c_str());
	
	for(TransmissionData* tData : outputData){
		if(tData != NULL && tData->getTransmissionDataSize() != 0) {
			std::ofstream nodeFile;
			std::stringstream fileName;
			fileName<< folder.str()<<"/"<<tData->nodeName<<".txt";
			nodeFile.open(fileName.str());
			nodeFile<<"#\tX\tY\n";
			std::pair<float,uint16_t> dataInfo = tData->getTransmissionData(0);
			float prevTime = dataInfo.first;
			uint16_t buffer;
			int j = 0;
			for(int i = 0; i < maxTime; i++){

				std::pair<float,uint16_t> dataInfo = tData->getTransmissionData(j);

				if(i == dataInfo.first) {

					nodeFile << i << ";" << sizeof(dataInfo.second) << "\n";
					j++;

				} else {
					nodeFile << i << ";" << 0 << "\n";
				}
				
			}
			nodeFile.close();
		}
	}*/
	
}

void SimulationControl::run() {

	int flag = 0;

	while(1) {
		char recv_message[1000001];
		
		// ARRUMAR ISSO, POIS TA DANDO BUFFER OVERFLOW QUANDO TEM MUITOS NODOS. 
		// FAZER COM WHILE (PRECISA ACHAR O FIM DA MENSAGEM)
		// Message terminator: }]]

		std::string messageString;
		bool ready = false;
		
		do {

			int num = recv(mosaikSocket, recv_message, 5000,0);

			//std::cout << "Num: " << num << std::endl;
			//std::cout << "ultimo: " << recv_message[num-1] << std::endl;

			for(int i = 0; i<num;i++) {
				messageString.push_back(recv_message[i]);
			}

			// Message terminator: }]]
			if(recv_message[num-1] == ']' && recv_message[num-2] == ']' && recv_message[num-3] == '}')
				ready = true;

		} while(!ready);


		//if(flag < 3) {
		//	std::cout << "messageString: " << messageString << std::endl; 
		//	flag++;
		//}

		currentMessage = new MosaikCall(messageString);
		std::string messageInstruction = currentMessage->getInstruction();

		//cout << messageInstruction << endl;

		if(messageInstruction.compare("init")==0){
			simulationInit();
		}else if(messageInstruction.compare("create")==0){
			simulationCreate();
		}else if(messageInstruction.compare("step")==0){
			simulationStep();
		}else if(messageInstruction.compare("stop")==0){
			simulationStop();
			return;
		}


	}

}

}
