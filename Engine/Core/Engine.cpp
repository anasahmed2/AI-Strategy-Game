#include "Engine.h"

Engine::Engine(int windowWidth, int windowHeight, const std::string& title) {
    // Create window
    window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(windowWidth, windowHeight), title);
    window->setFramerateLimit(60);
    
    // Initialize registry
    registry = std::make_shared<ComponentRegistry>();
    
    // Initialize systems
    inputSystem = std::make_shared<InputSystem>();
    renderSystem = registry->registerSystem<RenderSystem>();
    physicsSystem = registry->registerSystem<PhysicsSystem>();
    resourceSystem = registry->registerSystem<ResourceSystem>();
    combatSystem = registry->registerSystem<CombatSystem>();
    aiSystem = registry->registerSystem<AISystem>();
    
    // Event system (standalone)
    eventSystem = std::make_shared<EventSystem>();
    
    // Pathfinding
    pathfinder = std::make_shared<Pathfinder>(
        windowWidth / 32, windowHeight / 32, 32.0f);
    
    // Connect render system to window
    renderSystem->setRenderTarget(window.get());
    
    // Reset clock
    clock.restart();
}

Engine::~Engine() {
    if (window) {
        window->close();
    }
}

bool Engine::isRunning() const {
    return window && window->isOpen();
}

void Engine::pollEvents() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
        }
        
        // Pass events to input system
        inputSystem->handleEvent(event);
    }
}

void Engine::update(float deltaTime) {
    // Update all systems
    registry->update(deltaTime);
}

void Engine::render() {
    window->clear(sf::Color(40, 40, 40));  // Dark gray background
    
    // Render all entities
    renderSystem->render();
    
    window->display();
}

float Engine::getDeltaTime() const {
    return deltaTime;
}
