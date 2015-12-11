
#include "DOSAttacker.h"

namespace ns3 {

bool incrementDOSStepSize = true;


DOSAttacker::DOSAttacker(Ptr<Socket> socket,const Address &l, const Address &t, uint32_t nodeId, std::string nodeName,uint32_t interval,std::string Protocol, std::string serverName, uint32_t attackStartTime):
Sensor(socket, l, t,  nodeId, nodeName, interval,Protocol, serverName)
{
    m_interval = interval;
    m_attack_wait_interval = attackStartTime;
    m_dos_interval = 30;
    server = t;
    m_socket_send=socket;//TODO teoricamente não precisa disso
    m_packetsGet=0;
    m_local=l;
    targetName = serverName;
    attackStarted = false;

    if(Protocol.compare("Modbus")==0)
    	messageProtocol = new ModbusProtocol();

    transmissionData = new TransmissionData(interval,nodeName,nodeId);
    expectedDataCount = 0;
	validDataCount = 0;


}

DOSAttacker::~DOSAttacker() { }

TransmissionData* DOSAttacker::getTransmissionData(){ return transmissionData; }


void DOSAttacker::ScheduleNextTx(uint32_t dos_interval) {

    DOSAttackEvent = Simulator::Schedule(MilliSeconds(dos_interval),		
                            &DOSAttacker::SendDOS,
                            this);

}

void DOSAttacker::ScheduleNextTxSeconds(uint32_t dos_interval) {


	std::cout << "Waiting for " << Seconds(dos_interval) << " seconds ..." << std::endl;

    DOSAttackEvent = Simulator::Schedule(Seconds(dos_interval),		
                            &DOSAttacker::SendDOS,
                            this);

}


void DOSAttacker::StartApplication (void) {

	ScadaApplication::StartApplication();


	ScheduleNextTxSeconds(m_attack_wait_interval);

	m_socket_send->Bind();
	m_socket_send->Connect(server);

}


void DOSAttacker::StopApplication (void)
{
	ScadaApplication::StopApplication();
	m_socket_send->Close();
}


void DOSAttacker::Send(std::string message) {

	if(attackStarted == false) {
		std::cout << "Attack started by " << m_nodeName << " at " << Simulator::Now().GetSeconds() << "s" << std::endl;
		attackStarted = true;
	}

	std::string reply = messageProtocol->createReply(message,inputRegisters,m_nodeId);
	
	m_data = (uint8_t*) reply.c_str();
	packetSize=reply.length();
	
	Ptr<Packet> p =Create<Packet>(m_data, packetSize);
	
	int a = m_socket_send->Send(p);

	//std::cout << "a: " << a << std::endl;

	Address addr;
	m_socket_send->GetSockName (addr);
	
	/*
	std::cout<<"(DOSAttacker) At time " 	<< Simulator::Now ().GetSeconds ()
		<< "s DOSAttacker installed in "	<< m_nodeName	<< " " << (InetSocketAddress::ConvertFrom (server)).GetIpv4()
		<< " sent "  	<<  p->GetSize () 
		<< " bytes to " << targetName
		<< std::endl;
	*/
	packetsSent++;
}

void DOSAttacker::SendDOS() {

	std::stringstream message;
	std::stringstream messageData;

	messageData<<std::setfill('0')<<std::setw(8)<<std::hex<<1; // Addr First Register
    messageData<<std::setfill('0')<<std::setw(4)<<std::hex<<1; // Num Registers--Precisa ser definido de fora ou padrão

	message<<std::setfill('0')<<std::setw(4)<<std::hex<<65535;
	message<<std::setfill('0')<<std::setw(4)<<std::hex<<0;
	message<<std::setfill('0')<<std::setw(4)<<std::hex<<messageData.str().size();
	message<<std::setfill('0')<<std::setw(2)<<std::hex<<255;
	message<<std::setfill('0')<<std::setw(2)<<std::hex<<4;
	
	message << messageData.str();

	std::string reply = messageProtocol->createReply(message.str(),inputRegisters,m_nodeId);
	
	m_data = (uint8_t*) reply.c_str();
	packetSize=reply.length();
	
	Ptr<Packet> p =Create<Packet>(m_data, packetSize);
	
	int a = m_socket_send->Send(p);

	//std::cout << "a: " << a << std::endl;

	Address addr;
	m_socket_send->GetSockName (addr);
	
	/*
	std::cout<<"(DOSAttacker) At time " 	<< Simulator::Now ().GetSeconds ()
		<< "s DOSAttacker installed in "	<< m_nodeName	<< " " << (InetSocketAddress::ConvertFrom (server)).GetIpv4()
		<< " sent "  	<<  p->GetSize () 
		<< " bytes to " << targetName
		<< std::endl;
	*/
	packetsSent++;

	
	if(packetsSent%10 == 0)
		if(incrementDOSStepSize) {
		
			m_dos_interval += 2;
			
			if(m_dos_interval > 50)
				incrementDOSStepSize = false;

		}
		else {
		
			m_dos_interval -= 2;


			if(m_dos_interval < 4)
				incrementDOSStepSize = true;

		}
	
	ScheduleNextTx(m_dos_interval);

}


void DOSAttacker::HandleRead(Ptr<Socket> socket)
{
	ScadaApplication::HandleRead(socket);

	Address addr;
	socket->GetSockName(addr);

	/*std::cout << "(DOSAttacker) At time " << Simulator::Now().GetSeconds() << "s DOSAttacker installed in " << m_nodeName << " " 
		<< (InetSocketAddress::ConvertFrom (addr)).GetIpv4() << " received a request! " << std::endl;
	*/

	if(packetDataSize > 0) {
		Send(std::string((char*)packetData));
	}
    
}


}
