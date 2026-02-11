#pragma once

#include <memory>
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
    std::shared_ptr<Entity> spawnWorker(Vector2 position);
    std::shared_ptr<Entity> spawnSoldier(Vector2 position);
    std::shared_ptr<Entity> spawnTank(Vector2 position);
    
    // Building spawning
    std::shared_ptr<Entity> spawnBase(Vector2 position);
    std::shared_ptr<Entity> spawnResourceMine(Vector2 position);
    std::shared_ptr<Entity> spawnTurret(Vector2 position);

private:
    std::shared_ptr<Engine> engine;
    
    // Game state
    void setupLevel();
    void setupAI();
};
