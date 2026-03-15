#include "ResourceSystem.h"
#include <algorithm>
#include <limits>

namespace {
std::shared_ptr<Entity> findEntityById(const std::vector<std::shared_ptr<Entity>>& entities, std::uint32_t id) {
    for (const auto& entity : entities) {
        if (entity && entity->getId() == id && entity->isActive() && !entity->isDestroyed()) {
            return entity;
        }
    }
    return nullptr;
}

std::shared_ptr<Entity> findNearestResourceNode(const std::vector<std::shared_ptr<Entity>>& entities,
                                                const Vector2& from) {
    std::shared_ptr<Entity> best = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (const auto& candidate : entities) {
        if (!candidate || !candidate->isActive() || candidate->isDestroyed()) {
            continue;
        }

        auto node = candidate->getComponent<ResourceNodeComponent>();
        auto transform = candidate->getComponent<TransformComponent>();
        if (!node || !transform || node->amountRemaining <= 0.0f) {
            continue;
        }

        float d = transform->position.distance(from);
        if (d < bestDistance) {
            bestDistance = d;
            best = candidate;
        }
    }

    return best;
}

std::shared_ptr<Entity> findNearestBase(const std::vector<std::shared_ptr<Entity>>& entities,
                                        const Vector2& from,
                                        Faction faction) {
    std::shared_ptr<Entity> best = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (const auto& candidate : entities) {
        if (!candidate || !candidate->isActive() || candidate->isDestroyed()) {
            continue;
        }

        auto role = candidate->getComponent<RoleComponent>();
        auto team = candidate->getComponent<TeamComponent>();
        auto transform = candidate->getComponent<TransformComponent>();
        if (!role || !team || !transform || role->role != EntityRole::Base || team->faction != faction) {
            continue;
        }

        float d = transform->position.distance(from);
        if (d < bestDistance) {
            bestDistance = d;
            best = candidate;
        }
    }

    return best;
}
}  // namespace

void ResourceSystem::update(float deltaTime) {
    // Process worker gather/return loops.
    for (auto& entity : entities) {
        if (!entity || !entity->isActive() || entity->isDestroyed()) {
            continue;
        }

        auto collector = entity->getComponent<ResourceCollectorComponent>();
        auto command = entity->getComponent<CommandComponent>();
        auto transform = entity->getComponent<TransformComponent>();
        auto movement = entity->getComponent<MovementComponent>();
        auto team = entity->getComponent<TeamComponent>();
        auto role = entity->getComponent<RoleComponent>();
        
        if (!collector || !command || !transform || !movement || !team || !role) {
            continue;
        }

        if (role->role != EntityRole::Worker) {
            continue;
        }

        if (command->type == CommandType::Gather) {
            if (collector->carryAmount >= collector->carryCapacity) {
                command->type = CommandType::ReturnToBase;
                continue;
            }

            std::shared_ptr<Entity> nodeEntity = nullptr;
            if (command->targetEntityId != 0) {
                nodeEntity = findEntityById(entities, command->targetEntityId);
            }

            if (!nodeEntity) {
                nodeEntity = findNearestResourceNode(entities, transform->position);
                if (nodeEntity) {
                    command->targetEntityId = nodeEntity->getId();
                }
            }

            if (!nodeEntity) {
                continue;
            }

            auto nodeTransform = nodeEntity->getComponent<TransformComponent>();
            auto node = nodeEntity->getComponent<ResourceNodeComponent>();
            if (!node || !nodeTransform || node->amountRemaining <= 0.0f) {
                command->targetEntityId = 0;
                continue;
            }

            float distance = transform->position.distance(nodeTransform->position);
            if (distance > collector->gatherRange) {
                movement->setTarget(nodeTransform->position);
                continue;
            }

            float freeCapacity = std::max(0.0f, collector->carryCapacity - collector->carryAmount);
            float gathered = std::min({collector->collectionRate * deltaTime, freeCapacity, node->amountRemaining});
            collector->carryAmount += gathered;
            node->amountRemaining -= gathered;

            if (node->amountRemaining <= 0.0f) {
                node->amountRemaining = 0.0f;
                auto render = nodeEntity->getComponent<RenderComponent>();
                if (render) {
                    render->visible = false;
                }
                auto selection = nodeEntity->getComponent<SelectionComponent>();
                if (selection) {
                    selection->isSelectable = false;
                }
            }

            if (collector->carryAmount >= collector->carryCapacity || node->amountRemaining <= 0.0f) {
                command->type = CommandType::ReturnToBase;
                command->targetEntityId = 0;
            }
        } else if (command->type == CommandType::ReturnToBase) {
            std::shared_ptr<Entity> base = nullptr;
            if (command->targetEntityId != 0) {
                base = findEntityById(entities, command->targetEntityId);
            }

            if (!base) {
                base = findNearestBase(entities, transform->position, team->faction);
                if (base) {
                    command->targetEntityId = base->getId();
                }
            }

            if (!base) {
                continue;
            }

            auto baseTransform = base->getComponent<TransformComponent>();
            if (!baseTransform) {
                continue;
            }

            float distance = transform->position.distance(baseTransform->position);
            if (distance > collector->dropOffRange) {
                movement->setTarget(baseTransform->position);
                continue;
            }

            if (collector->carryAmount > 0.0f) {
                addResource(team->faction, collector->resourceType, collector->carryAmount);
                addResourceToEntity(base, collector->resourceType, collector->carryAmount);
                collector->carryAmount = 0.0f;
            }

            command->type = CommandType::Gather;
            command->targetEntityId = 0;
        }
    }
}

void ResourceSystem::setRequiredComponents() {
    // Keep empty to let this system inspect workers, bases, and resource nodes together.
}

void ResourceSystem::addResource(const std::string& resourceType, float amount) {
    if (globalResources.find(resourceType) == globalResources.end()) {
        globalResources[resourceType] = amount;
    } else {
        globalResources[resourceType] += amount;
    }
}

void ResourceSystem::removeResource(const std::string& resourceType, float amount) {
    if (globalResources.find(resourceType) != globalResources.end()) {
        globalResources[resourceType] = std::max(0.0f, globalResources[resourceType] - amount);
    }
}

float ResourceSystem::getResource(const std::string& resourceType) const {
    auto it = globalResources.find(resourceType);
    if (it != globalResources.end()) {
        return it->second;
    }
    return 0.0f;
}

void ResourceSystem::addResource(Faction faction, const std::string& resourceType, float amount) {
    if (amount <= 0.0f) {
        return;
    }

    teamResources[static_cast<int>(faction)][resourceType] += amount;
}

bool ResourceSystem::spendResource(Faction faction, const std::string& resourceType, float amount) {
    if (amount <= 0.0f) {
        return true;
    }

    auto& bank = teamResources[static_cast<int>(faction)][resourceType];
    if (bank < amount) {
        return false;
    }

    bank -= amount;
    return true;
}

float ResourceSystem::getResource(Faction faction, const std::string& resourceType) const {
    auto factionIt = teamResources.find(static_cast<int>(faction));
    if (factionIt == teamResources.end()) {
        return 0.0f;
    }

    auto resourceIt = factionIt->second.find(resourceType);
    if (resourceIt == factionIt->second.end()) {
        return 0.0f;
    }

    return resourceIt->second;
}

void ResourceSystem::addResourceToEntity(std::shared_ptr<Entity> entity, 
                                        const std::string& resourceType, float amount) {
    auto container = entity->getComponent<ResourceContainerComponent>();
    if (!container) return;
    
    auto& resources = container->resources;
    if (resources.find(resourceType) == resources.end()) {
        resources[resourceType] = amount;
    } else {
        resources[resourceType] += amount;
    }
}

void ResourceSystem::removeResourceFromEntity(std::shared_ptr<Entity> entity, 
                                            const std::string& resourceType, float amount) {
    auto container = entity->getComponent<ResourceContainerComponent>();
    if (!container) return;
    
    auto& resources = container->resources;
    if (resources.find(resourceType) != resources.end()) {
        resources[resourceType] = std::max(0.0f, resources[resourceType] - amount);
    }
}
