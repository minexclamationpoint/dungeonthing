#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <map>
#include <set>

using namespace std;
using namespace sf;

const int wallProb = 54; // size of grid
const int Nx = 100; // size of grid
const int Ny = 100; // size of grid
const int offset = 10;
const int dx[4] = { 1, 0, -1, 0 };
const int dy[4] = { 0, 1, 0, -1 };

struct Cell {
	int x, y;
	int weight = 0;
	bool wall = false;
	bool path = false;
	bool isStartCell = false;
	bool isEndCell = false;
	Cell* neighbors[4];
	//weighted distance for dijkstra
	int distance = INT_MAX;
	float f = FLT_MAX, g = FLT_MAX, h = FLT_MAX; // A* values
	Cell* parent = nullptr;
	void setNeighbors(vector<vector<Cell>>& grid, Cell* cell){
		for (int i = 0; i < 4; i++){
			int nx = cell->x + dx[i], ny = cell->y + dy[i];
			if (inBounds(nx, ny)) {
				neighbors[i] = &grid[nx][ny]
			}
		}
	}
};

struct Compare {
	bool operator() (const Cell* a, const Cell* b) const {
		return a->f > b->f;
	}
};

struct compareDist {
	bool operator() (const Cell* a, const Cell* b) const {
		return a->distance > b->distance;
	}
};

void drawGrid(vector<vector<Cell>>& grid, RenderWindow& window) {
	for (int i = 0; i < Nx; i++) {
		for (int j = 0; j < Ny; j++) {
			RectangleShape rect(Vector2f(offset, offset));
			rect.setOutlineThickness(1);
			rect.setOutlineColor(Color::Black);
			if (grid[i][j].isStartCell || grid[i][j].isEndCell) {
				rect.setFillColor(Color::Green);
			}
			else if (grid[i][j].wall) {
				rect.setFillColor(Color::Black);
			}
			else if (grid[i][j].path) {
				rect.setFillColor(Color::Cyan);
			}
			else {
				//for regular cells (not wall, start/end, or part of the path
				//create a gradient from white to red, red being cells with highest weight
				Color color(255, 255, 255);
				color.b -= grid[i][j].weight*20;
				color.g -= grid[i][j].weight*20;
				rect.setFillColor(color);
			}
			rect.setPosition(j * offset, i * offset);
			window.draw(rect);
		}
	}
}

/*
void drawPath(Cell* end, RenderWindow& window) {
	VertexArray lines(LinesStrip, 0);
	while (end != nullptr) {
		Vertex v(Vector2f(end->y * offset + 15, end->x * offset + 15), Color::Red);
		lines.append(v);
		end = end->parent;
	}
	window.draw(lines);
}
*/

float heuristic(Cell* a, Cell* b) {
	return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

bool inBounds(int x, int y) {
	return x >= 0 && x < Nx&& y >= 0 && y < Ny;
}

vector<Cell*> getNeighbors(vector<vector<Cell>>& grid, Cell* cell) {
	vector<Cell*> neighbors;
	for (int i = 0; i < 4; i++) {
		int nx = cell->x + dx[i], ny = cell->y + dy[i];
		if (inBounds(nx, ny) && !grid[nx][ny].wall) {
			neighbors.push_back(&grid[nx][ny]);
		}
	}
	return neighbors;
}

//sets weight/"danger" of a cell to a random number between 1 and 10
void weightCells(vector<vector<Cell>>& grid) {
	for (int i = 0; i < Nx; i++) {
		for (int j = 0; j < Ny; j++) {
			grid[i][j].weight = rand() % 10 + 1;
		}
	}
}

//Dijkstra pathfinding algorithm
void Dijkstra(vector<vector<Cell>>& grid, Cell* start, Cell* goal) {
	//leftmost x coord is 1, uppermost y coord is 1
	//rightmost x coord is Nx-2, downmost y coord is Ny-2
	//the coordinates here work like [y][x]
	map<Cell*, bool> visited;
	priority_queue<Cell*, vector<Cell*>, compareDist> frontier;
	Cell* current;

	//set start node's distance to 0
	start->distance = 0;
	frontier.push(start);

	//set current node to start
	current = start;
	
	while (!frontier.empty()) {
		//find the cell with the smallest distance in the not done set
		//make that the current node, pop
		current = frontier.top();
		frontier.pop();
		visited[current] = true;
		if (current == goal) {
			break;
		}
		
		//relax all of the current node's edges
		//get all of the current node's neighbors
		current->setNeighbors(grid, current);
		for (Cell* next : current->neighbors) {
			if (next->distance > current->distance + next->weight)
			{
				//update the distance
				//have to do it for the notdone set as well
				next->distance = current->distance + next->weight;
				//update the predecessor
				next->parent = current;
				if (!visited[next]) {
					frontier.push(next);
				}
			}
		}
	}
	
	current = goal;
	while (current->parent) {
		current->path = true;
		current = current->parent;
	}
	
}

//A* algorithm :3
void AStar(vector<vector<Cell>>& grid, Cell* start, Cell* goal) {
	priority_queue<Cell*, vector<Cell*>, Compare> frontier;
	frontier.push(start);
	map<Cell*, bool> visited;

	Cell* current;
	while (!frontier.empty()) {
		current = frontier.top();
		frontier.pop();
		if (current == goal) {
			break;
		}
		visited[current] = true;
		current->setNeighbors(grid, current);
		for (Cell* next : current->neighbors) {
			float newCost = current->g + 1;
			if (newCost < next->g || !visited[next]) {
				next->g = newCost;
				next->h = heuristic(next, goal);
				next->f = next->g + next->h;
				next->parent = current;
				if (!visited[next]) {
					frontier.push(next);
					visited[next] = true;
				}
			}
		}
	}
	
	current = goal;
	while (current->parent) {
		current->path = true;
		current = current->parent;
	}
}

void createRandomWalls(vector<vector<Cell>>& grid, int wallProbability) {
	for (int i = 0; i < Nx; i++) {
		for (int j = 0; j < Ny; j++) {
			if (rand() % 100 < wallProbability) {
				grid[i][j].wall = true;
			}
		}
	}
}

void createRooms(vector<vector<Cell>>& grid, int wallProbability, Cell* startCell) {
	//sets the neighbors for startCell
	startCell->setNeighbors(grid, startCell);
	//iterates through the neighbors vector, recursively calls createRooms() and clears the next wall, if the random number is cleared
	for (Cell* next : startCell->neighbors) {
		if ((next) && (rand() % 100 + 1 < wallProbability) && (next->wall) ) {
			next->wall = false;
			createRooms(grid, wallProbability, next);
		}
	}
}

int main() {
	// create grid
	vector<vector<Cell>> grid(Nx, vector<Cell>(Ny));
	for (int i = 0; i < Nx; i++) {
		for (int j = 0; j < Ny; j++) {
			grid[i][j].x = i;
			grid[i][j].y = j;
			if (i == 0 || i == Nx - 1 || j == 0 || j == Ny - 1) {
				grid[i][j].wall = true;
			}
		}
	}

	//TODO: allow for input of end cell, make it so the graph renders first, then the algo runs
	// set start and goal
	Cell* start = &grid[1][1];
	Cell* goal = &grid[Nx - 2][Ny - 2];
	start->isStartCell = true;
	goal->isEndCell = true;
	//make sure they aren't walls <3
	start->wall = false;
	goal->wall = false;
	createRooms(grid, wallProb, start); // 40% chance of a cell being a wall
	weightCells(grid);
	
	cout << "Enter 1 for Dijkstra, 2 for A*, 3 for no path." << endl;
	int n;
	cin >> n;
	
	if(n==1) {
		//run Dijkstra
		Dijkstra(grid, start, goal);
	}
	else if(n==2) {
		//run A*
		AStar(grid, start, goal);
	}
	
	// draw grid and path
	RenderWindow window(VideoMode(Nx * offset, Ny * offset), "Shortest Path");
	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
		}
		window.clear(Color::White);
		drawGrid(grid, window);
		//drawPath(goal, window);
		window.display();
	}
	return 0;
}

//#include <iostream>
//#include <vector>
//#include <cstdlib>
//#include <ctime>
//
//struct Cell {
//    int x, y;
//    int costTravelTo = 0;
//    bool wall = false;
//    bool visited = false;
//    bool isStartCell = false;
//    bool isEndCell = false;
//    float f = 0, g = 0, h = 0; // A* values
//    Cell* parent = nullptr;
//};
//
//void printLayout(const std::vector<std::vector<Cell>>& layout) {
//    for (const auto& row : layout) {
//        for (const auto& cell : row) {
//            if (cell.wall) {
//                std::cout << "# ";
//            }
//            else if (cell.isStartCell) {
//                std::cout << "S ";
//            }
//            else if (cell.isEndCell) {
//                std::cout << "E ";
//            }
//            else {
//                std::cout << ". ";
//            }
//        }
//        std::cout << std::endl;
//    }
//}
//
//int main() {
//    const int numRows = 10;
//    const int numCols = 10;
//    std::vector<std::vector<Cell>> layout(numRows, std::vector<Cell>(numCols));
//
//    // Randomly assign costTravelTo values to cells
//    std::srand(std::time(nullptr));
//    for (auto& row : layout) {
//        for (auto& cell : row) {
//            cell.costTravelTo = std::rand() % 10 + 1;
//        }
//    }
//
//    // Set start and end cells
//    layout[0][0].isStartCell = true;
//    layout[numRows - 1][numCols - 1].isEndCell = true;
//
//    printLayout(layout);
//
//    return 0;
//}
//
//
//#include <iostream>
//#include <vector>
//#include <cstdlib>
//#include <ctime>
//
//struct Cell {
//    int x, y;
//    bool visited = false;
//    bool isWallCell = false;
//    bool isStartCell = false;
//    bool isEndCell = false;
//};
//
//void printLayout(const std::vector<std::vector<Cell>>& layout) {
//    for (const auto& row : layout) {
//        for (const auto& cell : row) {
//            if (cell.wall) {
//                std::cout << "# ";
//            }
//            else if (cell.isStartCell) {
//                std::cout << "S ";
//            }
//            else if (cell.isEndCell) {
//                std::cout << "E ";
//            }
//            else {
//                std::cout << ". ";
//            }
//        }
//        std::cout << std::endl;
//    }
//}
//
//void addObstacles(std::vector<std::vector<Cell>>& layout) {
//    const int numRows = layout.size();
//    const int numCols = layout[0].size();
//
//    // Add a vertical obstacle in the middle of the grid
//    for (int row = 0; row < numRows; ++row) {
//        layout[row][numCols / 2].wall = true;
//    }
//
//    // Add a horizontal obstacle in the middle of the grid
//    for (int col = 0; col < numCols; ++col) {
//        layout[numRows / 2][col].wall = true;
//    }
//}
//
//int main() {
//    const int numRows = 10;
//    const int numCols = 10;
//    std::vector<std::vector<Cell>> layout(numRows, std::vector<Cell>(numCols));
//
//    // Randomly assign costTravelTo values to cells
//    std::srand(std::time(nullptr));
//    for (auto& row : layout) {
//        for (auto& cell : row) {
//            cell.costTravelTo = std::rand() % 10 + 1;
//        }
//    }
//
//    // Set start and end cells
//    layout[0][0].isStartCell = true;
//    layout[numRows - 1][numCols - 1].isEndCell = true;
//
//    // Add obstacles
//    addObstacles(layout);
//
//    printLayout(layout);
//
//    return 0;
//}

//
//#include <iostream>
//#include <vector>
//#include <stack>
//#include <random>
//
//struct Cell {
//	int x, y;
//	bool visited = false;
//	bool isWallCell = false;
//	bool isStartCell = false;
//	bool isEndCell = false;
//};
//
//int main() {
//	// Define the size of the grid
//	const int Nx = 50;
//	const int Ny = 50;
//
//	// Create the grid of cells
//	std::vector<std::vector<Cell>> grid(Nx, std::vector<Cell>(Ny));
//	for (int i = 0; i < Nx; i++) {
//		for (int j = 0; j < Ny; j++) {
//			grid[i][j].x = i;
//			grid[i][j].y = j;
//		}
//	}
//
//	// Set the start and end cells
//	grid[0][0].isStartCell = true;
//	grid[Nx - 1][Ny - 1].isEndCell = true;
//
//	// Set some random wall cells
//	std::random_device rd;
//	std::mt19937 gen(rd());
//	std::uniform_int_distribution<> dis(0, 1);
//	for (int i = 0; i < Nx; i++) {
//		for (int j = 0; j < Ny; j++) {
//			if (dis(gen) == 1 && !(i == 0 && j == 0) && !(i == Nx - 1 && j == Ny - 1)) {
//				grid[i][j].isWallCell = true;
//			}
//		}
//	}
//
//	// Use depth-first search to ensure there is a path from start to end
//	std::stack<Cell> stack;
//	stack.push(grid[0][0]);
//	while (!stack.empty()) {
//		Cell current = stack.top();
//		stack.pop();
//		current.visited = true;
//		int x = current.x;
//		int y = current.y;
//		if (x == Nx - 1 && y == Ny - 1) {
//			break;
//		}
//		if (x > 0 && !grid[x - 1][y].visited && !grid[x - 1][y].isWallCell) {
//			stack.push(grid[x - 1][y]);
//		}
//		if (x < Nx - 1 && !grid[x + 1][y].visited && !grid[x + 1][y].isWallCell) {
//			stack.push(grid[x + 1][y]);
//		}
//		if (y > 0 && !grid[x][y - 1].visited && !grid[x][y - 1].isWallCell) {
//			stack.push(grid[x][y - 1]);
//		}
//		if (y < Ny - 1 && !grid[x][y + 1].visited && !grid[x][y + 1].isWallCell) {
//			stack.push(grid[x][y + 1]);
//		}
//	}
//
//	// Print the grid
//	for (int i = 0; i < Nx; i++) {
//		for (int j = 0; j < Ny; j++) {
//			if (grid[i][j].isStartCell) {
//				std::cout << "S ";
//			}
//			else if (grid[i][j].isEndCell) {
//				std::cout << "E ";
//			}
//			else if (grid[i][j].isWallCell) {
//				std::cout << "# ";
//			}
//			else {
//				std::cout << ". ";
//			}
//		}
//		std::cout << std::endl;
//
//		return 0;
//	}
//}

