#pragma once

#include <memory>
#include "../Engine/Core/Engine.h"
#include "../Engine/ECS/Entity.h"

class GameManager {
public:
    GameManager();
    ~GameManager();
    
    // Engine access
    std::shared_ptr<Engine> getEngine() { return engine; }
    
    // Game initialization
    void initialize();
    void shutdown();
    
    // Game loop
    void update(float deltaTime);
    void render();
    bool isRunning();
    
    // Unit spawning
    std::shared_ptr<Entity> spawnWorker(Vector2 position, Faction faction, bool isAIControlled);
    std::shared_ptr<Entity> spawnSoldier(Vector2 position, Faction faction, bool isAIControlled);
    std::shared_ptr<Entity> spawnTank(Vector2 position, Faction faction, bool isAIControlled);
    std::shared_ptr<Entity> spawnScout(Vector2 position, Faction faction, bool isAIControlled);
    
    // Building spawning
    std::shared_ptr<Entity> spawnBase(Vector2 position, Faction faction, bool isAIControlled);
    std::shared_ptr<Entity> spawnResourceMine(Vector2 position);
    std::shared_ptr<Entity> spawnTurret(Vector2 position, Faction faction, bool isAIControlled);
    std::shared_ptr<Entity> spawnObstacle(Vector2 position, Vector2 size);

private:
    enum class BuildMode {
        None,
        Turret,
        Base
    };

    std::shared_ptr<Engine> engine;
    BuildMode buildMode = BuildMode::None;
    float actionCooldown = 0.0f;
    float enemyProductionTimer = 0.0f;
    
    // Game state
    void setupLevel();
    void setupAI();
    void handlePlayerActions(float deltaTime);
    void handleProduction();
    void handleBuildingPlacement();
};
