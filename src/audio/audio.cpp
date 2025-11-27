// // src/audio/audio.cpp
// #include "audio.h"

// #include <SDL2/SDL.h>
// #include <SDL2/SDL_mixer.h>

// #include <unordered_map>
// #include <iostream>

// namespace {

//     std::unordered_map<std::string, Mix_Music*> g_musics;
//     std::unordered_map<std::string, Mix_Chunk*> g_sounds;
//     bool g_initialized = false;

// }

// namespace Audio {

//     bool init() {
//         if (g_initialized) return true;

//         // Inicializa só o subsistema de áudio
//         if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
//             std::cerr << "[Audio] Erro ao inicializar SDL audio: "
//                       << SDL_GetError() << std::endl;
//             return false;
//         }

//         // Abre o dispositivo de áudio
//         if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
//             std::cerr << "[Audio] Erro ao abrir áudio: "
//                       << Mix_GetError() << std::endl;
//             return false;
//         }

//         // Inicializa suporte a OGG/MP3 se precisar (opcional)
//         int flags = MIX_INIT_OGG | MIX_INIT_MP3;
//         int initted = Mix_Init(flags);
//         if ((initted & flags) != flags) {
//             std::cerr << "[Audio] Aviso: nem todos formatos foram iniciados: "
//                       << Mix_GetError() << std::endl;
//             // não dou return false pq ainda dá pra tocar WAV e o que tiver
//         }

//         g_initialized = true;
//         std::cout << "[Audio] SDL_mixer inicializado.\n";
//         return true;
//     }

//     void shutdown() {
//         if (!g_initialized) return;

//         // Libera todos os recursos carregados
//         for (auto& [id, mus] : g_musics) {
//             Mix_FreeMusic(mus);
//         }
//         g_musics.clear();

//         for (auto& [id, snd] : g_sounds) {
//             Mix_FreeChunk(snd);
//         }
//         g_sounds.clear();

//         Mix_CloseAudio();
//         Mix_Quit();
//         SDL_QuitSubSystem(SDL_INIT_AUDIO);

//         g_initialized = false;
//         std::cout << "[Audio] SDL_mixer finalizado.\n";
//     }

//     bool loadMusic(const std::string& id, const std::string& path) {
//         Mix_Music* music = Mix_LoadMUS(path.c_str());
//         if (!music) {
//             std::cerr << "[Audio] Erro ao carregar música " << path
//                       << ": " << Mix_GetError() << std::endl;
//             return false;
//         }
//         g_musics[id] = music;
//         return true;
//     }

//     void playMusic(const std::string& id, int loops) {
//         auto it = g_musics.find(id);
//         if (it == g_musics.end()) {
//             std::cerr << "[Audio] Música com id '" << id << "' não encontrada.\n";
//             return;
//         }
//         if (Mix_PlayMusic(it->second, loops) == -1) {
//             std::cerr << "[Audio] Erro ao tocar música: "
//                       << Mix_GetError() << std::endl;
//         }
//     }

//     void stopMusic() {
//         Mix_HaltMusic();
//     }

//     bool loadSound(const std::string& id, const std::string& path) {
//         Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
//         if (!chunk) {
//             std::cerr << "[Audio] Erro ao carregar som " << path
//                       << ": " << Mix_GetError() << std::endl;
//             return false;
//         }
//         g_sounds[id] = chunk;
//         return true;
//     }

//     void playSound(const std::string& id, int loops) {
//         auto it = g_sounds.find(id);
//         if (it == g_sounds.end()) {
//             std::cerr << "[Audio] Som com id '" << id << "' não encontrado.\n";
//             return;
//         }
//         if (Mix_PlayChannel(-1, it->second, loops) == -1) {
//             std::cerr << "[Audio] Erro ao tocar som: "
//                       << Mix_GetError() << std::endl;
//         }
//     }

//     void setMusicVolume(int volume) {
//         if (volume < 0) volume = 0;
//         if (volume > 128) volume = 128;
//         Mix_VolumeMusic(volume);
//     }

//     void setSoundVolume(const std::string& id, int volume) {
//         auto it = g_sounds.find(id);
//         if (it == g_sounds.end()) return;

//         if (volume < 0) volume = 0;
//         if (volume > 128) volume = 128;

//         Mix_VolumeChunk(it->second, volume);
//     }

// } // namespace Audio
