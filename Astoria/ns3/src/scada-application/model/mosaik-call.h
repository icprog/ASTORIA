
#ifndef MOSAIK_CALL_H
#define MOSAIK_CALL_H


#include "include/rapidjson/document.h"
#include <string>
#include <vector>
#include <stdint.h>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <iostream>

#include "topology.h"

using namespace rapidjson;

namespace ns3{

const int SUCCESS = 1;
const int FAILURE = 2;
const int HEADER_SIZE = 4;


class MosaikCall {

public:
	MosaikCall(std::string messageString);
	uint32_t getType();
	uint32_t getId();
	std::string getInstruction();
	std::vector<std::string> getStdContent();
	std::vector<std::pair<std::string,float> >getStepData();
	std::vector<std::pair<std::string,std::string> >getInitData();

	std::string createReply(std::string messageContent);
	std::string createReply(std::vector<node_ns3*> messageContent); //Para o create

private:
	Document message;
  	std::string removeHeader(std::string message);
	std::string generateCreateData(std::vector<node_ns3*> createData);
	std::string writeMsgSize(std::string messagePayload);


};

}



#endif
