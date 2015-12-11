
#ifndef TCP_SCADA_SERVER_H
#define TCP_SCADA_SERVER_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

#include "transmission-data.h"
#include "ns3/scada-application.h"
#include "ns3/modbus-protocol.h"

// Usado na transmissao dos dados para o analizador
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <unistd.h>
//#include <sys/types.h>
#include <sys/socket.h>
//#include <netinet/in.h>
#include <arpa/inet.h>

namespace ns3 {

class Server: public ScadaApplication {

public:
    Server(std::vector<std::pair<uint32_t, std::pair<std::string,Address> >> t, const Address &l, uint32_t nodeId, std::string nodeName, uint32_t interval,std::string protocolType);
    virtual ~Server();
	void StartApplication();
	void StopApplication();
	void ScheduleNextTx();
	std::vector<TransmissionData*> getStationData();
	std::vector<TransmissionData*> getSensorData();


private:
	void HandleRead(Ptr<Socket> socket);
	void SendBroadcast();
	void SendUnicast(uint32_t index);
	void Send();
	void HandleAccept(Ptr<Socket> s, const Address& from);
	
	uint32_t packetsSent;
	
	std::vector<std::pair<uint32_t,std::pair<std::string,Address> >> targets;
	std::vector<Ptr<Socket> > m_socketSendList;
	std::vector<Ptr<Socket> > m_socketReceiveList;

	std::vector<TransmissionData*> stationData;
	std::vector<TransmissionData*> sensorData;

	EventId sendEvent;

	uint32_t m_interval;
	uint32_t m_functionCode;
	uint32_t m_nextStation;

	ScadaProtocol *messageProtocol;

};

}
#endif
