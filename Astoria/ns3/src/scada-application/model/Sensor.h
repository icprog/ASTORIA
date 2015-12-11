
#ifndef TCP_SCADA_SENSOR_H
#define TCP_SCADA_SENSOR_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

#include <sys/socket.h>
#include "modbus-protocol.h"
#include "scada-application.h"
#include "transmission-data.h"

namespace ns3 {

class Sensor: public ScadaApplication
{
public:
	Sensor (Ptr<Socket> socket ,const Address &l, const Address &t, uint32_t nodeId, std::string nodeName,uint32_t interval,std::string Protocol, std::string serverName);
	virtual ~Sensor ();

	void StartApplication (void);
	void StopApplication (void);
	void AddData(float data);
	
	TransmissionData* getTransmissionData();
	Address server;

protected:
	void HandleRead(Ptr<Socket> socket);
	void Send(std::string message);
	
	void ScheduleNextDataInput();
	void RecvInputData();

	uint32_t m_interval;
	Ptr<Socket> m_socket_send;

	uint32_t m_packetsGet;
	std::vector<uint8_t*>  m_dataList;
	std::vector<uint32_t>  m_dataSizeList;
	uint32_t m_functionCode;
	std::string rtuName;

	ScadaProtocol *messageProtocol;


	uint32_t expectedDataCount;
	EventId dataInputEvent;
};

}
#endif
