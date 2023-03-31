#include <iostream>
#include <vector>
#include <random>
#include <string>
using namespace std;

struct Node {
    std::string id;
    std::vector<std::string> neighbors;

    Node(const std::string& id) : id(id) {}
};

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
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1);
        for (int i = 0; i < numNodes; ++i) {
            for (int j = i + 1; j < numNodes; ++j) {
                if (dis(gen) < connectProbability) {
                    nodes[i].neighbors.push_back(nodes[j].id);
                    nodes[j].neighbors.push_back(nodes[i].id);
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
