
#include "MSIAttacker.h"

namespace ns3 {


MSIAttacker::MSIAttacker(Ptr<Socket> socket,const Address &l, const Address &t, uint32_t nodeId, std::string nodeName,uint32_t interval,std::string Protocol, std::string serverName):ScadaApplication(l,nodeId,nodeName)
{
    m_interval = interval;
    server = t;
    m_socket_send=socket;//TODO teoricamente não precisa disso
    m_packetsGet=0;
    m_local=l;
    targetName = serverName;

    if(Protocol.compare("Modbus")==0)
    	messageProtocol = new ModbusProtocol();

    transmissionData = new TransmissionData(interval,nodeName,nodeId);
    expectedDataCount = 0;
	validDataCount = 0;

}

MSIAttacker::~MSIAttacker() { }

TransmissionData* MSIAttacker::getTransmissionData(){ return transmissionData; }

void MSIAttacker::AddData(float value) {

	inputRegisters.resize(validDataCount+1);
	if(packetsSent < 400)
		inputRegisters[validDataCount] = value;
	else
		inputRegisters[validDataCount] = value/10;
	validDataCount++;
	
	transmissionData->addTransmissionData(Simulator::Now().GetSeconds(),value);
	
}

void MSIAttacker::RecvInputData() {

	expectedDataCount++;
	if(expectedDataCount > validDataCount){
		inputRegisters.push_back(0);		
	}

	ScheduleNextDataInput();
}

void MSIAttacker::ScheduleNextDataInput() {

    dataInputEvent = Simulator::Schedule(Seconds(m_interval),		
                            &MSIAttacker::RecvInputData,
                            this);
}


void MSIAttacker::StartApplication (void) {

	ScadaApplication::StartApplication();

	m_socket_send->Bind();
	m_socket_send->Connect(server);

}


void MSIAttacker::StopApplication (void)
{
	ScadaApplication::StopApplication();
	m_socket_send->Close();
}


void MSIAttacker::Send(std::string message) {

	std::string reply = messageProtocol->createReply(message,inputRegisters,m_nodeId);
	
	m_data = (uint8_t*) reply.c_str();
	packetSize=reply.length();
	
	Ptr<Packet> p =Create<Packet>(m_data, packetSize);
	
	int a = m_socket_send->Send(p);

	//std::cout << "a: " << a << std::endl;

	Address addr;
	m_socket_send->GetSockName (addr);
	
	/*
	std::cout<<"(MSIAttacker) At time " 	<< Simulator::Now ().GetSeconds ()
		<< "s MSIAttacker installed in "	<< m_nodeName	<< " " << (InetSocketAddress::ConvertFrom (server)).GetIpv4()
		<< " sent "  	<<  p->GetSize () 
		<< " bytes to " << targetName
		<< std::endl;
	*/
	packetsSent++;
}


void MSIAttacker::HandleRead(Ptr<Socket> socket)
{
	ScadaApplication::HandleRead(socket);

	Address addr;
	socket->GetSockName(addr);

	/*std::cout << "(MSIAttacker) At time " << Simulator::Now().GetSeconds() << "s MSIAttacker installed in " << m_nodeName << " " 
		<< (InetSocketAddress::ConvertFrom (addr)).GetIpv4() << " received a request! " << std::endl;
	*/
	if(packetDataSize > 0) {
		Send(std::string((char*)packetData));
	}
    
}


}
