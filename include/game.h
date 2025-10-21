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
GameAction game_keyboard(unsigned char key, int x, int y); // Para 'q'
void game_mouse_motion(int x, int y);                      // Para mirar
void game_mouse_click(int button, int state);              // Para atirar/soltar gancho

#endif // GAME_H