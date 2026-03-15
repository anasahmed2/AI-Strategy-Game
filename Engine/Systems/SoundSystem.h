#pragma once

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Generates and plays simple procedural beep tones — no audio files needed.
class SoundSystem {
public:
    SoundSystem();

    void playSelect();   // unit selected
    void playMove();     // move order
    void playAttack();   // attack order
    void playProduce();  // unit produced
    void playGather();   // gather order / resource collected

private:
    static std::vector<sf::Int16> generateTone(float freqHz, float durationSec,
                                               float amplitude = 0.25f);
    void addSound(const std::string& name, float freqHz, float durationSec,
                  float amplitude = 0.25f);
    void play(const std::string& name);

    std::unordered_map<std::string, sf::SoundBuffer> buffers;
    std::unordered_map<std::string, sf::Sound>       sounds;
    bool audioReady = false;
};
