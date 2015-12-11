#include "topology.h"

namespace ns3{

node_ns3::node_ns3(){}
edge_ns3::edge_ns3(){}

Topology::Topology(){

}

void Topology::generateTopology(std::string topoFile) {

  setConnectionType();
  createTopologyNodes();
  createTopologyEdges();

}

std::vector<edge_ns3*> Topology::getEdgeList() {
  std::vector<edge_ns3*> edgeStructList;
  for(uint32_t i = 0; i < edgeList.size(); i++) {
  	edge_t graphEdge = edgeList.at(i);
    edge_ns3 *newEdge = new edge_ns3();
    newEdge->srcId = topology[boost::source(graphEdge,topology)].id;
    newEdge->targetId = topology[boost::target(graphEdge,topology)].id;
    newEdge->edgeWeight = topology[graphEdge].distance;
    edgeStructList.push_back(newEdge);
  }
  return edgeStructList;
}


std::map<uint32_t,edge_ns3*> Topology::getEdgeMapOne() {
  std::map<uint32_t,edge_ns3*> edgeStructList;
  bool equal;
  uint32_t srcId;
  uint32_t targetId;
  std::ofstream file;
  file.open("test.txt");

  uint32_t k = 0;
  for(uint32_t i = 0; i < edgeList.size(); i++) {
    
    edge_t graphEdge = edgeList.at(i);
    equal = false;

    srcId = topology[boost::source(graphEdge,topology)].id;
    targetId = topology[boost::target(graphEdge,topology)].id;
    for(std::map<uint32_t,edge_ns3*>::iterator it = edgeStructList.begin(); 
      it != edgeStructList.end(); ++it) {

      if(it->second->srcId == topology[boost::target(graphEdge,topology)].id && 
            it->second->targetId == topology[boost::source(graphEdge,topology)].id) {
        equal = true;
      }

      //if(this->getNodeTypeById(srcId) == "PV" || this->getNodeTypeById(targetId) == "PV")
      //  equal = true;

      //if(this->getNodeTypeById(srcId) == "tr" || this->getNodeTypeById(targetId) == "tr")
      //  equal = true;
    }

    if(!equal) {
      edge_ns3 *newEdge = new edge_ns3();
      newEdge->srcId = topology[boost::source(graphEdge,topology)].id;
      newEdge->targetId = topology[boost::target(graphEdge,topology)].id;
      newEdge->edgeWeight = topology[graphEdge].distance;
      edgeStructList.insert(std::pair<uint32_t,edge_ns3*>(newEdge->srcId+newEdge->targetId,newEdge));
    }
  }

  file.close();


  std::cout << "deu certo!" << std::endl;
  return edgeStructList;


}

std::list<edge_ns3*> Topology::getEdgeListOne() {
  
  std::vector<edge_ns3*> edgeStructList = this->getEdgeVectorOne();
  std::list<edge_ns3*> edgeList;


  for(int i = 0; i < edgeStructList.size(); i++)
    edgeList.push_back(edgeStructList[i]);

  return edgeList;

}

std::vector<edge_ns3*> Topology::getEdgeVectorOne() {
  std::vector<edge_ns3*> edgeStructList;
  bool equal;
  uint32_t srcId;
  uint32_t targetId;
  std::ofstream file;
  file.open("test.txt");

  for(uint32_t i = 0; i < edgeList.size(); i++) {
    
    edge_t graphEdge = edgeList.at(i);
    equal = false;

    srcId = topology[boost::source(graphEdge,topology)].id;
    targetId = topology[boost::target(graphEdge,topology)].id;
    
    for(uint32_t j = 0; j < edgeStructList.size(); j++) {

      if(edgeStructList[j]->srcId == topology[boost::target(graphEdge,topology)].id && 
            edgeStructList[j]->targetId == topology[boost::source(graphEdge,topology)].id) {
        equal = true;
      }

      //if(this->getNodeTypeById(srcId) == "PV" || this->getNodeTypeById(targetId) == "PV")
      //  equal = true;

      //if(this->getNodeTypeById(srcId) == "tr" || this->getNodeTypeById(targetId) == "tr")
      //  equal = true;
    }

    if(!equal) {
      edge_ns3 *newEdge = new edge_ns3();
      newEdge->srcId = topology[boost::source(graphEdge,topology)].id;
      newEdge->targetId = topology[boost::target(graphEdge,topology)].id;
      newEdge->edgeWeight = topology[graphEdge].distance;
      edgeStructList.push_back(newEdge);
    }
  
  }

  file.close();

  return edgeStructList;

}

/*
std::vector<edge_ns3*> Topology::getEdgeVectorOne() {
  std::vector<edge_ns3*> edgeStructList;
  bool equal;
  uint32_t srcId;
  uint32_t targetId;
  std::ofstream file;
  file.open("test.txt");

  for(uint32_t i = 0; i < edgeList.size(); i++) {
    
    edge_t graphEdge = edgeList.at(i);
    equal = false;

    srcId = topology[boost::source(graphEdge,topology)].id;
    targetId = topology[boost::target(graphEdge,topology)].id;

    for(uint32_t j = 0; j < edgeStructList.size(); j++) {

      if(edgeStructList[j]->srcId == topology[boost::target(graphEdge,topology)].id && 
            edgeStructList[j]->targetId == topology[boost::source(graphEdge,topology)].id) {
        equal = true;
      }

      if(this->getNodeTypeById(srcId) == "PV" || this->getNodeTypeById(targetId) == "PV")
        equal = true;

      if(this->getNodeTypeById(srcId) == "tr" || this->getNodeTypeById(targetId) == "tr")
        equal = true;
    }

    if(!equal) {
      edge_ns3 *newEdge = new edge_ns3();
      newEdge->srcId = topology[boost::source(graphEdge,topology)].id;
      newEdge->targetId = topology[boost::target(graphEdge,topology)].id;
      newEdge->edgeWeight = topology[graphEdge].distance;
      edgeStructList.push_back(newEdge);
    }
  
  }

  file.close();

  return edgeStructList;

}*/


std::vector<node_ns3*> Topology::getNodeList() {
  std::vector<node_ns3*> nodeList;
	for(vertex_t vertex : vertexList) {
		node_ns3* node = new node_ns3();
		node->nodeName = topology[vertex].name;
		node->nodeId = topology[vertex].id;		
		node->type = topology[vertex].type;
		nodeList.push_back(node);
	}
	
	return nodeList;
}

std::map<uint32_t,node_ns3*> Topology::getNodeMap() {
  std::map<uint32_t,node_ns3*> nodeMap;
  for(vertex_t vertex : vertexList) {
    node_ns3* node = new node_ns3();
    node->nodeName = topology[vertex].name;
    node->nodeId = topology[vertex].id;
    node->type = topology[vertex].type;
    nodeMap.insert(std::pair<uint32_t,node_ns3*>(node->nodeId,node));
  }
  
  return nodeMap;
}

std::vector<node_ns3*> Topology::getNodeList2(std::vector<std::string> exceptTypes) {
  std::vector<node_ns3*> nodeList;
  bool add;

  for(vertex_t vertex : vertexList) {

    add = false;

    for(int i = 0; i < exceptTypes.size(); i++)
      if(exceptTypes[i] == topology[vertex].type)
        add = true;

    if(!add) {
      node_ns3* node = new node_ns3();
      node->nodeName = topology[vertex].name;
      node->nodeId = topology[vertex].id;   
      node->type = topology[vertex].type;
      nodeList.push_back(node);
    }
      
  }
  
  return nodeList;
}

uint32_t Topology::getNodeId(std::string nodeName) {
	for(vertex_t vertex : vertexList) {
		if(topology[vertex].name == nodeName)
			return topology[vertex].id;
	}		
	return -1;
}

std::string Topology::getNodeNameById(uint32_t nodeId) {
  vertex_t node = findNodeById(nodeId);
  if(node != NULL) return topology[node].name;
  return "";
}

std::string Topology::getNodeTypeById(uint32_t nodeId){
  vertex_t node = findNodeById(nodeId);
  if(node != NULL) return topology[node].type;
  return "";
}

vertex_t Topology::findNodeById(uint32_t nodeId){
  for(vertex_t n : vertexList)
    if(topology[n].id == nodeId)
      return n;
  return NULL;
}

void Topology::setConnectionType(){

  rapidxml::xml_document<> topologyXML;
  std::ifstream file ("ns3topo.xml");

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content(buffer.str());
  topologyXML.parse<0>(&content[0]);

  file.close();

  rapidxml::xml_node<> *root;
  root = topologyXML.first_node();

  connectionType = root->first_attribute()->value();
  
}

void Topology::createTopologyNodesSCADA(){
  
  rapidxml::xml_node<> *root = readTopoFile();
  
  int nodeIndex = 0;
  bool push = true;

	for(rapidxml::xml_node<> *node = root->first_node(); node; node=node->next_sibling()){
    
    std::stringstream nodeName, nodeType;
  	nodeName << node->first_attribute()->value();
  	nodeType << node->first_attribute()->next_attribute()->value();
    
    if(nodeType.str() == "PV" || nodeType.str() == "tr") {
      push = false;
    }

    if(push) {
  		vertex_t a = boost::add_vertex(topology);
  		topology[a].name = nodeName.str();
  		topology[a].type = nodeType.str();
  		topology[a].id = nodeIndex;
  		vertexList.push_back(a);
  		
  		nodeIndex++;
    }

    push = true;

	}

  std::cout << "sai" << std::endl;

}

void Topology::createTopologyNodes() {

  rapidxml::xml_document<> topologyXML;
  std::ifstream file ("ns3topo.xml");

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content(buffer.str());
  topologyXML.parse<0>(&content[0]);

  file.close();

  rapidxml::xml_node<> *root;
  root = topologyXML.first_node();
  
  int nodeIndex = 0;
  bool push = true;

  for(rapidxml::xml_node<> *node = root->first_node(); node; node=node->next_sibling()){
    
    std::stringstream nodeName, nodeType;
    nodeName << node->first_attribute()->value();
    nodeType << node->first_attribute()->next_attribute()->value();
    
    vertex_t a = boost::add_vertex(topology);
    topology[a].name = nodeName.str();
    topology[a].type = nodeType.str();
    topology[a].id = nodeIndex;
    vertexList.push_back(a);
      
    nodeIndex++;

  }

}

void Topology::createTopologyEdgesSCADA(){
  rapidxml::xml_node<> *root = readTopoFile();

  for(rapidxml::xml_node<> *node = root->first_node(); node; node=node->next_sibling()){
    vertex_t srcNode = getVertexByName(node->first_attribute()->value());

    for(rapidxml::xml_node<> *edge = node->first_node(); edge; edge=edge->next_sibling()){

      vertex_t targetNode = getVertexByName(edge->first_attribute()->value());
      
      if(targetNode == NULL)
        std::cout << "targetNode null" << std::endl;

      if(srcNode == NULL)
        std::cout << "srcNode null" << std::endl;


      if(targetNode != NULL && srcNode != NULL) {
        edge_t newEdge = boost::add_edge(srcNode, targetNode, topology).first; // Erro aqui!
        uint32_t edgeWeight;
        edgeWeight =  std::atoi(edge->first_attribute()->next_attribute()->value());
        topology[newEdge].distance = edgeWeight;
        edgeList.push_back(newEdge);
      }

    }

  }
}

void Topology::createTopologyEdges(){
  
  rapidxml::xml_document<> topologyXML;
  std::ifstream file ("ns3topo.xml");

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content(buffer.str());
  topologyXML.parse<0>(&content[0]);

  file.close();

  rapidxml::xml_node<> *root;
  root = topologyXML.first_node();

  for(rapidxml::xml_node<> *node = root->first_node(); node; node=node->next_sibling()){

    //std::cout << "node: " << node->first_attribute()->value() << std::endl;
  //  std::cout << "to aqui opa 222 node: " << node << std::endl;
    //if(node == 0)
      //std::cout << "not found!" << std::endl;
//std::cout << "to aqui opa 333" << std::endl;
  //  std::cout << "edges 3: node size: " << node->first_attribute() << std::endl;
    
    vertex_t srcNode = getVertexByName(node->first_attribute()->value());
    
    //std::cout << "edges 4" << std::endl;
    for(rapidxml::xml_node<> *edge = node->first_node(); edge; edge=edge->next_sibling()){
      //std::cout << "edges 5 " << edge->first_attribute() << std::endl;
      //if(edge == NULL)
        //std::cout << "edge eh nulo!" << std::endl;
      //std::cout << "edges 5.1  " << std::endl;
      //std::cout << "edge: " << edge->first_attribute()->value() << std::endl;
      //if(edge != NULL)
      //  if(edge->first_attribute() == 0)
      //    std::cout << "first_attribute eh nulo!" << std::endl;
      //std::cout << "edges 5.2 " << std::endl;
     
 /*     if(edge != NULL)
      if(edge->first_attribute() != 0)
      if(edge->first_attribute()->value() == NULL)
        std::cout << "first_attribute_value eh nulo!" << std::endl;
      std::cout << "edges 5.3: " << edge << std::endl;
      vertex_t targetNode = NULL;
      std::cout << "edges to aqui: " << std::endl;
      if(edge != NULL) {
        std::cout << "edges to aqui2: " << std::endl;
        if(edge->first_attribute() != 0) {
          std::cout << "edges to aqui2.5: " << std::endl;
          if(edge->first_attribute()->value() == NULL)
            std::cout << "value eh nulo!" << std::endl;
          std::cout << "edges to aqui 2.75" << std::endl;
          std::cout << "edges to aqui3: strlen -> " << edge->first_attribute() << std::endl;
           std::cout << "edges to aqui3: strlen -> " << edge->first_attribute()->value_size() << std::endl;
          if(edge->first_attribute()->value_size() != 0 && edge->first_attribute()->value_size() < 30000000) {
*/
            //std::cout << "--last node: " << lastNode->first_attribute()->value() << std::endl;
            //std::cout << "--last edge: " << lastEdge->first_attribute()->value() << std::endl;
            vertex_t targetNode = getVertexByName(edge->first_attribute()->value());
            //std::cout << "edges to aqui5:" << std::endl;
/*          }
        }
      }*/


      /*std::cout << "edges 6" << std::endl;
      if(srcNode == NULL)
        std::cout << "srcNode eh nulo!" << std::endl;
      if(targetNode == NULL)
        std::cout << "targetNode eh nulo!" << std::endl;
*/

      //if(targetNode != NULL && srcNode != NULL) {
        //std::cout << "edges 6.5" << std::endl;
        edge_t newEdge = boost::add_edge(srcNode, targetNode, topology).first;
  //std::cout << "edges 7" << std::endl;
        uint32_t edgeWeight;
        //std::cout << "edges 8" << std::endl;
        edgeWeight =  std::atoi(edge->first_attribute()->next_attribute()->value());
  //std::cout << "edges 9" << std::endl;
        topology[newEdge].distance = edgeWeight;
  //std::cout << "edges 10" << std::endl;
        edgeList.push_back(newEdge);
        //std::cout << "edges 11" << std::endl;
      //}
    }
  }
}

uint32_t Topology::getNodeCountByType(std::string nodeType){
  uint32_t typeCount = 0;
  for(vertex_t vertex : vertexList)
    if(topology[vertex].type == nodeType)
      typeCount++;
  return typeCount;
}

vertex_t Topology::getVertexByName(std::string vertexName){

	for(vertex_t vertex : vertexList){
		if(topology[vertex].name == vertexName)
			return vertex;
	}
	return NULL;
}

rapidxml::xml_node<>* Topology::readTopoFile(){

  rapidxml::xml_document<> topologyXML;
	std::ifstream file ("ns3topo.xml");

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content(buffer.str());
	topologyXML.parse<0>(&content[0]);

	file.close();

  rapidxml::xml_node<> *root;
  root = topologyXML.first_node();

  return root;
}

}
