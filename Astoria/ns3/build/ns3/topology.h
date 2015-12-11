#ifndef TOPOLOGY_HELPER_H
#define TOPOLOGY_HELPER_H

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include <../include/rapidxml/rapidxml.hpp>
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/graph_utility.hpp"
#include "boost/graph/graph_traits.hpp"

namespace ns3{

class node_ns3 {
	public:
			node_ns3();
			std::string nodeName;
			uint32_t nodeId;
			std::string type;
};

class edge_ns3 {
	public:
			edge_ns3();
			uint32_t srcId;
			uint32_t targetId;
			uint32_t edgeWeight;
};

struct Vertex {
  uint32_t id;
  std::string name;
  std::string type;
};
struct Edge {
  uint32_t distance;
};

typedef boost::adjacency_list<boost::setS, boost::listS, boost::undirectedS,
    	Vertex,Edge> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
typedef boost::graph_traits<Graph>::edge_iterator edge_iter;


class Topology {
  public:
    Topology();
    void generateTopology(std::string topoFile);
    std::string getNodeNameById(uint32_t nodeId);
    std::string getNodeTypeById(uint32_t nodeId);
    std::vector<edge_ns3*> getEdgeList();
    std::map<uint32_t,node_ns3*> getNodeMap();
    std::vector<edge_ns3*> getEdgeVectorOne();
    std::list<edge_ns3*> getEdgeListOne();
    std::map<uint32_t,edge_ns3*> getEdgeMapOne();
    std::vector<node_ns3*> getNodeList();
    std::vector<node_ns3*> getNodeList2(std::vector<std::string> exceptTypes);
    uint32_t getNodeCountByType(std::string type);
    uint32_t getNodeId(std::string nodeName);

  private:
    std::vector<vertex_t> vertexList;
    std::vector<edge_t> edgeList;
    Graph topology;
    std::string connectionType;

    std::string topoFileName;

	  rapidxml::xml_node<>* readTopoFile();

    vertex_t findNodeById(uint32_t nodeId);
    vertex_t getVertexByName(std::string vertexName);
    void createTopologyNodes();
	  void createTopologyEdges();
    void createTopologyEdgesSCADA();
    void createTopologyNodesSCADA();
	  void setConnectionType();

};
}
#endif
