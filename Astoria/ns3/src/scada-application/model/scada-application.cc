
#include "scada-application.h"

namespace ns3 {


ScadaApplication::ScadaApplication(const Address &l, uint32_t nodeId, std::string nodeName){

	m_local=l;
	m_nodeName = nodeName;
	m_nodeId = nodeId;
	m_socket=0;

}

ScadaApplication::~ScadaApplication(){

	m_socket=0;

}

void ScadaApplication::StartApplication(){

	if (!m_socket){
		m_socket = Socket::CreateSocket (GetNode (),TcpSocketFactory::GetTypeId());
      	m_socket->Bind(m_local);
      	m_socket->Listen();
      	m_socket->ShutdownSend();
	}

   	m_socket->SetRecvCallback (MakeCallback (&ScadaApplication::HandleRead, this));
	m_socket->SetAcceptCallback (
    		MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
    		MakeCallback (&ScadaApplication::HandleAccept, this));	
}

void ScadaApplication::StopApplication() {

	if (m_socket != 0){
			m_socket->Close ();
      		m_socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>> ());
    }

	for(uint32_t i=0;i<m_socketList.size();++i)
        m_socketList.at(i)->Close();
}

void ScadaApplication::HandleRead(Ptr<Socket> socket){

	Ptr<Packet> packet;
	packetDataSize=0;
	
  	while ((packet = socket->RecvFrom(from))) {
  	
		packetData = new uint8_t[packet->GetSize ()];
		packet->CopyData (packetData, packet->GetSize ());
		packetDataSize=packet->GetSize();
	}

}

void ScadaApplication::HandleAccept(Ptr<Socket> s, const Address& from) {

	s->SetRecvCallback (MakeCallback (&ScadaApplication::HandleRead, this));
	m_socketList.push_back(s);

}

void ScadaApplication::ScheduleNextTx() {}

void ScadaApplication::AddData(float value) {

	inputRegisters.resize(validDataCount+1);
	inputRegisters[validDataCount] = value;
	validDataCount++;
	
	transmissionData->addTransmissionData(Simulator::Now().GetSeconds(),value);

}

TransmissionData* ScadaApplication::getTransmissionData(){ 
	TransmissionData* data;
	return data;
}
std::vector<TransmissionData*> ScadaApplication::getSensorData(){ 
	std::vector<TransmissionData*> out;
	return out;
}
std::vector<TransmissionData*> ScadaApplication::getStationData(){

	std::vector<TransmissionData*> out;
	return out;

}
void ScadaApplication::AddSensorAddress(uint32_t , std::string , Address address){}




}

