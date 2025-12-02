/**
 * Para o jogo 3D, na parte da mecânica e da física, devido à proposta do jogo
 * ser de ensinar cinemática vetorial, tivemos muitas dificuldades relacionadas
 * a cálculos no geral e mostrar de uma forma que fique fácil de visualizar e
 * aprender o que o jogo se propõe a mostrar. Para isso, adotamos um novo padrão
 * de projeto em que separamos todas as constantes globais e fazemos os cálculos
 * com base nos seus valores para garantir a linearidade e a fluidez. Além
 * disso, pensamos em manter a mesma jogabilidade do modo 2D, no entanto, é
 * impossível definir corretamente a posição do mouse em um mundo
 * tridimensional. Tivemos de pensar em alternativas que mantessem o
 * entretenimento sem perder a proposta educativa. Além de algumas mudanças na
 * mecânica e na forma de jogar, também enfrentamos dificuldades em desenhar o
 * mundo visível de fato pois, além da complexidade do próprio mundo 3D, também
 * precisamos usar outras formas de desenho extremamente diferentes do mundo 2D,
 * tais como profundidade, perspectiva, iluminação, colisões, cálculos etc.
 * Assim como explicado na "main.cpp", todos os headers foram refeitos para
 * garantir a linearidade das funções e libs externas que são usadas na lógica.
 * As funções que usamos (para montagem dos VBOs, desenhos de texto, checagem de
 * colisões e etc) são funções amplamente usadas em jogos, portanto, são formas
 * otimizadas de conseguir os resultados esperados.
 */

#include "game_3D.h"

#include "audio.h"
#include "game.h"

// ---------------------------------------------------------------------------------------------------------

/**
 * Constantes usadas na definição de mundo e para visualização.
 */
const float PLAYER_HEIGHT_3D = 80.0f;
const float VIEW_WIDTH_3D = 800.0f;
const float VIEW_HEIGHT_3D = 600.0f;

// O mundo agora é um volume
const float WORLD_WIDTH_3D = 3000.0f;
const float WORLD_HEIGHT_3D = 600.0f;
const float WORLD_DEPTH_3D = 1000.0f;

const float DEATH_BOUNDARY_Y_3D = -100.0f;

/**
 * Constantes visuais e de input para o modo FPS e Carregamento.
 */
const float MAX_VISUAL_AIM_LENGTH_3D = 5.0f * PLAYER_HEIGHT_3D;
const float MAX_AIM_FORCE_DISPLAY_3D = 100.0f;
const float VELOCITY_VISUAL_SCALE_3D = 7.0f;
const float WIND_VECTOR_VISUAL_SCALE_3D = 500.0f;

const float MOUSE_SENSITIVITY_3D =
    0.2f;  // Velocidade de rotação da câmera (sensibilidade do mouse)
const float MAX_CHARGE_TIME_3D =
    2.0f;  // Tempo máximo em segundos para carregar a força total
const float MAX_PITCH_3D = 89.0f;  // Limite vertical superior da câmera (Pitch)
const float MIN_PITCH_3D = 5.0f;   // Limite vertical inferior da câmera (Pitch)

// ---------------------------------------------------------------------------------------------------------

GameObject_3D player_3D;
GameObject_3D door_3D;
Platform_3D* collidingPlatform_3D = NULL;

// Vetores para armazenar as definições de objetos 3D do nível
std::vector<Platform_3D> platforms_3D;
std::vector<BreakableWall_3D> breakableWalls_3D;
std::vector<WindZone_3D> windZones_3D;
std::vector<SpikeZone_3D> spikeZones_3D;

// Vetores para armazenar os VBOs
std::vector<VBO_Info> platformVBOs;
std::vector<VBO_Info> wallVBOs;
std::vector<VBO_Info> spikeVBOs;

// Constante para a profundidade do mundo
const float WORLD_DEPTH = 50.0f;
const float Z_CENTER = 0.0f;

/**
 * O parâmetro dos níveis que se difere do mundo 2D é o "aimConvergenceDist".
 * Com a mecânica que definimos para o disparo, por mais que o z (profundidade)
 * seja fixado e o x (direta e esquerda) também sejam, o y (cima e baixo) ainda
 * pode se movimentar livremente e, em um mundo tridimensional, isso forma um
 * plano. Não é possível definir a posição exata da mira. O problema disso é que
 * o gancho era solto em um lugar diferente de onde a mira apontava. Para
 * corrigir esse problema, definimos uma distância da mira ao personagem para
 * que o y fosse uma reta horizontal. Essa distância define o "ponto ótimo" de
 * disparo. Não conseguimos deixar esse valor dinâmico a depender de onde atira,
 * é uma lógica extremamente complexa.
 */
struct LevelParameters_3D {
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
  float maxHookLength;
  float aimConvergenceDist;
};

LevelParameters_3D levelParameters_3D;

// ---------------------------------------------------------------------------------------------------------

/**
 * Posição absoluta da Câmera. Como estamos no modo de terceira pessoa,
 * escolheremos uma visualização próxima ao player mas com algumas modificações
 * para permitir a observação dos vetores na tela
 */
float cameraPosX_3D = 0.0f;
float cameraPosY_3D = 0.0f;
float cameraPosZ_3D = 200.0f;

/**
 * Ângulos de Euler (Yaw/Pitch) para controle da mira (Mouse Look). Esse
 * controle teve de ser adicionado pois, em alguns casos, o personagem poderia
 * olhar muito para cima e acabar olhando abaixo do chão. É um problema comum em
 * jogos FPS e buscamos uma boa solução para esse caso
 */
float cameraYaw_3D = -90.0f;  // Rotação horizontal (eixo Y)
float cameraPitch_3D = 0.0f;  // Rotação vertical (eixo X)
float aimPitch_3D = 20.0f;    // Ângulo real da mira

/**
 * Definimos um limite para a câmera diferente do limite da mira.
 */
const float CAMERA_STOP_PITCH = 30.0f;
const float MAX_AIM_PITCH = 85.0f;

/**
 * Vetor unitário que define a direção para onde o player está olhando (Direção
 * do Tiro). Inicialmente, para frente.
 */
float cameraFrontX_3D = 0.0f;
float cameraFrontY_3D = 0.0f;
float cameraFrontZ_3D = -1.0f;

/**
 * Controles de movimentação do personagem.
 */
bool isPressingLeft_3D = false;
bool isPressingRight_3D = false;
bool isPressingForward_3D = false;
bool isPressingBackward_3D = false;

bool isMouseFree_3D =
    false;  // Variável para controlar mouse em modo FPS ou ponteiro livre
bool mouseInitialized_3D =
    false;  // Controle interno para evitar pulo no primeiro frame

float cameraDistance = 30.0f;  // Distância da câmera ao jogador

// Variáveis de Display List (Chão Grade)
GLuint floorListID = 0;
const float WORLD_SIZE = 100.0f;  // Tamanho visual da grade
const float FLOOR_Y = 0.0f;

int lastMouseX_3D = 0;
int lastMouseY_3D = 0;

// ---------------------------------------------------------------------------------------------------------

// Estados principais do gancho (lógica do 2D)
bool isGrounded_3D = false;
bool isHooked_3D = false;
bool isHookFiring_3D = false;

// Estado de carregamento do gancho - lógica nova
bool isChargingHook_3D =
    false;  // Verdadeiro enquanto o botão de tiro está pressionado
float chargeStartTime_3D = 0.0f;     // Tempo em ms/s que o carregamento começou
float currentChargeForce_3D = 0.0f;  // Força de lançamento atual
float chargePercentage_3D = 0.0f;    // Percentual de carga

// Variável para sinalizar a soltura da corda
bool requestHookRelease_3D = false;

// Ponto onde o gancho prendeu. Usado para movimentação do personagem e debug
float hookPointX_3D = 0;
float hookPointY_3D = 0;
float hookPointZ_3D = 0;

// Posição do projétil do gancho
float hookProjectileX_3D = 0;
float hookProjectileY_3D = 0;
float hookProjectileZ_3D = 0;

// Velocidade do projétil
float hookProjectileVelX_3D = 0;
float hookProjectileVelY_3D = 0;
float hookProjectileVelZ_3D = 0;

// Posição de onde sai o gancho
float hookOriginX_3D = 0.0f;
float hookOriginY_3D = 0.0f;
float hookOriginZ_3D = 0.0f;

float ropeLength_3D = 0;
float currentPullForce_3D = 0.0f;

// ---------------------------------------------------------------------------------------------------------

// Forças físicas (Adicionado Z para todos os vetores)

float forceNormalX_3D = 0;
float forceNormalY_3D = 0;
float forceNormalZ_3D = 0;

float forceFrictionX_3D = 0;
float forceFrictionY_3D = 0;
float forceFrictionZ_3D = 0;

float forceTensionX_3D = 0;
float forceTensionY_3D = 0;
float forceTensionZ_3D = 0;

float forceResultantX_3D = 0;
float forceResultantY_3D = 0;
float forceResultantZ_3D = 0;

// ---------------------------------------------------------------------------------------------------------

float lastVelocityMag_3D = 0.0f;
float currentAcceleration_3D = 0.0f;

int shotsRemaining_3D = 0;
bool isGameOver_3D = false;
int gameOverTimer_3D = 0;
bool isGameVictory_3D = false;
int gameVictoryTimer_3D = 0;

float runAnimationTimer_3D = 0.0f;
int currentRunFrame_3D = 0;

static int lastTime_3D = 0;

GLuint platformListID_3D = 0;
GLuint doorListID_3D = 0;
GLuint spikeListID_3D = 0;
GLuint windZoneListID_3D = 0;

// Texturas de ambiente (Plataformas, Paredes, Porta, Spikes)
GLuint platformTextureID = 0;
GLuint wallTextureID = 0;
GLuint doorTextureID = 0;
GLuint spikeTextureID = 0;

GLuint playerTexture = 0;      // ID da textura do player
float playerRotationY = 0.0f;  // Rotação do player em graus

// VBOs específicos de objetos de ambiente (quando carregados via OBJ)
VBO_Info floorVBO = {0, 0};
VBO_Info doorVBO = {0, 0};
VBO_Info envWallVBO = {0, 0};
VBO_Info envSpikeVBO = {0, 0};

// Cor do material do chão por nível (R, G, B)
float floorColor[3] = {0.1f, 0.5f, 0.1f};

// Variáveis de Controle de Animação
PlayerAnimState currentPlayerAnimState = ANIM_IDLE;
float animTimer = 0.0f;
const float RUN_FRAME_DURATION =
    0.3f;  // Duração de cada frame de corrida em segundos

// Estrutura para facilitar a organização dos modelos de corrida
struct PlayerModel {
  GLuint vbo;
  int triCount;
};

// VBOs e TriCounts para as 7 variações de corrida + Parado + Gancho
PlayerModel playerModels[7];  // playerModels[0] a [4] para corrida, [5] para
                              // idle, [6] para grappler

// --------------------------------- Funções auxiliares para desenho de mundo
// ---------------------------------------

/**
 * Cria os VBOs para toda a geometria estática do nível 3D. Função análoga à
 * função feita no 2D para criação das display lists. Nesse caso, a forma de
 * criar é um pouco diferente pois é necessário limpar a memória, criar os VBOs
 * e armazenar o seu valor na memória para manipulações posteriores
 */
void createLevelVBOs() {
  // Limpeza de VBOs antigos para liberar a memória, visto que VBOs são
  // armazenadas na CPU
  for (auto& info : platformVBOs) {
    if (info.vboID != 0) glDeleteBuffers(1, &info.vboID);
  }
  platformVBOs.clear();

  for (auto& info : wallVBOs) {
    if (info.vboID != 0) glDeleteBuffers(1, &info.vboID);
  }
  wallVBOs.clear();

  for (auto& info : spikeVBOs) {
    if (info.vboID != 0) glDeleteBuffers(1, &info.vboID);
  }
  spikeVBOs.clear();

  for (const auto& p : platforms_3D) {
    int triCount = 0;
    tri* platformTris =
        createBoxTris(p.x, p.y, p.z, p.w, p.h, p.d, p.r, p.g, p.b, triCount);

    VBO_Info info;
    info.vboID = criaVBODeTris(triCount, platformTris);
    info.triCount = triCount;
    platformVBOs.push_back(info);

    free(platformTris);
  }

  for (const auto& w : breakableWalls_3D) {
    if (w.isBroken) continue;

    int triCount = 0;
    tri* wallTris =
        createBoxTris(w.x, w.y, w.z, w.w, w.h, w.d, w.r, w.g, w.b, triCount);

    VBO_Info info;
    info.vboID = criaVBODeTris(triCount, wallTris);
    info.triCount = triCount;
    wallVBOs.push_back(info);

    free(wallTris);
  }

  for (const auto& s : spikeZones_3D) {
    int triCount = 0;
    tri* spikeTris =
        createBoxTris(s.x, s.y, s.z, s.w, s.h, s.d, 1.0f, 0.0f, 0.0f, triCount);

    VBO_Info info;
    info.vboID = criaVBODeTris(triCount, spikeTris);
    info.triCount = triCount;
    spikeVBOs.push_back(info);

    free(spikeTris);
  }
}

/**
 * Carrega os modelos OBJ de ambiente (Plataformas, Paredes, Porta, Spikes).
 * Tenta carregar os arquivos .obj referenciados nos dados de nível.
 * Se falharem, mantém os VBOs padrão criados por createLevelVBOs().
 *
 * Esta função deve ser chamada após a definição dos dados de ambiente em
 * gameStartLevel_3D().
 */
void loadEnvironmentModels(int level) {
  std::string levelDir = "models/level" + std::to_string(level) + "/";
  std::string globalDir = "models/global/";

  printf("Carregando modelos de ambiente para o level %d...\n", level);
  // --- Primeiro: limpa VBOs específicos anteriores (se existirem)
  if (floorVBO.vboID != 0) {
    glDeleteBuffers(1, &floorVBO.vboID);
    floorVBO.vboID = 0;
    floorVBO.triCount = 0;
  }
  if (doorVBO.vboID != 0) {
    glDeleteBuffers(1, &doorVBO.vboID);
    doorVBO.vboID = 0;
    doorVBO.triCount = 0;
  }
  if (envWallVBO.vboID != 0) {
    glDeleteBuffers(1, &envWallVBO.vboID);
    envWallVBO.vboID = 0;
    envWallVBO.triCount = 0;
  }
  if (envSpikeVBO.vboID != 0) {
    glDeleteBuffers(1, &envSpikeVBO.vboID);
    envSpikeVBO.vboID = 0;
    envSpikeVBO.triCount = 0;
  }

  // Helper lambda para carregar um OBJ em um VBO_Info (procura no levelDir e em
  // seguida no globalDir)
  auto tryLoadSingleOBJ = [&](const std::string& baseName, VBO_Info& outInfo,
                              const std::string& preferDir) -> bool {
    std::string path = preferDir + baseName;
    FILE* f = fopen(path.c_str(), "r");
    if (!f) {
      path = globalDir + baseName;
      f = fopen(path.c_str(), "r");
    }
    if (!f) return false;
    fclose(f);
    std::vector<tri> tris;
    carregaOBJemTris(path.c_str(), tris);
    if (tris.empty()) return false;
    // cria VBO
    if (outInfo.vboID != 0) {
      glDeleteBuffers(1, &outInfo.vboID);
    }
    outInfo.vboID = criaVBODeTris((int)tris.size(), tris.data());
    outInfo.triCount = (int)tris.size();
    return true;
  };

  // Carrega floor.obj (chão) - prefer level, depois global
  if (!tryLoadSingleOBJ("floor.obj", floorVBO, levelDir)) {
    // garante zeros se falhar
    floorVBO.vboID = 0;
    floorVBO.triCount = 0;
  } else {
    printf("Modelo floor.obj carregado (VBO=%u, tris=%d)\n", floorVBO.vboID,
           floorVBO.triCount);
  }

  // Carrega door.obj (porta)
  if (!tryLoadSingleOBJ("door.obj", doorVBO, levelDir)) {
    doorVBO.vboID = 0;
    doorVBO.triCount = 0;
  } else {
    printf("Modelo door.obj carregado (VBO=%u, tris=%d)\n", doorVBO.vboID,
           doorVBO.triCount);
  }

  // Carrega walls.obj (paredes/blocos)
  // Tenta carregar walls.obj (plural) ou wall.obj (singular)
  if (!tryLoadSingleOBJ("walls.obj", envWallVBO, levelDir) &&
      !tryLoadSingleOBJ("wall.obj", envWallVBO, levelDir)) {
    envWallVBO.vboID = 0;
    envWallVBO.triCount = 0;
  } else {
    printf("Modelo wall(s).obj carregado (VBO=%u, tris=%d)\n", envWallVBO.vboID,
           envWallVBO.triCount);
  }

  // Carrega spikes.obj (global ou level)
  // Tenta carregar spikes.obj (plural) ou spike.obj (singular)
  if (!tryLoadSingleOBJ("spikes.obj", envSpikeVBO, levelDir) &&
      !tryLoadSingleOBJ("spike.obj", envSpikeVBO, levelDir)) {
    envSpikeVBO.vboID = 0;
    envSpikeVBO.triCount = 0;
  } else {
    printf("Modelo spike(s).obj carregado (VBO=%u, tris=%d)\n",
           envSpikeVBO.vboID, envSpikeVBO.triCount);
  }

  // Mantemos também o carregamento de platform.obj/platforms.obj (se houver)
  // e armazenamos um único VBO para instanciação das plataformas de nível.
  {
    VBO_Info tmp = {0, 0};
    bool loaded = false;
    if (tryLoadSingleOBJ("platform.obj", tmp, levelDir))
      loaded = true;
    else if (tryLoadSingleOBJ("platforms.obj", tmp, levelDir))
      loaded = true;
    else if (tryLoadSingleOBJ("platform.obj", tmp, globalDir))
      loaded = true;
    else if (tryLoadSingleOBJ("platforms.obj", tmp, globalDir))
      loaded = true;

    if (loaded) {
      for (auto& info : platformVBOs) {
        if (info.vboID != 0) glDeleteBuffers(1, &info.vboID);
      }
      platformVBOs.clear();
      platformVBOs.push_back(tmp);
      printf(
          "Modelo platform(s).obj carregado com sucesso! (VBO=%u, tris=%d)\n",
          tmp.vboID, tmp.triCount);
    }
  }
}

/**
 * Cria a grade do chão para ajudar na visualização da movimentação. Usado
 * apenas para debug
 * TODO - remover essa visualização para aplicação de texturas, serve apenas
 * para ver a física
 */
void createWorldDisplayLists() {
  if (floorListID != 0) glDeleteLists(floorListID, 1);

  floorListID = glGenLists(1);
  glNewList(floorListID, GL_COMPILE);
  glDisable(GL_LIGHTING);
  glLineWidth(1.0f);

  glColor3f(1.0f, 1.0f, 1.0f);

  glBegin(GL_LINES);
  for (float i = -WORLD_SIZE; i <= WORLD_SIZE; i += 2.0f) {
    glVertex3f(i, FLOOR_Y, -WORLD_SIZE);
    glVertex3f(i, FLOOR_Y, WORLD_SIZE);
  }
  for (float i = -WORLD_SIZE; i <= WORLD_SIZE; i += 2.0f) {
    glVertex3f(-WORLD_SIZE, FLOOR_Y, i);
    glVertex3f(WORLD_SIZE, FLOOR_Y, i);
  }
  glEnd();
  glEnable(GL_LIGHTING);
  glEndList();
}

// --------------------------------- Funções auxiliares para desenhos gerais
// ---------------------------------------

/**
 * Desenha a tela de Vitória (Sobreposição Verde)
 * TODO - alterar a tela de vitória para aplicar as texturas e assets
 */
void drawVictoryScreen(int w, int h) {
  // Fundo Verde Transparente
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Cor: R, G, B, Alpha (0.0, 0.8, 0.0, 0.5)
  drawRect(0, 0, w, h, 0.0f, 0.8f, 0.0f, 0.5f);

  glDisable(GL_BLEND);

  // Texto Centralizado
  glColor3f(1.0f, 1.0f, 1.0f);  // Branco
  drawTextCentered(w / 2.0f, h / 2.0f - 20, "NIVEL COMPLETADO!",
                   GLUT_BITMAP_TIMES_ROMAN_24);
  drawTextCentered(w / 2.0f, h / 2.0f + 20, "Aguarde...",
                   GLUT_BITMAP_HELVETICA_18);
}

/**
 * Desenha a tela de Game Over (Sobreposição Vermelha)
 * TODO - alterar a tela de vitória para aplicar as texturas e assets
 */
void drawGameOverScreen(int w, int h) {
  // Fundo Vermelho Transparente
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Cor: R, G, B, Alpha (0.8, 0.0, 0.0, 0.5)
  drawRect(0, 0, w, h, 0.8f, 0.0f, 0.0f, 0.5f);

  glDisable(GL_BLEND);

  // Texto Centralizado
  glColor3f(1.0f, 1.0f, 1.0f);  // Branco
  drawTextCentered(w / 2.0f, h / 2.0f - 20, "GAME OVER",
                   GLUT_BITMAP_TIMES_ROMAN_24);
  drawTextCentered(w / 2.0f, h / 2.0f + 20, "Reiniciando...",
                   GLUT_BITMAP_HELVETICA_18);
}

/**
 * Tela de desenho da caixa de informações das constantes e vetores. Versão
 * análoga à usada em 2D
 */
void drawPhysicsDebugHUD_3D() {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  int w = glutGet(GLUT_WINDOW_WIDTH);
  int h = glutGet(GLUT_WINDOW_HEIGHT);
  gluOrtho2D(0, w, h, 0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  std::vector<std::string> infoLines;
  char buffer[100];

  float vMag = sqrt(player_3D.velocityX * player_3D.velocityX +
                    player_3D.velocityY * player_3D.velocityY +
                    player_3D.velocityZ * player_3D.velocityZ);
  sprintf(buffer, "v: %.1f m/s", vMag);
  infoLines.push_back(buffer);

  sprintf(buffer, "a: %.2f m/s^2", currentAcceleration_3D);
  infoLines.push_back(buffer);

  float weightForce =
      levelParameters_3D.gravity * levelParameters_3D.playerMass;
  sprintf(buffer, "P: %.2f N", weightForce);
  infoLines.push_back(buffer);

  if (isGrounded_3D) {
    sprintf(buffer, "N: %.2f N", forceNormalY_3D);
    infoLines.push_back(buffer);

    float fricMag = sqrt(forceFrictionX_3D * forceFrictionX_3D +
                         forceFrictionZ_3D * forceFrictionZ_3D);
    if (fricMag > 0.001f) {
      sprintf(buffer, "Fat: %.2f N", fricMag);
      infoLines.push_back(buffer);
    }
  }

  if (isHooked_3D) {
    float tMag = sqrt(forceTensionX_3D * forceTensionX_3D +
                      forceTensionY_3D * forceTensionY_3D +
                      forceTensionZ_3D * forceTensionZ_3D);

    sprintf(buffer, "T: %.2f N", tMag);
    infoLines.push_back(buffer);
  }

  if (collidingPlatform_3D) {
    sprintf(buffer, "mu_chao: %.2f", collidingPlatform_3D->frictionCoefficient);
    infoLines.push_back(buffer);
  }

  float lineHeight = 15.0f;
  float margin = 10.0f;
  float padding = 20.0f;

  float maxWidth = 0;
  for (const auto& line : infoLines) {
    maxWidth = std::max(maxWidth,
                        (float)getTextWidth(line.c_str(), GLUT_BITMAP_9_BY_15));
  }

  float blockWidth = maxWidth + (2 * padding) + 10.0f;
  float blockHeight = (infoLines.size() * lineHeight) + (2 * padding) + 5.0f;
  float blockX = w - margin - blockWidth;
  float blockY = margin;

  if (texDisplayGrappler != 0) {
    glColor3f(1, 1, 1);
    drawTexturedRect(blockX, blockY, blockWidth, blockHeight,
                     texDisplayGrappler, false, false);
  } else {
    drawRect(blockX, blockY, blockWidth, blockHeight, 0.0f, 0.0f, 0.0f,
             0.7f);  // Preto transparente
  }

  float currentY = blockY + padding + 10;
  for (const auto& line : infoLines) {
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(blockX + padding, currentY, line.c_str(), GLUT_BITMAP_9_BY_15);
    currentY += lineHeight;
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

/**
 * Tela de desenho da caixa de informações da quantidade de disparos restante.
 * Versão análoga à usada em 2D
 */
void drawShotCounterHUD_3D() {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  int w = glutGet(GLUT_WINDOW_WIDTH);
  int h = glutGet(GLUT_WINDOW_HEIGHT);
  gluOrtho2D(0, w, h, 0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  char shotText[50];

  sprintf(shotText, "Disparos: %d / %d", shotsRemaining_3D,
          levelParameters_3D.maxShots);

  float textWidth = getTextWidth(shotText, GLUT_BITMAP_9_BY_15);
  float padding = 18.0f;
  float blockWidth = textWidth + (2 * padding) + 20.0f;
  float blockHeight = 6.0f + (2 * padding) + 6.0f;
  float blockX = w / 2.0f - blockWidth / 2.0f;
  float blockY = 10.0f;

  if (texDisplayGrappler != 0) {
    glColor3f(1, 1, 1);
    drawTexturedRect(blockX, blockY, blockWidth, blockHeight,
                     texDisplayGrappler, false, false);
  } else {
    drawRect(blockX, blockY, blockWidth, blockHeight, 0.0f, 0.0f, 0.0f, 0.7f);
  }

  glColor3f(1.0f, 1.0f, 1.0f);
  drawTextCentered(w / 2.0f, blockY + padding + 10, shotText,
                   GLUT_BITMAP_9_BY_15);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

/**
 * Desenha a barra de carregamento do gancho
 */
void drawChargeBar(int w, int h) {
  if (!isChargingHook_3D) return;

  float barCX = w / 2.0f;
  float barCY = h - 100.0f;
  float barW = 200.0f;
  float barH = 20.0f;

  glColor3f(0.2f, 0.2f, 0.2f);
  drawRect(barCX - barW / 2, barCY, barW, barH, 0.2f, 0.2f, 0.2f);

  float fillW = barW * chargePercentage_3D;
  glColor3f(1.0f, 1.0f - chargePercentage_3D,
            0.0f);  // Lógica de ir de amarelo até vermelho para representar
                    // melhor a força usada
  drawRect(barCX - barW / 2, barCY, fillW, barH, 1.0f,
           1.0f - chargePercentage_3D, 0.0f);

  char forceText[50];
  sprintf(forceText, "Forca: %.1f N", currentChargeForce_3D);

  glColor3f(1.0f, 1.0f, 1.0f);
  // Desenha logo acima da barra
  drawTextCentered(barCX, barCY - 20.0f, forceText, GLUT_BITMAP_HELVETICA_18);
}

/**
 * Desenha toda a interface 2D (HUD, Barra, Telas Finais).
 * Substitui a drawHUD anterior.
 */
void drawHUD() {
  // Configura projeção 2D
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  int w = glutGet(GLUT_WINDOW_WIDTH);
  int h = glutGet(GLUT_WINDOW_HEIGHT);
  gluOrtho2D(
      0, w, h,
      0);  // Y invertido (0 no topo) para bater com as coordenadas do mouse

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // Desabilita efeitos 3D para desenhar a UI chapada
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  if (isGameVictory_3D) {
    drawVictoryScreen(w, h);
  } else if (isGameOver_3D) {
    drawGameOverScreen(w, h);
  } else {
    drawChargeBar(w, h);

    drawPhysicsDebugHUD_3D();
    drawShotCounterHUD_3D();

    float centerX = w / 2.0f;
    float centerY = h / 2.0f;

    /**
     * Essa é a lógica que define onde será o tiro do gancho. Tivemos muita
     * dificuldade em encontrar uma maneira correta para esse cálculo e que
     * funcionasse para a maioria dos casos
     */
    float pitchDiffDeg = aimPitch_3D - cameraPitch_3D;
    float pitchDiffRad = pitchDiffDeg * M_PI / 180.0f;

    float fovDeg = 45.0f;
    float fovRad = (fovDeg / 2.0f) * M_PI / 180.0f;

    float screenHalfHeight = h / 2.0f;
    float offsetY = screenHalfHeight * (tan(pitchDiffRad) / tan(fovRad));

    float crossY = centerY + offsetY;
    float crossX = centerX;

    // Desenha a cruz da mira
    glColor3f(0.0f, 1.0f, 0.0f);
    glLineWidth(2.0f);
    float size = 10.0f;
    glBegin(GL_LINES);
    glVertex2f(crossX - size, crossY);
    glVertex2f(crossX + size, crossY);
    glVertex2f(crossX, crossY - size);
    glVertex2f(crossX, crossY + size);
    glEnd();
  }

  // Restaura matrizes e estados 3D para voltar para o modo de jogo
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

/**
 * Desenha os vetores de debug. Equivalente visual ao jogo 2D, mas adaptado para
 * o mundo tridimensional
 */
void drawDebugVectors() {
  float cx = player_3D.x + player_3D.w / 2.0f;
  float cy = player_3D.y + player_3D.h / 2.0f;
  float cz = player_3D.z + player_3D.d / 2.0f;
  Vector_3D center = {cx, cy, cz};

  Vector_3D feet = {cx, cy - player_3D.h / 2.0f, cz};

  float scaleForce = levelParameters_3D.vectorVisualScale;

  // Peso
  float weightVal = levelParameters_3D.playerMass * levelParameters_3D.gravity;
  Vector_3D vecP = {0, -weightVal, 0};

  // Cria um ponto deslocado apenas para desenhar o peso
  Vector_3D weightPos = {center.x + 2.0f, center.y, center.z};
  drawVector_3D(center, vecP, scaleForce, 1.0f, 0.0f, 1.0f, "P");

  // Velocidade
  float vMag = sqrt(player_3D.velocityX * player_3D.velocityX +
                    player_3D.velocityY * player_3D.velocityY +
                    player_3D.velocityZ * player_3D.velocityZ);

  if (vMag > 0.1f) {
    float scaleVel = 0.5f;  // Ou podemos usar VELOCITY_VISUAL_SCALE_3D, mas
                            // optamos pela escala local de velocidade

    Vector_3D vecV = {player_3D.velocityX, player_3D.velocityY,
                      player_3D.velocityZ};
    drawVector_3D(center, vecV, scaleVel, 0.0f, 1.0f, 0.0f, "V");
  }

  // Normal e atrito
  if (isGrounded_3D) {
    // Normal
    Vector_3D vecN = {0, forceNormalY_3D, 0};
    drawVector_3D(feet, vecN, scaleForce, 0.0f, 1.0f, 1.0f, "N");

    // Atrito
    float fricMag = sqrt(forceFrictionX_3D * forceFrictionX_3D +
                         forceFrictionZ_3D * forceFrictionZ_3D);
    if (fricMag > 0.1f) {
      // Desenhamos um pouquinho acima do pé (+0.1 em Y) para a linha não sumir
      // dentro da grade do chão
      Vector_3D feetRaised = {feet.x, feet.y + 0.1f, feet.z};
      Vector_3D vecFat = {forceFrictionX_3D, 0, forceFrictionZ_3D};
      drawVector_3D(feetRaised, vecFat, scaleForce, 1.0f, 0.0f, 0.0f, "Fat");
    }
  }

  // Tensão
  if (isHooked_3D) {
    float ropeOriginY = center.y + (player_3D.h * 0.4f);

    Vector_3D ropeStartPos = {center.x, ropeOriginY, center.z};

    Vector_3D vecT = {forceTensionX_3D, forceTensionY_3D, forceTensionZ_3D};

    drawVector_3D(ropeStartPos, vecT, scaleForce, 0.2f, 0.2f, 1.0f, "T");
  }

  // Mira/lançamento se estiver carregando
  // ---------------------------------------------------
  if (isChargingHook_3D && !isHooked_3D) {
    float yawRad = cameraYaw_3D * M_PI / 180.0f;
    float pitchRad = cameraPitch_3D * M_PI / 180.0f;

    float dirX = -sin(yawRad) * cos(pitchRad);
    float dirY = -sin(pitchRad);
    float dirZ = -cos(yawRad) * cos(pitchRad);

    float forceVal = currentChargeForce_3D;
    Vector_3D vecAim = {dirX * forceVal, dirY * forceVal, dirZ * forceVal};

    drawVector_3D(center, vecAim, scaleForce, 0.8f, 0.8f, 0.8f, "F_imp");
  }
}

// ------------------------------------ CallBacks do jogo
// ---------------------------------------

/**
 * Chamada quando o nível 3D é iniciado. Nessa função as variáveis de controle
 * são resetadas para que não hajam inconsistências na passagem de um nível para
 * o outro. Estrutura análoga ao 2D adaptada para o uso de VBOs.
 */
void gameStartLevel_3D(int level) {
  printf("gameStartLevel_3D() chamado para o nivel %d.\n", level);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // Configurar Luz Ambiente Global (ilumina todos os objetos mesmo na sombra)
  GLfloat light_ambient[] = {0.5f, 0.5f, 0.5f, 1.0f};
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

  // Configurar Luz Direcional (GL_LIGHT0) - vem de cima
  GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat light_position[] = {0.0f, 1.0f, 0.5f, 0.0f};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  // Definir cor do céu dependendo do nível
  // Define cor do céu e cor de material do chão por nível
  if (level == 3) {
    // Espaço / noite
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    floorColor[0] = 0.3f;
    floorColor[1] = 0.3f;
    floorColor[2] = 0.3f;  // cinza escuro
  } else if (level == 2) {
    // Dia claro (neve)
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    floorColor[0] = 0.9f;
    floorColor[1] = 0.9f;
    floorColor[2] = 0.9f;  // branco
  } else {
    // Level 1 e padrão: dia/grama
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    floorColor[0] = 0.1f;
    floorColor[1] = 0.5f;
    floorColor[2] = 0.1f;  // verde
  }

  player_3D.velocityX = 0.0f;
  player_3D.velocityY = 0.0f;
  player_3D.velocityZ = 0.0f;
  player_3D.w = 1.0f;
  player_3D.h = 2.0f;
  player_3D.d = 1.0f;

  isPressingForward_3D = false;
  isPressingBackward_3D = false;
  isPressingLeft_3D = false;
  isPressingRight_3D = false;

  isGrounded_3D = true;
  isHooked_3D = false;
  isHookFiring_3D = false;
  isChargingHook_3D = false;
  requestHookRelease_3D = false;

  cameraYaw_3D = 0.0f;
  cameraPitch_3D = 20.0f;
  aimPitch_3D = 20.0f;
  cameraDistance = 20.0f;

  lastVelocityMag_3D = 0.0f;
  currentAcceleration_3D = 0.0f;
  forceNormalX_3D = 0;
  forceNormalY_3D = 0;
  forceNormalZ_3D = 0;
  forceFrictionX_3D = 0;
  forceFrictionY_3D = 0;
  forceFrictionZ_3D = 0;
  forceTensionX_3D = 0;
  forceTensionY_3D = 0;
  forceTensionZ_3D = 0;

  isGameOver_3D = false;
  isGameVictory_3D = false;

  platforms_3D.clear();
  breakableWalls_3D.clear();
  windZones_3D.clear();
  spikeZones_3D.clear();

  glutSetCursor(GLUT_CURSOR_NONE);
  int w = glutGet(GLUT_WINDOW_WIDTH);
  int h = glutGet(GLUT_WINDOW_HEIGHT);
  gameReshape_3D(w, h);
  glutWarpPointer(w / 2, h / 2);
  mouseInitialized_3D = false;

  switch (level) {
    case 1:
      // Parâmetros Físicos da fase
      levelParameters_3D.playerMass = 10.0f;
      levelParameters_3D.playerWalkAccel = 60.0f;
      levelParameters_3D.maxWalkSpeed = 15.0f;
      levelParameters_3D.maxPlayerSpeed = 40.0f;
      levelParameters_3D.maxHookLength = 100.0f;
      levelParameters_3D.vectorVisualScale = 0.04f;
      levelParameters_3D.gravity = 25.0f;
      levelParameters_3D.hookSpeed = 150.0f;
      levelParameters_3D.maxShots = 15;
      levelParameters_3D.maxPullStrengthPhysics = 150.0f;
      levelParameters_3D.dampingFactor = 0.995f;
      levelParameters_3D.aimConvergenceDist = 40.0f;

      // Chão
      platforms_3D.push_back({-WORLD_WIDTH_3D / 2, -5.0f, -WORLD_DEPTH_3D / 2,
                              WORLD_WIDTH_3D, 5.0f, WORLD_DEPTH_3D, 0.3f, 0.3f,
                              0.3f, true, 0.8f});

      // Parede atrás
      platforms_3D.push_back({-10.0f, 0.0f, -20.0f, 20.0f, 15.0f, 5.0f, 0.6f,
                              0.6f, 0.7f, true, 0.5f});

      // Paredes flutuantes na frente
      platforms_3D.push_back(
          {0.0f, 0.0f, 15.0f, 5.0f, 4.0f, 5.0f, 0.5f, 0.5f, 0.8f, true, 0.5f});
      platforms_3D.push_back(
          {0.0f, 4.0f, 35.0f, 5.0f, 8.0f, 5.0f, 0.5f, 0.5f, 0.8f, true, 0.5f});
      platforms_3D.push_back(
          {0.0f, 6.0f, 65.0f, 8.0f, 2.0f, 8.0f, 0.5f, 0.5f, 0.8f, true, 0.5f});

      // "Caverna" para teste do gancho
      platforms_3D.push_back({-25.0f, 0.0f, 80.0f, 5.0f, 40.0f, 60.0f, 0.4f,
                              0.4f, 0.4f, true, 0.3f});  // Esq
      platforms_3D.push_back({25.0f, 0.0f, 80.0f, 5.0f, 40.0f, 60.0f, 0.4f,
                              0.4f, 0.4f, true, 0.3f});  // Dir
      platforms_3D.push_back({-25.0f, 35.0f, 80.0f, 55.0f, 2.0f, 60.0f, 0.7f,
                              0.7f, 0.7f, true, 0.3f});

      // Espinhos dentro da caverna
      spikeZones_3D.push_back({-20.0f, 0.1f, 80.0f, 45.0f, 1.0f, 60.0f});

      // Parede quebrável
      platforms_3D.push_back({-10.0f, 6.0f, 150.0f, 20.0f, 2.0f, 20.0f, 0.5f,
                              0.5f, 0.8f, true, 0.5f});  // Plataforma de pouso
      breakableWalls_3D.push_back({-5.0f, 8.0f, 160.0f, 10.0f, 10.0f, 2.0f,
                                   0.8f, 0.4f, 0.1f, 400.0f,
                                   false});  // Parede Frágil

      // Porta
      door_3D.x = 0.0f;
      door_3D.y = 8.0f;
      door_3D.z = 180.0f;
      door_3D.w = 4.0f;
      door_3D.h = 6.0f;
      door_3D.d = 4.0f;

      // Posição inicial do jogador
      player_3D.x = 0.0f;
      player_3D.y = 2.0f;
      player_3D.z = 0.0f;
      break;

    default:
      gameStartLevel_3D(1);
      return;
  }

  // Força o modo FPS no início da fase e deixa o cursor sempre no centro da
  // tela
  isMouseFree_3D = false;
  glutSetCursor(GLUT_CURSOR_NONE);
  gameReshape_3D(w, h);
  glutWarpPointer(w / 2, h / 2);

  // Reinicia a flag de "primeiro movimento" para evitar solavancos
  mouseInitialized_3D = false;

  // Define a quantidade de tiros restantes (escolhida em cada fase) e define em
  // qual plataforma o jogador irá começar a fase
  shotsRemaining_3D = levelParameters_3D.maxShots;
  if (!platforms_3D.empty())
    collidingPlatform_3D = &platforms_3D[0];
  else
    collidingPlatform_3D = NULL;

  createWorldDisplayLists();
  createLevelVBOs();
  loadEnvironmentModels(level);  // Carrega modelos OBJ de ambiente (substitui
                                 // VBOs padrão se disponível)
}

/**
 * Loop principal da física do jogo.
 * Essa função, assim como no modo 2D, é o coração de todos os cálculos e
 * mecânica do jogo
 */
GameAction gameUpdate_3D() {
  // Controle de vitória e derrota. Análogo ao modo 2D
  if (isGameVictory_3D) {
    if (gameVictoryTimer_3D == 180) {
      Audio::stopMusic();
      Audio::playSound("win_level", 0);
    }
    gameVictoryTimer_3D--;
    if (gameVictoryTimer_3D <= 0) {
      isGameVictory_3D = false;
      return GAME_ACTION_LEVEL_WON;
    }
    return GAME_ACTION_CONTINUE;
  }
  if (isGameOver_3D) {
    if (gameOverTimer_3D == 180) {
      Audio::stopMusic();
      Audio::playSound("game_over", 0);
    }
    gameOverTimer_3D--;
    if (gameOverTimer_3D <= 0) {
      isGameOver_3D = false;
      return GAME_ACTION_LEVEL_LOST;
    }
    return GAME_ACTION_CONTINUE;
  }

  // Variável de tempo para controle da movimentação
  int currentTime = glutGet(GLUT_ELAPSED_TIME);
  float deltaTime = (float)(currentTime - lastTime_3D) / 1000.0f;
  lastTime_3D = currentTime;
  if (deltaTime > 0.1f) deltaTime = 0.1f;

  float prevVelX = player_3D.velocityX;
  float prevVelY = player_3D.velocityY;
  float prevVelZ = player_3D.velocityZ;

  float prevPositionPlayerX = player_3D.x;
  float prevPositionPlayerY = player_3D.y;
  float prevPositionPlayerZ = player_3D.z;

  float pCx = player_3D.x + player_3D.w / 2.0f;
  float pCy = player_3D.y + player_3D.h / 2.0f;
  float pCz = player_3D.z + player_3D.d / 2.0f;

  forceNormalX_3D = 0;
  forceNormalY_3D = 0;
  forceNormalZ_3D = 0;
  forceFrictionX_3D = 0;
  forceFrictionY_3D = 0;
  forceFrictionZ_3D = 0;
  forceTensionX_3D = 0;
  forceTensionY_3D = 0;
  forceTensionZ_3D = 0;

  float accelX = 0, accelY = -levelParameters_3D.gravity, accelZ = 0;

  // Correntes de vento
  for (const auto& wind : windZones_3D) {
    if (isPointInsideBox(pCx, pCy, pCz, wind.x, wind.y, wind.z, wind.w, wind.h,
                         wind.d)) {
      accelX += wind.accelX;
      accelY += wind.accelY;
      accelZ += wind.accelZ;
    }
  }

  // Carregamento da força do gancho
  if (isChargingHook_3D) {
    float chargeTime =
        (glutGet(GLUT_ELAPSED_TIME) - chargeStartTime_3D) / 1000.0f;
    chargePercentage_3D = chargeTime / MAX_CHARGE_TIME_3D;
    if (chargePercentage_3D > 1.0f) chargePercentage_3D = 1.0f;
    currentChargeForce_3D = chargePercentage_3D * MAX_AIM_FORCE_DISPLAY_3D;
  }

  // Disparo do gancho
  if (requestHookRelease_3D) {
    requestHookRelease_3D = false;

    if (isHooked_3D) {
      isHooked_3D = false;
      isGrounded_3D = false;
      printf("UPDATE: Soltou do gancho.\n");
    } else {
      if (shotsRemaining_3D > 0) {
        isHookFiring_3D = true;
        isChargingHook_3D = false;

        shotsRemaining_3D--;

        printf("UPDATE: DISPARO CONFIRMADO! Forca: %.1f. Restam: %d\n",
               currentChargeForce_3D, shotsRemaining_3D);

        /**
         * Assim como explicado no cabeçalho, esse foi o cálculo que escolhemos
         * usar para definir onde o gancho iria atirar. Não conseguimos fazer
         * isso ser dinâmico a depender da distância que estava olhando,
         * portanto, fixamos uma distância de "tiro ótimo"
         */

        // ------------------------------------------------------------

        float fireYawRad = cameraYaw_3D * M_PI / 180.0f;
        float firePitchRad = aimPitch_3D * M_PI / 180.0f;

        // Direção da mira para onde o olho (câmera) está olhando
        float dirX = -sin(fireYawRad) * cos(firePitchRad);
        float dirY = -sin(firePitchRad);
        float dirZ = -cos(fireYawRad) * cos(firePitchRad);

        // Recalcula posição exata da câmera para garantir sincronia
        float camRadPitch = cameraPitch_3D * M_PI / 180.0f;
        float camRadYaw = cameraYaw_3D * M_PI / 180.0f;

        float camX = pCx + cameraDistance * cos(camRadPitch) * sin(camRadYaw);
        float camY =
            pCy + (player_3D.h / 2.0f) + cameraDistance * sin(camRadPitch);
        float camZ = pCz + cameraDistance * cos(camRadPitch) * cos(camRadYaw);

        float aimRadPitch = aimPitch_3D * M_PI / 180.0f;
        float aimRadYaw = cameraYaw_3D * M_PI / 180.0f;

        float dirViewX = -sin(aimRadYaw) * cos(aimRadPitch);
        float dirViewY = -sin(aimRadPitch);
        float dirViewZ = -cos(aimRadYaw) * cos(aimRadPitch);

        float dist = levelParameters_3D.aimConvergenceDist;
        float targetX = camX + (dirViewX * dist);
        float targetY = camY + (dirViewY * dist);
        float targetZ = camZ + (dirViewZ * dist);

        float spawnDist = 1.5f;
        hookProjectileX_3D = pCx + (dirViewX * spawnDist);
        hookProjectileY_3D =
            pCy + (player_3D.h * 0.6f) + (dirViewY * spawnDist);
        hookProjectileZ_3D = pCz + (dirViewZ * spawnDist);

        float vecX = targetX - hookProjectileX_3D;
        float vecY = targetY - hookProjectileY_3D;
        float vecZ = targetZ - hookProjectileZ_3D;

        float len = sqrt(vecX * vecX + vecY * vecY + vecZ * vecZ);
        hookProjectileVelX_3D = (vecX / len) * levelParameters_3D.hookSpeed;
        hookProjectileVelY_3D = (vecY / len) * levelParameters_3D.hookSpeed;
        hookProjectileVelZ_3D = (vecZ / len) * levelParameters_3D.hookSpeed;

        // ------------------------------------------------------------

        float forceFactor = currentChargeForce_3D / MAX_AIM_FORCE_DISPLAY_3D;
        currentPullForce_3D =
            forceFactor * levelParameters_3D.maxPullStrengthPhysics;
      }
    }
  }

  // Pojétil do gancho
  if (isHookFiring_3D) {
    float prevHX = hookProjectileX_3D;
    float prevHY = hookProjectileY_3D;
    float prevHZ = hookProjectileZ_3D;

    hookProjectileX_3D += hookProjectileVelX_3D * deltaTime;
    hookProjectileY_3D += hookProjectileVelY_3D * deltaTime;
    hookProjectileZ_3D += hookProjectileVelZ_3D * deltaTime;

    // Verifica se o gancho já atingiu o tamanho máximo
    float distOriginSq = pow(hookProjectileX_3D - pCx, 2) +
                         pow(hookProjectileY_3D - pCy, 2) +
                         pow(hookProjectileZ_3D - pCz, 2);

    if (distOriginSq >
        levelParameters_3D.maxHookLength * levelParameters_3D.maxHookLength) {
      isHookFiring_3D = false;
      printf("FIM DE CURSO: Gancho nao alcancou nada.\n");
    } else {
      for (auto& p : platforms_3D) {
        /**
         * Um problema persistente foi o de atirar para cima e o gancho se
         * prender no chão. Para contornar isso, ignoramos a plataforma que o
         * jogador está encostando
         */
        if (checkAABBCollision(player_3D.x - 0.5f, player_3D.y - 0.5f,
                               player_3D.z - 0.5f, player_3D.w + 1.0f,
                               player_3D.h + 1.0f, player_3D.d + 1.0f, p.x, p.y,
                               p.z, p.w, p.h, p.d)) {
          continue;
        }

        if (p.isHookable && p.frictionCoefficient > 0.001f) {
          float hX, hY, hZ;
          if (lineBoxIntersection(prevHX, prevHY, prevHZ, hookProjectileX_3D,
                                  hookProjectileY_3D, hookProjectileZ_3D, p, hX,
                                  hY, hZ)) {
            // Código de colisão confirmada análogo ao 2D
            isHookFiring_3D = false;
            isHooked_3D = true;
            hookPointX_3D = hX;
            hookPointY_3D = hY;
            hookPointZ_3D = hZ;
            float distSq =
                pow(hX - pCx, 2) + pow(hY - pCy, 2) + pow(hZ - pCz, 2);
            ropeLength_3D = sqrt(distSq);
            isGrounded_3D = false;
            printf("COLISAO: Gancho PRENDEU em %.2f, %.2f, %.2f!\n", hX, hY,
                   hZ);
            break;
          }
        }
      }
    }
  }
  // Física para movumentação ao prender
  if (isHooked_3D) {
    // Vetor do player até o gancho
    float vecX = hookPointX_3D - pCx;
    float vecY = hookPointY_3D - pCy;
    float vecZ = hookPointZ_3D - pCz;
    float currDist = sqrt(vecX * vecX + vecY * vecY + vecZ * vecZ);

    if (currDist > 0.1f) {
      // Direção normalizada
      float normX = vecX / currDist;
      float normY = vecY / currDist;
      float normZ = vecZ / currDist;

      // Adiciona aceleração na direção do gancho baseada na carga e na
      // constante de aceleração definida na fase
      accelX += normX * currentPullForce_3D;
      accelY += normY * currentPullForce_3D;
      accelZ += normZ * currentPullForce_3D;

      // Atualiza vetores de debug
      forceTensionX_3D = normX * currentPullForce_3D;
      forceTensionY_3D = normY * currentPullForce_3D;
      forceTensionZ_3D = normZ * currentPullForce_3D;

      // Uma das principais lógicas do gancho: O jogador deve se aproximar do
      // ponto de disparo
      if (currDist < ropeLength_3D) {
        ropeLength_3D = currDist;
      }

      // Restrição da corda do gancho
      if (currDist > ropeLength_3D) {
        float stretch = currDist - ropeLength_3D;

        // Caso o jogador se afaste do ponto que está preso ao gancho, sua
        // posição é atualizada
        player_3D.x += normX * stretch;
        player_3D.y += normY * stretch;
        player_3D.z += normZ * stretch;

        // Corrige a velocidade do plater, anulando a velocidade de afastamento
        // do ponto que está preso no gancho
        float velAlongRope = player_3D.velocityX * normX +
                             player_3D.velocityY * normY +
                             player_3D.velocityZ * normZ;
        if (velAlongRope < 0) {
          player_3D.velocityX -= normX * velAlongRope;
          player_3D.velocityY -= normY * velAlongRope;
          player_3D.velocityZ -= normZ * velAlongRope;
        }
      }

      // Se a força Y for positiva e forte, tira o status de "Grounded" para
      // evitar que o atrito do chão freie o puxão inicial
      if (currentPullForce_3D > 10.0f && normY > 0.2f) {
        isGrounded_3D = false;
        collidingPlatform_3D = NULL;
      }
    }

    // Resistência do ar para não balançar no gancho eternamente
    float airDamping = 0.99f;
    player_3D.velocityX *= airDamping;
    player_3D.velocityY *= airDamping;
    player_3D.velocityZ *= airDamping;
  }

  // Movimentação no chão (em contato com alguma plataforma). Análogo ao 2D
  if (isGrounded_3D) {
    forceNormalY_3D =
        levelParameters_3D.gravity * levelParameters_3D.playerMass;
    float yawRad = cameraYaw_3D * M_PI / 180.0f;
    float fwdX = -sin(yawRad);
    float fwdZ = -cos(yawRad);
    float rightX = cos(yawRad);
    float rightZ = -sin(yawRad);
    float walkForce = levelParameters_3D.playerWalkAccel;
    float targetRotation = cameraYaw_3D;
    bool isMoving = false;

    if (isPressingForward_3D) {
      targetRotation = cameraYaw_3D + 0.0f;  // Frente
      accelX += fwdX * walkForce;
      accelZ += fwdZ * walkForce;
      isMoving = true;
    }
    if (isPressingBackward_3D) {
      targetRotation = cameraYaw_3D + 180.0f;  // Trás
      accelX -= fwdX * walkForce;
      accelZ -= fwdZ * walkForce;
      isMoving = true;
    }
    if (isPressingLeft_3D) {
      targetRotation = cameraYaw_3D + 90.0f;  // Esquerda
      accelX -= rightX * walkForce;
      accelZ -= rightZ * walkForce;
      isMoving = true;
    }
    if (isPressingRight_3D) {
      targetRotation = cameraYaw_3D - 90.0f;  // Direita
      accelX += rightX * walkForce;
      accelZ += rightZ * walkForce;
      isMoving = true;
    }

    if (isMoving) {
      playerRotationY = targetRotation;
    }

    float frictionCoef = 0.8f;
    if (collidingPlatform_3D)
      frictionCoef = collidingPlatform_3D->frictionCoefficient;
    float damping = 5.0f * frictionCoef;
    if (!isMoving) damping *= 2.0f;

    player_3D.velocityX -= player_3D.velocityX * damping * deltaTime;
    player_3D.velocityZ -= player_3D.velocityZ * damping * deltaTime;
    forceFrictionX_3D = -player_3D.velocityX * damping;
    forceFrictionZ_3D = -player_3D.velocityZ * damping;
  }

  // --------------------------------------------------------------------------------
  // LÓGICA DE ATUALIZAÇÃO DA ANIMAÇÃO DO PLAYER
  // (Utiliza as variáveis globais: currentPlayerAnimState, animTimer,
  // RUN_FRAME_DURATION)
  // --------------------------------------------------------------------------------

  // Variável que checa se alguma tecla de movimento horizontal está sendo
  // pressionada
  bool isMovingHorizontally = isPressingForward_3D || isPressingBackward_3D ||
                              isPressingLeft_3D || isPressingRight_3D;

  // 1. Prioridade: Gancho
  if (isHooked_3D) {
    currentPlayerAnimState = ANIM_GRAPPLER;
    animTimer = 0.0f;  // Reseta o timer, pois a pose de gancho é estática
  }
  // 2. Movimento no Chão (Corrida)
  else if (isGrounded_3D && isMovingHorizontally) {
    currentPlayerAnimState = ANIM_RUNNING;

    // Atualiza o timer de animação
    animTimer += deltaTime;

    // Volta o timer para 0 se o ciclo de 5 frames (5 * 0.05s = 0.25s)
    // terminar
    if (animTimer >= 5.0f * RUN_FRAME_DURATION) {
      animTimer = 0.0f;
    }
  }
  // 3. Padrão: Parado
  else {
    currentPlayerAnimState = ANIM_IDLE;
    animTimer = 0.0f;  // Reseta o timer
  }

  // Nota: Se você quiser a animação de "Gancho" também quando o projétil
  // estiver atirando (antes de prender), adicione a checagem: isHookFiring_3D
  // --------------------------------------------------------------------------------

  player_3D.velocityX += accelX * deltaTime;
  player_3D.velocityY += accelY * deltaTime;
  player_3D.velocityZ += accelZ * deltaTime;

  // Cálculo dos limites de velocidade (aplicado apenas ao andar sobre uma
  // plataforma para não restringir o movimento de lançamento pelo gancho)
  if (isGrounded_3D) {
    float velXZ = sqrt(player_3D.velocityX * player_3D.velocityX +
                       player_3D.velocityZ * player_3D.velocityZ);
    if (velXZ > levelParameters_3D.maxWalkSpeed) {
      float s = levelParameters_3D.maxWalkSpeed / velXZ;
      player_3D.velocityX *= s;
      player_3D.velocityZ *= s;
    }
  }

  // --------------------------------------------------------------------------------

  float totalSpeed = sqrt(player_3D.velocityX * player_3D.velocityX +
                          player_3D.velocityY * player_3D.velocityY +
                          player_3D.velocityZ * player_3D.velocityZ);

  // Pequeno offset para evitar "z-fighting" na colisão
  float epsilon = 0.01f;

  // --------------------------------------------------------------------------------

  // Verificações em X

  player_3D.x += player_3D.velocityX * deltaTime;

  // Verifica paredes quebráveis
  for (auto& wall : breakableWalls_3D) {
    if (wall.isBroken) continue;
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                           player_3D.h, player_3D.d, wall.x, wall.y, wall.z,
                           wall.w, wall.h, wall.d)) {
      float impactForce = levelParameters_3D.playerMass * totalSpeed;
      if (impactForce >= wall.strength) {
        wall.isBroken = true;
        player_3D.velocityX *= 0.6f;

        createLevelVBOs();
      } else {
        if (player_3D.velocityX > 0)
          player_3D.x = wall.x - player_3D.w - epsilon;
        else if (player_3D.velocityX < 0)
          player_3D.x = wall.x + wall.w + epsilon;
        player_3D.velocityX = 0;
      }
    }
  }

  // Verifica Plataformas
  for (const auto& p : platforms_3D) {
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                           player_3D.h, player_3D.d, p.x, p.y, p.z, p.w, p.h,
                           p.d)) {
      if (player_3D.velocityX > 0)
        player_3D.x = p.x - player_3D.w - epsilon;
      else if (player_3D.velocityX < 0)
        player_3D.x = p.x + p.w + epsilon;
      player_3D.velocityX = 0;
    }
  }

  // --------------------------------------------------------------------------------

  // Verificações em Z

  player_3D.z += player_3D.velocityZ * deltaTime;

  // Verifica paredes quebráveis
  for (auto& wall : breakableWalls_3D) {
    if (wall.isBroken) continue;
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                           player_3D.h, player_3D.d, wall.x, wall.y, wall.z,
                           wall.w, wall.h, wall.d)) {
      float impactForce = levelParameters_3D.playerMass * totalSpeed;
      if (impactForce >= wall.strength) {
        wall.isBroken = true;
        player_3D.velocityZ *= 0.6f;
      } else {
        if (player_3D.velocityZ > 0)
          player_3D.z = wall.z - player_3D.d - epsilon;
        else if (player_3D.velocityZ < 0)
          player_3D.z = wall.z + wall.d + epsilon;
        player_3D.velocityZ = 0;
      }
    }
  }

  // Verifica Plataformas
  for (const auto& p : platforms_3D) {
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                           player_3D.h, player_3D.d, p.x, p.y, p.z, p.w, p.h,
                           p.d)) {
      if (player_3D.velocityZ > 0)
        player_3D.z = p.z - player_3D.d - epsilon;
      else if (player_3D.velocityZ < 0)
        player_3D.z = p.z + p.d + epsilon;
      player_3D.velocityZ = 0;
    }
  }

  // --------------------------------------------------------------------------------

  // Verificações em y

  player_3D.y += player_3D.velocityY * deltaTime;

  isGrounded_3D = false;
  collidingPlatform_3D = NULL;

  // Verifica plataformas
  for (auto& p : platforms_3D) {
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                           player_3D.h, player_3D.d, p.x, p.y, p.z, p.w, p.h,
                           p.d)) {
      if (player_3D.velocityY <= 0) {
        player_3D.y = p.y + p.h;
        player_3D.velocityY = 0;
        isGrounded_3D = true;
        collidingPlatform_3D = &p;
      } else if (player_3D.velocityY > 0) {
        player_3D.y = p.y - player_3D.h - epsilon;
        player_3D.velocityY = 0;
      }
    }
  }

  for (auto& wall : breakableWalls_3D) {
    if (wall.isBroken) continue;
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                           player_3D.h, player_3D.d, wall.x, wall.y, wall.z,
                           wall.w, wall.h, wall.d)) {
      if (player_3D.velocityY <= 0) {
        player_3D.y = wall.y + wall.h;
        player_3D.velocityY = 0;
        isGrounded_3D = true;
      } else {
        player_3D.y = wall.y - player_3D.h - epsilon;
        player_3D.velocityY = 0;
      }
    }
  }

  // --------------------------------------------------------------------------------

  if (deltaTime > 0.0001f)  // Evita divisão por zero
  {
    float dvX = player_3D.velocityX - prevVelX;
    float dvY = player_3D.velocityY - prevVelY;
    float dvZ = player_3D.velocityZ - prevVelZ;

    // Aceleração vetorial (m/s^2)
    float ax = dvX / deltaTime;
    float ay = dvY / deltaTime;
    float az = dvZ / deltaTime;

    currentAcceleration_3D = sqrt(ax * ax + ay * ay + az * az);
  } else {
    currentAcceleration_3D = 0.0f;
  }

  // --------------------------------------------------------------------------------

  if (player_3D.y < DEATH_BOUNDARY_Y_3D) {
    isGameOver_3D = true;
    gameOverTimer_3D = 180;
    return GAME_ACTION_CONTINUE;
  }

  // Verificação com espinhos e porta (mais simplificada que as outras pois,
  // até então, espinhos só ficam no chão). Não escolhemos fazer espinhos em
  // paredes ou no teto pois já estava ficando pesada a modelagem

  for (const auto& spike : spikeZones_3D) {
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                           player_3D.h, player_3D.d, spike.x, spike.y, spike.z,
                           spike.w, spike.h, spike.d)) {
      isGameOver_3D = true;
      gameOverTimer_3D = 180;
      return GAME_ACTION_CONTINUE;
    }
  }

  // Porta
  if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                         player_3D.h, player_3D.d, door_3D.x, door_3D.y,
                         door_3D.z, door_3D.w, door_3D.h, door_3D.d)) {
    isGameVictory_3D = true;
    gameVictoryTimer_3D = 180;

    return GAME_ACTION_CONTINUE;
  }

  return GAME_ACTION_CONTINUE;
}

/**
 * Loop principal de renderização.
 * Essa função, assim como no modo 2D, é o coração da visualização do jogo
 */
void gameDisplay_3D() {
  // Limpeza
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  float yawRad = cameraYaw_3D * M_PI / 180.0f;
  float pitchRad = cameraPitch_3D * M_PI / 180.0f;

  float cx = player_3D.x + player_3D.w / 2.0f;
  float cy = player_3D.y + player_3D.h / 2.0f;
  float cz = player_3D.z + player_3D.d / 2.0f;

  float camX = cx + cameraDistance * cos(pitchRad) * sin(yawRad);
  float camY = cy + cameraDistance * sin(pitchRad);
  float camZ = cz + cameraDistance * cos(pitchRad) * cos(yawRad);

  float targetY = cy + 3.0f;

  gluLookAt(camX, camY, camZ, cx, targetY, cz, 0.0f, 1.0f, 0.0f);

  // Luz
  GLfloat lightPos[] = {0.0f, 100.0f, 0.0f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

  // Desenho do chão: se carregamos um modelo OBJ para o chão, desenha-o;
  // caso contrário, usa a display list de grade (para debug)
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  int stride = sizeof(vert);

  // Desenha um VBO único (com textura opcional)
  auto drawSingleVBO = [&](const VBO_Info& info, GLuint textureID) {
    if (info.vboID == 0 || info.triCount == 0) return;
    if (textureID != 0) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, textureID);
      glColor3f(1.0f, 1.0f, 1.0f);
    }
    glBindBuffer(GL_ARRAY_BUFFER, info.vboID);
    glVertexPointer(3, GL_FLOAT, stride, (void*)offsetof(vert, pos));
    glColorPointer(4, GL_FLOAT, stride, (void*)offsetof(vert, cor));
    glNormalPointer(GL_FLOAT, stride, (void*)offsetof(vert, normal));
    glTexCoordPointer(2, GL_FLOAT, stride, (void*)offsetof(vert, tex));
    glDrawArrays(GL_TRIANGLES, 0, info.triCount * 3);
    if (textureID != 0) {
      glBindTexture(GL_TEXTURE_2D, 0);
      glDisable(GL_TEXTURE_2D);
    }
  };

  // 1) Floor (obj) if available - aplica cor de material do chão e escala
  glPushMatrix();
  // Posiciona o chão na altura correta: usa a altura da primeira plataforma se
  // disponível
  float floorDrawY = FLOOR_Y;
  if (!platforms_3D.empty()) floorDrawY = platforms_3D[0].y;
  glTranslatef(0.0f, floorDrawY, 0.0f);
  // Escala para cobrir a área do mundo (modelo assumido ~1 unidade)
  float floorScaleX = WORLD_WIDTH_3D;
  float floorScaleZ = WORLD_DEPTH_3D;
  glScalef(floorScaleX, 1.0f, floorScaleZ);
  // Aplica cor de material (GL_COLOR_MATERIAL deve usar essa cor). Se houver
  // textura de plataforma, a textura será usada; caso contrário aplicamos
  // material diretamente para que o chão tenha cor.
  GLfloat matDiff[4] = {floorColor[0], floorColor[1], floorColor[2], 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matDiff);
  if (floorVBO.vboID != 0 && floorVBO.triCount > 0) {
    if (platformTextureID == 0) {
      // força cor se não houver textura
      glColor3f(floorColor[0], floorColor[1], floorColor[2]);
    }
    drawSingleVBO(floorVBO, platformTextureID);
  } else {
    // Se estivermos desenhando a grade de debug, mantemos a escala e cor
    glColor3f(floorColor[0], floorColor[1], floorColor[2]);
    glCallList(floorListID);
  }
  glPopMatrix();

  // 2) Plataformas (podem ser VBOs gerados ou um modelo de plataformas
  // carregado em platformVBOs) Reutiliza a função existente para listas de VBOs
  auto drawListWithTexture = [&](const std::vector<VBO_Info>& list,
                                 GLuint textureID) {
    if (textureID != 0) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, textureID);
      glColor3f(1.0f, 1.0f, 1.0f);
    }
    for (const auto& info : list) {
      if (info.vboID == 0) continue;
      glBindBuffer(GL_ARRAY_BUFFER, info.vboID);
      glVertexPointer(3, GL_FLOAT, stride, (void*)offsetof(vert, pos));
      glColorPointer(4, GL_FLOAT, stride, (void*)offsetof(vert, cor));
      glNormalPointer(GL_FLOAT, stride, (void*)offsetof(vert, normal));
      glTexCoordPointer(2, GL_FLOAT, stride, (void*)offsetof(vert, tex));
      glDrawArrays(GL_TRIANGLES, 0, info.triCount * 3);
    }
    if (textureID != 0) {
      glBindTexture(GL_TEXTURE_2D, 0);
      glDisable(GL_TEXTURE_2D);
    }
  };

  // Desenho das plataformas: se houver um único modelo platforms.obj carregado
  // (platformVBOs.size()==1) reaplicamos esse modelo para cada plataforma
  if (platformVBOs.size() == 1 && platformVBOs[0].vboID != 0) {
    const auto& modelInfo = platformVBOs[0];
    for (const auto& p : platforms_3D) {
      glPushMatrix();
      glTranslatef(p.x + p.w / 2.0f, p.y, p.z + p.d / 2.0f);
      glScalef(p.w, p.h, p.d);
      if (platformTextureID != 0) {
        drawSingleVBO(modelInfo, platformTextureID);
      } else {
        glColor3f(p.r, p.g, p.b);
        drawSingleVBO(modelInfo, 0);
      }
      glPopMatrix();
    }
  }
  // Caso os VBOs individuais existam em quantidade igual às plataformas,
  // desenha cada VBO sem transform extra (foram gerados já posicionados)
  else if (!platforms_3D.empty() &&
           platformVBOs.size() == platforms_3D.size()) {
    for (size_t i = 0; i < platforms_3D.size(); ++i) {
      const auto& info = platformVBOs[i];
      if (info.vboID == 0) continue;
      glBindBuffer(GL_ARRAY_BUFFER, info.vboID);
      glVertexPointer(3, GL_FLOAT, stride, (void*)offsetof(vert, pos));
      glColorPointer(4, GL_FLOAT, stride, (void*)offsetof(vert, cor));
      glNormalPointer(GL_FLOAT, stride, (void*)offsetof(vert, normal));
      glTexCoordPointer(2, GL_FLOAT, stride, (void*)offsetof(vert, tex));
      if (platformTextureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, platformTextureID);
        glColor3f(1.0f, 1.0f, 1.0f);
      }
      glDrawArrays(GL_TRIANGLES, 0, info.triCount * 3);
      if (platformTextureID != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
      }
    }
  }
  // Fallback visual: desenha cada plataforma como um cubo simples
  else {
    for (const auto& p : platforms_3D) {
      glPushMatrix();
      glTranslatef(p.x + p.w / 2.0f, p.y + p.h / 2.0f, p.z + p.d / 2.0f);
      glScalef(p.w, p.h, p.d);
      if (platformTextureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, platformTextureID);
        glColor3f(1.0f, 1.0f, 1.0f);
      } else {
        glColor3f(0.6f, 0.6f, 0.6f);
      }
      glutSolidCube(1.0);
      if (platformTextureID != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
      }
      glPopMatrix();
    }
  }

  // 3) Walls: se houver modelo único carregado em envWallVBO reaplicamos por
  // parede
  if (envWallVBO.vboID != 0 && envWallVBO.triCount > 0) {
    for (const auto& w : breakableWalls_3D) {
      if (w.isBroken) continue;
      glPushMatrix();
      glTranslatef(w.x + w.w / 2.0f, w.y, w.z + w.d / 2.0f);
      glScalef(w.w, w.h, w.d);
      drawSingleVBO(envWallVBO, wallTextureID);
      glPopMatrix();
    }
  }
  // Senão tenta desenhar VBOs individuais ou fallback em cubos
  else if (!breakableWalls_3D.empty() &&
           wallVBOs.size() == breakableWalls_3D.size()) {
    for (size_t i = 0; i < breakableWalls_3D.size(); ++i) {
      const auto& info = wallVBOs[i];
      if (info.vboID == 0) continue;
      glBindBuffer(GL_ARRAY_BUFFER, info.vboID);
      glVertexPointer(3, GL_FLOAT, stride, (void*)offsetof(vert, pos));
      glColorPointer(4, GL_FLOAT, stride, (void*)offsetof(vert, cor));
      glNormalPointer(GL_FLOAT, stride, (void*)offsetof(vert, normal));
      glTexCoordPointer(2, GL_FLOAT, stride, (void*)offsetof(vert, tex));
      if (wallTextureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, wallTextureID);
        glColor3f(1.0f, 1.0f, 1.0f);
      }
      glDrawArrays(GL_TRIANGLES, 0, info.triCount * 3);
      if (wallTextureID != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
      }
    }
  } else {
    for (const auto& w : breakableWalls_3D) {
      if (w.isBroken) continue;
      glPushMatrix();
      glTranslatef(w.x + w.w / 2.0f, w.y + w.h / 2.0f, w.z + w.d / 2.0f);
      glScalef(w.w, w.h, w.d);
      if (wallTextureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, wallTextureID);
        glColor3f(1.0f, 1.0f, 1.0f);
      } else {
        glColor3f(w.r, w.g, w.b);
      }
      glutSolidCube(1.0);
      if (wallTextureID != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
      }
      glPopMatrix();
    }
  }

  // 4) Spikes: desenha zonas de spikes visualmente
  if (!spikeZones_3D.empty()) {
    for (const auto& s : spikeZones_3D) {
      // Se houver um VBO geral de spikes (modelo), desenha com transform
      if (envSpikeVBO.vboID != 0 && envSpikeVBO.triCount > 0) {
        glPushMatrix();
        glTranslatef(s.x + s.w / 2.0f, s.y + s.h / 2.0f, s.z + s.d / 2.0f);
        glScalef(s.w, s.h, s.d);
        drawSingleVBO(envSpikeVBO, spikeTextureID);
        glPopMatrix();
      } else {
        // Fallback: desenha um plano/placa vermelha para indicar perigo
        glPushMatrix();
        glTranslatef(s.x + s.w / 2.0f, s.y + 0.05f, s.z + s.d / 2.0f);
        glScalef(s.w, 0.1f, s.d);
        glColor3f(0.5f, 0.0f, 0.0f);
        glutSolidCube(1.0);
        glPopMatrix();
      }
    }
  } else {
    // Se não houver zonas descritas, ainda desenha spikes VBOs se houver
    if (envSpikeVBO.vboID != 0 && envSpikeVBO.triCount > 0) {
      drawSingleVBO(envSpikeVBO, spikeTextureID);
    } else {
      drawListWithTexture(spikeVBOs, spikeTextureID);
    }
  }

  // Porta (obj) com textura — usa VBO se carregado, senão fallback para cubo
  glPushMatrix();
  glTranslatef(door_3D.x, door_3D.y, door_3D.z);
  glTranslatef(door_3D.w / 2, door_3D.h / 2, door_3D.d / 2);
  glScalef(door_3D.w, door_3D.h, door_3D.d);

  if (doorVBO.vboID != 0 && doorVBO.triCount > 0) {
    // desenha o VBO da porta (assume que os UVs estão corretos)
    drawSingleVBO(doorVBO, doorTextureID);
  } else {
    if (doorTextureID != 0) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, doorTextureID);
      glColor3f(1.0f, 1.0f, 1.0f);
    } else {
      glColor3f(0.0f, 1.0f, 0.0f);
    }
    glutSolidCube(1.0);
    if (doorTextureID != 0) {
      glBindTexture(GL_TEXTURE_2D, 0);
      glDisable(GL_TEXTURE_2D);
    }
  }
  glPopMatrix();

  // Após desenhar a porta, desligamos os client states usados para VBOs
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  // --- SELEÇÃO E DESENHO DO PLAYER ---

  // 1. DECLARAÇÃO DAS VARIÁVEIS LOCAIS (FALTOU ISSO!)
  GLuint currentVBO = 0;
  int currentTriCount = 0;
  bool isGrappling_3D = false;

  // 2. LÓGICA DE ANIMAÇÃO
  switch (currentPlayerAnimState) {
    case ANIM_IDLE:
      currentVBO = playerModels[5].vbo;
      currentTriCount = playerModels[5].triCount;
      break;

    case ANIM_GRAPPLER:
      currentVBO = playerModels[6].vbo;
      currentTriCount = playerModels[6].triCount;
      break;

    case ANIM_RUNNING: {
      // Calcula o índice do frame de corrida (0 a 4)
      int frameIndex = (int)(animTimer / RUN_FRAME_DURATION) % 5;

      currentVBO = playerModels[frameIndex].vbo;
      currentTriCount = playerModels[frameIndex].triCount;
      break;
    }  // <<< ADICIONE ESTA CHAVE DE FECHAMENTO

    default:
      currentVBO = playerModels[5].vbo;
      currentTriCount = playerModels[5].triCount;
      break;
  }

  // 1. Prioridade: Animação do Gancho
  if (isGrappling_3D) {
    currentPlayerAnimState = ANIM_GRAPPLER;
  }
  // 2. Prioridade: Animação de Corrida
  else if (isPressingForward_3D || isPressingBackward_3D || isPressingLeft_3D ||
           isPressingRight_3D) {
    currentPlayerAnimState = ANIM_RUNNING;
  }
  // 3. Padrão: Animação Parada
  else {
    currentPlayerAnimState = ANIM_IDLE;
  }

  // Atualiza o timer de animação (apenas se estiver correndo)
  if (currentPlayerAnimState == ANIM_RUNNING) {
    animTimer += 1.0f / 60.0f;  // Assumindo 60 FPS
  } else {
    animTimer = 0.0f;  // Reseta o timer em outros estados
  }

  // Player
  glPushMatrix();
  // Translação para a posição do jogador
  glTranslatef(player_3D.x, player_3D.y, player_3D.z);

  // Ajuste de Escala e Offset
  // Modelos importados raramente vêm no tamanho certo ou com o pivô no pé.
  // Você vai precisar "brincar" com esses valores:
  glTranslatef(
      player_3D.w / 2, 0.0f,
      player_3D.d / 2);  // Centralizar no pivô (ajuste conforme necessário)
  float scaleFactor =
      1.0f;  // Se o modelo for gigante, use 0.1f, se for minusculo, use 10.0f
  glScalef(scaleFactor, scaleFactor, scaleFactor);

  // Rotação (Opcional: fazer o boneco olhar para a direção da câmera)
  glRotatef(playerRotationY + 180.0f, 0.0f, 1.0f, 0.0f);

  // SE o carregamento funcionou, desenha o modelo OBJ
  if (currentVBO != 0 && playerTexture != 0) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, playerTexture);
    glColor3f(1.0f, 1.0f, 1.0f);  // Cor branca para não tingir a textura

    glBindBuffer(GL_ARRAY_BUFFER, currentVBO);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);  // Habilita UVs

    int stride = sizeof(vert);

    // Offset de Posição
    glVertexPointer(3, GL_FLOAT, stride, (void*)offsetof(vert, pos));
    // Offset de Normal
    glNormalPointer(GL_FLOAT, stride, (void*)offsetof(vert, normal));
    // Offset de Textura (UV)
    glTexCoordPointer(2, GL_FLOAT, stride, (void*)offsetof(vert, tex));

    glDrawArrays(GL_TRIANGLES, 0, currentTriCount * 3);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
  } else {
    // Fallback: Desenha o cubo vermelho se o modelo falhar
    glTranslatef(0, player_3D.h / 2, 0);
    glScalef(player_3D.w, player_3D.h, player_3D.d);
    if (isGameOver_3D)
      glColor3f(0.2f, 0.0f, 0.0f);
    else
      glColor3f(1.0f, 0.0f, 0.0f);
    glutSolidCube(1.0);
  }
  glPopMatrix();

  // Corda
  if (isHooked_3D || isHookFiring_3D) {
    // Desabilita luz para garantir que a linha tenha cor sólida
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glColor3f(0.8f, 0.8f, 0.8f);

    // Calcula posição em relação ao player para início da linha do gancho
    float cx = player_3D.x + player_3D.w / 2.0f;
    float cyShot = player_3D.y + player_3D.h / 2.0f + (player_3D.h * 0.4f);
    float cz = player_3D.z + player_3D.d / 2.0f;

    glBegin(GL_LINES);
    glVertex3f(cx, cyShot, cz);

    if (isHookFiring_3D) {
      glVertex3f(hookProjectileX_3D, hookProjectileY_3D, hookProjectileZ_3D);
    } else {
      glVertex3f(hookPointX_3D, hookPointY_3D, hookPointZ_3D);
    }
    glEnd();

    // Tensão
    if (isHooked_3D) {
      // Calcula o ponto médio da corda
      float midX = (cx + hookPointX_3D) / 2.0f;
      float midY = (cyShot + hookPointY_3D) / 2.0f;
      float midZ = (cz + hookPointZ_3D) / 2.0f;

      // Pega o valor da tensão
      float tMag = sqrt(forceTensionX_3D * forceTensionX_3D +
                        forceTensionY_3D * forceTensionY_3D +
                        forceTensionZ_3D * forceTensionZ_3D);

      char tensionStr[32];
      sprintf(tensionStr, "%.1f N", tMag);

      glColor3f(0.2f, 0.2f, 1.0f);

      // Desenha um pouco acima da linha para não sobrepor totalmente
      drawText3D(midX, midY + 1.0f, midZ, tensionStr);
    }

    // Desenha a ponta do gancho se ele estiver voando
    if (isHookFiring_3D) {
      glPushMatrix();
      glTranslatef(hookProjectileX_3D, hookProjectileY_3D, hookProjectileZ_3D);
      glColor3f(1.0f, 1.0f, 0.0f);
      glutSolidSphere(0.3f, 8, 8);
      glPopMatrix();
    }

    glEnable(GL_LIGHTING);
  }

  // Desabilita luz e textura para o texto ficar nítido e com cor sólida
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);

  glColor3f(1.0f, 1.0f, 0.0f);

  char frictionText[32];

  for (const auto& p : platforms_3D) {
    // Calcula o centro da plataforma na parte de cima para escolher onde
    // colocar o texto
    float cx = p.x + p.w / 2.0f;
    float cy = p.y + p.h + 0.5f;
    float cz = p.z + p.d / 2.0f;

    sprintf(frictionText, "u: %.2f", p.frictionCoefficient);

    drawText3D(cx, cy, cz, frictionText);
  }

  glColor3f(1.0f, 0.5f, 0.0f);

  char strText[32];
  for (const auto& w : breakableWalls_3D) {
    if (w.isBroken) continue;

    // Calcula o centro da parede na parte de cima para escolher onde colocar
    // o texto
    float cx = w.x + w.w / 2.0f;
    float cy = w.y + w.h + 0.5f;
    float cz = w.z + w.d / 2.0f;

    sprintf(strText, "%.0fN", w.strength);
    drawText3D(cx, cy, cz, strText);
  }

  // Reabilita iluminação para o resto da cena
  glEnable(GL_LIGHTING);

  drawDebugVectors();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  int w = glutGet(GLUT_WINDOW_WIDTH);
  int h = glutGet(GLUT_WINDOW_HEIGHT);
  gluOrtho2D(0, w, h, 0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  if (isGameVictory_3D) {
    drawVictoryScreen(w, h);
  } else if (isGameOver_3D) {
    drawGameOverScreen(w, h);
  } else  // Jogo está rodando normalmente
  {
    // Barra de carga
    if (isChargingHook_3D) {
      drawChargeBar(w, h);
    }
    drawPhysicsDebugHUD_3D();
    drawShotCounterHUD_3D();

    float centerX = w / 2.0f;
    float centerY = h / 2.0f;

    float pitchDiff = aimPitch_3D - cameraPitch_3D;

    /**
     * Mesma lógica de definir o local do tiro (algumas funções acima)
     */
    float pitchDiffRad = pitchDiff * M_PI / 180.0f;
    float fovDeg = 45.0f;
    float fovRad = (fovDeg / 2.0f) * M_PI / 180.0f;
    float screenHalfHeight = h / 2.0f;
    float crossY =
        centerY + screenHalfHeight * (tan(pitchDiffRad) / tan(fovRad));

    // Mira
    glColor3f(0.0f, 1.0f, 0.0f);
    glLineWidth(2.0f);
    float size = 10.0f;
    glBegin(GL_LINES);
    glVertex2f(centerX - size, crossY);
    glVertex2f(centerX + size, crossY);
    glVertex2f(centerX, crossY - size);
    glVertex2f(centerX, crossY + size);
    glEnd();
  }

  // Restaura estado OpenGL para o modo 3D
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

/**
 * Chamada quando a janela é redimensionada. Análogo ao modo 2D, exceto pela
 * perspectiva
 */
void gameReshape_3D(int w, int h) {
  // Previne divisão por zero se a janela for minimizada ou muito pequena
  if (h == 0) h = 1;

  float ratio = 1.0f * w / h;
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Visualização de perspectiva escolhida para se adaptar ao modo 3D em 3ª
  // pessoa
  gluPerspective(45.0f, (float)w / h, 0.1f, 2000.0f);
  glMatrixMode(GL_MODELVIEW);
}

/**
 * Callback quando o mouse se move
 */
void gameMouseMotion_3D(int x, int y) {
  lastMouseX_3D = x;
  lastMouseY_3D = y;

  if (isMouseFree_3D) return;

  int w = glutGet(GLUT_WINDOW_WIDTH);
  int h = glutGet(GLUT_WINDOW_HEIGHT);
  int cx = w / 2, cy = h / 2;

  if (!mouseInitialized_3D) {
    // Assim como no início da fase, fazemos o mouse ir para o centro da tela
    // em modo FPS
    mouseInitialized_3D = true;
    glutWarpPointer(cx, cy);
    return;
  }

  float dx = (float)(x - cx);
  float dy = (float)(cy - y);

  if (dx == 0 && dy == 0) return;

  cameraYaw_3D -= dx * MOUSE_SENSITIVITY_3D;
  aimPitch_3D -= dy * MOUSE_SENSITIVITY_3D;

  // Cálculo de onde a câmera deve ficar
  float targetCameraPitch = aimPitch_3D;

  // Cálculo dinâmico para definir um limite para o usuário não olhar tanto
  // para cima. Fizemos isso pois o chão acabava entrando na frente da
  // visualização da tela pelo personagem
  float margin = 2.0f;
  float heightDiff = (FLOOR_Y + margin) - player_3D.y;
  float ratio = heightDiff / cameraDistance;

  if (ratio < -1.0f) ratio = -1.0f;
  if (ratio > 1.0f) ratio = 1.0f;

  float dynamicMinPitch = asin(ratio) * 180.0f / M_PI;

  // Aplica os limites físicos
  if (targetCameraPitch < dynamicMinPitch) {
    targetCameraPitch = dynamicMinPitch;
  }
  if (targetCameraPitch > MAX_PITCH_3D) {
    targetCameraPitch = MAX_PITCH_3D;
  }

  // Trava da mira em relação à tela de visualização (essa trava é diferente
  // da trava da tela)
  const float PIXELS_PER_DEGREE = 15.0f;
  float maxAngleDiff = ((h / 2.0f) - 20.0f) / PIXELS_PER_DEGREE;

  if (aimPitch_3D < targetCameraPitch - maxAngleDiff) {
    aimPitch_3D = targetCameraPitch - maxAngleDiff;
  }
  if (aimPitch_3D > targetCameraPitch + maxAngleDiff) {
    aimPitch_3D = targetCameraPitch + maxAngleDiff;
  }
  cameraPitch_3D = targetCameraPitch;

  if (aimPitch_3D < -89.0f) aimPitch_3D = -89.0f;
  if (aimPitch_3D > 89.0f) aimPitch_3D = 89.0f;

  glutWarpPointer(cx, cy);
}

/**
 * Chamada quando um botão do mouse é clicado.
 */
void gameMouseClick_3D(int button, int state) {
  if (isGameOver_3D || isGameVictory_3D) return;

  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      // Inicia carregamento da força do gancho
      if (!isHooked_3D && !isHookFiring_3D && shotsRemaining_3D > 0) {
        isChargingHook_3D = true;
        chargeStartTime_3D = glutGet(GLUT_ELAPSED_TIME);
        currentChargeForce_3D = 0.0f;
        chargePercentage_3D = 0.0f;
        printf("MOUSE DOWN: Carregando...\n");
      } else {
        printf("MOUSE DOWN: Bloqueado (Preso=%d, Voando=%d, Tiros=%d)\n",
               isHooked_3D, isHookFiring_3D, shotsRemaining_3D);
      }
    } else if (state == GLUT_UP) {
      // Finaliza o carregamento e solicita disparo do gancho
      if (isChargingHook_3D) {
        isChargingHook_3D = false;
        requestHookRelease_3D = true;
        printf("MOUSE UP: Solicitando disparo!\n");
      }
    }
  }
}

/**
 * Chamada quando uma tecla é pressionada.
 */
GameAction gameKeyDown_3D(unsigned char key, int x, int y) {
  switch (key) {
    case 'w':
    case 'W':
      isPressingForward_3D = true;
      break;
    case 'a':
    case 'A':
      isPressingLeft_3D = true;
      break;
    case 's':
    case 'S':
      isPressingBackward_3D = true;
      break;
    case 'd':
    case 'D':
      isPressingRight_3D = true;
      break;

    // Tecla 32 = Espaço
    case 32:
      if (isHooked_3D || isHookFiring_3D) {
        isHooked_3D = false;
        isHookFiring_3D = false;
        isGrounded_3D = false;

        // Limpa vetores visuais
        forceTensionX_3D = 0;
        forceTensionY_3D = 0;
        forceTensionZ_3D = 0;

        printf("COMANDO: Gancho cancelado/solto pelo Espaco!\n");
      }

      if (isChargingHook_3D) {
        isChargingHook_3D = false;
        chargePercentage_3D = 0.0f;
        printf("COMANDO: Carregamento cancelado.\n");
      }
      break;

    case 'm':
    case 'M':
      isMouseFree_3D = !isMouseFree_3D;
      if (isMouseFree_3D)
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
      else {
        glutSetCursor(GLUT_CURSOR_NONE);
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);
        glutWarpPointer(w / 2, h / 2);
        mouseInitialized_3D = false;
      }
      break;

    case 'q':
    case 'Q':
    // Tecla 27 = ESC
    case 27:
      isMouseFree_3D = true;
      glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
      return GAME_ACTION_EXIT_TO_MENU;
  }
  return GAME_ACTION_CONTINUE;
}

/**
 * Chamada quando uma tecla é solta.
 */
void gameKeyUp_3D(unsigned char key, int x, int y) {
  switch (key) {
    case 'w':
    case 'W':
      isPressingForward_3D = false;
      break;
    case 'a':
    case 'A':
      isPressingLeft_3D = false;
      break;
    case 's':
    case 'S':
      isPressingBackward_3D = false;
      break;
    case 'd':
    case 'D':
      isPressingRight_3D = false;
      break;
  }
}

// Funções de Teclas Especiais (não usamos para não complicar a jogabilidade)
void gameSpecialDown_3D(int key, int x, int y) {}
void gameSpecialUp_3D(int key, int x, int y) {}

/**
 * Carrega texturas e assets 3D do jogo, incluindo texturas do jogador e
 * ambiente. Inicializa os VBOs para todos os modelos animados do jogador e
 * texturas de ambiente.
 */
void loadGameTextures_3D() {
  printf("Carregando assets 3D...\n");
  std::string path;

  // 1. Carregar a TEXTURA do PLAYER (É a mesma para todos os OBJs)
  playerTexture = loadTexture_3D("assets/images/player/player_texture.png");

  // 2. Carregar as TEXTURAS DE AMBIENTE (colormap.png)
  // Assumindo que colormap.png está em assets/images/levelX/
  platformTextureID = loadTexture_3D("assets/images/level1/colormap.png");
  wallTextureID = loadTexture_3D("assets/images/level1/colormap.png");
  doorTextureID = loadTexture_3D("assets/images/level1/colormap.png");
  spikeTextureID = loadTexture_3D("assets/images/level1/colormap.png");

  // 3. Carregar os 5 OBJs de Corrida
  for (int i = 0; i < 5; ++i) {
    path = "models/player_correndo_" + std::to_string(i + 1) + ".obj";
    std::vector<tri> tris;
    carregaOBJemTris(path.c_str(), tris);

    if (!tris.empty()) {
      playerModels[i].triCount = tris.size();
      playerModels[i].vbo =
          criaVBODeTris(playerModels[i].triCount, tris.data());
      printf("Modelo %s carregado (VBO: %u)\n", path.c_str(),
             playerModels[i].vbo);
    } else {
      // Em caso de erro, definimos triCount como 0 para não tentar renderizar
      playerModels[i].triCount = 0;
      printf("ERRO: Falha ao carregar modelo: %s\n", path.c_str());
    }
  }

  // 4. Carregar o OBJ Parado (IDLE - Armazenado no índice 5)
  path = "models/player.obj";
  std::vector<tri> tris_idle;
  carregaOBJemTris(path.c_str(), tris_idle);
  playerModels[5].triCount = tris_idle.size();
  playerModels[5].vbo =
      criaVBODeTris(playerModels[5].triCount, tris_idle.data());
  printf("Modelo Parado (player.obj) carregado (VBO: %u)\n",
         playerModels[5].vbo);

  // 5. Carregar o OBJ Gancho (GRAPPLER - Armazenado no índice 6)
  path = "models/player_grappler.obj";
  std::vector<tri> tris_grappler;
  carregaOBJemTris(path.c_str(), tris_grappler);
  playerModels[6].triCount = tris_grappler.size();
  playerModels[6].vbo =
      criaVBODeTris(playerModels[6].triCount, tris_grappler.data());
  printf("Modelo Gancho (grappler.obj) carregado (VBO: %u)\n",
         playerModels[6].vbo);
}