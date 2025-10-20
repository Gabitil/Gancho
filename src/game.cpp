#include "game.h" // Inclui a nossa nova interface
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// --- TODAS AS VARIÁVEIS GLOBAIS E DEFINES DO JOGO VÊM PARA CÁ ---
#define MAX_VEHICLES 6
#define NUM_NPCS 2

float leftCoordinate = -100, rightCoordinate = 100, bottomCoordinate = -100, topCoordinate = 100, nearPlan = 0, farPlan = 1;

typedef struct {
    float x, y;
    float width, height;
    float bodyColor[3];
    float speed;
    bool active;
    bool isPlayerVehicle;
} Vehicle;

Vehicle vehicles[MAX_VEHICLES];

bool IS_GAME_OVER = false;
int GAME_OVER_TIMER = 0;
const int GAME_OVER_DURATION = 120;
float VELOCITY_ITERATOR = 1.0f;

/**
 * @remarks
 * Dada a dificuldade de fazer um desenho aceitável e bonito usando primitivas geométicas, busquei o desenho
 * de um avião por primitivas geométricas e adaptei para usar os parâmetros 
 */
void drawAirplane(Vehicle* v) {
    float x = v->x;
    float y = v->y;
    float w = v->width;
    float h = v->height;

    glColor3fv(v->bodyColor);
    glBegin(GL_TRIANGLES);
        // Primeiro triângulo da fuselagem
        glVertex3f(x - w * 0.1f, y - h * 0.5f, 0.0f);
        glVertex3f(x + w * 0.1f, y - h * 0.5f, 0.0f);
        glVertex3f(x + w * 0.1f, y + h * 0.5f, 0.0f);
        // Segundo triângulo da fuselagem
        glVertex3f(x + w * 0.1f, y + h * 0.5f, 0.0f);
        glVertex3f(x - w * 0.1f, y + h * 0.5f, 0.0f);
        glVertex3f(x - w * 0.1f, y - h * 0.5f, 0.0f);
    glEnd();

    // Triângulo do bico do avião
    glColor3f(0.8f, 0.8f, 0.8f); // Cinza claro
    glBegin(GL_TRIANGLES);
        glVertex3f(x - w * 0.1f, y + h * 0.5f, 0.0f);
        glVertex3f(x + w * 0.1f, y + h * 0.5f, 0.0f);
        glVertex3f(x, y + h * 0.65f, 0.0f);
    glEnd();

    // Asas
    glColor3f(0.6f, 0.6f, 0.6f); // Cinza
    glBegin(GL_TRIANGLES);
        glVertex3f(x - w * 0.1f, y + h * 0.2f, 0.0f);
        glVertex3f(x - w * 0.1f, y - h * 0.1f, 0.0f);
        glVertex3f(x - w * 0.5f, y, 0.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex3f(x + w * 0.1f, y + h * 0.2f, 0.0f);
        glVertex3f(x + w * 0.1f, y - h * 0.1f, 0.0f);
        glVertex3f(x + w * 0.5f, y, 0.0f);
    glEnd();
    
    // Detalhe da cauda
    glBegin(GL_TRIANGLES);
        // Esquerdo
        glVertex3f(x - w * 0.1f, y - h * 0.4f, 0.0f);
        glVertex3f(x - w * 0.1f, y - h * 0.5f, 0.0f);
        glVertex3f(x - w * 0.3f, y - h * 0.5f, 0.0f);
        // Direito
        glVertex3f(x + w * 0.1f, y - h * 0.4f, 0.0f);
        glVertex3f(x + w * 0.1f, y - h * 0.5f, 0.0f);
        glVertex3f(x + w * 0.3f, y - h * 0.5f, 0.0f);
    glEnd();

    // Cauda
    glColor3f(0.8f, 0.8f, 0.8f); // Cinza claro
    glBegin(GL_TRIANGLES);
        glVertex3f(x - w * 0.1f, y - h * 0.5f, 0.0f);
        glVertex3f(x + w * 0.1f, y - h * 0.5f, 0.0f);
        glVertex3f(x, y - h * 0.65f, 0.0f);
    glEnd();
}

/**
 * @remarks
 * Função para verificar se houve alguma colisão com outros veículos na tela. Na sua lógica é implementada
 * a verifiação de se há alguma coordenada entre os retângulos dos veículos e que esteja no mesmo espaço
 */
bool checkCollision(Vehicle* a, Vehicle* b) {
    // Coordenadas do retângulo do veículo A
    float ax1 = a->x - a->width / 2;
    float ax2 = a->x + a->width / 2;
    float ay1 = a->y - a->height / 2;
    float ay2 = a->y + a->height / 2;

    // Coordenadas do retângulo do veículo B
    float bx1 = b->x - b->width / 2;
    float bx2 = b->x + b->width / 2;
    float by1 = b->y - b->height / 2;
    float by2 = b->y + b->height / 2;

    // Verifica se não há colisão. Se qualquer uma dessas condições for verdadeira, eles não colidem.
    if (ax2 < bx1 || ax1 > bx2 || ay2 < by1 || ay1 > by2) {
        return false;
    }

    return true;
}

void updateNPCs() {
    // Itera apenas sobre os NPCs (ignorando o primeiro índice que é reservado para o player)
    for (int i = 1; i <= NUM_NPCS; i++) {
        if (vehicles[i].active) {
            // Movimento de cima para baixo
            vehicles[i].y -= vehicles[i].speed;
            
            // Se o NPC sair da tela por baixo, reinicia no topo com uma nova posição X e velocidade
            if (vehicles[i].y < bottomCoordinate - vehicles[i].height) {
                vehicles[i].y = topCoordinate + vehicles[i].height;
                // Posição X aleatória
                vehicles[i].x = leftCoordinate + (rand() % (int)(rightCoordinate - leftCoordinate));
                // Velocidade aleatória
                vehicles[i].speed = VELOCITY_ITERATOR + ((rand() % 10) / 10.0f);
                VELOCITY_ITERATOR += 0.03f; // Aumentando gradativamente a velocidade dos veículos
            }
        }
    }
}

void movePlayer(float dx, float dy) {
    if (IS_GAME_OVER) return; // Não move se o jogo acabou

    vehicles[0].x += dx;
    vehicles[0].y += dy;

    // Limites da caixa de visão para o jogador
    float half_w = vehicles[0].width / 2;
    float half_h = vehicles[0].height / 2;

    if (vehicles[0].x - half_w < leftCoordinate) vehicles[0].x = leftCoordinate + half_w;
    if (vehicles[0].x + half_w > rightCoordinate) vehicles[0].x = rightCoordinate - half_w;
    if (vehicles[0].y - half_h < bottomCoordinate) vehicles[0].y = bottomCoordinate + half_h;
    if (vehicles[0].y + half_h > topCoordinate) vehicles[0].y = topCoordinate - half_h;
}

void initVehicles() {

    // Player
    vehicles[0].x = 0;
    vehicles[0].y = -80;
    vehicles[0].width = 12;
    vehicles[0].height = 15;
    vehicles[0].bodyColor[0] = 0.2f; vehicles[0].bodyColor[1] = 0.5f; vehicles[0].bodyColor[2] = 1.0f; // Azul
    vehicles[0].speed = 5.0f;
    vehicles[0].active = true;
    vehicles[0].isPlayerVehicle = true;

    // NPCs
    for (int i = 1; i <= NUM_NPCS; i++) {
        vehicles[i].x = leftCoordinate + (rand() % (int)(rightCoordinate - leftCoordinate)); // Posição X aleatória
        vehicles[i].y = topCoordinate + (i * 50); // Posição Y inicial fora da tela, em cima
        vehicles[i].width = 12;
        vehicles[i].height = 15;
        vehicles[i].bodyColor[0] = 1.0f; vehicles[i].bodyColor[1] = 0.3f; vehicles[i].bodyColor[2] = 0.3f; // Vermelho
        vehicles[i].speed = 1.0f; 
        vehicles[i].active = true;
        vehicles[i].isPlayerVehicle = false;
    }

    // Garante que o resto da lista esteja inativo
    for (int i = NUM_NPCS + 1; i < MAX_VEHICLES; i++) {
        vehicles[i].active = false;
    }
}

// --- IMPLEMENTAÇÃO DAS FUNÇÕES DECLARADAS EM game.h ---

// Renomeada de init() para game_init()
void game_init() {
    srand(time(NULL));
    initVehicles();
}

// NOVO: Função para reiniciar o jogo quando o jogador clica em "Jogar"
void game_start() {
    IS_GAME_OVER = false;
    VELOCITY_ITERATOR = 1.0f;
    GAME_OVER_TIMER = 0;
    initVehicles(); // Reposiciona todos os veículos
    // glutTimerFunc(16, game_timer, 0); // Inicia o loop de animação do jogo
}

// Renomeada de reshape() para game_reshape()
void game_reshape(int x, int y) {
    glViewport(0, 0, x, y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float windowModeling = (float)x / (float)y;
    if (windowModeling == 0) windowModeling = 1;

    if (windowModeling >= 1.0) {
        glOrtho(leftCoordinate * windowModeling, rightCoordinate * windowModeling, bottomCoordinate, topCoordinate, nearPlan, farPlan);
    } else {
        glOrtho(leftCoordinate, rightCoordinate, bottomCoordinate / windowModeling, topCoordinate / windowModeling, nearPlan, farPlan);
    }
    glMatrixMode(GL_MODELVIEW);
}

// Renomeada de display() para game_display()
void game_display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Se o jogo não acabou, atualiza e verifica colisões
    if (!IS_GAME_OVER) {
        updateNPCs();

        // Checar colisão entre o jogador (vehicles[0]) e todos os NPCs
        for (int i = 1; i <= NUM_NPCS; i++) {
            if (vehicles[i].active && checkCollision(&vehicles[0], &vehicles[i])) {
                IS_GAME_OVER = true;
                GAME_OVER_TIMER = GAME_OVER_DURATION;
                VELOCITY_ITERATOR = 1.0f;
            }
        }
    }
    
    for (int i = 0; i < MAX_VEHICLES; i++) {
        if (vehicles[i].active) {
            // Renderiza apenas se estiver dentro da caixa de visão (otimização)
            bool is_inside_view = (vehicles[i].x + vehicles[i].width/2 > leftCoordinate &&
                                   vehicles[i].x - vehicles[i].width/2 < rightCoordinate &&
                                   vehicles[i].y + vehicles[i].height/2 > bottomCoordinate &&
                                   vehicles[i].y - vehicles[i].height/2 < topCoordinate);
            if (is_inside_view) {
                 drawAirplane(&vehicles[i]);
            }
        }
    }

    if (IS_GAME_OVER) {
        // Desenha a mensagem
        glColor3f(1.0f, 0.0f, 0.0f); // Cor vermelha
        glRasterPos2f(-45, 0); // Posição do texto
        const char *msg = "GAME OVER!";
        for (const char *c = msg; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
        }

        // Decrementa o timer
        GAME_OVER_TIMER--;
        if (GAME_OVER_TIMER <= 0) {
            IS_GAME_OVER = false;
            initVehicles(); // Reinicia a animação
        }
    }

    glutSwapBuffers();
}

// Renomeada de keyboard() para game_keyboard() e MODIFICADA
GameAction game_keyboard(unsigned char key, int x, int y) {
    float speed = vehicles[0].speed * (VELOCITY_ITERATOR / 2);
    switch(key) {
        case 'a':
        case 'A': movePlayer(-speed, 0); break;
        case 'd':
        case 'D': movePlayer(speed, 0); break;
        case 'w':
        case 'W': movePlayer(0, speed); break;
        case 's':
        case 'S': movePlayer(0, -speed); break;

        // NOVO: Condição para voltar ao menu
        case 'q':
        case 'Q':
            return GAME_ACTION_EXIT_TO_MENU;
    }
    return GAME_ACTION_CONTINUE; // Continua no jogo
}

// Renomeada de timer() para game_timer()
void game_timer(int value) {
    glutPostRedisplay();
    // A chamada recursiva será controlada pelo main.cpp para poder ser interrompida
}