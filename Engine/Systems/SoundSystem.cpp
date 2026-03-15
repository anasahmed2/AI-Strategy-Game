#include "SoundSystem.h"
#include <iostream>

SoundSystem::SoundSystem() {
    try {
        addSound("select",  880.0f, 0.07f, 0.18f);
        addSound("move",    660.0f, 0.10f, 0.18f);
        addSound("attack",  200.0f, 0.14f, 0.28f);
        addSound("produce", 440.0f, 0.22f, 0.22f);
        addSound("gather",  550.0f, 0.08f, 0.15f);
        audioReady = true;
    } catch (...) {
        // Audio not available in this environment — sounds silently disabled.
        audioReady = false;
    }
}

std::vector<sf::Int16> SoundSystem::generateTone(float freqHz, float durationSec,
                                                  float amplitude) {
    const int sampleRate  = 44100;
    int       numSamples  = static_cast<int>(sampleRate * durationSec);
    float     fadeSamples = sampleRate * 0.01f; // 10 ms fade-in/out
    std::vector<sf::Int16> samples(numSamples);
    for (int i = 0; i < numSamples; ++i) {
        float t   = static_cast<float>(i) / sampleRate;
        float env = 1.0f;
        if (i < fadeSamples)
            env = static_cast<float>(i) / fadeSamples;
        else if (i > numSamples - fadeSamples)
            env = static_cast<float>(numSamples - i) / fadeSamples;
        samples[i] = static_cast<sf::Int16>(
            amplitude * 32767.0f * env *
            std::sin(2.0f * static_cast<float>(M_PI) * freqHz * t));
    }
    return samples;
}

void SoundSystem::addSound(const std::string& name, float freqHz,
                            float durationSec, float amplitude) {
    auto samples = generateTone(freqHz, durationSec, amplitude);
    sf::SoundBuffer buf;
    if (buf.loadFromSamples(samples.data(), samples.size(), 1, 44100)) {
        buffers[name] = std::move(buf);
        sounds[name].setBuffer(buffers[name]);
    }
}

void SoundSystem::play(const std::string& name) {
    if (!audioReady) return;
    auto it = sounds.find(name);
    if (it != sounds.end() &&
        it->second.getStatus() != sf::Sound::Playing) {
        it->second.play();
    }
}

void SoundSystem::playSelect()  { play("select");  }
void SoundSystem::playMove()    { play("move");    }
void SoundSystem::playAttack()  { play("attack");  }
void SoundSystem::playProduce() { play("produce"); }
void SoundSystem::playGather()  { play("gather");  }
