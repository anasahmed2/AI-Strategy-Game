#include "GameManager.h"
#include <iostream>
#include "Units/Worker.h"
#include "Units/Soldier.h"
#include "Units/Tank.h"
#include "Buildings/Base.h"
#include "Buildings/ResourceMine.h"
#include "Buildings/Turret.h"

GameManager::GameManager() {
    // Create engine (800x600 window)
    engine = std::make_shared<Engine>(1200, 800, "AI Strategy Game");
}

GameManager::~GameManager() {
    shutdown();
}

void GameManager::initialize() {
    setupLevel();
    setupAI();
}

void GameManager::shutdown() {
    // Clean up handled by smart pointers
}

void GameManager::update(float deltaTime) {
    engine->pollEvents();
    engine->update(deltaTime);
}

void GameManager::render() {
    engine->render();
}

bool GameManager::isRunning() {
    return engine->isRunning();
}

std::shared_ptr<Entity> GameManager::spawnWorker(Vector2 position) {
    auto worker = std::make_shared<Worker>();
    return worker->create(engine, position);
}

std::shared_ptr<Entity> GameManager::spawnSoldier(Vector2 position) {
    auto soldier = std::make_shared<Soldier>();
    return soldier->create(engine, position);
}

std::shared_ptr<Entity> GameManager::spawnTank(Vector2 position) {
    auto tank = std::make_shared<Tank>();
    return tank->create(engine, position);
}

std::shared_ptr<Entity> GameManager::spawnBase(Vector2 position) {
    auto base = std::make_shared<Base>();
    return base->create(engine, position);
}

std::shared_ptr<Entity> GameManager::spawnResourceMine(Vector2 position) {
    auto mine = std::make_shared<ResourceMine>();
    return mine->create(engine, position);
}

std::shared_ptr<Entity> GameManager::spawnTurret(Vector2 position) {
    auto turret = std::make_shared<Turret>();
    return turret->create(engine, position);
}

void GameManager::setupLevel() {
    // Spawn player base
    spawnBase(Vector2(100.0f, 100.0f));
    
    // Spawn enemy base
    spawnBase(Vector2(1100.0f, 700.0f));
    
    // Spawn resources
    spawnResourceMine(Vector2(400.0f, 400.0f));
    spawnResourceMine(Vector2(800.0f, 300.0f));
    
    // Spawn initial units
    spawnWorker(Vector2(150.0f, 150.0f));
    spawnWorker(Vector2(200.0f, 150.0f));
    spawnSoldier(Vector2(250.0f, 150.0f));
}

void GameManager::setupAI() {
    // AI setup will be done per-unit
}
