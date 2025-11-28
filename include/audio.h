#pragma once

#include <string>

namespace Audio {

    // Inicializa subsistema de áudio (SDL + SDL_mixer)
    bool init();

    // Libera tudo (chamar no final do programa)
    void shutdown();

    // Carrega música de fundo (ogg/mp3) com um id
    bool loadMusic(const std::string& id, const std::string& path);

    // Toca uma música carregada
    void playMusic(const std::string& id, int loops = -1); // -1 = loop infinito

    // Para a música atual
    void stopMusic();

    // Carrega efeito sonoro (wav) com um id
    bool loadSound(const std::string& id, const std::string& path);

    // Toca um efeito sonoro
    void playSound(const std::string& id, int loops = 0); // 0 = toca 1 vez

    // Volume global da música (0–128)
    void setMusicVolume(int volume);

    // Volume de um efeito específico (0–128)
    void setSoundVolume(const std::string& id, int volume);

}

void loadGameAudio();