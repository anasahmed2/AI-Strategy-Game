set(ENGINE_SOURCES
    Core/Engine.cpp
    Math/Vector2.cpp
    ECS/Entity.cpp
    ECS/ComponentRegistry.cpp
    Systems/InputSystem.cpp
    Systems/RenderSystem.cpp
    Systems/PhysicsSystem.cpp
    Systems/ResourceSystem.cpp
    Systems/CombatSystem.cpp
    Systems/EventSystem.cpp
    AI/StateMachine.cpp
    AI/BehaviorTree.cpp
    AI/AISystem.cpp
    Pathfinding/Pathfinder.cpp
)

set(ENGINE_HEADERS
    Core/Engine.h
    Core/GameLoop.h
    Math/Vector2.h
    Math/Vector3.h
    ECS/ComponentRegistry.h
    ECS/Entity.h
    ECS/Component.h
    ECS/System.h
    Systems/InputSystem.h
    Systems/RenderSystem.h
    Systems/PhysicsSystem.h
    Systems/ResourceSystem.h
    Systems/CombatSystem.h
    Systems/EventSystem.h
    AI/StateMachine.h
    AI/BehaviorTree.h
    AI/Blackboard.h
    AI/AISystem.h
    Pathfinding/Pathfinder.h
)

add_library(Engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Engine PUBLIC sfml-graphics sfml-window sfml-system)
