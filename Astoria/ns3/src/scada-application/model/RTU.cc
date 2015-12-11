
#include "RTU.h"

#define PORTA_SRV 2023 // porta do servidor
#define STR_IPSERVER "127.0.0.1"

namespace ns3 {


Station::Station(Ptr<Socket> socket,const Address &l, const Address &t, uint32_t nodeId, std::string nodeName,uint32_t interval,std::string Protocol):ScadaApplication(l,nodeId,nodeName)
{
    m_interval = interval;
    server = t;
    m_socket_server=socket;//TODO teoricamente não precisa disso
    m_packetsGet=0;
    m_local=l;

    if(Protocol.compare("Modbus")==0)
    	messageProtocol = new ModbusProtocol();

    transmissionData = new TransmissionData(interval,nodeName,nodeId);
    expectedDataCount = 0;
    validDataCount = 0;

    ScheduleNextDataInput();
}

Station::~Station() { }

Address Station::GetStationAddress() {
	return m_local;
}

TransmissionData* Station::getTransmissionData(){ return transmissionData; }
std::vector<TransmissionData*> Station::getSensorData(){ return sensorData; }

void Station::AddData(float value) {

	inputRegisters.resize(validDataCount+1);
	inputRegisters[validDataCount] = value;
	validDataCount++;

	transmissionData->addTransmissionData(Simulator::Now().GetSeconds(),value);

}

void Station::RecvInputData() {

	expectedDataCount++;
	if(expectedDataCount > validDataCount){
		inputRegisters.push_back(0);		
	}

	ScheduleNextDataInput();
}

void Station::ScheduleNextDataInput() {

    	dataInputEvent = Simulator::Schedule(Seconds(m_interval),		
                                    &Station::RecvInputData,
                                    this);

}


void Station::StartApplication (void) {

	m_socket_server->Bind();
	m_socket_server->Connect(server);

	// Connect with sensors
	for(uint32_t i=0;i < sensorsAddress.size(); i++) {
		Ptr<Socket> newSocket = Socket::CreateSocket (GetNode(),TcpSocketFactory::GetTypeId());
		newSocket->Bind();
		newSocket->Connect(sensorsAddress.at(i).second.second);

		std::pair<uint32_t, std::pair<std::string,Ptr<Socket>>> socketPair;
		socketPair.first = sensorsAddress.at(i).first;
		socketPair.second.first = sensorsAddress.at(i).second.first;
		socketPair.second.second = newSocket;
		m_socket_sensor_send.push_back(socketPair);
	
		sensorData.push_back( new TransmissionData(m_interval,sensorsAddress.at(i).second.first, sensorsAddress.at(i).first));
		sensorLastData.push_back(std::make_pair(sensorsAddress.at(i).first, std::make_pair(sensorsAddress.at(i).second.first,0)));
		sensorRequestTime.push_back(std::make_pair(sensorsAddress.at(i).first,0));
		bufferOverflowCounter.push_back(std::make_pair(sensorsAddress.at(i).first,0));

	}

	ScadaApplication::StartApplication();

	ScheduleNextTx();
}


void Station::StopApplication (void)
{

	ScadaApplication::StopApplication();
	m_socket_server->Close();
	m_socket_server;

	if (requestEvent.IsRunning()) {
		Simulator::Cancel (requestEvent);
	}

	for(uint32_t i=0;i<m_socket_sensor_send.size();i++)
		m_socket_sensor_send.at(i).second.second->Close();

	m_socket_sensor_send.clear();


}


void Station::ReplyServer(std::string message) {

	std::vector<std::string> reply = messageProtocol->createReply(message,inputRegisters,m_nodeId,sensorLastData);
	
	for(int i = 0; i < reply.size(); i++) {
		m_data = (uint8_t*) reply.at(i).c_str();
		packetSize=reply.at(i).length();

		Ptr<Packet> p =Create<Packet>(m_data, packetSize);

		int a = m_socket_server->Send(p);


		Address addr;
		m_socket_server->GetSockName (addr);
		
		/*std::cout<<"(RTU-S-SERVER) At time " 	<< Simulator::Now ().GetSeconds ()
			<< "s Station "	<< m_nodeName	<< " " << (InetSocketAddress::ConvertFrom (addr)).GetIpv4()
			<< " sent "  	<<  p->GetSize () 
			<< " bytes to Server "
			<< std::endl;
		*/

		
	}
}


void Station::HandleRead (Ptr<Socket> socket)
{
	ScadaApplication::HandleRead(socket);

	bool isDataFromSensor = false;

	if(packetDataSize > 0) {

		std::vector<std::pair<uint32_t,uint16_t>> data = messageProtocol->getMessageData(std::string((char*)packetData));
		uint16_t plength = messageProtocol->getPacketLength();
		//uint16_t plength = 12;

		if (m_nodeName == "RTU2")
			plength = plength + 6;
		if (m_nodeName == "RTU4")
			plength = plength * 2;
		if (m_nodeName == "RTU5")
			plength = plength * 4;
		if (m_nodeName == "RTU9")
			plength = plength - 6;


		std::string nodeName = "";
		uint32_t nodeId = 0;

		if(data.at(0).first != -1) {
			for(uint32_t i = 0; i< sensorsAddress.size(); i++) {
				if(InetSocketAddress::ConvertFrom(sensorsAddress.at(i).second.second).GetIpv4()==InetSocketAddress::ConvertFrom(from).GetIpv4()) {
					nodeId = sensorsAddress.at(i).first;
					nodeName = sensorsAddress.at(i).second.first;
					isDataFromSensor = true;
					//std::cout << "(RTU-R-SENSOR) At time " << Simulator::Now().GetSeconds() << "s Station " << m_nodeName << " received from " << nodeName << " " << (InetSocketAddress::ConvertFrom (sensorsAddress.at(i).second.second)).GetIpv4() <<  " !" << std::endl;


/*
		// ------------------SOCKET-----------------------------
		//	- pode ser uma boa fazer um outro método privte pra lidar com isso e não piorar esse macarrão
		struct sockaddr_in peerstruct;
		int peer_len;
		char buffer[100];
	 
		// Cria o socket na familia AF_INET (Internet) e do tipo UDP (SOCK_DGRAM)
		int s1 = socket(AF_INET, SOCK_DGRAM, 0);
		if(s1 >= 0) {

			// Cria a estrutura com quem vai conversar 
			peerstruct.sin_family = AF_INET;
			peerstruct.sin_port = htons(PORTA_SRV);
			peerstruct.sin_addr.s_addr = inet_addr(STR_IPSERVER); 
			peer_len = sizeof(peerstruct);
	
			// Envia pacotes Hello e aguarda resposta
			strcpy(buffer,"Hello from ");
			//strcat(buffer, m_nodeName);
			//strcat(buffer, ".");
			sendto(s1, buffer, sizeof(buffer), 0, (struct sockaddr *)&peerstruct, peer_len);
			close(s1);
		}
		// ----------------------------------------------------
*/



					// Buffer overflow vulnerability (House_470 -> RTU9)
					
						//std::cout << "DIFERENÇA: " << Simulator::Now().GetSeconds() - sensorRequestTime.at(i).first << std::endl;
						// RTU cannot receive 0,00000001s => 100
						if(Simulator::Now().GetSeconds() - sensorRequestTime.at(i).first < 50) {
							if(bufferOverflowCounter.at(i).second == 200000) {
							
								std::cout << nodeName << " halted at " << Simulator::Now().GetSeconds() << " s" << std::endl;
								StopApplication();

							}

							//std::cout << "contador " << nodeName << ": " << bufferOverflowCounter.at(i).second << std::endl;

							bufferOverflowCounter.at(i).second += 1;

						}

						sensorRequestTime.at(i).first = Simulator::Now().GetSeconds();

					for(int j = 0; j < sensorData.size(); j++) {
						if(sensorData[j]->nodeName == nodeName) {
							//std::cout << "NODE NAME = " << nodeName << " data: " << data << std::endl;
							sensorData[j]->addTransmissionData(Simulator::Now().GetSeconds(), data.at(0).second);
							std::pair<float,uint16_t> a = sensorData[j]->getTransmissionData(0);
							sensorLastData[j].second.second = data.at(0).second;
							//std::cout << "a.first " << a.first << " a.second " << a.second << std::endl;
							j = sensorData.size();
						}
					}
				
					i = sensorsAddress.size();
				}
			}
		}
		
		if(isDataFromSensor) {
			 std::ofstream out;

 			// std::ios::app is the open mode "append" meaning
 			// new data will be written to the end of the file.
 			out.open("RTU_data.txt", std::ios::app);
			
			std::string source_str = std::to_string(data.at(0).first);
			//std::string source_str = "";
 			std::string target_str = std::to_string(m_nodeId);
			std::string len_str = std::to_string(plength);
 			out << std::setfill('0') << std::setw(4) << source_str << std::setfill('0') << std::setw(4) << target_str << std::setfill('0') << std::setw(8) << len_str << Simulator::Now().GetSeconds() << std::endl;
			//out << "source: " << source_str << " target: " << target_str << " length: " << len_str << " time: " << Simulator::Now().GetSeconds() << std::endl;
			out.close();
		}
		
		if(!isDataFromSensor) {
			Address addr;
			m_socket_server->GetSockName (addr);
			//std::cout << "(RTU-R-SERVER) At time " << Simulator::Now().GetSeconds() << "s Station " << m_nodeName << " received data from Server " << (InetSocketAddress::ConvertFrom (addr)).GetIpv4() <<  " !" << std::endl;
			
			if(packetDataSize>0) {
				ReplyServer(std::string((char*)packetData));
			}

		}

		
	}

}

void Station::AddSensorAddress(uint32_t id, std::string name, Address address) {

	std::pair<uint32_t, std::pair<std::string,Address>> pair;
	pair.first = id;
	pair.second.first = name;
	pair.second.second = address;
	sensorsAddress.push_back(pair);

}


std::vector<std::pair<uint32_t, std::pair<std::string,Address>>>  Station::GetSensorAddress() {
	return sensorsAddress;
}


void Station::ScheduleNextTx() {

    uint32_t temp_interval = 2000;
    int chance = rand() % 100;

    if (chance < 15) {
	temp_interval = 2200;
	
    }
    requestEvent = Simulator::Schedule(MilliSeconds(temp_interval),		
                     &Station::RequestSensors, this);        
}

void Station::RequestSensors() {

	std::string message = messageProtocol->createRequest(4, packetsSentSensors);//functionCode = 4 e qual Reg lê, no caso o ultimo

	m_data = (uint8_t*) message.c_str();
	packetSizeSensors=message.length();
	Ptr<Packet> p =Create<Packet>(m_data,packetSizeSensors);
	
	for(uint32_t i = 0; i < sensorsAddress.size(); i++) {
		std::pair<uint32_t, std::pair<std::string,Ptr<Socket>>> sensor = m_socket_sensor_send.at(i);
		int a = (sensor.second.second)->Send(p);		//Broadcast
		Address addr;
		(sensor.second.second)->GetSockName(addr);
		//std::cout << "Send to " << sensor.first << " " << (InetSocketAddress::ConvertFrom (addr)).GetIpv4() << std::endl;
	}

	Address addr;
	m_socket_server->GetSockName (addr);
	/*std::cout<<"(RTU-S-SENSOR) At time " << Simulator::Now().GetSeconds()
		<< "s " << m_nodeName << " " <<(InetSocketAddress::ConvertFrom (addr)).GetIpv4()
		<< " sent "<<  p->GetSize () 
		<< " bytes to ALL Sensors "
		<<std::endl;
	*/
	packetSizeSensors=0;
	packetsSentSensors++;

	ScheduleNextTx();

}

}
