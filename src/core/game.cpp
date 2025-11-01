/**
 * Por padrão de projeto, todas as variáveis numéricas usadas são explicitamente
 * do tipo float, exceto contadores e variáveis de controle. Isso é feito para
 * garantir a segurança e a linearidade dos resultados calculados.
 */

#include "game.h"

#include <GL/freeglut.h>
#include <stdio.h>

#include "utils.h"
// #include <math.h>
#include <algorithm>
#include <string>
#include <vector>

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

// ----------------------------------------------------------------------------------------------------------------

/**
 * Constantes usadas na definição de mundo e para visualização (câmera, espaço
 * para desenhos, etc) Estas são fixas e não mudam por fase.
 */
const float PLAYER_HEIGHT = 80.0f;
const float VIEW_WIDTH = 800.0f;
const float VIEW_HEIGHT = 600.0f;
const float WORLD_WIDTH = 3000.0f;
const float WORLD_HEIGHT = 600.0f;
const float DEATH_BOUNDARY_Y =
    -100.0f;  // Constante usada para verificar se o personagem "caiu",
              // limitador para sua queda

/**
 * Constantes visuais.
 */
const float MAX_VISUAL_AIM_LENGTH = 5.0f * PLAYER_HEIGHT;
const float MAX_AIM_FORCE_DISPLAY = 100.0f;
const float VELOCITY_VISUAL_SCALE = 7.0f;
const float WIND_VECTOR_VISUAL_SCALE = 500.0f;

// ----------------------------------------------------------------------------------------------------------------

/**
 * Definição das variáveis que serão usadas no game.
 */
GameObject player;
GameObject door;
Platform* collidingPlatform = NULL;

std::vector<Platform> platforms;
std::vector<WindZone> windZones;
std::vector<BreakableWall> breakableWalls;
std::vector<SpikeZone> spikeZones;
std::vector<RiverZone> riverZones;

struct LevelParameters {
  float gravity;
  float playerMass;
  float playerWalkAccel;
  float maxWalkSpeed;
  float maxPlayerSpeed;
  float dampingFactor;
  float hookSpeed;
  float maxPullStrengthPhysics;
  float vectorVisualScale;
  int maxShots;
};

LevelParameters levelParameters;

// int CURRENT_LEVEL = 1;    // Variável de controle do level atual
bool isGrounded = false;  // Variável para verificar se o personagem está em
                          // contato com alguma plataforma
bool isHooked = false;    // Variável para identificar se o personagem está
                          // pendurado em alguma plataforma
bool isHookFiring =
    false;  // Variável para controlar se o gancho está em modo de disparo, ou
            // seja, se ele está em trajetória para se prender em algo
float hookPointX = 0,
      hookPointY = 0;  // Variáveis para indicar em qual ponto de uma plataforma
                       // o personagem está preso
float
    hookProjectileX = 0,
    hookProjectileY =
        0;  // Variáveis para controle das posições X e Y do projétil do gancho
float hookProjectileVelX = 0,
      hookProjectileVelY = 0;  // Variáveis para controle das velocidades em X e
                               // Y do projétil do gancho
float ropeLength = 0;
float currentPullForce = 0.0f;
float mouseGameX = 0, mouseGameY = 0;  // Variáveis para controlar as posições X
                                       // e Y do mouse durante o game
float aimDisplayForce = 0;  // Variável para controle da força de lançamento do
                            // personagem pelo grappling hook
float cameraLeft = 0, cameraBottom = 0;
bool isPressingLeft =
    false;  // Variável para controlar se o personagem está pressionando as
            // teclas de movimentação para a esquerda
bool isPressingRight =
    false;  // Variável para controlar se o personagem está pressionando as
            // teclas de movimentação para a direita
float forceNormal =
    0;  // Variável para controlar a força normal aplicada ao personagem
float forceFriction =
    0;  // Variável para controlar a força de atrito aplicada ao personagem
float forceTensionX =
    0;  // Variável para controlar a posição X da força de tensão na corda
        // aplicada ao personagem e ao fio do grappling hook
float forceTensionY =
    0;  // Variável para controlar a posição Y da força de tensão na corda
        // aplicada ao personagem e ao fio do grappling hook

// Controle de animação do player
bool isFacingRight = true;  // Direção que o player está olhando

/**
 * Variáveis para controle da aceleração do personagem.
 */
float lastVelocityMag = 0.0f;
float currentAcceleration = 0.0f;

/**
 * Controle de tiros e de game over
 */
int shotsRemaining = 0;
bool isGameOver = false;
int gameOverTimer = 0;

/**
 * Controle da tela de vitória
 */
bool isGameVictory = false;
int gameVictoryTimer = 0;

// Variáveis para controle da animação de corrida (Definições)
float runAnimationTimer = 0.0f;
int currentRunFrame = 0;

// Variável para cálculo do Delta Time (tempo decorrido entre frames)
static int lastTime = 0;  // Armazena o tempo da última atualização em ms

// Variável global do nível atual (Definição da variável declarada em game.h)

/**
 * Como parte dos requisitos técnicos, os elementos estáticos das fases serão
 * renderizados em listas de apresentação para fins de otimização das operações
 */
GLuint platformListID = 0;
GLuint doorListID = 0;
GLuint spikeListID = 0;
GLuint windZoneListID = 0;

// ----------------------------------------------------------------------------------------------------------------

void createDisplayLists() {
  if (platformListID != 0) glDeleteLists(platformListID, 1);
  if (windZoneListID != 0) glDeleteLists(windZoneListID, 1);
  if (spikeListID != 0) glDeleteLists(spikeListID, 1);
  if (doorListID != 0) glDeleteLists(doorListID, 1);

  platformListID = glGenLists(1);
  windZoneListID = glGenLists(1);
  spikeListID = glGenLists(1);
  doorListID = glGenLists(1);

  glNewList(platformListID, GL_COMPILE);
  for (size_t i = 0; i < platforms.size(); i++) {
    Platform* platform = &platforms[i];
    glPushMatrix();
    glTranslatef(platform->x, platform->y, 0.0f);
    drawRect(0, 0, platform->w, platform->h, platform->r, platform->g,
             platform->b);
    glColor3f(1.0, 1.0, 1.0);
    drawText(20, 15, ("mu_" + std::to_string(i + 1)).c_str());
    glPopMatrix();
  }
  glEndList();

  glNewList(windZoneListID, GL_COMPILE);
  for (size_t i = 0; i < windZones.size(); i++) {
    WindZone* windZone = &windZones[i];
    glPushMatrix();
    glTranslatef(windZone->x, windZone->y, 0.0f);
    drawRect(0, 0, windZone->w, windZone->h, 1.0f, 1.0f, 1.0f, 0.2f);

    float windMag = sqrt(windZone->accelX * windZone->accelX +
                         windZone->accelY * windZone->accelY);
    if (windMag > 0.001f) {
      char windLabel[50];
      sprintf(windLabel, "Corrente de Vento: %.2f N",
              windMag * levelParameters.playerMass);
      drawVector(windZone->w / 2, windZone->h / 2, windZone->accelX,
                 windZone->accelY, WIND_VECTOR_VISUAL_SCALE, 0.0f, 0.8f, 1.0f,
                 windLabel);
    }
    glPopMatrix();
  }
  glEndList();

  glNewList(spikeListID, GL_COMPILE);
  for (size_t i = 0; i < spikeZones.size(); i++) {
    SpikeZone* spikeZone = &spikeZones[i];
    drawSpikes(spikeZone->x, spikeZone->y, spikeZone->w, spikeZone->h,
               spikeZone->r, spikeZone->g, spikeZone->b);
  }
  glEndList();

  glNewList(doorListID, GL_COMPILE);
  glPushMatrix();
  glTranslatef(door.x, door.y, 0.0f);
  drawRect(0, 0, door.w, door.h, door.r, door.g, door.b);
  glPopMatrix();
  glEndList();
}

/**
 * Essa função é responsável por limpar todos os elementos da fase anterior e
 * estruturar a nova fase, incluindo seus parâmetros de física e movimentação do
 * personagem.
 */

int CURRENT_LEVEL = 1;

void gameStartLevel(int level) {
  platforms.clear();
  windZones.clear();
  breakableWalls.clear();
  spikeZones.clear();
  riverZones.clear();

  CURRENT_LEVEL = level;

  switch (level) {
    case 1:
      levelParameters.gravity = -0.1f;
      levelParameters.playerMass = 10.0f;
      levelParameters.playerWalkAccel = 0.1f;
      levelParameters.maxWalkSpeed = 3.0f;
      levelParameters.maxPlayerSpeed = 30.0f;
      levelParameters.dampingFactor = 0.99f;
      levelParameters.hookSpeed = 25.0f;
      levelParameters.maxPullStrengthPhysics = 0.4f;
      levelParameters.vectorVisualScale =
          MAX_VISUAL_AIM_LENGTH / levelParameters.maxPullStrengthPhysics;
      levelParameters.maxShots = 8;

      // Chão (evita o rio)
      platforms.push_back({0, 0, 700, 40, 0.2f, 0.6f, 0.2f, true, 0.8f});
      platforms.push_back(
          {1100, 0, WORLD_WIDTH - 1100, 40, 0.2f, 0.6f, 0.2f, true, 0.8f});

      // Plataformas suspensas
      platforms.push_back({400, 250, 200, 50, 0.4f, 0.4f, 0.4f, true, 0.2f});
      platforms.push_back({750, 350, 250, 50, 0.4f, 0.4f, 0.4f, true, 0.15f});
      platforms.push_back({1200, 280, 200, 50, 0.4f, 0.4f, 0.4f, true, 0.25f});
      platforms.push_back({1600, 400, 300, 50, 0.4f, 0.4f, 0.4f, true, 0.1f});
      platforms.push_back({2100, 320, 250, 50, 0.4f, 0.4f, 0.4f, true, 0.2f});

      // Rio mortal
      riverZones.push_back({700, 0, 400, 40});

      // Corrente de vento
      windZones.push_back({1000, 40, 120, 350, 0.0f, 0.06f});
      windZones.push_back({2000, 40, 100, 300, -0.04f, 0.0f});

      // Paredes quebráveis
      breakableWalls.push_back(
          {1400, 40, 40, 180, 0.6f, 0.4f, 0.2f, 40.0f, false});
      breakableWalls.push_back(
          {2400, 40, 40, 200, 0.6f, 0.4f, 0.2f, 30.0f, false});

      // Espinhos no chão
      spikeZones.push_back({200, 40, 150, 30, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({1500, 40, 200, 30, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({2200, 40, 180, 30, 1.0f, 0.0f, 0.0f});

      // Espinhos nas plataformas (teto)
      spikeZones.push_back({400, 290, 200, 30, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({1600, 440, 300, 30, 1.0f, 0.0f, 0.0f});

      player = {50, platforms[0].h, 40, PLAYER_HEIGHT, 0.9f, 0.1f, 0.1f, 0, 0};
      door = {WORLD_WIDTH - 150, platforms[0].h, 50, 100, 0.5f, 0.3f, 0.0f};
      break;

    case 2:
      levelParameters.gravity = -0.08f;
      levelParameters.playerMass = 8.0f;
      levelParameters.playerWalkAccel = 0.12f;
      levelParameters.maxWalkSpeed = 4.0f;
      levelParameters.maxPlayerSpeed = 35.0f;
      levelParameters.dampingFactor = 0.995f;
      levelParameters.hookSpeed = 28.0f;
      levelParameters.maxPullStrengthPhysics = 0.35f;
      levelParameters.vectorVisualScale =
          MAX_VISUAL_AIM_LENGTH / levelParameters.maxPullStrengthPhysics;
      levelParameters.maxShots = 6;

      // Chão fragmentado (evita rios)
      platforms.push_back({0, 0, 500, 40, 0.1f, 0.1f, 0.8f, true, 0.5f});
      platforms.push_back({850, 0, 400, 40, 0.1f, 0.1f, 0.8f, true, 0.5f});
      platforms.push_back(
          {1600, 0, WORLD_WIDTH - 1600, 40, 0.1f, 0.1f, 0.8f, true, 0.5f});

      // Plataformas suspensas
      platforms.push_back({250, 220, 180, 60, 0.5f, 0.5f, 0.5f, true, 0.15f});
      platforms.push_back({550, 350, 200, 60, 0.5f, 0.5f, 0.5f, true, 0.1f});
      platforms.push_back({900, 280, 220, 60, 0.5f, 0.5f, 0.5f, true, 0.2f});
      platforms.push_back({1300, 400, 250, 60, 0.5f, 0.5f, 0.5f, true, 0.12f});
      platforms.push_back({1700, 320, 200, 60, 0.5f, 0.5f, 0.5f, true, 0.18f});
      platforms.push_back({2100, 450, 280, 60, 0.5f, 0.5f, 0.5f, true, 0.08f});
      platforms.push_back({2500, 280, 200, 60, 0.5f, 0.5f, 0.5f, true, 0.15f});

      // Rios mortais
      riverZones.push_back({500, 0, 350, 40});
      riverZones.push_back({1250, 0, 350, 40});

      // Correntes de vento
      windZones.push_back({800, 40, 150, 400, 0.0f, 0.07f});
      windZones.push_back({2200, 40, 120, 350, 0.05f, 0.0f});

      // Paredes quebráveis
      breakableWalls.push_back(
          {1100, 40, 35, 120, 0.6f, 0.4f, 0.2f, 18.0f, false});
      breakableWalls.push_back(
          {2000, 40, 35, 150, 0.6f, 0.4f, 0.2f, 20.0f, false});

      // Espinhos no chão
      spikeZones.push_back({150, 40, 120, 30, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({1650, 40, 250, 30, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({2400, 40, 200, 30, 1.0f, 0.0f, 0.0f});

      // Espinhos nas plataformas
      spikeZones.push_back({250, 270, 180, 25, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({900, 330, 220, 25, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({1700, 370, 200, 25, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({2100, 500, 280, 25, 1.0f, 0.0f, 0.0f});

      player = {80, platforms[0].h, 40, PLAYER_HEIGHT, 0.9f, 0.1f, 0.1f, 0, 0};
      door = {WORLD_WIDTH - 120, platforms[0].h, 50, 100, 0.5f, 0.3f, 0.0f};
      break;

    case 3:
      levelParameters.gravity = -0.06f;
      levelParameters.playerMass = 6.0f;
      levelParameters.playerWalkAccel = 0.15f;
      levelParameters.maxWalkSpeed = 5.0f;
      levelParameters.maxPlayerSpeed = 40.0f;
      levelParameters.dampingFactor = 0.998f;
      levelParameters.hookSpeed = 32.0f;
      levelParameters.maxPullStrengthPhysics = 0.28f;
      levelParameters.vectorVisualScale =
          MAX_VISUAL_AIM_LENGTH / levelParameters.maxPullStrengthPhysics;
      levelParameters.maxShots = 5;

      // Chão muito fragmentado
      platforms.push_back({0, 0, 400, 40, 0.3f, 0.3f, 0.3f, true, 0.3f});
      platforms.push_back({750, 0, 350, 40, 0.3f, 0.3f, 0.3f, true, 0.3f});
      platforms.push_back({1450, 0, 300, 40, 0.3f, 0.3f, 0.3f, true, 0.3f});
      platforms.push_back(
          {2100, 0, WORLD_WIDTH - 2100, 40, 0.3f, 0.3f, 0.3f, true, 0.3f});

      // Plataformas suspensas desafiadoras
      platforms.push_back({300, 280, 150, 50, 0.6f, 0.4f, 0.2f, true, 0.08f});
      platforms.push_back({600, 420, 180, 50, 0.6f, 0.4f, 0.2f, true, 0.05f});
      platforms.push_back({900, 320, 160, 50, 0.6f, 0.4f, 0.2f, true, 0.1f});
      platforms.push_back({1200, 450, 200, 50, 0.6f, 0.4f, 0.2f, true, 0.06f});
      platforms.push_back({1550, 350, 180, 50, 0.6f, 0.4f, 0.2f, true, 0.09f});
      platforms.push_back({1850, 480, 220, 50, 0.6f, 0.4f, 0.2f, true, 0.04f});
      platforms.push_back({2250, 360, 200, 50, 0.6f, 0.4f, 0.2f, true, 0.07f});
      platforms.push_back({2600, 260, 180, 50, 0.6f, 0.4f, 0.2f, true, 0.1f});

      // Rios mortais (sem chão)
      riverZones.push_back({400, 0, 350, 40});
      riverZones.push_back({1100, 0, 350, 40});
      riverZones.push_back({1750, 0, 350, 40});

      // Correntes de vento perigosas
      windZones.push_back({850, 40, 180, 450, 0.0f, 0.09f});
      windZones.push_back({1400, 40, 150, 400, -0.06f, 0.0f});
      windZones.push_back({2400, 40, 180, 450, 0.0f, 0.08f});

      // Paredes quebráveis resistentes
      breakableWalls.push_back(
          {1300, 40, 25, 140, 0.6f, 0.4f, 0.2f, 15.0f, false});
      breakableWalls.push_back(
          {2100, 40, 25, 160, 0.6f, 0.4f, 0.2f, 18.0f, false});

      // Muitos espinhos no chão
      spikeZones.push_back({550, 40, 180, 30, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({1500, 40, 200, 30, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({2150, 40, 220, 30, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({2650, 40, 180, 30, 1.0f, 0.0f, 0.0f});

      // Espinhos nas plataformas (teto e piso)
      spikeZones.push_back({300, 320, 150, 25, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({600, 465, 180, 25, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({900, 365, 160, 25, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({1200, 495, 200, 25, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({1850, 525, 220, 25, 1.0f, 0.0f, 0.0f});
      spikeZones.push_back({2250, 405, 200, 25, 1.0f, 0.0f, 0.0f});

      player = {120, platforms[0].h, 40, PLAYER_HEIGHT, 0.9f, 0.1f, 0.1f, 0, 0};
      door = {WORLD_WIDTH - 100, platforms[0].h, 50, 100, 0.5f, 0.3f, 0.0f};
      break;

    default:
      gameStartLevel(1);
      return;
  }

  if (!platforms.empty()) {
    collidingPlatform = &platforms[0];
    isGrounded = true;
  } else {
    collidingPlatform = NULL;
    isGrounded = false;
  }

  isHooked = false;
  isHookFiring = false;
  isPressingLeft = false;
  isPressingRight = false;
  lastVelocityMag = 0.0f;
  currentAcceleration = 0.0f;
  isGameOver = false;
  shotsRemaining = levelParameters.maxShots;

  for (size_t i = 0; i < breakableWalls.size(); i++) {
    breakableWalls[i].isBroken = false;
  }

  createDisplayLists();
}

/**
 * Função responsável por definir a área de desenho do OpenGL
 */
void gameReshape(int width, int height) { glViewport(0, 0, width, height); }

/**
 * Todas as atualizações/interações do game são controladas por essa função.
 * Nela foram definidas todas as regras de lógica que escolhemos para a base de
 * funcionamento da movimentação do jogo e de como o personagem interage com os
 * objetos.
 */
GameAction gameUpdate() {
  if (isGameVictory) {
    gameVictoryTimer--;
    if (gameVictoryTimer <= 0) {
      isGameVictory = false;
      return GAME_ACTION_LEVEL_WON;
    }
    return GAME_ACTION_CONTINUE;
  }

  if (isGameOver) {
    gameOverTimer--;
    if (gameOverTimer <= 0) {
      isGameOver = false;
      return GAME_ACTION_LEVEL_LOST;
    }
    return GAME_ACTION_CONTINUE;
  }

  // --------------------------------------- Cálculos de velocidade
  // -------------------------------------------------

  float prevPositionPlayerX = player.x;
  float prevPositionPlayerY = player.y;

  forceNormal = 0;
  forceFriction = 0;
  forceTensionX = 0;
  forceTensionY = 0;
  float playerAccelerationX = 0, playerAccelerationY = levelParameters.gravity;

  /**
   * A força de vento aplica uma aceleração extra ao personagem ao entrar dentro
   * da zona definida. Para isso, é usada a função de verificação de colisão de
   * pontos e, além disso, a aceleração incrementada ao personagem é definida
   * pela força da zona de vento
   */
  for (size_t i = 0; i < windZones.size(); i++) {
    WindZone* windZone = &windZones[i];
    if (isPointInside(player.x + player.w / 2, player.y + player.h / 2,
                      windZone->x, windZone->y, windZone->w, windZone->h)) {
      playerAccelerationX += windZone->accelX;
      playerAccelerationY += windZone->accelY;
    }
  }

  /**
   * O gancho (grappling hook) se diferencia de uma teia do homem-aranha pois
   * ela puxa o personagem na direção do lançamento, com a mesma força escolhida
   * por ele ao se lançar. Para simular esse comportamento, ao se prender, a
   * seguinte função calcula a tensão na corda aplicada ao personagem quando ele
   * está preso
   */
  if (isHooked) {
    float playerCenterX = player.x + player.w / 2,
          playerCenterY = player.y + player.h / 2;
    float vecToHookX = hookPointX - playerCenterX,
          vecToHookY = hookPointY - playerCenterY;
    float dist = sqrt(vecToHookX * vecToHookX + vecToHookY * vecToHookY);
    if (dist > 0.01f) {
      float normX = vecToHookX / dist, normY = vecToHookY / dist;
      playerAccelerationX += normX * currentPullForce;
      playerAccelerationY += normY * currentPullForce;
      forceTensionX = normX * currentPullForce;
      forceTensionY = normY * currentPullForce;
    }
  }

  /**
   * Movimentação do personagem (definida pela constante de aceleração ao andar)
   * no chão.
   */
  if (isGrounded) {
    if (isPressingLeft) {
      playerAccelerationX -= levelParameters.playerWalkAccel;
      isFacingRight = false;
    }
    if (isPressingRight) {
      playerAccelerationX += levelParameters.playerWalkAccel;
      isFacingRight = true;
    }

    if (collidingPlatform) {
      forceNormal = -levelParameters.gravity;
      float frictionAccel =
          collidingPlatform->frictionCoefficient * forceNormal;
      if (fabs(player.velocityX + playerAccelerationX) < frictionAccel) {
        player.velocityX = 0;
        playerAccelerationX = 0;
        forceFriction = 0;
      } else {
        if (player.velocityX > 0) {
          playerAccelerationX -= frictionAccel;
          forceFriction = -frictionAccel;
        } else if (player.velocityX < 0) {
          playerAccelerationX += frictionAccel;
          forceFriction = frictionAccel;
        }
      }
    }
  }

  /// --- CÁLCULO DO DELTA TIME (NOVO CÓDIGO) ---
  int currentTime = glutGet(GLUT_ELAPSED_TIME);
  // O deltaTime (em segundos) é o tempo decorrido / 1000.0f
  float deltaTime = (float)(currentTime - lastTime) / 1000.0f;
  lastTime = currentTime;

  // --- O RESTO DA SUA LÓGICA DE FÍSICA AQUI (usando 'deltaTime') ---
  // ... (Sua lógica existente de player.x += player.velocityX * deltaTime,
  // etc.)

  // Lógica de Animação de Corrida (agora usa o 'deltaTime' calculado)
  float velMag = fabs(player.velocityX);

  // Condição para iniciar/manter a animação: está no chão E a velocidade é
  // notável
  if (isGrounded && velMag > 0.5f) {
    // Multiplica por 1000.0f para usar o timer em milissegundos
    runAnimationTimer += deltaTime * 1000.0f;

    if (runAnimationTimer >= RUN_ANIMATION_SPEED_MS) {
      // Alterna o frame: se 0, vira 1; se 1, vira 0.
      currentRunFrame = 1 - currentRunFrame;
      runAnimationTimer = 0.0f;
    }
  } else {
    // Se parou, reseta o timer e frame para o estado inicial (parado ou Frame
    // 0)
    runAnimationTimer = 0.0f;
    currentRunFrame = 0;
  }

  /**
   * Os valores calculados acima (gerados pelas possíveis interações com os
   * objetos) são incrementadas ao valor da velocidade do personagem
   */
  player.velocityX += playerAccelerationX;
  player.velocityY += playerAccelerationY;

  /**
   * Caso esteja no chão, o personagem não pode acelerar indefinidamente.
   * Portanto, sua velocidade final calculada
   */
  if (isGrounded && fabs(player.velocityX) > levelParameters.maxWalkSpeed) {
    player.velocityX = (player.velocityX > 0) ? levelParameters.maxWalkSpeed
                                              : -levelParameters.maxWalkSpeed;
  }

  /**
   * Para fins de visualização, também foi definida uma velocidade máxima para o
   * personagem, visando manter a proposta educativa do game.
   */
  float speed = sqrt(player.velocityX * player.velocityX +
                     player.velocityY * player.velocityY);
  if (speed > levelParameters.maxPlayerSpeed) {
    player.velocityX =
        (player.velocityX / speed) * levelParameters.maxPlayerSpeed;
    player.velocityY =
        (player.velocityY / speed) * levelParameters.maxPlayerSpeed;
  }

  player.x += player.velocityX;
  player.y += player.velocityY;

  // --------------------------------------- Cálculos de colisão
  // -------------------------------------------------

  /**
   * Para dificultar os controles de lançamento, o personagem não deve
   * ultrapassar os limites de mundo. Para isso, a seguinte verificação visa
   * encerrar o game com derrota caso os limites estabelecidos sejam
   * ultrapassados
   */
  if (player.x < -player.w || player.x > WORLD_WIDTH ||
      player.y < DEATH_BOUNDARY_Y) {
    isGameOver = true;
    gameOverTimer = 180;  // Aproximadamente 3 segundos a 60 FPS
    return GAME_ACTION_CONTINUE;
  }

  /**
   * Verificação da colisão com os espinhos (ainda por meio de colisão de
   * retângulos)
   */
  for (size_t i = 0; i < spikeZones.size(); i++) {
    SpikeZone* spikeZone = &spikeZones[i];
    if (checkRectangleCollision(player.x, player.y, player.w, player.h,
                                spikeZone->x, spikeZone->y, spikeZone->w,
                                spikeZone->h)) {
      isGameOver = true;
      gameOverTimer = 180;  // Aproximadamente 3 segundos a 60 FPS
      return GAME_ACTION_CONTINUE;
    }
  }

  /**
   * Verificação da colisão com os rios (morte instantânea)
   */
  for (size_t i = 0; i < riverZones.size(); i++) {
    RiverZone* riverZone = &riverZones[i];
    if (checkRectangleCollision(player.x, player.y, player.w, player.h,
                                riverZone->x, riverZone->y, riverZone->w,
                                riverZone->h)) {
      isGameOver = true;
      gameOverTimer = 180;
      return GAME_ACTION_CONTINUE;
    }
  }

  /**
   * Para o cálculo das paredes quebráveis, além de gerar uma colisão, ela
   * também deve afetar a velocidade final do personagem (a sua velocidade deve
   * sofrer interferência da força por ele deixada ao colidir com a parede).
   * Portanto, essa função visa verificar a colisão e, além disso, alterar a
   * velocidade do personagem
   */
  for (size_t i = 0; i < breakableWalls.size(); i++) {
    BreakableWall* breakableWall = &breakableWalls[i];
    if (breakableWall->isBroken) continue;

    if (checkRectangleCollision(player.x, player.y, player.w, player.h,
                                breakableWall->x, breakableWall->y,
                                breakableWall->w, breakableWall->h)) {
      float impactVelX = player.velocityX;
      float impactVelY = player.velocityY;
      float impactSpeed =
          sqrt(impactVelX * impactVelX + impactVelY * impactVelY);

      /**
       * Pela lei de Newton, a força F aplicada em algo é a massa vezes a sua
       * velocidade
       */
      float impactForce = levelParameters.playerMass * impactSpeed;

      if (impactForce >= breakableWall->strength) {  // Quebra a parede
        breakableWall->isBroken = true;
        player.velocityX *= 0.7f;
        player.velocityY *= 0.7f;

        /**
         * Caso a força não seja suficiente para atravessar a parede, o
         * personagem deve parar na posição imediatamente anterior antes da
         * colisão (posição do frame anterior)
         */
      } else {
        player.x = prevPositionPlayerX;
        player.y = prevPositionPlayerY;
        player.velocityX = 0;
        player.velocityY = 0;
      }
    }
  }

  /**
   * Caso o personagem colida com alguma plataforma, ele deve se desprender
   * imediatamente
   */
  isGrounded = false;
  collidingPlatform = NULL;
  for (size_t i = 0; i < platforms.size(); i++) {
    Platform* platform = &platforms[i];
    if (checkRectangleCollision(player.x, player.y, player.w, player.h,
                                platform->x, platform->y, platform->w,
                                platform->h)) {
      /**
       * Caso colida com alguma plataforma, a seguinte verificação serve para
       * tratar os casos de colisão superior e colisão inferior, pois, na
       * estruturação feita, o chão também é uma plataforma
       */
      if (player.velocityY > 0 && (player.y + player.h) > platform->y &&
          player.y < platform->y) {
        player.y = platform->y - player.h;
        player.velocityY = 0;
        if (isHooked) isHooked = false;
      } else if (player.velocityY <= 0 &&
                 player.y < (platform->y + platform->h) &&
                 (player.y + player.h) > (platform->y + platform->h)) {
        player.y = platform->y + platform->h;
        isGrounded = true;
        collidingPlatform = platform;
        forceNormal = -levelParameters.gravity - (player.velocityY * 0.5f);
        player.velocityY = 0;
        if (isHooked) isHooked = false;
      }
    }
  }

  /**
   * A seguinte verificação visa implementar a lógica de velocidade do
   * personagem ao se prender com o gancho. Dada a complexidade das operações,
   * buscamos a implementação dessa física para o cálculo das posições.
   */
  if (isHooked) {
    float playerCenterX = player.x + player.w / 2;
    float playerCenterY = player.y + player.h / 2;
    float vectorToHookX = hookPointX - playerCenterX;
    float vectorToHookY = hookPointY - playerCenterY;
    float currentDistance =
        sqrt(vectorToHookX * vectorToHookX + vectorToHookY * vectorToHookY);

    if (currentDistance > ropeLength && currentDistance > 0.01f) {
      float stretchAmount = currentDistance - ropeLength;
      float normalizedDirectionX = vectorToHookX / currentDistance;
      float normalizedDirectionY = vectorToHookY / currentDistance;

      player.x += normalizedDirectionX * stretchAmount;
      player.y += normalizedDirectionY * stretchAmount;

      float velocityAlongRope = player.velocityX * normalizedDirectionX +
                                player.velocityY * normalizedDirectionY;
      if (velocityAlongRope > 0) {
        player.velocityX -= normalizedDirectionX * velocityAlongRope;
        player.velocityY -= normalizedDirectionY * velocityAlongRope;
      }
    }

    /**
     * Reduz lentamente a velocidade para que o balanço pare eventualmente
     * (damping)
     */
    player.velocityX *= levelParameters.dampingFactor;
    player.velocityY *= levelParameters.dampingFactor;

    isGrounded = false;
  }

  /**
   * Essa função verifica se o gancho está em modo de disparo (sendo lançado) e,
   * caso sim, verifica o ponto em que o jogador deve se prender para ser
   * lançado por meio de variáveis de retorno na chamada da função (hitX e
   * hitY). Para isso, é usada a função utilitária de interseção entre uma linha
   * e um retângulo
   */
  if (isHookFiring) {
    float prevHookX = hookProjectileX, prevHookY = hookProjectileY;
    hookProjectileX += hookProjectileVelX;
    hookProjectileY += hookProjectileVelY;

    bool hit = false;
    for (size_t i = 0; i < platforms.size(); i++) {
      Platform* p = &platforms[i];
      float hitX, hitY;
      // Para simular um comportamento físico, o gancho não se prende em atrito
      // 0
      if (p->isHookable && p->frictionCoefficient > 0.0f &&
          lineRectIntersection(prevHookX, prevHookY, hookProjectileX,
                               hookProjectileY, *p, hitX, hitY)) {
        isHookFiring = false;
        isHooked = true;
        hookPointX = hitX;
        hookPointY = hitY;
        float dx = hookPointX - (player.x + player.w / 2),
              dy = hookPointY - (player.y + player.h / 2);
        ropeLength = sqrt(dx * dx + dy * dy);
        isGrounded = false;
        hit = true;
        break;
      }
    }

    /**
     * O tamanho do gancho só possui os limites do tamanho da tela. Caso passe
     * dos limites de visualização, o lançamento deve ser cancelado
     */
    if (!hit) {
      if (hookProjectileX < cameraLeft ||
          hookProjectileX > cameraLeft + VIEW_WIDTH ||
          hookProjectileY < cameraBottom ||
          hookProjectileY > cameraBottom + VIEW_HEIGHT) {
        isHookFiring = false;
      }
    }
  }

  float currentVelocityMag = sqrt(player.velocityX * player.velocityX +
                                  player.velocityY * player.velocityY);
  currentAcceleration = currentVelocityMag - lastVelocityMag;
  lastVelocityMag = currentVelocityMag;

  // if (shotsRemaining <= 0 && !isGameOver) {
  //   isGameOver = true;
  //   gameOverTimer = 180;
  //   return GAME_ACTION_CONTINUE;
  // }

  /**
   * Condição de vitória: Caso o personagem alcançe a porta
   */
  if (checkRectangleCollision(player.x, player.y, player.w, player.h, door.x,
                              door.y, door.w, door.h)) {
    isGameVictory = true;
    gameVictoryTimer = 180;
    return GAME_ACTION_CONTINUE;
  }

  /**
   * Caso nenhuma das condições seja atendida nas verificações dessa função, o
   * game deve apenas continuar o fluxo comum e passar para a próxima cena
   */
  return GAME_ACTION_CONTINUE;
}

/**
 * Para mostrar todos os valores dos vetores na caixa de visualização, são
 * necessárias diversas variáveis de controle e operações matemáticas para
 * garantir a coerência dos dados. Além de definir uma escala fixa para
 * facilitar a jogabilidade do usuário, também é necessário que os valores dos
 * vetores estejam coerentes.
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
  float padding = 20.0f;

  std::vector<std::string> infoLines;
  char buffer[100];

  float playerVelocity = sqrt(player.velocityX * player.velocityX +
                              player.velocityY * player.velocityY);
  sprintf(buffer, "v: %.1f m/s", playerVelocity);
  infoLines.push_back(buffer);

  sprintf(buffer, "a: %.2f m/s^2", currentAcceleration);
  infoLines.push_back(buffer);

  sprintf(buffer, "P: %.2f N",
          levelParameters.gravity * levelParameters.playerMass);
  infoLines.push_back(buffer);

  if (isGrounded) {
    sprintf(buffer, "N: %.2f N", forceNormal * levelParameters.playerMass);
    infoLines.push_back(buffer);
    if (fabs(forceFriction) > 0.001f) {
      sprintf(buffer, "Fat: %.2f N",
              fabs(forceFriction) * levelParameters.playerMass);
      infoLines.push_back(buffer);
    }
  }
  if (isHooked) {
    sprintf(buffer, "T: %.2f N", currentPullForce * levelParameters.playerMass);
    infoLines.push_back(buffer);
  }

  for (size_t i = 0; i < platforms.size(); ++i) {
    Platform* platform = &platforms[i];
    if (platform->x < cameraLeft + VIEW_WIDTH &&
        platform->x + platform->w > cameraLeft) {
      sprintf(buffer, "mu_%zu: %.2f", i + 1, platform->frictionCoefficient);
      infoLines.push_back(buffer);
    }
  }

  float maxWidth = 0;
  for (const auto& line : infoLines) {
    maxWidth = std::max(maxWidth,
                        (float)getTextWidth(line.c_str(), GLUT_BITMAP_9_BY_15));
  }

  float blockWidth = maxWidth + (2 * padding) + 10.0f;
  float blockHeight = (infoLines.size() * lineHeight) + (2 * padding) + 5.0f;
  float blockPositionX = glutGet(GLUT_WINDOW_WIDTH) - margin - blockWidth;
  float blockPositionY = margin;

  // NOVO: Desenha textura de fundo se disponível
  if (texDisplayGrappler != 0) {
    drawTexturedRect(blockPositionX, blockPositionY, blockWidth, blockHeight,
                     texDisplayGrappler, false, false);
  } else {
    // Fallback: fundo preto translúcido
    drawRect(blockPositionX, blockPositionY, blockWidth, blockHeight, 0.0f,
             0.0f, 0.0f, 0.7f);
  }

  float currentYForWrite = blockPositionY + padding + 10;
  for (const auto& line : infoLines) {
    float xPositionOfText = blockPositionX + padding;
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(xPositionOfText, currentYForWrite, line.c_str(),
             GLUT_BITMAP_9_BY_15);
    currentYForWrite += lineHeight;
  }

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

/**
 * Função responsável por desenhar a tela de game over
 */
void drawGameOverScreen() {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  // Configura a projeção para 2D (coordenadas de tela)
  gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // Desenha a textura de Game Over
  drawTexturedRect(0, 0, glutGet(GLUT_WINDOW_WIDTH),
                   glutGet(GLUT_WINDOW_HEIGHT), texGameOver, false, false);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

/**
 * Função responsável por desenhar a tela de vitória
 */
void drawGameVictoryScreen() {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  // Configura a projeção para 2D (coordenadas de tela)
  gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // Desenha a textura de Vitória
  drawTexturedRect(0, 0, glutGet(GLUT_WINDOW_WIDTH),
                   glutGet(GLUT_WINDOW_HEIGHT), texWinGame, false, false);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

/**
 * Essa função é responsável por desenhar o HUD de quantidade de disparos
 */
void drawShotCounterHUD() {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  char shotText[50];
  sprintf(shotText, "Disparos: %d / %d",
          shotsRemaining >= 0 ? shotsRemaining : 0, levelParameters.maxShots);

  float textWidth = getTextWidth(shotText, GLUT_BITMAP_9_BY_15);
  float padding = 18.0f;
  float blockWidth = textWidth + (2 * padding) + 20.0f;
  float blockHeight = 6.0f + (2 * padding) + 6.0f;
  float blockX = glutGet(GLUT_WINDOW_WIDTH) / 2.0f - blockWidth / 2.0f;
  float blockY = 10.0f;

  // NOVO: Desenha textura de fundo se disponível
  if (texDisplayGrappler != 0) {
    drawTexturedRect(blockX, blockY, blockWidth, blockHeight,
                     texDisplayGrappler, false, false);
  } else {
    // Fallback: fundo preto translúcido
    drawRect(blockX, blockY, blockWidth, blockHeight, 0.0f, 0.0f, 0.0f, 0.7f);
  }

  glColor3f(1.0f, 1.0f, 1.0f);
  drawTextCentered(glutGet(GLUT_WINDOW_WIDTH) / 2.0f, blockY + padding + 10,
                   shotText, GLUT_BITMAP_9_BY_15);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

/**
 * Essa função é a "main" do jogo. Ela é responsável por desenhar todos os
 * elementos da cena, além disso, também trata corretamente algumas das
 * variáveis globais e os parâmetros corretos para as funções. As chamadas das
 * funções não serão explicadas detalhadamente na função pois as funções menores
 * estão documentadas. Em suma, a função de display do game usa as primitivas
 * para desenhar alguns elementos e usa funções prontas para outros desenhos
 * mais complexos
 */
void gameDisplay() {
  glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  if (!isGameOver) {
    cameraLeft = player.x + player.w / 2 - VIEW_WIDTH / 2;
    if (cameraLeft < 0) cameraLeft = 0;
    if (cameraLeft + VIEW_WIDTH > WORLD_WIDTH)
      cameraLeft = WORLD_WIDTH - VIEW_WIDTH;
    cameraBottom = 0;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(cameraLeft, cameraLeft + VIEW_WIDTH, cameraBottom,
               cameraBottom + VIEW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Desenha o fundo
    drawTexturedRect(cameraLeft, cameraBottom, VIEW_WIDTH, VIEW_HEIGHT,
                     texBackground);

    // Desenha plataformas com textura
    for (size_t i = 0; i < platforms.size(); i++) {
      Platform* platform = &platforms[i];

      // Plataforma 0 é o chão
      if (i == 0) {
        drawTexturedRect(platform->x, platform->y, platform->w, platform->h,
                         texFloor);
      } else {
        drawTexturedRect(platform->x, platform->y, platform->w, platform->h,
                         texPlatform);
      }

      // Desenha o coeficiente de atrito
      glColor3f(1.0, 1.0, 1.0);
      drawText(platform->x + 20, platform->y + 15,
               ("mu_" + std::to_string(i + 1)).c_str());
    }

    // Desenha rios
    for (size_t i = 0; i < riverZones.size(); i++) {
      RiverZone* riverZone = &riverZones[i];
      drawTexturedRect(riverZone->x, riverZone->y, riverZone->w, riverZone->h,
                       texRiver);
    }

    // Desenha windZones
    glCallList(windZoneListID);

    // Desenha spikes com textura
    for (size_t i = 0; i < spikeZones.size(); i++) {
      SpikeZone* spikeZone = &spikeZones[i];
      drawTexturedRect(spikeZone->x, spikeZone->y, spikeZone->w, spikeZone->h,
                       texObstacleBottom);
    }

    // Desenha breakable walls
    for (size_t i = 0; i < breakableWalls.size(); i++) {
      BreakableWall* breakableWall = &breakableWalls[i];
      if (!breakableWall->isBroken) {
        glPushMatrix();
        glTranslatef(breakableWall->x, breakableWall->y, 0.0f);
        drawRect(0, 0, breakableWall->w, breakableWall->h, breakableWall->r,
                 breakableWall->g, breakableWall->b);

        glColor3f(1.0f, 1.0f, 1.0f);
        char strengthText[50];
        sprintf(strengthText, "Força: %.0f N", breakableWall->strength);
        int textWidth = getTextWidth(strengthText, GLUT_BITMAP_9_BY_15);
        drawText(breakableWall->w / 2 - textWidth / 2, breakableWall->h / 2 + 5,
                 strengthText, GLUT_BITMAP_9_BY_15);
        glPopMatrix();
      }
    }

    // Desenha porta
    drawTexturedRect(door.x, door.y, door.w, door.h, texDoor);

    // Desenha player com animação
    float playerCenterX = player.x + player.w / 2;
    float playerCenterY = player.y + player.h / 2;

    // Seleciona a textura do player
    GLuint currentPlayerTexture = texPlayer;
    bool flipPlayer = false;

    if (isGameOver) {
      currentPlayerTexture = texPlayerLose;
    } else if (isHooked || isHookFiring) {
      currentPlayerTexture = texPlayerJump;
      flipPlayer = !isFacingRight;  // Mantém a orientação no pulo/gancho
    } else if (isGrounded) {
      float velMag = fabs(player.velocityX);

      if (velMag > 0.5f) {
        // --- ESTÁ CORRENDO: Implementa a alternância de frames ---

        if (currentRunFrame == 0) {
          // Frame 0: Perna A na frente (texPlayerRunLeft)
          currentPlayerTexture = texPlayerRunLeft;
        } else {
          // Frame 1: Perna B na frente (texPlayerRunRight)
          currentPlayerTexture = texPlayerRunRight;
        }

        // Aplica a inversão (flip) se estiver correndo para a esquerda
        flipPlayer = !isFacingRight;

      } else {
        // Está parado - usa textura base
        currentPlayerTexture = texPlayer;
        flipPlayer = !isFacingRight;
      }
    } else {
      // No ar (não hooked)
      currentPlayerTexture = texPlayerJump;
      flipPlayer = !isFacingRight;
    }

    // Desenha o player
    drawTexturedRect(player.x, player.y, player.w, player.h,
                     currentPlayerTexture, flipPlayer);

    // Desenha vetores físicos
    glPushMatrix();
    glTranslatef(playerCenterX, playerCenterY, 0.0f);

    drawVector(0, 0, 0, levelParameters.gravity,
               levelParameters.vectorVisualScale, 0.2f, 0.2f, 1.0f, "P");

    float velMag = sqrt(player.velocityX * player.velocityX +
                        player.velocityY * player.velocityY);
    if (velMag > 0.01) {
      drawVector(0, 0, player.velocityX, player.velocityY,
                 VELOCITY_VISUAL_SCALE, 1.0f, 0.5f, 0.0f, "V");
    }
    if (isHooked) {
      drawVector(0, 0, forceTensionX, forceTensionY,
                 levelParameters.vectorVisualScale, 1.0f, 0.0f, 1.0f, "T");
    }
    if (isGrounded) {
      drawVector(0, -player.h / 2, 0, forceNormal,
                 levelParameters.vectorVisualScale, 0.0f, 1.0f, 1.0f, "N");
      if (fabs(forceFriction) > 0.001f) {
        drawVector(0, -player.h / 2 + 5, forceFriction, 0,
                   levelParameters.vectorVisualScale, 1.0f, 0.0f, 0.0f, "Fat");
      }
    }

    glPopMatrix();

    // Desenha mira do grappling hook
    float vAX = mouseGameX - playerCenterX, vAY = mouseGameY - playerCenterY;
    float rMD = sqrt(vAX * vAX + vAY * vAY);
    float cMD = fmin(rMD, MAX_VISUAL_AIM_LENGTH);
    aimDisplayForce = (cMD / MAX_VISUAL_AIM_LENGTH) * MAX_AIM_FORCE_DISPLAY;
    float vANX = 0, vANY = 0;
    if (rMD > 0.01f) {
      vANX = vAX / rMD;
      vANY = vAY / rMD;
    }

    float forcePercent = cMD / MAX_VISUAL_AIM_LENGTH;
    float aimPhysicsX =
        vANX * forcePercent * levelParameters.maxPullStrengthPhysics;
    float aimPhysicsY =
        vANY * forcePercent * levelParameters.maxPullStrengthPhysics;

    if (!isHooked && !isHookFiring) {
      char magText[50];
      sprintf(magText, "Forca: %.0f", aimDisplayForce);
      drawVector(playerCenterX, playerCenterY, aimPhysicsX, aimPhysicsY,
                 levelParameters.vectorVisualScale, 1.0f, 1.0f, 1.0f, magText);
    }

    // Desenha corda do grappling hook
    if (isHookFiring || isHooked) {
      float endX = isHookFiring ? hookProjectileX : hookPointX;
      float endY = isHookFiring ? hookProjectileY : hookPointY;
      if (endX < cameraLeft || endX > cameraLeft + VIEW_WIDTH ||
          endY < cameraBottom || endY > cameraBottom + VIEW_HEIGHT) {
        clipLineToView(playerCenterX, playerCenterY, endX, endY, VIEW_WIDTH,
                       VIEW_HEIGHT, cameraLeft, cameraBottom);
      }
      glColor3f(0.8f, 0.8f, 0.8f);
      glBegin(GL_LINES);
      glVertex2f(playerCenterX, playerCenterY);
      glVertex2f(endX, endY);
      glEnd();
    }
  }

  drawPhysicsDebugHUD();
  drawShotCounterHUD();

  if (isGameVictory) {
    drawGameVictoryScreen();
  }

  if (isGameOver) {
    drawGameOverScreen();
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------

/**
 * As funções abaixo se assemelham muito às funções definidas para as máquinas
 * de estados do OpenGL que estão na main. A diferença é que elas não serão
 * usadas como funções definitivas para o estado, mas sim dentro das funções da
 * main. Elas alteram os valores de variáveis do game. Variáveis essas que serão
 * usados nas funções definidas acima
 */

GameAction gameKeyDown(unsigned char key, int x, int y) {
  if (isGameOver) return GAME_ACTION_CONTINUE;
  switch (key) {
    case 'q':
    case 'Q':
      return GAME_ACTION_EXIT_TO_MENU;
    case 'a':
    case 'A':
      isPressingLeft = true;
      break;
    case 'd':
    case 'D':
      isPressingRight = true;
      break;
  }
  return GAME_ACTION_CONTINUE;
}

void gameKeyUp(unsigned char key, int x, int y) {
  if (isGameOver) return;
  switch (key) {
    case 'a':
    case 'A':
      isPressingLeft = false;
      break;
    case 'd':
    case 'D':
      isPressingRight = false;
      break;
  }
}

void gameSpecialDown(int key, int x, int y) {
  if (isGameOver) return;
  switch (key) {
    case GLUT_KEY_LEFT:
      isPressingLeft = true;
      break;
    case GLUT_KEY_RIGHT:
      isPressingRight = true;
      break;
  }
}

void gameSpecialUp(int key, int x, int y) {
  if (isGameOver) return;
  switch (key) {
    case GLUT_KEY_LEFT:
      isPressingLeft = false;
      break;
    case GLUT_KEY_RIGHT:
      isPressingRight = false;
      break;
  }
}

void gameMouseMotion(int x, int y) {
  if (isGameOver) return;
  int winW = glutGet(GLUT_WINDOW_WIDTH), winH = glutGet(GLUT_WINDOW_HEIGHT);
  if (winW == 0) winW = 1;
  if (winH == 0) winH = 1;
  mouseGameX = cameraLeft + (float)x / winW * VIEW_WIDTH;
  mouseGameY = cameraBottom + (float)(winH - y) / winH * VIEW_HEIGHT;
}

void gameMouseClick(int button, int state) {
  if (isGameOver) return;
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      if (shotsRemaining > 0 && !isHooked && !isHookFiring) {
        isHookFiring = true;
        shotsRemaining--;
        float playerCenterX = player.x + player.w / 2,
              playerCenterY = player.y + player.h / 2;
        hookProjectileX = playerCenterX;
        hookProjectileY = playerCenterY;

        float vAX = mouseGameX - playerCenterX,
              vAY = mouseGameY - playerCenterY;
        float rMD = sqrt(vAX * vAX + vAY * vAY);
        float cMD = fmin(rMD, MAX_VISUAL_AIM_LENGTH);
        float fP = cMD / MAX_VISUAL_AIM_LENGTH;

        currentPullForce = fP * levelParameters.maxPullStrengthPhysics;

        if (rMD > 0.01f) {
          hookProjectileVelX = (vAX / rMD) * levelParameters.hookSpeed;
          hookProjectileVelY = (vAY / rMD) * levelParameters.hookSpeed;
        } else {
          hookProjectileVelX = 0;
          hookProjectileVelY = levelParameters.hookSpeed;
        }
      }
    } else if (state == GLUT_UP) {
      isHooked = false;
      isHookFiring = false;
    }
  }
}