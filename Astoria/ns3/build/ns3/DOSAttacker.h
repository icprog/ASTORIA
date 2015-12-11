
#ifndef TCP_DOSATTACKER_H
#define TCP_DOSATTACKER_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

#include <sys/socket.h>
#include "modbus-protocol.h"
#include "Sensor.h"
#include "transmission-data.h"

namespace ns3 {

class DOSAttacker : public Sensor
{
public:
	DOSAttacker (Ptr<Socket> socket ,const Address &l, const Address &t, uint32_t nodeId, std::string nodeName, uint32_t interval, std::string Protocol, std::string serverName, uint32_t attackStartTime);
	virtual ~DOSAttacker();

	void StartApplication(void);
	void StopApplication(void);

	
	TransmissionData* getTransmissionData();
	Address server;
private:
	void ScheduleNextTx(uint32_t dos_interval);

	void HandleRead(Ptr<Socket> socket);
	void SendDOS();
	void Send(std::string message);
	void ScheduleNextTxSeconds(uint32_t dos_interval);
	

	uint32_t m_dos_interval;
	uint32_t m_attack_wait_interval;
	bool attackStarted;

	uint32_t m_packetsGet;
	uint32_t packetsSent;
	std::vector<uint8_t*>  m_dataList;
	std::vector<uint32_t>  m_dataSizeList;
	uint32_t m_functionCode;
	std::string targetName;

	ScadaProtocol *messageProtocol;

	TransmissionData* transmissionData;
	uint32_t validDataCount;
	uint32_t expectedDataCount;
	EventId dataInputEvent;
	EventId DOSAttackEvent;

};

}
#endif
