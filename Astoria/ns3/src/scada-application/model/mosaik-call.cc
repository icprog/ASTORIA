#include "mosaik-call.h"

using namespace rapidjson;

namespace ns3 {

MosaikCall::MosaikCall(std::string messageString){
  std::string jsonMessage = removeHeader(messageString);
  message.Parse<0>(jsonMessage.c_str());

}

std::string MosaikCall::removeHeader(std::string message){

  return message.substr(HEADER_SIZE,std::string::npos);
}

uint32_t MosaikCall::getId(){

  return message[1].GetInt();
}

uint32_t MosaikCall::getType(){

  return message[0].GetInt();
}

std::string MosaikCall::getInstruction(){

	if(message.IsArray()){
	  return message[2][0].GetString();
  }
	  
	 //TODO ver o q tava dando errado aqui
}

std::vector<std::string> MosaikCall::getStdContent(){

  const Value& contentList =message[2][1];
  std::vector<std::string> content;

  for(Value::ConstValueIterator itr = contentList.Begin(); itr != contentList.End();++itr)
    content.push_back(itr->GetString());

  return content;

}

std::vector<std::pair<std::string,std::string> >MosaikCall::getInitData(){

  const Value& initDataList =message[2][2];
  std::vector<std::pair<std::string, std::string> > data;

  for(Value::ConstMemberIterator itr = initDataList.MemberBegin(); itr != initDataList.MemberEnd();++itr){
  	std::stringstream valueData;
  	if(itr->value.IsString()) valueData << itr->value.GetString(); else valueData << itr->value.GetInt();
    std::pair<std::string,std::string> newData = std::make_pair(itr->name.GetString(),valueData.str());
    data.push_back(newData);
  }

  return data;

}

std::vector<std::pair<std::string,float> > MosaikCall::getStepData(){

  const Value& dataList =message[2][1][1];
  std::vector<std::pair<std::string, float> > data;

  for(Value::ConstMemberIterator itr = dataList.MemberBegin(); itr != dataList.MemberEnd();++itr){
    std::pair<std::string,float> newData = std::make_pair(itr->name.GetString(),0);
    Value::ConstMemberIterator powerData = itr->value.MemberBegin();
    newData.second = powerData->value.MemberBegin()->value.GetDouble();
    
    data.push_back(newData);
  }

  return data;

}

std::string MosaikCall::createReply(std::string messageContent){

  uint32_t msgResult = SUCCESS; // Definir casos de FAILURE

  std::stringstream payload;
  payload << "["
            << msgResult << ", "
            << getId() << ", "
            << messageContent
          << "]";
          

  return writeMsgSize(payload.str());
}

std::string MosaikCall::createReply(std::vector<node_ns3*> messageContent){

  std::string createData = generateCreateData(messageContent);
   return createReply(createData);

}

std::string MosaikCall::generateCreateData(std::vector<node_ns3*> createData){

  std::stringstream createLog; //gerando info de elementos criados, para o mosaik fazer referencia
  createLog << "[{"
            << "\"eid\": \"Network_0\","
            << "\"type\": \"Network\","
            << "\"rel\": [],"
            << "\"children\": [";
  uint32_t createAmount = 0;
  for(node_ns3* node : createData){
  	createAmount++;
		if(node->type == "MTU"){
			createLog << "{\"eid\": \"mtu" 
		          << "\","
		          <<"\"type\": \"MTU\""
              << "}";
		} else {
			createLog << "{\"eid\": \"" 
		          << node->nodeName << "\","
		          <<"\"type\": \"RTU\""
              << "}";
    }
              
    if(createAmount < createData.size())
      createLog << ",";
  }
  createLog << "]}]";
  
  return createLog.str();
}

std::string MosaikCall::writeMsgSize(std::string messagePayload){

	uint64_t bytes[HEADER_SIZE];
	std::stringstream messageHeader;
	uint32_t payloadSize = messagePayload.length();
	
	for(uint32_t i = 0; i<HEADER_SIZE-1; ++i){
		bytes[i] = payloadSize / (uint64_t)pow(2,(HEADER_SIZE-i-1)*8);
		payloadSize = payloadSize % (uint64_t)pow(2,(HEADER_SIZE-i-1)*8);
		messageHeader<<(char)bytes[i];
	}
	
	bytes[HEADER_SIZE-1]=payloadSize;
	messageHeader<<(char)bytes[HEADER_SIZE-1];
		

  std::stringstream fullMsg;
  fullMsg << messageHeader.str() << messagePayload;

  return fullMsg.str();
}

}

