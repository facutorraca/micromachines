//
// Created by javier on 17/11/19.
//

#ifndef MICROMACHINES_SOUNDSYSTEM_H
#define MICROMACHINES_SOUNDSYSTEM_H

#include <unordered_map>
#include <SDL2/SDL_mixer.h>

class SoundSystem {
    std::unordered_map<int32_t, Mix_Chunk*> sounds;
    Mix_Music* music;
    int32_t posx;
    int32_t posy;
    int32_t health;
public:
    SoundSystem();
    void center(int32_t x, int32_t y);
    void playBackgroundMusic();
    void pauseBackgroundMusic();
    void playEngineSound(int32_t id, int32_t x, int32_t y, int32_t vel);
    void playCountdownSound(int32_t number);
    void playEffectSound(int32_t effect);
    void playHealthChanged(int32_t health);
    ~SoundSystem();
};


#endif //MICROMACHINES_SOUNDSYSTEM_H
