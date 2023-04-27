#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

const int wallProb = 54; 
const int Nx = 100; // size of grid
const int Ny = 100; // size of grid  
const int offset = 10;
const int dx[4] = { 1, 0, -1, 0 };
const int dy[4] = { 0, 1, 0, -1 };

bool inBounds(int x, int y) {
	return x >= 0 && x < Nx&& y >= 0 && y < Ny;
}

struct Cell {
	int x, y;
	int costTravelTo = 0;
	bool wall = true;
	bool path = false;
	bool visited = false;
	bool isStartCell = false;
	bool isEndCell = false;
	Cell* neighbors[4];
	float f = 0, g = 0, h = 0; // A* values
	Cell* parent = nullptr;
	//Old getNeighbors() function, updated to instead set the neighbors private instance variable. 
	//No constructor, the nodes just have connections upon being written
	void setNeighbors(vector<vector<Cell>>& grid, Cell* cell) {
		for (int i = 0; i < 4; i++) {
			int nx = cell->x + dx[i], ny = cell->y + dy[i];
			if (inBounds(nx, ny)) {
				neighbors[i] = &grid[nx][ny];
			}
		}
	}
};

struct Compare {
	bool operator() (const Cell* a, const Cell* b) const {
		return a->f > b->f;
	}
};

void drawGrid(vector<vector<Cell>>& grid, RenderWindow& window) {
	for (int i = 0; i < Nx; i++) {
		for (int j = 0; j < Ny; j++) {
			RectangleShape rect(Vector2f(offset, offset));
			rect.setOutlineThickness(1);
			rect.setOutlineColor(Color::Black);
			if (grid[i][j].wall) {
				rect.setFillColor(Color::Black);
			}else if(grid[i][j].path){
				rect.setFillColor(Color::Green);
			}else {
				rect.setFillColor(Color::White);
			}
			rect.setPosition(j * offset, i * offset);
			window.draw(rect);
		}
	}
}

vector<Cell*> getNeighbors(Cell* cell) {
	vector<Cell*> nVec;
	for (Cell* next : cell->neighbors) {
		if (next) {
			nVec.push_back(next);
		}
	}
	return nVec;
}

void drawPath(Cell* end, RenderWindow& window) {
	VertexArray lines(LinesStrip, 0);
	while (end != nullptr) {
		Vertex v(Vector2f(end->y * offset + 15, end->x * offset + 15), Color::Red);
		lines.append(v);
		end = end->parent;
	}
	window.draw(lines);
}

float heuristic(Cell* a, Cell* b) {
	return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

priority_queue<Cell*, vector<Cell*>, Compare> AStar(vector<vector<Cell>>& grid, Cell* start, Cell* goal) {
	priority_queue<Cell*, vector<Cell*>, Compare> frontier;
	frontier.push(start);

	while (!frontier.empty()) {
		Cell* current = frontier.top();
		frontier.pop();
		if (current == goal) {
			return frontier;
		}
			current->visited = true;
			current->setNeighbors(grid, current);
		for (Cell* next : current->neighbors) {
			float newCost = current->g + 1;
			if (newCost < next->g || !next->visited) {
				next->g = newCost;
				next->h = heuristic(next, goal);
				next->f = next->g + next->h;
				next->parent = current;
				if (!next->visited) {
					frontier.push(next);
					next->visited = true;
					
				}
			}
		}
	}
	return frontier;
}


// renamed, used to be createRandomWalls(). Generates a dungeon recursively from a starting point, called endCell
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
	// removed the border, as everything starts as a wall anyways
	vector<vector<Cell>> grid(Nx, vector<Cell>(Ny));
	for (int i = 0; i < Nx; i++) {
		for (int j = 0; j < Ny; j++) {
			grid[i][j].x = i;
			grid[i][j].y = j;
		}
	}

	// create walls
	//Messing with things, made [1][1] and [Ny][Nx] start and end to test createRooms
	grid[1][1].wall = false;
	grid[Ny-1][Nx-1].wall = false;
	grid[1][1].isStartCell = true;
	grid[Ny-1][Nx-1].isEndCell = true;

	// run A*
	Cell* start = &grid[1][1];
	Cell* goal = &grid[Ny-1][Nx-1];
	//I'm not sure what the precise probability of a cell being a room is
	//It might be logarithmic or something?
	createRooms(grid, wallProb, start);
	//priority_queue<Cell*, vector<Cell*>, Compare> frontier = AStar(grid, start, goal);

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
		drawPath(goal, window);
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

