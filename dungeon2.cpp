#include <iostream>
#include <vector>
#include <random>
#include <string>
using namespace std;

struct Node {
    std::string id;
    //Vector of edges, with a connected node and a weight
    std::vector<std::pair<std::string, int>> neighbors;

    Node(const std::string& id) : id(id) {}
};

//TODO: add x,y values to dungeon nodes
class DungeonGenerator {
public:
    DungeonGenerator(int numNodes, float connectProbability) 
        : numNodes(numNodes), connectProbability(connectProbability)
    {
        // Create nodes
        for (int i = 0; i < numNodes; ++i) {
            nodes.emplace_back(std::to_string(i));
        }

        // Connect nodes randomly
        //TODO: start at central node, branch outward from there to create a contiguous dungeon
        //TODO: implement cycles 
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1);
        for (int i = 0; i < numNodes; ++i) {
            for (int j = i + 1; j < numNodes; ++j) {
                if (dis(gen) < connectProbability) {
                    float newFloat = dis(gen) * 100;
                    nodes[i].neighbors.push_back((nodes[j].id, (int)newFloat));
                    nodes[j].neighbors.push_back((nodes[i].id, (int)newFloat));
                }
            }
        }
    }

    std::vector<Node>& getNodes() {
        return nodes;
    }

private:
    int numNodes;
    float connectProbability;
    std::vector<Node> nodes;
};

int main() {
    //TODO: add a printing of all of the weights
    DungeonGenerator dungeon(100, 0.5f);

    std::vector<Node>& nodes = dungeon.getNodes();
    for (Node& node : nodes) {
        std::cout << "Node " << node.id << " neighbors: ";
        for (const std::string& neighbor : node.neighbors) {
            std::cout << neighbor << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
