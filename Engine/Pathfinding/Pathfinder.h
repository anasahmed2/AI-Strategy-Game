#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>
#include "../Math/Vector2.h"

struct PathNode {
    Vector2 position;
    float gCost = 0.0f;  // Cost from start
    float hCost = 0.0f;  // Heuristic to goal
    float fCost = 0.0f;  // Total cost
    std::shared_ptr<PathNode> parent;
    
    bool operator>(const PathNode& other) const {
        return fCost > other.fCost;
    }
};

class Pathfinder {
public:
    Pathfinder(int gridWidth, int gridHeight, float cellSize);
    
    // Find path using A*
    std::vector<Vector2> findPath(Vector2 start, Vector2 goal);
    
    // Set obstacles in the grid
    void setObstacle(int gridX, int gridY, bool isObstacle);
    
    // Clear grid
    void clearGrid();
    
    // Get grid info
    int getGridWidth() const;
    int getGridHeight() const;

private:
    int gridWidth, gridHeight;
    float cellSize;
    std::vector<std::vector<bool>> obstacles;  // true = obstacle
    
    // Convert world coordinates to grid coordinates
    Vector2 worldToGrid(Vector2 worldPos) const;
    Vector2 gridToWorld(int gridX, int gridY) const;
    
    // Heuristic (Manhattan distance)
    float heuristic(Vector2 from, Vector2 to) const;
    
    // Get neighbors of a grid cell
    std::vector<std::pair<int, int>> getNeighbors(int gridX, int gridY) const;
};
