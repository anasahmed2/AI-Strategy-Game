#include "Pathfinder.h"
#include <algorithm>
#include <cmath>

Pathfinder::Pathfinder(int gridWidth, int gridHeight, float cellSize)
    : gridWidth(gridWidth), gridHeight(gridHeight), cellSize(cellSize) {
    obstacles.resize(gridHeight, std::vector<bool>(gridWidth, false));
}

std::vector<Vector2> Pathfinder::findPath(Vector2 start, Vector2 goal) {
    std::vector<Vector2> path;
    
    Vector2 startGrid = worldToGrid(start);
    Vector2 goalGrid = worldToGrid(goal);
    
    int startX = static_cast<int>(startGrid.x);
    int startY = static_cast<int>(startGrid.y);
    int goalX = static_cast<int>(goalGrid.x);
    int goalY = static_cast<int>(goalGrid.y);
    
    // Bounds checking
    if (startX < 0 || startX >= gridWidth || startY < 0 || startY >= gridHeight ||
        goalX < 0 || goalX >= gridWidth || goalY < 0 || goalY >= gridHeight) {
        path.push_back(goal);
        return path;
    }
    
    // OpenList priority queue
    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> openList;
    std::unordered_map<int, std::shared_ptr<PathNode>> closedList;
    
    // Start node
    PathNode startNode;
    startNode.position = startGrid;
    startNode.gCost = 0.0f;
    startNode.hCost = heuristic(startGrid, goalGrid);
    startNode.fCost = startNode.hCost;
    
    openList.push(startNode);
    
    while (!openList.empty()) {
        PathNode current = openList.top();
        openList.pop();
        
        int currentX = static_cast<int>(current.position.x);
        int currentY = static_cast<int>(current.position.y);
        
        // Goal reached
        if (currentX == goalX && currentY == goalY) {
            // Reconstruct path
            auto node = current.parent;
            while (node) {
                path.push_back(gridToWorld(static_cast<int>(node->position.x), 
                                          static_cast<int>(node->position.y)));
                node = node->parent;
            }
            std::reverse(path.begin(), path.end());
            path.push_back(goal);
            return path;
        }
        
        // Get neighbors
        auto neighbors = getNeighbors(currentX, currentY);
        for (auto [neighborX, neighborY] : neighbors) {
            if (obstacles[neighborY][neighborX]) continue;  // Skip obstacles
            
            Vector2 neighborPos(static_cast<float>(neighborX), static_cast<float>(neighborY));
            float newGCost = current.gCost + 1.0f;  // Assume unit cost
            
            // Skip if already checked
            int key = neighborY * gridWidth + neighborX;
            if (closedList.find(key) != closedList.end()) continue;
            
            PathNode neighborNode;
            neighborNode.position = neighborPos;
            neighborNode.gCost = newGCost;
            neighborNode.hCost = heuristic(neighborPos, goalGrid);
            neighborNode.fCost = neighborNode.gCost + neighborNode.hCost;
            neighborNode.parent = std::make_shared<PathNode>(current);
            
            openList.push(neighborNode);
        }
        
        int key = currentY * gridWidth + currentX;
        closedList[key] = std::make_shared<PathNode>(current);
    }
    
    // No path found
    path.push_back(goal);
    return path;
}

void Pathfinder::setObstacle(int gridX, int gridY, bool isObstacle) {
    if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight) {
        obstacles[gridY][gridX] = isObstacle;
    }
}

void Pathfinder::clearGrid() {
    for (auto& row : obstacles) {
        std::fill(row.begin(), row.end(), false);
    }
}

int Pathfinder::getGridWidth() const {
    return gridWidth;
}

int Pathfinder::getGridHeight() const {
    return gridHeight;
}

Vector2 Pathfinder::worldToGrid(Vector2 worldPos) const {
    return Vector2(worldPos.x / cellSize, worldPos.y / cellSize);
}

Vector2 Pathfinder::gridToWorld(int gridX, int gridY) const {
    return Vector2(gridX * cellSize + cellSize / 2.0f, 
                   gridY * cellSize + cellSize / 2.0f);
}

float Pathfinder::heuristic(Vector2 from, Vector2 to) const {
    // Manhattan distance heuristic for grid-based pathfinding
    return std::abs(from.x - to.x) + std::abs(from.y - to.y);
}

std::vector<std::pair<int, int>> Pathfinder::getNeighbors(int gridX, int gridY) const {
    std::vector<std::pair<int, int>> neighbors;
    
    // 4 cardinal directions
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};
    
    for (int i = 0; i < 4; ++i) {
        int newX = gridX + dx[i];
        int newY = gridY + dy[i];
        
        if (newX >= 0 && newX < gridWidth && newY >= 0 && newY < gridHeight) {
            neighbors.push_back({newX, newY});
        }
    }
    
    return neighbors;
}
