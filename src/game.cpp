/**
 * Por padrão de projeto, todas as variáveis numéricas usadas são explicitamente do tipo float, exceto
 * contadores e variáveis de controle. Isso é feito para garantir a segurança e a linearidade dos resultados calculados.
 */

#include "game.h"
#include <GL/freeglut.h>
#include "utils.h"
#include <stdio.h>
// #include <math.h>
#include <string>
#include <vector>
#include <algorithm>

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

// ----------------------------------------------------------------------------------------------------------------

/**
 * Para padronização dos cálculos e dos valores de vetores e deslocamentos, todas as constantes
 * usadas são fixas. Constantes padrões da física.
 */
const float GRAVITY = -0.1f; // Aceleração vertical (negativa = para baixo)
const float PLAYER_MASS = 10.0f; // Massa do jogador (para cálculos de força)
const float PLAYER_WALK_ACCEL = 0.1f; // Aceleração horizontal aplicada ao caminhar
const float MAX_WALK_SPEED = 3.0f; // Velocidade máxima permitida enquanto "no chão"
const float DAMPING_FACTOR = 0.99f; // Fator de amortecimento aplicado quando pendurado

/**
 * Constantes relacionadas ao gancho
 */
const float HOOK_SPEED = 25.0f;
const float MAX_PULL_STRENGTH_PHYSICS = 0.4f;

/**
 * Limitador de velocidade do personagem para garantir melhor visibilidade ao jogador dos 
 * vetores e das ações ao decorrer do game
 */
const float MAX_PLAYER_SPEED = 30.0f;

/**
 * Constantes usadas na definição de mundo e para visualização (câmera, espaço para desenhos, etc)
 */
const float PLAYER_HEIGHT = 40.0f;
const float VIEW_WIDTH = 800.0f;
const float VIEW_HEIGHT = 600.0f;
const float WORLD_WIDTH = 3000.0f;
const float WORLD_HEIGHT = 600.0f;
const float DEATH_BOUNDARY_Y = -100.0f; // Constante usada para verificar se o personagem "caiu", limitador para sua queda

/**
 * Os vetores seguem uma escala padrão para que a coerência da visualização seja válida ao jogador. 
 * Para isso, o valor máximo do vetor de lançamento do "grappling hook" será de 100N e o seu tamanho será de 
 * 5x o tamanho do personagem. A partir daí todos os outros vetores serão calculados a fim de manter a proporção de 
 * tamanho e ajudar a dimensionar, para o jogador, a força real de cada força aplicada.
 */
const float MAX_VISUAL_AIM_LENGTH = 5.0f * PLAYER_HEIGHT;
const float MAX_AIM_FORCE_DISPLAY = 100.0f;
const float VECTOR_VISUAL_SCALE = MAX_VISUAL_AIM_LENGTH / MAX_PULL_STRENGTH_PHYSICS;
const float VELOCITY_VISUAL_SCALE = 7.0f;
const float WIND_VECTOR_VISUAL_SCALE = 500.0f;

// ----------------------------------------------------------------------------------------------------------------

/**
 * Definição das variáveis que serão usadas no game.
 * Aqui também definimos o limite de quantidade de cada elemento. Essas 
 * quantidades serão usadas de forma variada de acordo com a fase.
 */
GameObject player;
GameObject door;
Platform platforms[10];
int numPlatforms = 0;
Platform* collidingPlatform = NULL;

WindZone windZones[10];
int numWindZones = 0;
BreakableWall breakableWalls[10];
int numBreakableWalls = 0;
SpikeZone spikeZones[10];
int numSpikeZones = 0;

int CURRENT_LEVEL = 1;
bool isGrounded = false; // Variável para verificar se o personagem está em contato com alguma plataforma
bool isHooked = false; // Variável para identificar se o personagem está pendurado em alguma plataforma
bool isHookFiring = false; // Variável para controlar se o gancho está em modo de disparo, ou seja, se ele está em trajetória para se prender em algo
float hookPointX = 0, hookPointY = 0; // Variáveis para indicar em qual ponto de uma plataforma o personagem está preso
float hookProjectileX = 0, hookProjectileY = 0;
float hookProjectileVelX = 0, hookProjectileVelY = 0;
float ropeLength = 0;
float currentPullForce = 0.0f;
float mouseGameX = 0, mouseGameY = 0; // Variáveis para controlar as posições X e Y do mouse durante o game
float aimDisplayForce = 0; // Variável para controle da força de lançamento do personagem pelo grappling hook
float cameraLeft = 0, cameraBottom = 0;
bool isPressingLeft = false; // Variável para controlar se o personagem está pressionando as teclas de movimentação para a esquerda
bool isPressingRight = false; // Variável para controlar se o personagem está pressionando as teclas de movimentação para a direita
float forceNormal = 0; // Variável para controlar a força normal aplicada ao personagem
float forceFriction = 0; // Variável para controlar a força de atrito aplicada ao personagem
float forceTensionX = 0; // Variável para controlar a posição X da força de tensão na corda aplicada ao personagem e ao fio do grappling hook
float forceTensionY = 0; // Variável para controlar a posição Y da força de tensão na corda aplicada ao personagem e ao fio do grappling hook

/**
 * Variáveis para controle da aceleração do personagem. 
 * Usada nos cálculos usando a velocidade. No sistema escolhido, não é uma aceleração
 * que define a velocidade, mas sim a velocidade fixada que define uma aceleração.
 * Desse modo fica mais simples de coordenar todos os movimentos e fazer os cálculos do game.
 */
float lastVelocityMag = 0.0f;
float currentAcceleration = 0.0f;

// ----------------------------------------------------------------------------------------------------------------

/**
 * Função para inicializar a quantidade de elementos de mundo, suas posições e outros parâmetros.
 * TODO - Variar a quantidade de elementos e as suas posições de acordo com as fases
 */
void gameInit() {
    // Plataformas
    numPlatforms = 2;
    platforms[0] = {0, 0, WORLD_WIDTH, 40, 0.2f, 0.6f, 0.2f, true, 0.8f};
    platforms[1] = {500, 400, 300, 40, 0.4f, 0.4f, 0.4f, true, 0.1f}; 
    
    // Correntes de Vento
    numWindZones = 1;
    windZones[0] = {800, 40, 100, 300, 0.0f, 0.05f};
    
    // Paredes Quebráveis
    numBreakableWalls = 1;
    breakableWalls[0] = {1200, 40, 40, 150, 0.6f, 0.4f, 0.2f, 20.0f, false};
    
    // Espinhos
    numSpikeZones = 1;
    spikeZones[0] = {1500, 40, 200, 20, 1.0f, 0.0f, 0.0f};
}

/**
 * Essa função é responsável por reiniciar todas as variáveis de controle dos níveis para que todos os 
 * obstáculos/elementos fiquem ativos e prontos para a interação do personagem
 */
void gameStartLevel(int level) {
    player = {50, platforms[0].h, 40, PLAYER_HEIGHT, 0.9f, 0.1f, 0.1f, 0, 0};
    door = {WORLD_WIDTH - 200, platforms[0].h, 30, 80, 0.5f, 0.3f, 0.0f};
    isGrounded = true; 
    isHooked = false; 
    isHookFiring = false;
    collidingPlatform = &platforms[0]; 
    isPressingLeft = false; 
    isPressingRight = false;
    lastVelocityMag = 0.0f;
    currentAcceleration = 0.0f;

    for (int i = 0; i < numBreakableWalls; i++) {
        breakableWalls[i].isBroken = false;
    }
}

/**
 * Função responsável por definir a área de desenho do OpenGL
 */
void gameReshape(int width, int height) { 
    glViewport(0, 0, width, height); 
}

/**
 * Todas as atualizações/interações do game são controladas por essa função. Nela foram definidas
 * todas as regras de lógica que escolhemos para a base de funcionamento da movimentação do jogo
 * e de como o personagem interage com os objetos.
 */
GameAction gameUpdate() {
    
    // --------------------------------------- Cálculos de velocidade -------------------------------------------------
    
    // Variáveis para controle da posição e velocidade anterior do personagem antes da atualização da próxima cena
    float prevPositionPlayerX = player.x;
    float prevPositionPlayerY = player.y;
    float prevVelocityX = player.velocityX;
    float prevVelocityY = player.velocityY;

    forceNormal = 0; 
    forceFriction = 0; 
    forceTensionX = 0; 
    forceTensionY = 0;
    float playerAccelerationX = 0, playerAccelerationY = GRAVITY;
    
    /**
     * A força de vento aplica uma aceleração extra ao personagem ao entrar dentro da zona definida. 
     * Para isso, é usada a função de verificação de colisão de pontos e, além disso, a aceleração incrementada
     * ao personagem é definida pela força da zona de vento
     */
    for (int i = 0; i < numWindZones; i++) {
        WindZone* windZone = &windZones[i];
        if (isPointInside(player.x + player.w / 2, player.y + player.h / 2, windZone->x, windZone->y, windZone->w, windZone->h)) {
            playerAccelerationX += windZone->accelX;
            playerAccelerationY += windZone->accelY;
        }
    }
    
    /**
     * O gancho (grappling hook) se diferencia de uma teia do homem-aranha pois ela puxa o personagem
     * na direção do lançamento, com a mesma força escolhida por ele ao se lançar. Para simular esse comportamento,
     * ao se prender, a seguinte função calcula a tensão na corda aplicada ao personagem quando ele está preso
     */
    if (isHooked) {
        float playerCenterX = player.x + player.w / 2, playerCenterY = player.y + player.h / 2;
        float vecToHookX = hookPointX - playerCenterX, vecToHookY = hookPointY - playerCenterY;
        float dist = sqrt(vecToHookX * vecToHookX + vecToHookY * vecToHookY);
        if (dist > 0.01f) {
            float normX = vecToHookX / dist, normY = vecToHookY / dist;
            playerAccelerationX += normX * currentPullForce; playerAccelerationY += normY * currentPullForce;
            forceTensionX = normX * currentPullForce; forceTensionY = normY * currentPullForce;
        }
    }
    
    /**
     * Movimentação do personagem (definida pela constante de aceleração ao andar) no chão.
     */
    if (isGrounded) {
        if (isPressingLeft) playerAccelerationX -= PLAYER_WALK_ACCEL;
        if (isPressingRight) playerAccelerationX += PLAYER_WALK_ACCEL;
        
        /* Ao entrar em contato com alguma plataforma, a sua aceleração é afetada pela quantidade de atrito 
        * existente. Esse atrito, pela fórmula da física, é a força normal multiplicada pelo coeficiente de atrito.
        */ 
       if (collidingPlatform){
           forceNormal = -GRAVITY;
           float frictionAccel = collidingPlatform->frictionCoefficient * forceNormal;
           if (fabs(player.velocityX + playerAccelerationX) < frictionAccel) {
               player.velocityX = 0; playerAccelerationX = 0; forceFriction = 0;
            } else {
                if (player.velocityX > 0) { 
                    playerAccelerationX -= frictionAccel; forceFriction = -frictionAccel; 
                } else if (player.velocityX < 0) { 
                    playerAccelerationX += frictionAccel; forceFriction = frictionAccel; 
                }
            }
        }
        
    }
    
    /**
     * Os valores calculados acima (gerados pelas possíveis interações com os objetos) são incrementadas ao valor 
     * da velocidade do personagem
     */
    player.velocityX += playerAccelerationX; 
    player.velocityY += playerAccelerationY;
    
    /**
     * Caso esteja no chão, o personagem não pode acelerar indefinidamente. Portanto, sua velocidade final calculada
     * é verificada para que não ultrapasse o valor máximo permitido para a fase
     */
    if (isGrounded && fabs(player.velocityX) > MAX_WALK_SPEED) {
        player.velocityX = (player.velocityX > 0) ? MAX_WALK_SPEED : -MAX_WALK_SPEED;
    }
    
    /**
     * Para fins de visualização, também foi definida uma velocidade máxima para o personagem, visando manter a proposta
     * educativa do game.
     */
    float speed = sqrt(player.velocityX * player.velocityX + player.velocityY * player.velocityY);
    if (speed > MAX_PLAYER_SPEED) {
        player.velocityX = (player.velocityX / speed) * MAX_PLAYER_SPEED;
        player.velocityY = (player.velocityY / speed) * MAX_PLAYER_SPEED;
    }
    
    player.x += player.velocityX; player.y += player.velocityY;
    
    // --------------------------------------- Cálculos de colisão -------------------------------------------------
    
    /**
     * Para dificultar os controles de lançamento, o personagem não deve ultrapassar os limites de mundo. Para isso, 
     * a seguinte verificação visa encerrar o game com derrota caso os limites estabelecidos sejam ultrapassados
     */
    if (player.x < -player.w || player.x > WORLD_WIDTH || player.y < DEATH_BOUNDARY_Y) {
        return GAME_ACTION_LEVEL_LOST;
    }
    
    /**
     * Verificação da colisão com os espinhos (ainda por meio de colisão de retângulos)
     */
    for (int i = 0; i < numSpikeZones; i++) {
        SpikeZone* spikeZone = &spikeZones[i];
        if (checkRectangleCollision(player.x, player.y, player.w, player.h, 
                               spikeZone->x, spikeZone->y, spikeZone->w, spikeZone->h)){
            return GAME_ACTION_LEVEL_LOST;
        }
    }

    /**
     * Para o cálculo das paredes quebráveis, além de gerar uma colisão, ela também deve afetar a velocidade
     * final do personagem (a sua velocidade deve sofrer interferência da força por ele deixada ao colidir com 
     * a parede). Portanto, essa função visa verificar a colisão e, além disso, alterar a velocidade do personagem
     */
    for (int i = 0; i < numBreakableWalls; i++) {
        BreakableWall* breakableWall = &breakableWalls[i];
        if (breakableWall->isBroken) continue;

        if (checkRectangleCollision(player.x, player.y, player.w, player.h, 
                               breakableWall->x, breakableWall->y, breakableWall->w, breakableWall->h)) {

            float impactVelX = player.velocityX;
            float impactVelY = player.velocityY;
            float impactSpeed = sqrt(impactVelX * impactVelX + impactVelY * impactVelY);
            
            /**
             * Pela lei de Newton, a força F aplicada em algo é a massa vezes a sua velocidade
             */
            float impactForce = PLAYER_MASS * impactSpeed;

            if (impactForce >= breakableWall->strength) { // Quebra a parede se a força for suficiente
                breakableWall->isBroken = true;
                // Reduz um pouco a velocidade do jogador ao quebrar
                player.velocityX *= 0.7f; 
                player.velocityY *= 0.7f;
            /**
             * Caso a força não seja suficiente para atravessar a parede, o personagem deve parar na posição imediatamente anterior
             * antes da colisão (posição do frame anterior)
             */
            } else {
                player.x = prevPositionPlayerX;
                player.y = prevPositionPlayerY;
                player.velocityX = 0;
                player.velocityY = 0;
                
                // /**
                //  * Controle complexo de velocidade. Como não é necessário zerar todas as velocidades do personagem (por exemplo, no caso de uma colisão
                //  * superior, a velocidade em x não é alterada), então é feita uma verificação de cada colisão com a parede quebrável e os parãmetros do personagem
                //  * são alterados de acordo com o verificado
                //  */
                // if (player.x + player.w > breakableWall->x && prevPositionPlayerX + player.w <= breakableWall->x) { // Colisão pela esquerda
                //     player.velocityX = 0;
                //     player.x = breakableWall->x - player.w;
                // } else if (player.x < breakableWall->x + breakableWall->w && prevPositionPlayerX >= breakableWall->x + breakableWall->w) { // Colisão pela direita
                //     player.velocityX = 0;
                //     player.x = breakableWall->x + breakableWall->w;
                // }
                // if (player.y + player.h > breakableWall->y && prevPositionPlayerY + player.h <= breakableWall->y) { // Colisão por baixo
                //     player.velocityY = 0;
                //     player.y = breakableWall->y - player.h;
                // } else if (player.y < breakableWall->y + breakableWall->h && prevPositionPlayerY >= breakableWall->y + breakableWall->h) { // Colisão por cima
                //     player.velocityY = 0;
                //     player.y = breakableWall->y + breakableWall->h;
                // }
            }
        }
    }

    // Colisão com Plataformas
    isGrounded = false; collidingPlatform = NULL;
    for (int i = 0; i < numPlatforms; i++) {
        Platform* platform = &platforms[i];
        /**
         * Caso o personagem colida com alguma plataforma, ele deve se desprender imediatamente
         */
        if (checkRectangleCollision(player.x, player.y, player.w, player.h, 
                                    platform->x, platform->y, platform->w, platform->h)) {
            
            /**
             * Caso colida com alguma plataforma, a seguinte verificação serve para tratar os casos de colisão superior e colisão inferior, pois, 
             * na estruturação feita, o chão também é uma plataforma
             */
            if (player.velocityY > 0 && (player.y + player.h) > platform->y && player.y < platform->y) {
                player.y = platform->y - player.h; 
                player.velocityY = 0; 
                if (isHooked) isHooked = false;
            } else if (player.velocityY <= 0 && player.y < (platform->y + platform->h) && (player.y + player.h) > (platform->y + platform->h)) {
                player.y = platform->y + platform->h; 
                isGrounded = true; 
                collidingPlatform = platform;
                forceNormal = -GRAVITY - (player.velocityY * 0.5f);
                player.velocityY = 0; 
                if (isHooked) isHooked = false;
            }
        }
    }

    /**
     * A seguinte verificação visa implementar a lógica de velocidade do personagem ao se prender com o gancho. 
     * Dada a complexidade das operações, buscamos a implementação dessa física e pedimos uma explicação detalhada 
     * do funcionamento do algorítmo.
     */
    if (isHooked) {
    // 1. Encontrar o centro do jogador
    float playerCenterX = player.x + player.w / 2;
    float playerCenterY = player.y + player.h / 2;

    // 2. Calcular o vetor do jogador ATÉ o ponto do gancho
    float vectorToHookX = hookPointX - playerCenterX;
    float vectorToHookY = hookPointY - playerCenterY;

    // 3. Calcular a distância atual até o gancho (magnitude do vetor)
    float currentDistance = sqrt(vectorToHookX * vectorToHookX + vectorToHookY * vectorToHookY);

    // 4. Lógica de Restrição da Corda (Física do Pêndulo)
    // Se a distância atual for maior que o comprimento da corda (ou seja, a corda está esticada)
    if (currentDistance > ropeLength && currentDistance > 0.01f) {
        
        // 4a. Calcular o quanto a corda esticou além do seu comprimento
        float stretchAmount = currentDistance - ropeLength;

        // 4b. Calcular a direção normalizada (vetor unitário) da corda
        float normalizedDirectionX = vectorToHookX / currentDistance;
        float normalizedDirectionY = vectorToHookY / currentDistance;

        // 4c. Corrigir a posição do jogador
        // "Teleporta" o jogador de volta para a ponta da corda, removendo o excesso (stretchAmount)
        player.x += normalizedDirectionX * stretchAmount;
        player.y += normalizedDirectionY * stretchAmount;

        // 4d. Corrigir a velocidade do jogador (a parte mais importante do pêndulo)
        // Calcula o "produto escalar" da velocidade atual com a direção da corda.
        // Isso nos diz quanta da velocidade do jogador está se movendo "para fora", esticando a corda.
        float velocityAlongRope = player.velocityX * normalizedDirectionX + player.velocityY * normalizedDirectionY;

        // Se o jogador estiver se movendo para longe do gancho (esticando a corda)...
        if (velocityAlongRope > 0) { 
            // ...remove essa componente da velocidade.
            // Isso faz o jogador "quicar" na ponta da corda e começar o balanço.
            player.velocityX -= normalizedDirectionX * velocityAlongRope; 
            player.velocityY -= normalizedDirectionY * velocityAlongRope; 
        }
    }

    // 5. Aplicar Amortecimento (Damping)
    // Reduz lentamente a velocidade para que o balanço pare eventualmente
    player.velocityX *= DAMPING_FACTOR; 
    player.velocityY *= DAMPING_FACTOR;
    
    // 6. Atualizar Estado
    // Se está pendurado no gancho, não pode estar no chão
    isGrounded = false;
}

    /**
     * Essa função verifica se o gancho está em modo de disparo (sendo lançado) e, caso sim, verifica o ponto 
     * em que o jogador deve se prender para ser lançado por meio de variáveis de retorno na chamada da função (hitX e hitY). 
     * Para isso, é usada a função utilitária de interseção entre uma linha e um retângulo
     */
    if (isHookFiring) {
        float prevHookX = hookProjectileX, prevHookY = hookProjectileY;
        hookProjectileX += hookProjectileVelX; hookProjectileY += hookProjectileVelY;
        
        bool hit = false;
        for (int i = 0; i < numPlatforms; i++) {
            Platform* p = &platforms[i];
            float hitX, hitY;
            // Gancho não prende em atrito 0
            if (p->isHookable && p->frictionCoefficient > 0.0f && lineRectIntersection(prevHookX, prevHookY, hookProjectileX, hookProjectileY, *p, hitX, hitY)) {
                isHookFiring = false; 
                isHooked = true;
                hookPointX = hitX; 
                hookPointY = hitY;
                float dx = hookPointX - (player.x + player.w / 2), dy = hookPointY - (player.y + player.h / 2);
                ropeLength = sqrt(dx * dx + dy * dy);
                isGrounded = false;
                hit = true;
                break;
            }
        }

        /**
         * O tamanho do gancho só possui os limites do tamanho da tela. Caso passe dos limites de visualização, o lançamento deve ser cancelado
         */
        if (!hit) {
            if (hookProjectileX < cameraLeft || 
                hookProjectileX > cameraLeft + VIEW_WIDTH ||
                hookProjectileY < cameraBottom ||
                hookProjectileY > cameraBottom + VIEW_HEIGHT)
            {
                isHookFiring = false;
            }
        }
    }
    
    float currentVelocityMag = sqrt(player.velocityX * player.velocityX + player.velocityY * player.velocityY);
    currentAcceleration = currentVelocityMag - lastVelocityMag;
    lastVelocityMag = currentVelocityMag;

    /**
     * Condição de vitória: Caso o personagem alcançe a porta
     */
    if (checkRectangleCollision(player.x, player.y, player.w, player.h, 
                                door.x, door.y, door.w, door.h)) {
        return GAME_ACTION_LEVEL_WON;
    }

    /**
     * Caso nenhuma das condições seja atendida nas verificações dessa função, o game deve apenas continuar o fluxo comum e passar para a próxima cena
     */
    return GAME_ACTION_CONTINUE;
}

/**
 * Para mostrar todos os valores dos vetores na caixa de visualização, são necessárias diversas variáveis 
 * de controle e operações matemáticas para garantir a coerência dos dados. Além de definir uma escala fixa
 * para facilitar a jogabilidade do usuário, também é necessário que os valores dos vetores estejam coerentes.
 */
void drawPhysicsDebugHUD() {
    glMatrixMode(GL_PROJECTION); 
    glPushMatrix(); 
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
    glMatrixMode(GL_MODELVIEW); 
    glPushMatrix(); 
    glLoadIdentity();

    float lineHeight = 15.0f;
    float margin = 10.0f;
    float padding = 8.0f;

    /**
     * Vetor de strings usado para controlar quais as informações estarão disponíveis na caixa de visualização. 
     * Para que não fique tão poluída a tela com informações desnecessárias, alguns valores só aparecem caso
     * algumas condições sejam satisfeitas
     */
    std::vector<std::string> infoLines;
    char buffer[100];
    
    float playerVelocity = sqrt(player.velocityX * player.velocityX + player.velocityY * player.velocityY);
    sprintf(buffer, "v: %.1f m/s", playerVelocity); infoLines.push_back(buffer);
    
    sprintf(buffer, "a: %.2f m/s^2", currentAcceleration); infoLines.push_back(buffer);

    sprintf(buffer, "P: %.2f N", GRAVITY * PLAYER_MASS); infoLines.push_back(buffer);
    
    if (isGrounded) {
        sprintf(buffer, "N: %.2f N", forceNormal * PLAYER_MASS); infoLines.push_back(buffer);
        if (fabs(forceFriction) > 0.001f) {
            sprintf(buffer, "Fat: %.2f N", fabs(forceFriction) * PLAYER_MASS); infoLines.push_back(buffer); 
        }
    }
    if (isHooked) {
        sprintf(buffer, "T: %.2f N", currentPullForce * PLAYER_MASS); infoLines.push_back(buffer);
    }
    
    /**
     * Para mais fins de otimização dos valores e limpeza da tela, as constantes de coeficiente de atrito só aparecem 
     * caso a plataforma esteja visível na tela
     */
    for (int i = 0; i < numPlatforms; ++i) {
        Platform* platform = &platforms[i];
        if (platform->x < cameraLeft + VIEW_WIDTH && platform->x + platform->w > cameraLeft) {
            sprintf(buffer, "mu_%d: %.2f", i + 1, platform->frictionCoefficient);
            infoLines.push_back(buffer);
        }
    }
    
    /**
     * Para centralizar as operações na caixa corretamente, é feito um controle a partir da maior frase de todas
     * para definir qual o tamanho da caixa de visualização
     */
    float maxWidth = 0;
    for (const auto& line : infoLines) {
        maxWidth = std::max(maxWidth, (float)getTextWidth(line.c_str(), GLUT_BITMAP_9_BY_15));
    }

    float blockWidth = maxWidth + 2 * padding;
    float blockHeight = (infoLines.size() * lineHeight) + (padding * 2) - (lineHeight - 10);
    float blockPositionX = glutGet(GLUT_WINDOW_WIDTH) - margin - blockWidth;
    float blockPositionY = margin;

    drawRect(blockPositionX, blockPositionY, blockWidth, blockHeight, 0.0f, 0.0f, 0.0f, 0.7f); // Retângulo preto

    float currentYForWrite = blockPositionY + padding + 10;
    for (const auto& line : infoLines) {
        float xPositionOfText = blockPositionX + padding;
        
        glColor3f(1.0f, 1.0f, 1.0f); // Texto branco (para contrastar bem com o bloco desenhado)
        drawText(xPositionOfText, currentYForWrite, line.c_str(), GLUT_BITMAP_9_BY_15);
        currentYForWrite += lineHeight;
    }

    glMatrixMode(GL_PROJECTION); 
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW); 
    glPopMatrix();
}

/**
 * Essa função é a "main" do jogo. Ela é responsável por desenhar todos os elementos da cena, além disso, também
 * trata corretamente algumas das variáveis globais e os parâmetros corretos para as funções. As chamadas das funções
 * não serão explicadas detalhadamente na função pois as funções menores estão documentadas. Em suma, a função de display do 
 * game usa as primitivas para desenhar alguns elementos e usa funções prontas para outros desenhos mais complexos
 */
void gameDisplay() {
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    cameraLeft = player.x + player.w / 2 - VIEW_WIDTH / 2;
    if (cameraLeft < 0) cameraLeft = 0;
    if (cameraLeft + VIEW_WIDTH > WORLD_WIDTH) cameraLeft = WORLD_WIDTH - VIEW_WIDTH;
    cameraBottom = 0;

    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();
    gluOrtho2D(cameraLeft, cameraLeft + VIEW_WIDTH, cameraBottom, cameraBottom + VIEW_HEIGHT);
    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity();

    for (int i = 0; i < numPlatforms; i++) {
        Platform* platform = &platforms[i];
        glPushMatrix();
            glTranslatef(platform->x, platform->y, 0.0f);
            drawRect(0, 0, platform->w, platform->h, platform->r, platform->g, platform->b);
            glColor3f(1.0, 1.0, 1.0);
            drawText(20, 15, ("mu_" + std::to_string(i + 1)).c_str());
        glPopMatrix();
    }
    
    for (int i = 0; i < numWindZones; i++) {
        WindZone* windZone = &windZones[i];
        glPushMatrix();
            glTranslatef(windZone->x, windZone->y, 0.0f);
            drawRect(0, 0, windZone->w, windZone->h, 1.0f, 1.0f, 1.0f, 0.2f); // Retângulo branco quase transparente

            /**
             * Toda corrente de vento possui um vetor que indica o seu módulo, direção e sentido.
             */
            float windMag = sqrt(windZone->accelX * windZone->accelX + windZone->accelX * windZone->accelY);
            if (windMag > 0.001f) {
                char windLabel[50];
                sprintf(windLabel, "Corrente de Vento: %.2f N", windMag * PLAYER_MASS);
                drawVector(windZone->w / 2, windZone->h / 2, windZone->accelX, windZone->accelY, WIND_VECTOR_VISUAL_SCALE, 0.0f, 0.8f, 1.0f, windLabel);
            }
        glPopMatrix();
    }
    
    for (int i = 0; i < numBreakableWalls; i++) {
        BreakableWall* breakableWall = &breakableWalls[i];
        if (!breakableWall->isBroken) {
            glPushMatrix();
                glTranslatef(breakableWall->x, breakableWall->y, 0.0f);
                drawRect(0, 0, breakableWall->w, breakableWall->h, breakableWall->r, breakableWall->g, breakableWall->b);
                
                glColor3f(1.0f, 1.0f, 1.0f);
                char strengthText[50];
                sprintf(strengthText, "Força: %.0f N", breakableWall->strength);
                
                int textWidth = getTextWidth(strengthText, GLUT_BITMAP_9_BY_15);
                drawText(breakableWall->w / 2 - textWidth / 2, breakableWall->h / 2, strengthText, GLUT_BITMAP_9_BY_15);
            glPopMatrix();
        }
    }
    
    for (int i = 0; i < numSpikeZones; i++) {
        SpikeZone* spikeZone = &spikeZones[i];
        drawSpikes(spikeZone->x, spikeZone->y, spikeZone->w, spikeZone->h, spikeZone->r, spikeZone->g, spikeZone->b);
    }
    
    glPushMatrix();
        glTranslatef(door.x, door.y, 0.0f);
        drawRect(0, 0, door.w, door.h, door.r, door.g, door.b);
    glPopMatrix();
    
    float playerCenterX = player.x + player.w / 2, playerCenterY = player.y + player.h / 2;

    glPushMatrix();
        glTranslatef(playerCenterX, playerCenterY, 0.0f);
        
        drawRect(-player.w / 2, -player.h / 2, player.w, player.h, player.r, player.g, player.b);

        drawVector(0, 0, 0, GRAVITY, VECTOR_VISUAL_SCALE, 0.2f, 0.2f, 1.0f, "P");
        float velMag = sqrt(player.velocityX * player.velocityX + player.velocityY * player.velocityY);
        if (velMag > 0.01) {
            drawVector(0, 0, player.velocityX, player.velocityY, VELOCITY_VISUAL_SCALE, 1.0f, 0.5f, 0.0f, "V");
        }
        if (isHooked) {
            drawVector(0, 0, forceTensionX, forceTensionY, VECTOR_VISUAL_SCALE, 1.0f, 0.0f, 1.0f, "T");
        }
        if (isGrounded) {
            drawVector(0, -player.h/2, 0, forceNormal, VECTOR_VISUAL_SCALE, 0.0f, 1.0f, 1.0f, "N");
            if (fabs(forceFriction) > 0.001f) {
                drawVector(0, -player.h/2 + 5, forceFriction, 0, VECTOR_VISUAL_SCALE, 1.0f, 0.0f, 0.0f, "Fat");
            }
        }
    
    glPopMatrix();

    float vAX = mouseGameX - playerCenterX, vAY = mouseGameY - playerCenterY;
    float rMD = sqrt(vAX * vAX + vAY * vAY);
    float cMD = fmin(rMD, MAX_VISUAL_AIM_LENGTH);
    aimDisplayForce = (cMD / MAX_VISUAL_AIM_LENGTH) * MAX_AIM_FORCE_DISPLAY;
    float vANX = 0, vANY = 0; if (rMD > 0.01f) { vANX = vAX / rMD; vANY = vAY / rMD; }

    float forcePercent = cMD / MAX_VISUAL_AIM_LENGTH;
    float aimPhysicsX = vANX * forcePercent * MAX_PULL_STRENGTH_PHYSICS;
    float aimPhysicsY = vANY * forcePercent * MAX_PULL_STRENGTH_PHYSICS;
    
    if (!isHooked && !isHookFiring) {
        char magText[50];
        sprintf(magText, "Forca: %.0f", aimDisplayForce);
        drawVector(playerCenterX, playerCenterY, aimPhysicsX, aimPhysicsY, VECTOR_VISUAL_SCALE, 1.0f, 1.0f, 1.0f, magText); 
    }

    if (isHookFiring || isHooked) {
        float endX = isHookFiring ? hookProjectileX : hookPointX;
        float endY = isHookFiring ? hookProjectileY : hookPointY;
        if (endX < cameraLeft || endX > cameraLeft + VIEW_WIDTH || endY < cameraBottom || endY > cameraBottom + VIEW_HEIGHT) {
            clipLineToView(playerCenterX, playerCenterY, endX, endY, VIEW_WIDTH, VIEW_HEIGHT, cameraLeft, cameraBottom);
        }
        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_LINES); glVertex2f(playerCenterX, playerCenterY); glVertex2f(endX, endY); glEnd();
    }
    
    drawPhysicsDebugHUD();
}

// ---------------------------------------------------------------------------------------------------------------------------------------

/**
 * As funções abaixo se assemelham muito às funções definidas para as máquinas de estados do OpenGL que estão na main. A diferença é que 
 * elas não serão usadas como funções definitivas para o estado, mas sim dentro das funções da main. Elas alteram os valores de variáveis
 * do game. Variáveis essas que serão usados nas funções definidas acima
 */

GameAction gameKeyDown(unsigned char key, int x, int y) {
    switch (key) {
        case 'q': case 'Q': return GAME_ACTION_EXIT_TO_MENU;
        case 'a': case 'A': isPressingLeft = true; break;
        case 'd': case 'D': isPressingRight = true; break;
    }
    return GAME_ACTION_CONTINUE;
}

void gameKeyUp(unsigned char key, int x, int y) {
    switch (key) {
        case 'a': case 'A': isPressingLeft = false; break;
        case 'd': case 'D': isPressingRight = false; break;
    }
}

void gameSpecialDown(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT: isPressingLeft = true; break;
        case GLUT_KEY_RIGHT: isPressingRight = true; break;
    }
}

void gameSpecialUp(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT: isPressingLeft = false; break;
        case GLUT_KEY_RIGHT: isPressingRight = false; break;
    }
}

void gameMouseMotion(int x, int y) {
    int winW = glutGet(GLUT_WINDOW_WIDTH), winH = glutGet(GLUT_WINDOW_HEIGHT);
    if (winW == 0) winW = 1; if (winH == 0) winH = 1;
    mouseGameX = cameraLeft + (float)x / winW * VIEW_WIDTH;
    mouseGameY = cameraBottom + (float)(winH - y) / winH * VIEW_HEIGHT;
}

void gameMouseClick(int button, int state) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            if (!isHooked && !isHookFiring) {
                isHookFiring = true;
                float playerCenterX = player.x + player.w / 2, playerCenterY = player.y + player.h / 2;
                hookProjectileX = playerCenterX; hookProjectileY = playerCenterY;
                
                float vAX = mouseGameX - playerCenterX, vAY = mouseGameY - playerCenterY;
                float rMD = sqrt(vAX * vAX + vAY * vAY);
                float cMD = fmin(rMD, MAX_VISUAL_AIM_LENGTH);
                float fP = cMD / MAX_VISUAL_AIM_LENGTH;
                currentPullForce = fP * MAX_PULL_STRENGTH_PHYSICS;

                if (rMD > 0.01f) {
                    hookProjectileVelX = (vAX / rMD) * HOOK_SPEED;
                    hookProjectileVelY = (vAY / rMD) * HOOK_SPEED;
                } else {
                    hookProjectileVelX = 0; hookProjectileVelY = HOOK_SPEED;
                }
            }
        } 
        else if (state == GLUT_UP) {
            isHooked = false; 
            isHookFiring = false;
        }
    }
}