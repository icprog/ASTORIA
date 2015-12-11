#include "RTU.h"

namespace ns3 {

ModbusProtocol::ModbusProtocol() : ScadaProtocol(){

	headerSize = 7; // header has 7 bytes

	h_transactionId = 65535;
	h_protocolId = 0;	// Default 0
	h_length = 65535;
	h_unitId = 255;		// Default 255
	h_functionCode = 4;

	numPacketsSent = 0;//TODO acho q não precisa contar aqui mais
	//inputRegisters.push_back(0);//depois de sincronizar get do server, precisa tirar isso
}

std::string ModbusProtocol::createRequest(uint16_t functionCode, uint32_t param1 = 0){//TODO definir os outros parametros como vão funcionar

	std::stringstream messageData;
	std::stringstream message;

	std::stringstream aux;
	std::string test;

	uint32_t func;

	//std::cout << "param1: " << param1 << std::endl;

	messageData<<std::setfill('0')<<std::setw(8)<<std::hex<<param1; // Addr First Register
    messageData<<std::setfill('0')<<std::setw(4)<<std::hex<<1; // Num Registers--Precisa ser definido de fora ou padrão

 //   std::cout << "messageData size before: " << messageData.str().size() << std::endl;

	message << createHeader(h_transactionId, h_protocolId, messageData.str().size(), h_unitId, functionCode);

	message << messageData.str();

	//test = removeHeader(message.str());
	//test = messageData.str();
	
	//aux << test.substr(0,8);
 
	//aux >> std::hex >> std::setw(8) >> std::setfill('0') >> func;
	//std::cout << "aux: " << aux.str() << " size: " << aux.str().size() << std::endl;
	//std::cout << "messageData: " << messageData.str() << " size: " << messageData.str().size() << std::endl;
	//std::cout << "test size: " << test.size() << std::endl;
	//aux << test.substr(0	,4);
	//aux >> std::hex >> func;
	//std::cout << "param1 after: " << func << std::endl;

	++numPacketsSent;

	return message.str();	
}

std::string ModbusProtocol::createRequest2(uint16_t functionCode, uint32_t param1 = 0){//TODO definir os outros parametros como vão funcionar

	std::stringstream messageData;
	std::stringstream message;
	std::stringstream message2;

	std::stringstream aux;

	uint16_t func;

	message2 << createHeader(h_transactionId, h_protocolId, h_length, h_unitId, functionCode);

	//std::cout << "header: " << message2 << std::endl;

	//aux << removeHeader(message2.str());
	//aux >> std::hex >> std::setw(4) >> std::setfill('0') >> func;
	//std::cout << "header removed: " << aux.str() << " size: " << aux.str().size()  << " func: " << func << std::endl;

	
 	if(functionCode==4) {
		messageData<<std::setfill('0')<<std::setw(2)<<std::hex<<functionCode;//Read Holding Registers
		messageData<<std::setfill('0')<<std::setw(4)<<std::hex<<param1;//Addr First Register
		messageData<<std::setfill('0')<<std::setw(4)<<std::hex<<1;//Num Registers--Precisa ser definido de fora ou padrão
    }

    message2<<std::setfill('0')<<std::setw(4)<<std::hex<<param1;//Addr First Register

    uint32_t dataLength = messageData.str().length()+2;//messageData+unit identifier
	
    message<<std::setfill('0')<<std::setw(4)<<std::hex<<numPacketsSent; //Transaction Identifier
    message<<std::setfill('0')<<std::setw(4)<<std::hex<<0;//Protocol Identifier(0=Modbus)
    message<<std::setfill('0')<<std::setw(4)<<std::hex<<dataLength+2;//data+unit identifier
    message<<std::setfill('0')<<std::setw(2)<<std::hex<<0;//talvez remover e eliminar SubServer(unit identifier)
        
	message<<messageData.str();
	
	++numPacketsSent;
	
	return message.str();	
}

std::string ModbusProtocol::createHeader(uint16_t transactionId, uint16_t protocolId, uint16_t length, uint16_t unitId, uint16_t functionCode) {

	std::stringstream header;

	header<<std::setfill('0')<<std::setw(4)<<std::hex<<transactionId;
	header<<std::setfill('0')<<std::setw(4)<<std::hex<<protocolId;
	header<<std::setfill('0')<<std::setw(4)<<std::hex<<length;
	header<<std::setfill('0')<<std::setw(2)<<std::hex<<unitId;
	header<<std::setfill('0')<<std::setw(2)<<std::hex<<functionCode;

	return header.str();

}

std::string ModbusProtocol::removeHeader(std::string message) {

	std::stringstream dataStream;

	uint16_t transactionId;
	dataStream << message.substr(0,4);
	dataStream >> std::hex >> std::setw(4) >> std::setfill('0') >> transactionId;
	//std::cout << "transactionId decoded: ";
	//std::cout << transactionId << std::endl;
	h_transactionId = transactionId;

	uint16_t protocolId;
	dataStream.clear();
	dataStream << message.substr(4,4);
	dataStream >> std::hex >> std::setw(4) >> std::setfill('0') >> protocolId;
	//std::cout << "protocolId decoded: ";
	//std::cout << protocolId << std::endl;
	h_protocolId = protocolId;

	uint16_t length;
	dataStream.clear();
	dataStream << message.substr(8,4);
	dataStream >> std::hex >> std::setw(4) >> std::setfill('0') >> length;
	//std::cout << "length decoded: ";
	//std::cout << length << std::endl;
	h_length = length;

	uint16_t unitId;
	dataStream.clear();
	dataStream << message.substr(12,2);
	dataStream >> std::hex >> std::setw(2) >> std::setfill('0') >> unitId;
	//std::cout << "unitId decoded: ";
	//std::cout << unitId << std::endl;
	h_unitId = unitId;

	uint16_t functionCode;
	dataStream.clear();
	dataStream << message.substr(14,2);
	dataStream >> std::hex >> std::setw(2) >> std::setfill('0') >> functionCode;
	//std::cout << "functionCode decoded: ";
	//std::cout << dataStream.str() << "  " << std::endl;
	//std::cout << functionCode << std::endl;
	h_functionCode = functionCode;

	std::string messageData;
	messageData = message.substr(16,h_length);

	return messageData;

}


// FUNÇÃO SÓ RETORNA UM VETOR DE PARES DE INT32 E INT16
// PARA O CLIENTE (MTU -> *RTU OU RTU -> *SENSOR) É O PEDIDO (PRIMEIRO REG E NÚMERO DE REGS)
// PARA O SERVIDOR (RTU -> *MTU OU SENSOR -> *RTU) É UMA LISTA DE VALORES (ID DO REMETENTE E VALOR MEDIDO)
std::vector<std::pair<uint32_t,uint16_t>> ModbusProtocol::getMessageData(std::string message) {

	std::vector<std::pair<uint32_t,uint16_t>> dataValue;

	uint32_t id;
	uint16_t value;

	std::stringstream dataStream;
	dataStream << removeHeader(message);
	std::stringstream dataStream2;

	if((h_length != dataStream.str().size()) || (dataStream.str().size() == 0 || h_length == 0)) {
		std::cout << "Length is not right!" << std::endl;
		std::cout << "h_length: " << h_length << " | dataStream: " << dataStream.str().size() << std::endl;
		dataValue.push_back(std::make_pair(-1,-1));
		return dataValue;
	}

	//std::cout << "---Length: " << h_length << std::endl;


	for(int i = 0; i < h_length/12; i++) {
		dataStream2.clear();
		dataStream2 << dataStream.str().substr(12*i,8);
		dataStream2 >> std::hex >> id;
		dataStream2.clear();
		dataStream2 << dataStream.str().substr(12*i+8,4);
		dataStream2 >> std::hex >> value;
		dataValue.push_back(std::make_pair(id,value));
	}

	//std::cout << " getMessageData -> nodeId: " << dataValue.at(0).first << " replyData:" << dataValue.at(0).second << std::endl;
	//std::cout << "data message hex: " << dataStream.str() << std::endl;

	return dataValue;
}


std::vector<std::string> ModbusProtocol::createReply(std::string message, std::vector<float> registers, uint32_t nodeId, std::vector<std::pair<uint32_t, std::pair<std::string, uint16_t>>> extraData) {

	std::stringstream messageStream;
	std::string messageData;
	uint32_t messageLength;
	uint16_t functionCode;

	messageData = removeHeader(message);

	functionCode = h_functionCode;

	std::stringstream replyData;
	std::stringstream reply;
	std::stringstream replyStream;

	replyData<<std::setfill('0')<<std::setw(8)<<std::hex<<nodeId;		 // Node id

	switch(functionCode) {

	        case 01:
	                break;
	        case 02:
	                break;
	        case 03:
	                break;
	        case 04:
	        	replyData<<std::setfill('0')<<std::setw(4)<<std::hex<<getInputRegisters(messageData, registers);	// Measured value
	                break;
	        default:
	                break;
	}

	int begin = 0;
	int sensorPacketSize = 35;
	if(sensorPacketSize > extraData.size())
		sensorPacketSize = extraData.size();
	int end = begin + sensorPacketSize;


	std::vector<std::string> repliesVector;

	// Max bytes
	while(begin < extraData.size()) {
	


		for(int i = begin; i < end; i++) {

			replyData<<std::setfill('0')<<std::setw(8)<<std::hex<<extraData[i].first;	// ID
			replyData<<std::setfill('0')<<std::setw(4)<<std::hex<<extraData[i].second.second;	// VALUE

		}

		// Clear stringstream
		reply.str(std::string());
		reply.clear();

		//std::cout << "replyData length:  " << sizeof(replyData.str().size()) << std::endl;
		//std::cout << "char size:  " << sizeof(char) << std::endl;
		//std::cout << "reply data bytes:  " << replyData.str().size() * sizeof(char) << std::endl;
		//std::cout << "reply data bits:  " << replyData.str().size() * sizeof(char) * 8 << std::endl;

		reply << createHeader(h_transactionId, h_protocolId, replyData.str().size(), h_unitId, functionCode);

		//std::cout << "replyData length:  " << replyData.str().size() << std::endl;
		//std::cout << "h_length: " << h_length << std::endl;

		reply << replyData.str();

		//std::cout << "nodeId: " << nodeId << " replyData:" << replyData.str() << std::endl;
		//std::cout << "reply: " << reply.str() << std::endl;

	    ++numPacketsSent;

		repliesVector.push_back(reply.str());
		// Clear
		replyData.str(std::string());
		reply.clear();
		begin = end;
		end = begin + sensorPacketSize;
		if(end > extraData.size())
			end = extraData.size();

	}



    return repliesVector;

}


std::string ModbusProtocol::createReply(std::string message, std::vector<float> registers, uint32_t nodeId){

	std::stringstream messageStream;
	std::string messageData;
	uint32_t messageLength;
	uint16_t functionCode;
	
	messageData = removeHeader(message);
	
	functionCode = h_functionCode;

	std::stringstream replyData;
	std::stringstream replyStream;

	replyData<<std::setfill('0')<<std::setw(8)<<std::hex<<nodeId;		 // Node id

	switch(functionCode) {
	      
	        case 01:
	                break;
	        case 02:
	                break;
	        case 03:
	                break;
	        case 04:
	        	replyData<<std::setfill('0')<<std::setw(4)<<std::hex<<getInputRegisters(messageData, registers);	// Measured value
	                break;
	        default:
	                break;
	}

	std::stringstream reply;

	reply << createHeader(h_transactionId, h_protocolId, replyData.str().size(), h_unitId, functionCode);

	reply << replyData.str();

	//std::cout << "nodeId: " << nodeId << " replyData:" << replyData.str() << std::endl;
	//std::cout << "reply: " << reply.str() << std::endl;
        
    ++numPacketsSent;
    return reply.str();
}

std::string ModbusProtocol::getInputRegisters(std::string message, std::vector<float> registers){

	/*std::cout << "registers: ";
	for(int i = 0; i < registers.size(); i++)
		std::cout << registers[i] << " ";
	std::cout << std::endl;
	*/
	std::stringstream messageStream;

	uint32_t startIndex, numRegisters;
	messageStream.str(message.substr(0,8));
	messageStream>>std::hex>> startIndex;
	
	messageStream.str(message.substr(8,4));
	numRegisters = atoi(messageStream.str().c_str());


	std::stringstream registerInfo;
	if(registers.size() > 0){
		if(startIndex+numRegisters < registers.size()-1){
			for(uint32_t i = startIndex; i<startIndex+numRegisters;++i){
				registerInfo<<std::setfill('0')<<std::setw(2)<<std::hex<<registers[i];
			}
		}else{
			registerInfo<<std::setfill('0')<<std::setw(2)<<std::hex<<(uint16_t)registers[registers.size()-1];
		}
	}else{
		registerInfo<<std::setfill('0')<<std::setw(2)<<std::hex<<0;
	}
	
	return registerInfo.str();
}
uint16_t ModbusProtocol::getPacketLength() {
/*
	std::stringstream dataStream;

	std::string length;
	dataStream.clear();
	dataStream << message.substr(8,4);
	dataStream >> std::hex >> std::setw(4) >> std::setfill('0') >> length;
	//std::cout << "length decoded: ";
	//std::cout << length << std::endl;
	return length;
*/
	return h_length;
}
}

