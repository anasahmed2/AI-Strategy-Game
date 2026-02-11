set(GAME_SOURCES
    main.cpp
    GameManager.cpp
    Units/Unit.cpp
    Units/Worker.cpp
    Units/Soldier.cpp
    Units/Tank.cpp
    Buildings/Building.cpp
    Buildings/Base.cpp
    Buildings/ResourceMine.cpp
    Buildings/Turret.cpp
)

set(GAME_HEADERS
    GameManager.h
    Units/Unit.h
    Units/Worker.h
    Units/Soldier.h
    Units/Tank.h
    Buildings/Building.h
    Buildings/Base.h
    Buildings/ResourceMine.h
    Buildings/Turret.h
)

add_executable(AIStrategyGame ${GAME_SOURCES} ${GAME_HEADERS})
target_link_libraries(AIStrategyGame PUBLIC Engine)
