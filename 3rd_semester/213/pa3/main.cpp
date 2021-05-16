#include "Graph.h"
#include <iostream>
/*You can add additional libraries here to read and process the dataset*/
#include <fstream> // file read/write

using namespace std;

// tester
int main() {

    /*// read input file
    ifstream inputF;

    // Metal trade Graph
    Graph graph;

    fstream file("world_trade.txt");

    string vertices;
    getline(file, vertices, '\t'); // skip header
    getline(file, vertices, '\n'); // number of vertices

    int numvertices = atoi( vertices.c_str() );
    HashTable<int, Node> nodeTable;

    // Build the graph from the txt input file
    for (int i = 0; i < numvertices; i++) {

        string vid, country, continent, gdp;
        getline(file, vid, '\t');
        getline(file, country, '\t');
        getline(file, continent, '\t');
        getline(file, gdp, '\n');

        Node node(atoi(vid.c_str()), country, continent, atol(gdp.c_str()));
        nodeTable.Insert(node.getVid(), node);

        graph.addNode(node);
    }
    getline(file, vertices, '\n'); // skip header

    // create connections
    string line;
    while (getline(file, line)) {
        int pos = line.find("\t");
        int vid1 = atoi(line.substr(0, pos).c_str());

        line = line.substr(pos+1);
        pos = line.find("\t");
        int vid2 = atoi(line.substr(0, pos).c_str());

        line = line.substr(pos+1);
        long import = atol(line.c_str());

        graph.addConnection(nodeTable.Get(vid1), nodeTable.Get(vid2), import);
    }

    file.close();
		
		
	// YOU CAN WRITE YOUR TEST CASES BELOW

    Node nodeSrc = nodeTable.Get(24);
    Node nodeDest = nodeTable.Get(29);

    cout << "\n\nnodeSrc: " << nodeSrc.getCountry() << " " << nodeSrc.getVid() << "\n";
    cout << "nodeDest: " << nodeDest.getCountry() << " " << nodeDest.getVid() << "\n";

    list<string> pathSrcDest = graph.findLeastCostPath(nodeSrc, nodeDest);
    list<string>::iterator itr = pathSrcDest.begin();
	
    cout << "findLeastCostPath(" << nodeSrc.getCountry() << ", " << nodeDest.getCountry() << ")\n\n";

    for (; itr != pathSrcDest.end(); itr++) {

        cout << *itr << " -> ";
    }

    cout << "\n\nisCyclic(): " << graph.isCyclic() << "\n";*/


    // CUSTOM GRAPH --------------------------------------------

    Graph customGraph;
    Node a(0, "a", "a", 0);
    Node b(0, "b", "b", 0);
    Node c(0, "c", "c", 0);
    Node d(0, "d", "d", 0);
    Node e(0, "e", "e", 0);

    customGraph.addNode(a);
    customGraph.addNode(b);
    customGraph.addNode(c);
    customGraph.addNode(d);
    customGraph.addNode(e);

    customGraph.addConnection(a, b, 3);
    //customGraph.addConnection(b, a, 3);
    customGraph.addConnection(b, c, 3);
    customGraph.addConnection(c, d, 3);
    customGraph.addConnection(d, e, 3);
    customGraph.addConnection(e, a, 3);
    //customGraph.addConnection(a, c, 2);
    //customGraph.addConnection(b, d, 1);
    //customGraph.addConnection(d, d, 3);

    // --------------------------------

    list<Node> gotAdjacentNodes = customGraph.getAdjacentNodes(e);
    list<Node>::iterator itt = gotAdjacentNodes.begin();

    for (; itt != gotAdjacentNodes.end(); itt++) {

        cout << itt->getCountry() << ", ";
    }

    cout << "\b\b  \b\b\n\n";


    list<string> foundLeastCostPath = customGraph.findLeastCostPath(e, e);
    list<string>::iterator ittt = foundLeastCostPath.begin();

    for (; ittt != foundLeastCostPath.end(); ittt++) {

        cout << *ittt << ", ";
    }

    cout << "\b\b  \b\b\n\n";


    list<string> gotBFSPath = customGraph.getBFSPath(e, e);
    list<string>::iterator itttt = gotBFSPath.begin();

    for (; itttt != gotBFSPath.end(); itttt++) {

        cout << *itttt << ", ";
    }

    cout << "\b\b  \b\b\n\n";


    cout << customGraph.isCyclic() << "\n";


    return 0;
}
