#include <iostream>
#include <chrono>
#include "GameManager.h"

int main() {
    try {
        // Create game manager
        GameManager gameManager;
        gameManager.initialize();
        
        // Game loop
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        while (gameManager.isRunning()) {
            // Calculate delta time
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            
            // Cap delta time to prevent large jumps
            if (deltaTime > 0.033f) deltaTime = 0.033f;
            
            // Update
            gameManager.update(deltaTime);
            
            // Render
            gameManager.render();
        }
        
        gameManager.shutdown();
        
        std::cout << "Game closed successfully." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
