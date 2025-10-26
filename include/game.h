/**
 * Assim como em "menu.h", as "structs" e as assinaturas das funções também foram definidas no 
 * header do game. Para diferenciar das funções da main, todas as funções de estados do game possuem
 * o prefixo "game". Também foi usada a lógica de #ifndef e #define.
 */

#ifndef GAME_H
#define GAME_H

/**
 * Lógica modularizada para controle dos estados do jogo
 */
enum GameAction {
    GAME_ACTION_CONTINUE,
    GAME_ACTION_EXIT_TO_MENU,
    GAME_ACTION_LEVEL_WON,
    GAME_ACTION_LEVEL_LOST
};

/**
 * Todos os elementos do jogo (correntes de vento, espinhos, jogador, etc) são modularizados
 * para garantir que sempre terão os mesmos comportamentos e sigam uma estrutura padrão. Os 
 * valores passíveis de mudanças são variáveis controladas pela lógica do game ou pela fase em questão
 */
struct GameObject {
    float x, y, w, h;
    float r, g, b;
    float velocityX, velocityY;
};

struct Platform {
    float x, y, w, h;
    float r, g, b;
    bool isHookable;
    float frictionCoefficient; // Coeficiente de atrito, responsável pela aceleração ou desaceleração do personagem
};

struct WindZone {
    float x, y, w, h;
    float accelX, accelY; // Aceleração que o vento aplica ao jogador
};

struct BreakableWall {
    float x, y, w, h;
    float r, g, b;
    float strength; // Resistência da parede
    bool isBroken;
};

struct SpikeZone {
    float x, y, w, h;
    float r, g, b;
};


void createDisplayLists();
void gameStartLevel();
void gameStartLevel(int level);
GameAction gameUpdate();
void gameDisplay();
void gameReshape(int w, int h);
void gameMouseMotion(int x, int y);
void gameMouseClick(int button, int state);

GameAction gameKeyDown(unsigned char key, int x, int y);
void gameKeyUp(unsigned char key, int x, int y);
void gameSpecialDown(int key, int x, int y);
void gameSpecialUp(int key, int x, int y);

#endif