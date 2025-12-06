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
#include "decorations.h"
#include "game.h"

// ---------------------------------------------------------------------------------------------------------

/**
 * Constantes usadas na definição de mundo e para visualização.
 */
const float PLAYER_HEIGHT_3D = 80.0f;
const float VIEW_WIDTH_3D = 800.0f;
const float VIEW_HEIGHT_3D = 600.0f;

// O mundo agora é um volume
const float WORLD_WIDTH_3D = 800.0f;
const float WORLD_HEIGHT_3D = 600.0f;
const float WORLD_DEPTH_3D = 800.0f;

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
const float FLOOR_Y = -100.0f;  // Chão visual na mesma altura do game over

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

// Ponto onde o gancho prendeu (usado para movimentação e debug)
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
GLuint decorationTextureID = 0;  // Textura para objetos decorativos (colormap.png)
GLuint colormap2TextureID = 0;    // colormap2.png
GLuint colormap3TextureID = 0;    // colormap3.png

GLuint playerTexture = 0;      // ID da textura do player
float playerRotationY = 0.0f;  // Rotação do player em graus

// VBOs específicos de objetos de ambiente (quando carregados via OBJ)
VBO_Info floorVBO = {0, 0};
VBO_Info doorVBO = {0, 0};
VBO_Info envWallVBO = {0, 0};
VBO_Info envSpikeVBO = {0, 0};

// VBOs para novos elementos de cena
VBO_Info blockLargeVBO = {0, 0};     // Corredor (bloco sólido grande)
VBO_Info spawnVBO = {0, 0};          // Plataforma de spawn do jogador
VBO_Info platformVBO = {0, 0};       // Plataformas flutuantes (antigo block-tall)
VBO_Info flowersVBO = {0, 0};        // Decoração
VBO_Info plantVBO = {0, 0};          // Decoração
VBO_Info rocksVBO = {0, 0};          // Decoração
VBO_Info decorationTall1VBO = {0, 0}; // Decoração alta 1 (antigo tree)
VBO_Info decorationTall2VBO = {0, 0}; // Decoração alta 2 (antigo tree-pine)
VBO_Info decorationTall3VBO = {0, 0}; // Decoração alta 3
VBO_Info decoration1VBO = {0, 0};     // Decoração pequena 1
VBO_Info decoration2VBO = {0, 0};     // Decoração pequena 2
VBO_Info decoration3VBO = {0, 0};     // Decoração pequena 3

// Vetores para armazenar posições de objetos decorativos
std::vector<DecoObject> hexagons_3D;
std::vector<DecoObject> flowers_3D;
std::vector<DecoObject> plants_3D;
std::vector<DecoObject> rocks_3D;
std::vector<DecoObject> pines_3D;
std::vector<DecoObject> trees_3D;
std::vector<DecoObject> decoration1_3D;
std::vector<DecoObject> decoration2_3D;
std::vector<DecoObject> decoration3_3D;

// Cor do material do chão por nível (R, G, B)
float floorColor[3] = {0.4f, 0.25f, 0.15f};  // Marrom terra para levels 1 e 2

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

  // Desabilita o carregamento de floor.obj para usar cor sólida
  // Se habilitado, carrega floor.obj (chão) - prefer level, depois global
  // if (!tryLoadSingleOBJ("floor.obj", floorVBO, levelDir)) {
    // garante zeros se falhar - força uso da grade colorida
    floorVBO.vboID = 0;
    floorVBO.triCount = 0;
  // } else {
  //   printf("Modelo floor.obj carregado (VBO=%u, tris=%d)\n", floorVBO.vboID,
  //          floorVBO.triCount);
  // }

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

  // Carrega spikes.obj (level-specific, não mais global)
  // Tenta carregar spikes.obj (plural) ou spike.obj (singular)
  if (!tryLoadSingleOBJ("spike.obj", envSpikeVBO, levelDir) &&
      !tryLoadSingleOBJ("spikes.obj", envSpikeVBO, levelDir)) {
    envSpikeVBO.vboID = 0;
    envSpikeVBO.triCount = 0;
  } else {
    printf("Modelo spike(s).obj carregado (VBO=%u, tris=%d)\n",
           envSpikeVBO.vboID, envSpikeVBO.triCount);
  }

  // Carrega spawn.obj (plataforma de spawn, antiga platform.obj)
  // Armazena um único VBO para instâncias de spawn.
  {
    VBO_Info tmp = {0, 0};
    bool loaded = false;
    if (tryLoadSingleOBJ("spawn.obj", tmp, levelDir))
      loaded = true;
    else if (tryLoadSingleOBJ("spawns.obj", tmp, levelDir))
      loaded = true;
    else if (tryLoadSingleOBJ("spawn.obj", tmp, globalDir))
      loaded = true;
    else if (tryLoadSingleOBJ("spawns.obj", tmp, globalDir))
      loaded = true;

    if (loaded) {
      if (spawnVBO.vboID != 0) glDeleteBuffers(1, &spawnVBO.vboID);
      spawnVBO = tmp;
      printf(
          "Modelo spawn(s).obj carregado com sucesso! (VBO=%u, tris=%d)\n",
          tmp.vboID, tmp.triCount);
    }
  }

  // --- Carrega modelos de decoração ambiental ---
  
  // block-large.obj (corredor sólido)
  if (!tryLoadSingleOBJ("block-large.obj", blockLargeVBO, levelDir)) {
    blockLargeVBO.vboID = 0;
    blockLargeVBO.triCount = 0;
  } else {
    printf("Modelo block-large.obj carregado (VBO=%u, tris=%d)\n", 
           blockLargeVBO.vboID, blockLargeVBO.triCount);
  }

  // platform.obj (plataformas flutuantes, antigo block-tall.obj)
  if (!tryLoadSingleOBJ("platform.obj", platformVBO, levelDir)) {
    platformVBO.vboID = 0;
    platformVBO.triCount = 0;
  } else {
    printf("Modelo platform.obj carregado (VBO=%u, tris=%d)\n", 
           platformVBO.vboID, platformVBO.triCount);
  }

  // flowers.obj (decoração)
  if (!tryLoadSingleOBJ("flowers.obj", flowersVBO, levelDir)) {
    flowersVBO.vboID = 0;
    flowersVBO.triCount = 0;
  } else {
    printf("Modelo flowers.obj carregado (VBO=%u, tris=%d)\n", 
           flowersVBO.vboID, flowersVBO.triCount);
  }

  // plant.obj (decoração)
  if (!tryLoadSingleOBJ("plant.obj", plantVBO, levelDir)) {
    plantVBO.vboID = 0;
    plantVBO.triCount = 0;
  } else {
    printf("Modelo plant.obj carregado (VBO=%u, tris=%d)\n", 
           plantVBO.vboID, plantVBO.triCount);
  }

  // rocks.obj (decoração)
  if (!tryLoadSingleOBJ("rocks.obj", rocksVBO, levelDir)) {
    rocksVBO.vboID = 0;
    rocksVBO.triCount = 0;
  } else {
    printf("Modelo rocks.obj carregado (VBO=%u, tris=%d)\n", 
           rocksVBO.vboID, rocksVBO.triCount);
  }

  // decoration-tall2.obj (decoração alta 2, antigo tree-pine.obj)
  if (!tryLoadSingleOBJ("decoration-tall2.obj", decorationTall2VBO, levelDir)) {
    decorationTall2VBO.vboID = 0;
    decorationTall2VBO.triCount = 0;
  } else {
    printf("Modelo decoration-tall2.obj carregado (VBO=%u, tris=%d)\n", 
           decorationTall2VBO.vboID, decorationTall2VBO.triCount);
  }

  // decoration-tall1.obj (decoração alta 1, antigo tree.obj)
  if (!tryLoadSingleOBJ("decoration-tall1.obj", decorationTall1VBO, levelDir)) {
    decorationTall1VBO.vboID = 0;
    decorationTall1VBO.triCount = 0;
  } else {
    printf("Modelo decoration-tall1.obj carregado (VBO=%u, tris=%d)\n", 
           decorationTall1VBO.vboID, decorationTall1VBO.triCount);
  }

  // decoration-tall3.obj (decoração alta 3)
  if (!tryLoadSingleOBJ("decoration-tall3.obj", decorationTall3VBO, levelDir)) {
    decorationTall3VBO.vboID = 0;
    decorationTall3VBO.triCount = 0;
  } else {
    printf("Modelo decoration-tall3.obj carregado (VBO=%u, tris=%d)\n", 
           decorationTall3VBO.vboID, decorationTall3VBO.triCount);
  }

  // decoration1.obj (decoração pequena 1)
  if (!tryLoadSingleOBJ("decoration1.obj", decoration1VBO, levelDir)) {
    decoration1VBO.vboID = 0;
    decoration1VBO.triCount = 0;
  } else {
    printf("Modelo decoration1.obj carregado (VBO=%u, tris=%d)\n", 
           decoration1VBO.vboID, decoration1VBO.triCount);
  }

  // decoration2.obj (decoração pequena 2)
  if (!tryLoadSingleOBJ("decoration2.obj", decoration2VBO, levelDir)) {
    decoration2VBO.vboID = 0;
    decoration2VBO.triCount = 0;
  } else {
    printf("Modelo decoration2.obj carregado (VBO=%u, tris=%d)\n", 
           decoration2VBO.vboID, decoration2VBO.triCount);
  }

  // decoration3.obj (decoração pequena 3)
  if (!tryLoadSingleOBJ("decoration3.obj", decoration3VBO, levelDir)) {
    decoration3VBO.vboID = 0;
    decoration3VBO.triCount = 0;
  } else {
    printf("Modelo decoration3.obj carregado (VBO=%u, tris=%d)\n", 
           decoration3VBO.vboID, decoration3VBO.triCount);
  }
}

/**
 * Cria a grade do chão para ajudar na visualização da movimentação.
 * Agora usa um plano sólido que respeita a cor de material (floorColor)
 */
void createWorldDisplayLists() {
  if (floorListID != 0) glDeleteLists(floorListID, 1);

  floorListID = glGenLists(1);
  glNewList(floorListID, GL_COMPILE);
  
  // Desenha um plano sólido que respeita o material/iluminação
  glBegin(GL_QUADS);
  glNormal3f(0.0f, 1.0f, 0.0f);  // Normal apontando para cima
  glVertex3f(-1.0f, 0.0f, -1.0f);
  glVertex3f(1.0f, 0.0f, -1.0f);
  glVertex3f(1.0f, 0.0f, 1.0f);
  glVertex3f(-1.0f, 0.0f, 1.0f);
  glEnd();
  
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
    // Dia claro (floresta)
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    floorColor[0] = 0.9f;
    floorColor[1] = 0.9f;
    floorColor[2] = 0.9f;  // branco
  } else {
    // Level 1 e padrão: dia/floresta
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    floorColor[0] = 0.55f;
    floorColor[1] = 0.35f;
    floorColor[2] = 0.2f;  // marrom terra mais claro
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

  cameraYaw_3D = 180.0f;
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
      // ========== LEVEL 1: Campo de Treinamento - Corredor Verde ==========
      levelParameters_3D.playerMass = 10.0f;
      levelParameters_3D.playerWalkAccel = 60.0f;
      levelParameters_3D.maxWalkSpeed = 15.0f;
      levelParameters_3D.maxPlayerSpeed = 40.0f;
      levelParameters_3D.maxHookLength = 150.0f;
      levelParameters_3D.vectorVisualScale = 0.04f;
      levelParameters_3D.gravity = 25.0f;
      levelParameters_3D.hookSpeed = 150.0f;
      levelParameters_3D.maxShots = 20;
      levelParameters_3D.maxPullStrengthPhysics = 150.0f;
      levelParameters_3D.dampingFactor = 0.995f;
      levelParameters_3D.aimConvergenceDist = 40.0f;

      // CHÃO - Corredor comprido no eixo Z (0 a 500) - SÓLIDO
      platforms_3D.push_back({-25.0f, -5.0f, 400.0f,
                              40.0f, 10.0f, 200.0f, 
                              0.2f, 0.6f, 0.2f, true, 0.8f, true});  // isSolid = true

      // PLATAFORMA DE SPAWN INICIAL
      platforms_3D.push_back({-5.0f, 0.0f, 5.0f,
                              12.0f, 4.0f, 12.0f, 
                              0.3f, 0.7f, 0.3f, true, 0.9f, false});  // isSolid = false


      // PLATAFORMAS SUSPENSAS (progressão ao longo do corredor Z)
      // Plataforma de spawn inicial

      // PAREDES MENORES PARA GRAPPLING (substituindo plataformas)
      // Parede 1 
      platforms_3D.push_back({-15.0f, 5.0f, 50.0f, 
                              8.0f, 8.0f, 8.0f, 
                              0.5f, 0.5f, 0.8f, true, 0.5f, false});

      // Parede 2 
      platforms_3D.push_back({20.0f, 5.0f, 150.0f, 
                              8.0f, 8.0f, 8.0f, 
                              0.5f, 0.5f, 0.8f, true, 0.5f, false});

      // Parede 3
      platforms_3D.push_back({-10.0f, 10.0f, 240.0f, 
                              8.0f, 8.0f, 8.0f, 
                              0.5f, 0.5f, 0.8f, true, 0.5f, false});

      // Parede 4
      platforms_3D.push_back({-15.0f, 8.0f, 320.0f, 
                              8.0f, 8.0f, 8.0f, 
                              0.6f, 0.6f, 0.7f, true, 0.5f, false});

      // OBSTÁCULOS
      // Spikes no chão (zona 1)
      spikeZones_3D.push_back({0.0f, 0.1f, 100.0f, 
                               10.0f, 10.0f, 10.0f});
      spikeZones_3D.push_back({10.0f, 0.1f, 100.0f, 
                               10.0f, 10.0f, 10.0f});
      spikeZones_3D.push_back({0.0f, 0.1f, 110.0f, 
                               10.0f, 10.0f, 10.0f});                               
      spikeZones_3D.push_back({10.0f, 0.1f, 110.0f, 
                               10.0f, 10.0f, 10.0f}); 

      // Spikes no chão (zona 2)
      spikeZones_3D.push_back({0.0f, 0.1f, 210.0f, 
                               10.0f, 10.0f, 10.0f});
      spikeZones_3D.push_back({10.0f, 0.1f, 210.0f, 
                               10.0f, 10.0f, 10.0f});
      spikeZones_3D.push_back({0.0f, 0.1f, 220.0f, 
                               10.0f, 10.0f, 10.0f});
      spikeZones_3D.push_back({10.0f, 0.1f, 220.0f, 
                               10.0f, 10.0f, 10.0f});

      // Parede 5
      platforms_3D.push_back({-10.0f, 20.0f, 420.0f, 
                              8.0f, 8.0f, 8.0f, 
                              0.5f, 0.5f, 0.8f, true, 0.5f, false});
      // Parede quebrável 1
      // breakableWalls_3D.push_back({-25.0f, -5.0f, 420.0f, 
      //                              40.0f, 12.0f, 3.0f,
      //                              0.8f, 0.4f, 0.1f, 300.0f, false});

      // PORTA no final do corredor
      door_3D.x = -15.0f;
      door_3D.y = -5.0f;
      door_3D.z = 500.0f;
      door_3D.w = 20.0f;
      door_3D.h = 20.0f;
      door_3D.d = 4.0f;

      // Posição inicial do jogador (no chão, início do corredor)
      player_3D.x = 0.0f;
      player_3D.y = 3.0f;  // Em cima da plataforma de spawn (h=2.0) + altura do jogador (h=2.0)
      player_3D.z = 10.0f;
      break;

    case 2:
      // ========== LEVEL 2: Corredor de Gelo - Desafiador ==========
      levelParameters_3D.playerMass = 10.0f;
      levelParameters_3D.playerWalkAccel = 55.0f;
      levelParameters_3D.maxWalkSpeed = 17.0f;
      levelParameters_3D.maxPlayerSpeed = 42.0f;
      levelParameters_3D.maxHookLength = 350.0f;
      levelParameters_3D.vectorVisualScale = 0.04f;
      levelParameters_3D.gravity = 28.0f;
      levelParameters_3D.hookSpeed = 160.0f;
      levelParameters_3D.maxShots = 18;
      levelParameters_3D.maxPullStrengthPhysics = 165.0f;
      levelParameters_3D.dampingFactor = 0.992f;
      levelParameters_3D.aimConvergenceDist = 45.0f;

      // CHÃO - Corredor comprido no eixo Z (0 a 600) - SÓLIDO
      platforms_3D.push_back({-25.0f, 0.0f, 425.0f,
                              40.0f, 10.0f, 200.0f, 
                              0.7f, 0.8f, 0.85f, true, 0.4f, true});  // isSolid = true (gelo escorregadio)

      // PLATAFORMA DE SPAWN INICIAL
      platforms_3D.push_back({-5.0f, 0.0f, 5.0f,
                              12.0f, 4.0f, 12.0f, 
                              0.75f, 0.8f, 0.85f, true, 0.9f, false});

      // PAREDES MENORES PARA GRAPPLING (progressão ao longo do corredor)
      // Parede 1
      platforms_3D.push_back({-15.0f, 10.0f, 90.0f, 
                              9.0f, 9.0f, 9.0f, 
                              0.6f, 0.65f, 0.75f, true, 0.4f, false});

      // Parede 2 
      platforms_3D.push_back({20.0f, 7.0f, 160.0f, 
                              9.0f, 9.0f, 9.0f, 
                              0.6f, 0.65f, 0.75f, true, 0.4f, false});

      // Parede 3
      platforms_3D.push_back({-15.0f, 12.0f, 250.0f, 
                              9.0f, 9.0f, 9.0f, 
                              0.6f, 0.65f, 0.75f, true, 0.4f, false});

      // Parede 4
      platforms_3D.push_back({-25.0f, 10.0f, 340.0f, 
                              9.0f, 9.0f, 9.0f, 
                              0.65f, 0.7f, 0.8f, true, 0.4f, false});

      // Parede 5
      platforms_3D.push_back({20.0f, 20.0f, 400.0f, 
                              9.0f, 9.0f, 9.0f, 
                              0.6f, 0.65f, 0.75f, true, 0.4f, false});

      // Parede 6
      platforms_3D.push_back({20.0f, -10.0f, 400.0f, 
                              25.0f, 1.0f, 25.0f, 
                              0.6f, 0.65f, 0.75f, true, 0.4f, false});

      // Parede 7
      platforms_3D.push_back({50.0f, 20.0f, 450.0f, 
                              9.0f, 9.0f, 9.0f, 
                              0.6f, 0.65f, 0.75f, true, 0.4f, false});
  
      // Parede 8
      platforms_3D.push_back({-30.0f, 20.0f, 450.0f, 
                              9.0f, 9.0f, 9.0f, 
                              0.6f, 0.65f, 0.75f, true, 0.4f, false});

      // OBSTÁCULOS
      // Spikes no chão (zona 1)
      spikeZones_3D.push_back({0.0f, -1.0f, 120.0f, 
                               15.0f, 18.0f, 15.0f});
       

      // Spikes no chão (zona 2)
      spikeZones_3D.push_back({-20.0f, -1.0f, 280.0f, 
                               15.0f, 18.0f, 15.0f});

      // Spikes no chão (zona 3 - mais perigosa)
      spikeZones_3D.push_back({-10.0f, -1.0f, 370.0f, 
                                15.0f, 18.0f, 15.0f});

      // Parede 9 (antes da porta)
      platforms_3D.push_back({0.0f, 30.0f, 580.0f, 
                              9.0f, 9.0f, 9.0f, 
                              0.6f, 0.65f, 0.75f, true, 0.4f, false});

      // Parede quebrável 1
      // Hitbox aumentado em altura para cobrir toda a área visual do modelo
      breakableWalls_3D.push_back({-25.0f, -5.0f, 550.0f, 
                                   40.0f, 20.0f, 3.0f,
                                   0.8f, 0.5f, 0.2f, 350.0f, false});

      // PORTA no final do corredor
      door_3D.x = -15.0f;
      door_3D.y = 0.0f;
      door_3D.z = 600.0f;
      door_3D.w = 25.0f;
      door_3D.h = 20.0f;
      door_3D.d = 4.0f;

      player_3D.x = 0.0f;
      player_3D.y = 3.0f;  // Em cima da plataforma de spawn (h=2.0) + altura do jogador (h=2.0)
      player_3D.z = 10.0f;
      break;

    case 3:
      // ========== LEVEL 3: Estação Espacial - Máximo Desafio ==========
      levelParameters_3D.playerMass = 10.0f;
      levelParameters_3D.playerWalkAccel = 50.0f;
      levelParameters_3D.maxWalkSpeed = 18.0f;
      levelParameters_3D.maxPlayerSpeed = 48.0f;
      levelParameters_3D.maxHookLength = 400.0f;
      levelParameters_3D.vectorVisualScale = 0.04f;
      levelParameters_3D.gravity = 22.0f;
      levelParameters_3D.hookSpeed = 170.0f;
      levelParameters_3D.maxShots = 16;
      levelParameters_3D.maxPullStrengthPhysics = 180.0f;
      levelParameters_3D.dampingFactor = 0.990f;
      levelParameters_3D.aimConvergenceDist = 50.0f;

      // CHÃO - Corredor comprido no eixo Z (0 a 800) - SÓLIDO
      platforms_3D.push_back({-25.0f, 0.0f, 525.0f,
                              40.0f, 10.0f, 300.0f, 
                              0.25f, 0.25f, 0.3f, true, 0.7f, true});  // isSolid = true

      // PLATAFORMA DE SPAWN INICIAL
      platforms_3D.push_back({-5.0f, 0.0f, 5.0f,
                              12.0f, 4.0f, 12.0f, 
                              0.35f, 0.35f, 0.4f, true, 0.9f, false});

      // PAREDES MENORES PARA GRAPPLING (circuito longo e desafiador)
      // Seção 1 - Início
      platforms_3D.push_back({-25.0f, 12.0f, 100.0f, 
                              14.0f, 10.0f, 10.0f, 
                              0.4f, 0.4f, 0.5f, true, 0.5f, false});

      platforms_3D.push_back({20.0f, 9.0f, 180.0f, 
                              14.0f, 10.0f, 10.0f, 
                              0.4f, 0.4f, 0.5f, true, 0.5f, false});

      // Seção 2 - Elevação progressiva
      platforms_3D.push_back({-18.0f, 15.0f, 270.0f, 
                              14.0f, 10.0f, 10.0f, 
                              0.4f, 0.4f, 0.5f, true, 0.5f, false});

      platforms_3D.push_back({-20.0f, 12.0f, 360.0f, 
                              14.0f, 10.0f, 10.0f, 
                              0.45f, 0.45f, 0.55f, true, 0.5f, false});

      platforms_3D.push_back({18.0f, 18.0f, 450.0f, 
                              14.0f, 10.0f, 10.0f, 
                              0.4f, 0.4f, 0.5f, true, 0.5f, false});

      // Seção 3 - Desafio máximo
      platforms_3D.push_back({10.0f, 35.0f, 530.0f, 
                              14.0f, 10.0f, 10.0f, 
                              0.4f, 0.4f, 0.5f, true, 0.5f, false});
      platforms_3D.push_back({-15.0f, 40.0f, 560.0f, 
                              14.0f, 10.0f, 10.0f, 
                              0.4f, 0.4f, 0.5f, true, 0.5f, false});

      platforms_3D.push_back({-8.0f, 40.0f, 690.0f, 
                              14.0f, 10.0f, 10.0f, 
                              0.4f, 0.4f, 0.5f, true, 0.5f, false});

      // Parede 9 (mais alta, antes da porta)
      platforms_3D.push_back({0.0f, 30.0f, 750.0f, 
                              14.0f, 10.0f, 10.0f, 
                              0.4f, 0.4f, 0.5f, true, 0.5f, false});

      // OBSTÁCULOS
      // Spikes no chão (zona 1)
      spikeZones_3D.push_back({-15.0f, -1.0f, 120.0f, 
                               12.0f, 16.0f, 12.0f});
      spikeZones_3D.push_back({10.0f, -5.0f, 150.0f, 
                               12.0f, 16.0f, 12.0f});

      // Spikes no chão (zona 2)
      // spikeZones_3D.push_back({-5.0f, -1.0f, 220.0f, 
      //                          12.0f, 16.0f, 12.0f});
      spikeZones_3D.push_back({-5.0f, -1.0f, 250.0f, 
                               12.0f, 16.0f, 12.0f});

      // Spikes no chão (zona 3)
      spikeZones_3D.push_back({-15.0f, -5.0f, 315.0f, 
                               12.0f, 16.0f, 12.0f});

      // Spikes no chão (zona 4)
      spikeZones_3D.push_back({-10.0f, -3.0f, 400.0f, 
                               12.0f, 16.0f, 12.0f});
      spikeZones_3D.push_back({0.0f, 0.0f, 420.0f, 
                               12.0f, 16.0f, 12.0f});

      // Spikes no chão (zona 5)
      spikeZones_3D.push_back({0.0f, 0.1f, 490.0f, 
                               12.0f, 16.0f, 12.0f});

      // Spikes no chão (zona 6 - área extensa)
      spikeZones_3D.push_back({-8.0f, 5.0f, 650.0f, 
                               12.0f, 16.0f, 12.0f});
      spikeZones_3D.push_back({5.0f, 10.0f, 670.0f, 
                               12.0f, 16.0f, 12.0f});

      // Paredes quebráveis (2 para tornar mais desafiador)
      breakableWalls_3D.push_back({-25.0f, -5.0f, 580.0f, 
                                   40.0f, 20.0f, 3.0f,
                                   0.7f, 0.35f, 0.15f, 400.0f, false});

      breakableWalls_3D.push_back({-25.0f, -5.0f, 720.0f, 
                                   40.0f, 20.0f, 3.0f,
                                   0.7f, 0.35f, 0.15f, 450.0f, false});

      // PORTA no final do corredor (mais alta)
      door_3D.x = -10.0f;
      door_3D.y = -5.0f;
      door_3D.z = 790.0f;
      door_3D.w = 15.0f;
      door_3D.h = 25.0f;
      door_3D.d = 4.0f;

      player_3D.x = 0.0f;
      player_3D.y = 3.0f;  // Em cima da plataforma de spawn (h=2.0) + altura do jogador (h=2.0)
      player_3D.z = 10.0f;
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

  // Reinicia flag de primeiro movimento
  mouseInitialized_3D = false;

  // Define a quantidade de tiros restantes (escolhida em cada fase) e define em
  // qual plataforma o jogador irá começar a fase
  shotsRemaining_3D = levelParameters_3D.maxShots;
  if (!platforms_3D.empty())
    collidingPlatform_3D = &platforms_3D[0];
  else
    collidingPlatform_3D = NULL;

  // --- DECORAÇÕES AMBIENTAIS (populadas após definir plataformas) ---
  hexagons_3D.clear();
  flowers_3D.clear();
  plants_3D.clear();
  rocks_3D.clear();
  pines_3D.clear();
  trees_3D.clear();
  decoration1_3D.clear();
  decoration2_3D.clear();
  decoration3_3D.clear();

  if (level == 1) {
    loadLevel1Decorations(hexagons_3D, flowers_3D, plants_3D, rocks_3D, pines_3D, trees_3D, decoration1_3D, decoration2_3D, decoration3_3D);
  } else if (level == 2) {
    loadLevel2Decorations(hexagons_3D, flowers_3D, plants_3D, rocks_3D, pines_3D, trees_3D, decoration1_3D, decoration2_3D, decoration3_3D);
  } else if (level == 3) {
    loadLevel3Decorations(hexagons_3D, flowers_3D, plants_3D, rocks_3D, pines_3D, trees_3D, decoration1_3D, decoration2_3D, decoration3_3D);
  }
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

    // Se já estava grudado, apenas solta e dispara novo gancho
    if (isHooked_3D) {
      isHooked_3D = false;
      isGrounded_3D = false;
      printf("UPDATE: Soltou do gancho anterior.\n");
    }
    
    // Dispara novo gancho
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

      // Direção da mira
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

      // Ajusta o ponto de origem do gancho para a altura do centro do personagem
      float spawnDist = 1.5f;
      hookProjectileX_3D = pCx + (dirViewX * spawnDist);
      hookProjectileY_3D = pCy + (dirViewY * spawnDist);
      hookProjectileZ_3D = pCz + (dirViewZ * spawnDist);

      // Ajuste de altura do gancho por nível
      float heightOffsetMultiplier = 1.15f;
      if (activeLevel_3D == 2) heightOffsetMultiplier = 1.55f;
      else if (activeLevel_3D == 3) heightOffsetMultiplier = 1.8f;
      float heightOffset = player_3D.h * heightOffsetMultiplier;
      float vecX = targetX - hookProjectileX_3D;
      float vecY = (targetY + heightOffset) - hookProjectileY_3D;
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
    float distOrigin = sqrt(distOriginSq);

    if (distOriginSq >
        levelParameters_3D.maxHookLength * levelParameters_3D.maxHookLength) {
      isHookFiring_3D = false;
      printf("FIM DE CURSO: Gancho nao alcancou nada. Dist: %.1f, Max: %.1f\n", 
             distOrigin, levelParameters_3D.maxHookLength);
    } else {
      for (auto& p : platforms_3D) {
        /**
         * Um problema persistente foi o de atirar para cima e o gancho se
         * prender no chão. Para contornar isso, ignoramos APENAS a plataforma
         * onde o jogador está pisando (collidingPlatform_3D).
         * Removida a verificação de AABB expandida que impedia grudar em
         * plataformas próximas verticalmente.
         */
        if (collidingPlatform_3D != NULL && &p == collidingPlatform_3D) {
          continue;
        }

        if (p.isHookable) {
          // Level 3: ajusta hitbox do gancho pra não pegar na transparência
          Platform_3D hookPlatform = p;
          if (activeLevel_3D == 3) {
            float hookWidthMultiplier = 0.65f;
            float widthReduction = p.w * (1.0f - hookWidthMultiplier) / 2.0f;
            float depthReduction = p.d * (1.0f - hookWidthMultiplier) / 2.0f;
            hookPlatform.x += widthReduction;
            hookPlatform.w *= hookWidthMultiplier;
            hookPlatform.z += depthReduction;
            hookPlatform.d *= hookWidthMultiplier;
          }
          float hX, hY, hZ;
          if (lineBoxIntersection(prevHX, prevHY, prevHZ, hookProjectileX_3D,
                                  hookProjectileY_3D, hookProjectileZ_3D, hookPlatform, hX,
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

    // AUTO-GRUDAMENTO: Se chegou muito perto do ponto de hook, gruda e zera velocidade
    const float SNAP_DISTANCE = 3.0f;  // Distância para "grudar" automaticamente
    if (currDist < SNAP_DISTANCE) {
      // Zera todas as velocidades para parar de balançar
      player_3D.velocityX = 0.0f;
      player_3D.velocityY = 0.0f;
      player_3D.velocityZ = 0.0f;
      
      // Opcional: posiciona exatamente no ponto de hook (comentado para permitir pequeno movimento)
      player_3D.x = hookPointX_3D - player_3D.w / 2.0f;
      player_3D.y = hookPointY_3D - player_3D.h;
      player_3D.z = hookPointZ_3D - player_3D.d / 2.0f;
    } else {
      // Resistência do ar (só aplica se ainda longe)
      float airDamping = 0.95f;  // Aumentado de 0.99 para 0.95 (mais forte)
      player_3D.velocityX *= airDamping;
      player_3D.velocityY *= airDamping;
      player_3D.velocityZ *= airDamping;
    }
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
    animTimer = 0.0f;  // Reseta timer (pose de gancho é estática)
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

  // Pequeno offset anti z-fighting
  float epsilon = 0.01f;

  // --------------------------------------------------------------------------------

  // Verificações em X

  player_3D.x += player_3D.velocityX * deltaTime;

  // Verifica paredes quebráveis
  for (auto& wall : breakableWalls_3D) {
    if (wall.isBroken) continue;
    // Aumenta hitbox em altura nos Levels 2 e 3
    float wallHitboxH = (activeLevel_3D == 2 || activeLevel_3D == 3) ? wall.h * 1.5f : wall.h;
    float wallHitboxY = (activeLevel_3D == 2 || activeLevel_3D == 3) ? wall.y - (wallHitboxH - wall.h) / 2.0f : wall.y;
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                           player_3D.h, player_3D.d, wall.x, wallHitboxY, wall.z,
                           wall.w, wallHitboxH, wall.d)) {
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
    // Level 3: hitbox mais largo
    float hitboxW = (activeLevel_3D == 3) ? p.w * 1.1f : p.w;
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                           player_3D.h, player_3D.d, p.x, p.y, p.z, hitboxW, p.h,
                           p.d)) {
      if (player_3D.velocityX > 0)
        player_3D.x = p.x - player_3D.w - epsilon;
      else if (player_3D.velocityX < 0)
        player_3D.x = p.x + hitboxW + epsilon;
      player_3D.velocityX = 0;
    }
  }

  // --------------------------------------------------------------------------------

  // Verificações em Z

  player_3D.z += player_3D.velocityZ * deltaTime;

  // Verifica paredes quebráveis
  for (auto& wall : breakableWalls_3D) {
    if (wall.isBroken) continue;
    // Aumenta hitbox em altura nos Levels 2 e 3
    float wallHitboxH = (activeLevel_3D == 2 || activeLevel_3D == 3) ? wall.h * 1.5f : wall.h;
    float wallHitboxY = (activeLevel_3D == 2 || activeLevel_3D == 3) ? wall.y - (wallHitboxH - wall.h) / 2.0f : wall.y;
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                           player_3D.h, player_3D.d, wall.x, wallHitboxY, wall.z,
                           wall.w, wallHitboxH, wall.d)) {
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
    // Level 3: hitbox ligeiramente mais largo para compensar escala visual da plataforma
    float hitboxW = (activeLevel_3D == 3) ? p.w * 1.1f : p.w;
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                           player_3D.h, player_3D.d, p.x, p.y, p.z, hitboxW, p.h,
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
    // Level 3: hitbox mais largo
    float hitboxW = (activeLevel_3D == 3) ? p.w * 1.1f : p.w;
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                           player_3D.h, player_3D.d, p.x, p.y, p.z, hitboxW, p.h,
                           p.d)) {
      if (p.isSolid) {
        // Plataforma sólida: bloqueia de cima E de baixo (como parede)
        if (player_3D.velocityY <= 0) {
          player_3D.y = p.y + p.h;
          player_3D.velocityY = 0;
          isGrounded_3D = true;
          collidingPlatform_3D = &p;
        } else {
          player_3D.y = p.y - player_3D.h - epsilon;
          player_3D.velocityY = 0;
        }
      } else {
        // Plataforma normal: só bloqueia queda de cima
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
  }

  for (auto& wall : breakableWalls_3D) {
    if (wall.isBroken) continue;
    // Aumenta hitbox em altura nos Levels 2 e 3
    float wallHitboxH = (activeLevel_3D == 2 || activeLevel_3D == 3) ? wall.h * 1.5f : wall.h;
    float wallHitboxY = (activeLevel_3D == 2 || activeLevel_3D == 3) ? wall.y - (wallHitboxH - wall.h) / 2.0f : wall.y;
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                           player_3D.h, player_3D.d, wall.x, wallHitboxY, wall.z,
                           wall.w, wallHitboxH, wall.d)) {
      if (player_3D.velocityY <= 0) {
        player_3D.y = wallHitboxY + wallHitboxH;
        player_3D.velocityY = 0;
        isGrounded_3D = true;
      } else {
        player_3D.y = wallHitboxY - player_3D.h - epsilon;
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

  // Verificação com espinhos e porta (espinhos só no chão)

  for (const auto& spike : spikeZones_3D) {
    // Hitbox menor que o visual pra não matar injustamente nas bordas
    float hitboxMultiplier = 0.7f;
    if (activeLevel_3D == 1) {
      hitboxMultiplier = 0.65f;
    } else if (activeLevel_3D == 2) {
      hitboxMultiplier = 0.6f;
    } else if (activeLevel_3D == 3) {
      hitboxMultiplier = 0.5f;
    }
    
    // Calcula hitbox reduzido centralizado
    float hitboxW = spike.w * hitboxMultiplier;
    float hitboxH = spike.h * hitboxMultiplier;
    float hitboxD = spike.d * hitboxMultiplier;
    float offsetX = (spike.w - hitboxW) / 2.0f;
    float offsetY = (spike.h - hitboxH) / 2.0f;
    float offsetZ = (spike.d - hitboxD) / 2.0f;
    
    if (checkAABBCollision(player_3D.x, player_3D.y, player_3D.z, player_3D.w,
                           player_3D.h, player_3D.d, 
                           spike.x + offsetX, spike.y + offsetY, spike.z + offsetZ,
                           hitboxW, hitboxH, hitboxD)) {
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

  // Helper para selecionar textura correta por tipo de objeto e nível
  static bool textureDebugPrinted = false;
  auto getTextureForObject = [&](const std::string& objectType) -> GLuint {
    GLuint selectedTexture = 0;
    
    // Level 1:
    // - spawn: colormap
    // - platform: colormap
    // - block-large: colormap
    // - spike: colormap
    // - decoration-tall2: colormap
    // - decoration-tall1/3, decoration1/2/3: colormap3
    if (activeLevel_3D == 1) {
      if (objectType == "spawn") selectedTexture = decorationTextureID;
      else if (objectType == "platform") selectedTexture = decorationTextureID;
      else if (objectType == "block-large") selectedTexture = decorationTextureID;
      else if (objectType == "spike") selectedTexture = decorationTextureID;
      else if (objectType == "decoration-tall2") selectedTexture = decorationTextureID;
      else if (objectType == "decoration-tall" || objectType == "decoration") selectedTexture = colormap3TextureID;
      else selectedTexture = decorationTextureID; // padrão
      return selectedTexture;
    }
    // Level 2:
    // - spawn: colormap2
    // - platform: colormap (decorationTextureID) - MESMO PACOTE DO LEVEL1
    // - block-large: colormap (decorationTextureID)
    // - decoration-tall1/2/3, rocks, decoration1/2/3, spike: colormap3
    else if (activeLevel_3D == 2) {
      if (objectType == "spawn") {
        if (!textureDebugPrinted) {
          printf("[getTextureForObject] Level2 'spawn' -> colormap2TextureID=%u\n", colormap2TextureID);
        }
        return colormap2TextureID;
      }
      if (objectType == "platform") {
        if (!textureDebugPrinted) {
          printf("[getTextureForObject] Level2 'platform' -> decorationTextureID=%u (colormap.png)\n", decorationTextureID);
        }
        return decorationTextureID;  // platform usa colormap.png (mesmo do level1)
      }
      if (objectType == "block-large") return decorationTextureID;
      if (objectType == "decoration-tall" || objectType == "decoration-tall2" || objectType == "decoration" || 
          objectType == "rocks" || objectType == "spike") return colormap3TextureID;
      return decorationTextureID; // padrão
    }
    // Level 3:
    // - spawn, rocks, block-large: colormap2
    // - spike, door, decoration-tall1/2/3, decoration1/2/3: colormap3
    else if (activeLevel_3D == 3) {
      if (objectType == "spawn" || objectType == "rocks" || objectType == "block-large") return colormap2TextureID;
      if (objectType == "spike" || objectType == "door" || 
          objectType == "decoration-tall" || objectType == "decoration-tall2" || objectType == "decoration") return colormap3TextureID;
      return decorationTextureID; // padrão
    }
    return decorationTextureID; // fallback
  };

  // Desenha um VBO único (com textura opcional)
  static bool debugPrinted = false;
  auto drawSingleVBO = [&](const VBO_Info& info, GLuint textureID) {
    if (info.vboID == 0 || info.triCount == 0) return;
    if (textureID != 0) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, textureID);
      glColor3f(1.0f, 1.0f, 1.0f);
      if (!debugPrinted) {
        printf("[drawSingleVBO] Aplicando textura ID: %u\n", textureID);
      }
    } else if (!debugPrinted) {
      printf("[drawSingleVBO] Sem textura (textureID=0)\n");
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
  // Posiciona o chão na altura definida em FLOOR_Y (altura do game over)
  float floorDrawY = FLOOR_Y;
  glTranslatef(0.0f, floorDrawY, 0.0f);
  // Escala para cobrir a área do mundo (modelo assumido ~1 unidade)
  float floorScaleX = WORLD_WIDTH_3D;
  float floorScaleZ = WORLD_DEPTH_3D;
  glScalef(floorScaleX, 1.0f, floorScaleZ);
  // Aplica cor de material (GL_COLOR_MATERIAL deve usar essa cor).
  // O chão sempre usa cor sólida definida em floorColor (sem textura)
  GLfloat matDiff[4] = {floorColor[0], floorColor[1], floorColor[2], 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matDiff);
  if (floorVBO.vboID != 0 && floorVBO.triCount > 0) {
    // Sempre força cor sólida no chão (sem textura)
    glColor3f(floorColor[0], floorColor[1], floorColor[2]);
    drawSingleVBO(floorVBO, 0);  // Passa 0 para não usar textura
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

  // Desenho das plataformas: usa modelos OBJ específicos baseados no tipo
  // Ignora os VBOs gerados por createBoxTris (que não têm UVs corretos)
  // e usa os modelos OBJ carregados (spawn.obj, platform.obj, block-large.obj)
  if (!debugPrinted) {
    printf("[PLATFORM DEBUG] Desenhando %zu plataformas usando modelos OBJ\n", platforms_3D.size());
  }
  
  for (size_t i = 0; i < platforms_3D.size(); ++i) {
    const auto& p = platforms_3D[i];
    glPushMatrix();
    
    // Seleciona modelo e textura apropriados baseados no índice:
    // - Índice 0 (corredor sólido) = block-large (se disponível)
    // - Índice 1 (plataforma de spawn) = spawn
    // - Índices 2+ (paredes de grappling) = platform (se disponível)
    
    bool rendered = false;
    
    if (i == 0 && blockLargeVBO.vboID != 0 && blockLargeVBO.triCount > 0) {
      // Corredor sólido usa block-large.obj com textura
      glTranslatef(p.x + p.w / 2.0f, p.y + p.h / 2.0f, p.z + p.d / 2.0f);
      // Level 3: usa modelo de spawn (escala direta)
      // Levels 1 e 2: usa modelo block-large original (escala dividida)
      if (activeLevel_3D == 3) {
        glScalef(p.w, p.h, p.d);
      } else {
        glScalef(p.w / 2.082f, p.h / 2.0f, p.d / 2.082f);
      }
      drawSingleVBO(blockLargeVBO, getTextureForObject("block-large"));
      rendered = true;
    } else if (i == 1 && spawnVBO.vboID != 0 && spawnVBO.triCount > 0) {
      // Plataforma de spawn usa spawn.obj com textura específica
      glTranslatef(p.x + p.w / 2.0f, p.y + p.h / 2.0f, p.z + p.d / 2.0f);
      glScalef(p.w, p.h, p.d);
      drawSingleVBO(spawnVBO, getTextureForObject("spawn"));
      rendered = true;
    } else if (i > 1 && platformVBO.vboID != 0 && platformVBO.triCount > 0) {
      // Paredes de grappling usam platform.obj com textura
      glTranslatef(p.x + p.w / 2.0f, p.y, p.z + p.d / 2.0f);
      // Level 3: ajusta escala para diminuir altura e aumentar largura
      // Levels 1 e 2: usa escala padrão
      if (activeLevel_3D == 3) {
        glScalef(p.w / 1.8f, p.h / 1.35f, p.d / 1.8f);
      } else {
        glScalef(p.w / 2.082f, p.h, p.d / 2.082f);
      }
      drawSingleVBO(platformVBO, getTextureForObject("platform"));
      rendered = true;
    }
    
    // Fallback: se o modelo específico não existe, desenha cubo simples
    if (!rendered) {
      glTranslatef(p.x + p.w / 2.0f, p.y + p.h / 2.0f, p.z + p.d / 2.0f);
      glScalef(p.w, p.h, p.d);
      glColor3f(p.r, p.g, p.b);
      glutSolidCube(1.0);
    }
    
    glPopMatrix();
  }

  // 3) Walls: se houver modelo único carregado em envWallVBO reaplicamos por
  // parede
  if (envWallVBO.vboID != 0 && envWallVBO.triCount > 0) {
    for (const auto& w : breakableWalls_3D) {
      if (w.isBroken) continue;
      glPushMatrix();
      // Centraliza o modelo na parede
      glTranslatef(w.x + w.w / 2.0f, w.y + w.h / 2.0f, w.z + w.d / 2.0f);
      // Level 2: escala visual mantém altura 25 enquanto hitbox é 30
      if (activeLevel_3D == 2) {
        glScalef(w.w, 25.0f, w.d);
      } else {
        glScalef(w.w, w.h, w.d);
      }
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
        // Level 2 e 3: rotação 180° (spikes estavam virados para trás)
        if (activeLevel_3D == 2 || activeLevel_3D == 3) {
          glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        }
        glScalef(s.w, s.h, s.d);
        drawSingleVBO(envSpikeVBO, getTextureForObject("spike"));
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
  // Level 3: escala menor e rotação 180° (porta estava virada para trás)
  if (activeLevel_3D == 3) {
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    glScalef(door_3D.w * 0.7f, door_3D.h * 0.7f, door_3D.d * 0.7f);
  } else {
    glScalef(door_3D.w, door_3D.h, door_3D.d);
  }

  if (doorVBO.vboID != 0 && doorVBO.triCount > 0) {
    // desenha o VBO da porta (assume que os UVs estão corretos)
    drawSingleVBO(doorVBO, getTextureForObject("door"));
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

  // --- DECORAÇÕES AMBIENTAIS ---
  // Client states já estão habilitados desde o início de gameDisplay_3D

  // Helper lambda para desenhar múltiplas instâncias de um modelo
  auto drawDecorations = [&](const std::vector<DecoObject>& decos, const VBO_Info& vbo, const std::string& objectType) {
    if (vbo.vboID == 0 || vbo.triCount == 0) return;
    GLuint texture = getTextureForObject(objectType);
    for (const auto& deco : decos) {
      glPushMatrix();
      glTranslatef(deco.x, deco.y, deco.z);
      glRotatef(deco.rotationY, 0.0f, 1.0f, 0.0f);
      glScalef(deco.scale, deco.scale, deco.scale);
      drawSingleVBO(vbo, texture);
      glPopMatrix();
    }
  };

  // Desenha cada tipo de decoração
  drawDecorations(flowers_3D, flowersVBO, "decoration");
  drawDecorations(plants_3D, plantVBO, "decoration");
  drawDecorations(rocks_3D, rocksVBO, "rocks");
  drawDecorations(pines_3D, decorationTall2VBO, "decoration-tall2");
  drawDecorations(trees_3D, decorationTall1VBO, "decoration-tall");
  drawDecorations(decoration1_3D, decoration1VBO, "decoration");
  drawDecorations(decoration2_3D, decoration2VBO, "decoration");
  drawDecorations(decoration3_3D, decoration3VBO, "decoration");

  // Marcar que os debug prints já foram exibidos
  debugPrinted = true;
  textureDebugPrinted = true;

  // Desliga client states após decorações
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
    // Centro da plataforma (topo)
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

    // Centro da parede (topo)
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
    // Mouse pro centro da tela
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
  // para cima. Near plane acima do chão
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
      // Permite disparar mesmo quando grudado (solta automaticamente)
      if (!isHookFiring_3D && shotsRemaining_3D > 0) {
        isChargingHook_3D = true;
        chargeStartTime_3D = glutGet(GLUT_ELAPSED_TIME);
        currentChargeForce_3D = 0.0f;
        chargePercentage_3D = 0.0f;
        if (isHooked_3D) {
          printf("MOUSE DOWN: Carregando (vai soltar do gancho atual)...\n");
        } else {
          printf("MOUSE DOWN: Carregando...\n");
        }
      } else {
        printf("MOUSE DOWN: Bloqueado (Voando=%d, Tiros=%d)\n",
               isHookFiring_3D, shotsRemaining_3D);
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

// Funções de Teclas Especiais (não usado)
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

  // 1. Carregar a TEXTURA do PLAYER (textura específica por nível)
  std::string playerTexturePath = "assets/images/player/player_texture" + std::to_string(activeLevel_3D) + ".png";
  playerTexture = loadTexture_3D(playerTexturePath.c_str());

  // 2. Carregar as TEXTURAS DE AMBIENTE (colormap.png, colormap2.png, colormap3.png)
  // Cada nível tem suas próprias texturas colormap, colormap2 e colormap3
  std::string levelPath = "assets/images/level" + std::to_string(activeLevel_3D) + "/";
  
  // Limpar texturas antigas antes de carregar novas
  if (decorationTextureID != 0) glDeleteTextures(1, &decorationTextureID);
  if (colormap2TextureID != 0) glDeleteTextures(1, &colormap2TextureID);
  if (colormap3TextureID != 0) glDeleteTextures(1, &colormap3TextureID);
  
  decorationTextureID = loadTexture_3D((levelPath + "colormap.png").c_str());   // colormap padrão
  colormap2TextureID = loadTexture_3D((levelPath + "colormap2.png").c_str());   // colormap2
  colormap3TextureID = loadTexture_3D((levelPath + "colormap3.png").c_str());   // colormap3
  
  printf("Texturas carregadas para level %d:\n", activeLevel_3D);
  printf("  decorationTextureID (colormap.png): %u\n", decorationTextureID);
  printf("  colormap2TextureID (colormap2.png): %u\n", colormap2TextureID);
  printf("  colormap3TextureID (colormap3.png): %u\n", colormap3TextureID);
  
  // Texturas específicas por objeto (serão configuradas conforme regras por nível)
  // platformTextureID, wallTextureID, doorTextureID e spikeTextureID serão definidos
  // no momento do desenho baseado no nível
  platformTextureID = decorationTextureID;  // Padrão
  wallTextureID = decorationTextureID;      // Padrão
  doorTextureID = decorationTextureID;      // Padrão
  spikeTextureID = decorationTextureID;     // Padrão

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
      // Em caso de erro, não tenta renderizar
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
