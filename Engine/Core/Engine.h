#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "../ECS/ComponentRegistry.h"
#include "../Systems/InputSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/PhysicsSystem.h"
#include "../Systems/ResourceSystem.h"
#include "../Systems/CombatSystem.h"
#include "../Systems/EventSystem.h"
#include "../Systems/SelectionSystem.h"
#include "../Systems/MovementSystem.h"
#include "../AI/AISystem.h"
#include "../Pathfinding/Pathfinder.h"

class Engine {
public:
    Engine(int windowWidth, int windowHeight, const std::string& title);
    ~Engine();
    
    // Core systems
    std::shared_ptr<ComponentRegistry> getRegistry() { return registry; }
    std::shared_ptr<InputSystem> getInputSystem() { return inputSystem; }
    std::shared_ptr<RenderSystem> getRenderSystem() { return renderSystem; }
    std::shared_ptr<PhysicsSystem> getPhysicsSystem() { return physicsSystem; }
    std::shared_ptr<ResourceSystem> getResourceSystem() { return resourceSystem; }
    std::shared_ptr<CombatSystem> getCombatSystem() { return combatSystem; }
    std::shared_ptr<EventSystem> getEventSystem() { return eventSystem; }
    std::shared_ptr<SelectionSystem> getSelectionSystem() { return selectionSystem; }
    std::shared_ptr<MovementSystem> getMovementSystem() { return movementSystem; }
    std::shared_ptr<AISystem> getAISystem() { return aiSystem; }
    std::shared_ptr<Pathfinder> getPathfinder() { return pathfinder; }
    
    // Window management
    sf::RenderWindow* getWindow() { return window.get(); }
    bool isRunning() const;
    void pollEvents();
    
    // Game loop
    void update(float deltaTime);
    void render();
    
    float getDeltaTime() const;

private:
    std::unique_ptr<sf::RenderWindow> window;
    
    // ECS
    std::shared_ptr<ComponentRegistry> registry;
    
    // Core Systems
    std::shared_ptr<InputSystem> inputSystem;
    std::shared_ptr<RenderSystem> renderSystem;
    std::shared_ptr<PhysicsSystem> physicsSystem;
    std::shared_ptr<ResourceSystem> resourceSystem;
    std::shared_ptr<CombatSystem> combatSystem;
    std::shared_ptr<EventSystem> eventSystem;
    std::shared_ptr<SelectionSystem> selectionSystem;
    std::shared_ptr<MovementSystem> movementSystem;
    std::shared_ptr<AISystem> aiSystem;
    
    // AI & Pathfinding
    std::shared_ptr<Pathfinder> pathfinder;
    
    // Timing
    sf::Clock clock;
    float deltaTime = 0.0f;
};
