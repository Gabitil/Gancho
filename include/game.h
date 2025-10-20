#ifndef GAME_H
#define GAME_H

// Ação que o jogo pode sinalizar para o main.cpp
enum GameAction {
    GAME_ACTION_CONTINUE,
    GAME_ACTION_EXIT_TO_MENU
};

// Funções que o main.cpp poderá chamar
void game_init();                 // Para configurar o estado inicial do jogo
void game_start();                // Para (re)iniciar o jogo
void game_display();              // Para desenhar a cena do jogo
void game_reshape(int w, int h);  // Para lidar com o redimensionamento da janela no jogo
void game_timer(int value);       // Para o loop de animação do jogo
GameAction game_keyboard(unsigned char key, int x, int y); // Para lidar com o input do teclado

#endif // GAME_H