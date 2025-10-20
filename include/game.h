#ifndef GAME_H
#define GAME_H

// Ações que o jogo pode reportar ao main.cpp
enum GameAction {
    GAME_ACTION_CONTINUE,
    GAME_ACTION_EXIT_TO_MENU,
    GAME_ACTION_LEVEL_WON,
    GAME_ACTION_LEVEL_LOST // (Podemos usar no futuro)
};

// --- Funções de Interface do Jogo ---

// Chamada uma vez no início do programa
void game_init();

// Chamada sempre que uma fase for iniciada
void game_start_level(int level);

// Chamada a cada frame (lógica do jogo)
GameAction game_update();

// Chamada a cada frame (desenho do jogo)
void game_display();

// Chamada quando a janela é redimensionada ENQUANTO no jogo
void game_reshape(int w, int h);

// Chamada para teclas normais (WASD, 'q')
GameAction game_keyboard(unsigned char key, int x, int y);

// Chamada para teclas especiais (Setas)
GameAction game_special_keys(int key, int x, int y);

// Chamada quando o mouse se move (para a mira)
void game_mouse_motion(int x, int y);

#endif // GAME_H