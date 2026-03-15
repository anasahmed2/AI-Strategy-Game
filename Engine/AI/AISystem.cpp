#include "AISystem.h"
#include <limits>
#include <cstdint>

namespace {
class NullState final : public State {
public:
    explicit NullState(AIState type) : type(type) {}

    void enter() override {}
    void update(float) override {}
    void exit() override {}
    AIState getStateType() const override { return type; }

private:
    AIState type;
};

void initializeStateMachine(AIComponent& ai) {
    if (ai.stateMachineInitialized || !ai.stateMachine) {
        return;
    }

    ai.stateMachine->addState(AIState::Idle, std::make_shared<NullState>(AIState::Idle));
    ai.stateMachine->addState(AIState::Move, std::make_shared<NullState>(AIState::Move));
    ai.stateMachine->addState(AIState::Attack, std::make_shared<NullState>(AIState::Attack));
    ai.stateMachine->addState(AIState::Flee, std::make_shared<NullState>(AIState::Flee));
    ai.stateMachine->addState(AIState::Gather, std::make_shared<NullState>(AIState::Gather));
    ai.stateMachine->addState(AIState::Build, std::make_shared<NullState>(AIState::Build));
    ai.stateMachine->addState(AIState::Die, std::make_shared<NullState>(AIState::Die));

    ai.stateMachineInitialized = true;
}
}  // namespace

void AISystem::update(float deltaTime) {
    updateBlackboards();
    updateAIDecisions();

    for (auto& entity : entities) {
        auto ai = entity->getComponent<AIComponent>();
        if (!ai || !ai->stateMachine) {
            continue;
        }

        ai->stateMachine->update(deltaTime);
    }
}

void AISystem::setRequiredComponents() {
    require<TransformComponent>();
}

void AISystem::updateBlackboards() {
    for (const auto& entity : entities) {
        if (!entity || !entity->isActive() || entity->isDestroyed()) {
            continue;
        }

        auto ai = entity->getComponent<AIComponent>();
        auto transform = entity->getComponent<TransformComponent>();
        auto health = entity->getComponent<HealthComponent>();
        auto team = entity->getComponent<TeamComponent>();
        auto aiConfig = entity->getComponent<AIConfigComponent>();
        
        if (!ai || !transform || !team || !aiConfig || !aiConfig->enabled) {
            continue;
        }
        
        // Update basic info
        ai->blackboard.targetPosition = transform->position;
        if (health) {
            ai->blackboard.health = health->currentHealth;
            ai->blackboard.set("healthRatio", health->maxHealth > 0.0f ? health->currentHealth / health->maxHealth : 0.0f);
        }
        
        // Update sensory information
        updateSensory(entity);
    }
}

void AISystem::updateAIDecisions() {
    for (const auto& entity : entities) {
        if (!entity || !entity->isActive() || entity->isDestroyed()) {
            continue;
        }

        auto ai = entity->getComponent<AIComponent>();
        auto aiConfig = entity->getComponent<AIConfigComponent>();
        auto command = entity->getComponent<CommandComponent>();
        auto movement = entity->getComponent<MovementComponent>();
        auto path = entity->getComponent<PathComponent>();
        auto role = entity->getComponent<RoleComponent>();
        if (!ai || !aiConfig || !aiConfig->enabled || !command || !movement || !role) {
            continue;
        }

        initializeStateMachine(*ai);

        ai->blackboard.set("desiredAction", std::string());
        if (ai->behaviorTree) {
            ai->behaviorTree->update(ai->blackboard);
        }

        std::string desiredAction = ai->blackboard.get<std::string>("desiredAction");
        if (desiredAction.empty()) {
            float healthRatio = ai->blackboard.get<float>("healthRatio");
            if (healthRatio > 0.0f && healthRatio < aiConfig->retreatHealthThreshold && ai->blackboard.has("baseEntityId")) {
                desiredAction = "retreat";
            } else if (role->role == EntityRole::Worker) {
                desiredAction = ai->blackboard.resourceSpotted ? "gather" : "patrol";
            } else {
                desiredAction = ai->blackboard.enemySpotted ? "attack" : "patrol";
            }
        }

        if (desiredAction == "attack") {
            std::uint32_t enemyId = ai->blackboard.get<std::uint32_t>("enemyEntityId");
            command->type = CommandType::Attack;
            command->targetEntityId = enemyId;
            command->targetPosition = ai->blackboard.enemyPosition;
            movement->setTarget(ai->blackboard.enemyPosition);
            if (path) {
                path->waypoints = {ai->blackboard.enemyPosition};
                path->currentIndex = 0;
            }
            ai->stateMachine->changeState(AIState::Attack);
        } else if (desiredAction == "gather") {
            std::uint32_t nodeId = ai->blackboard.get<std::uint32_t>("resourceEntityId");
            command->type = CommandType::Gather;
            command->targetEntityId = nodeId;
            if (ai->blackboard.has("resourcePosition")) {
                Vector2 resourcePos = ai->blackboard.get<Vector2>("resourcePosition");
                command->targetPosition = resourcePos;
                movement->setTarget(resourcePos);
                if (path) {
                    path->waypoints = {resourcePos};
                    path->currentIndex = 0;
                }
            }
            ai->stateMachine->changeState(AIState::Gather);
        } else if (desiredAction == "retreat") {
            std::uint32_t baseId = ai->blackboard.get<std::uint32_t>("baseEntityId");
            command->type = CommandType::ReturnToBase;
            command->targetEntityId = baseId;
            if (ai->blackboard.has("basePosition")) {
                Vector2 basePos = ai->blackboard.get<Vector2>("basePosition");
                movement->setTarget(basePos);
                if (path) {
                    path->waypoints = {basePos};
                    path->currentIndex = 0;
                }
            }
            ai->stateMachine->changeState(AIState::Flee);
        } else {
            if (!movement->hasTarget) {
                int patrolStep = ai->blackboard.get<int>("patrolStep");
                patrolStep = (patrolStep + 1) % 4;
                ai->blackboard.set("patrolStep", patrolStep);

                float r = aiConfig->patrolRadius;
                Vector2 center = aiConfig->patrolCenter;
                Vector2 target = center;
                if (patrolStep == 0) target = Vector2(center.x + r, center.y);
                if (patrolStep == 1) target = Vector2(center.x, center.y + r);
                if (patrolStep == 2) target = Vector2(center.x - r, center.y);
                if (patrolStep == 3) target = Vector2(center.x, center.y - r);

                command->type = CommandType::Defend;
                command->targetPosition = target;
                command->defendPosition = target;
                movement->setTarget(target);
                if (path) {
                    path->waypoints = {target};
                    path->currentIndex = 0;
                }
            }

            ai->stateMachine->changeState(AIState::Move);
        }
    }
}

void AISystem::updateSensory(std::shared_ptr<Entity> entity) {
    auto ai = entity->getComponent<AIComponent>();
    auto transform = entity->getComponent<TransformComponent>();
    auto team = entity->getComponent<TeamComponent>();
    auto aiConfig = entity->getComponent<AIConfigComponent>();
    
    if (!ai || !transform || !team || !aiConfig) {
        return;
    }
    
    // Reset sensory data
    ai->blackboard.enemySpotted = false;
    ai->blackboard.resourceSpotted = false;
    ai->blackboard.set("enemyEntityId", static_cast<std::uint32_t>(0));
    ai->blackboard.set("resourceEntityId", static_cast<std::uint32_t>(0));
    ai->blackboard.set("baseEntityId", static_cast<std::uint32_t>(0));
    
    float bestEnemyDistance = aiConfig->engagementRange;
    float bestResourceDistance = std::numeric_limits<float>::max();
    float bestBaseDistance = std::numeric_limits<float>::max();

    for (const auto& candidate : entities) {
        if (!candidate || candidate->getId() == entity->getId() || !candidate->isActive() || candidate->isDestroyed()) {
            continue;
        }

        auto candidateTransform = candidate->getComponent<TransformComponent>();
        auto candidateTeam = candidate->getComponent<TeamComponent>();
        auto candidateRole = candidate->getComponent<RoleComponent>();
        if (!candidateTransform || !candidateTeam || !candidateRole) {
            continue;
        }

        float distance = transform->position.distance(candidateTransform->position);

        if (candidateTeam->faction != team->faction && candidateTeam->faction != Faction::Neutral) {
            if (distance < bestEnemyDistance) {
                bestEnemyDistance = distance;
                ai->blackboard.enemySpotted = true;
                ai->blackboard.enemyPosition = candidateTransform->position;
                ai->blackboard.set("enemyEntityId", candidate->getId());
            }
        }

        if (candidateRole->role == EntityRole::ResourceMine) {
            auto node = candidate->getComponent<ResourceNodeComponent>();
            if (node && node->amountRemaining > 0.0f && distance < bestResourceDistance) {
                bestResourceDistance = distance;
                ai->blackboard.resourceSpotted = true;
                ai->blackboard.set("resourceEntityId", candidate->getId());
                ai->blackboard.set("resourcePosition", candidateTransform->position);
            }
        }

        if (candidateRole->role == EntityRole::Base && candidateTeam->faction == team->faction) {
            if (distance < bestBaseDistance) {
                bestBaseDistance = distance;
                ai->blackboard.set("baseEntityId", candidate->getId());
                ai->blackboard.set("basePosition", candidateTransform->position);
            }
        }
    }
}
