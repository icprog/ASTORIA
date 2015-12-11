/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef TCP_SCADA_APPLICATION_H
#define TCP_SCADA_APPLICATION_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

#include "scada-protocol.h"
#include "transmission-data.h"

#include <cassert>
#include <iomanip>
#include <cstdint>


namespace ns3 {

class ScadaApplication : public Application {

public:
	ScadaApplication(const Address &l, uint32_t nodeId, std::string nodeName);
	virtual ~ScadaApplication();
	virtual void StartApplication();
	virtual void StopApplication();


	virtual void ScheduleNextTx();
	virtual void AddData(float);
	virtual TransmissionData* getTransmissionData();
	virtual std::vector<TransmissionData*> getSensorData();
	virtual std::vector<TransmissionData*> getStationData();
	virtual void AddSensorAddress(uint32_t id, std::string name, Address address);

protected:
	virtual void HandleRead(Ptr<Socket> socket);
	void HandleAccept(Ptr<Socket> s, const Address& from);


	Address m_local;
	Ptr<Socket> m_socket;

	uint8_t *m_data;
	uint8_t *packetData;
	uint32_t packetDataSize;
	uint32_t packetSize;
	std::vector<float> inputRegisters;
	uint32_t validDataCount;
	std::string m_nodeName;
	uint32_t m_nodeId;
	TransmissionData* transmissionData;

	std::vector<Ptr<Socket> > m_socketList;
	Address from;

};

}

#endif /* TCP_SCADA_APPLICATION_H */

