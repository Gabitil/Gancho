#ifndef GAME_H
#define GAME_H

// Ações que o jogo pode reportar ao main.cpp
enum GameAction {
    GAME_ACTION_CONTINUE,
    GAME_ACTION_EXIT_TO_MENU,
    GAME_ACTION_LEVEL_WON,
    GAME_ACTION_LEVEL_LOST
};

// --- Funções de Interface do Jogo ---

void game_init();
void game_start_level(int level);
GameAction game_update(); // Lógica/Física
void game_display();      // Desenho
void game_reshape(int w, int h);
void game_mouse_motion(int x, int y);              // Para mirar
void game_mouse_click(int button, int state);      // Para atirar/soltar gancho

// NOVO: Funções para teclas pressionadas/soltas
GameAction game_key_down(unsigned char key, int x, int y);
void game_key_up(unsigned char key, int x, int y);
void game_special_down(int key, int x, int y);
void game_special_up(int key, int x, int y);

#endif // GAME_H