// Define a implementação do STB apenas aqui
#define STB_IMAGE_IMPLEMENTATION

#include "utils.h"  // Traz glew.h e freeglut.h na ordem certa

#include <cstdio>
#include <cmath>

// Se precisar de definições de game ou game_3d
#include "game.h"
#include "game_3D.h"

// ----------------------------------------------------------------------------------------------------------------

/**
 * Função de teste para carregar uma textura usando a biblioteca STB Image.
 * Essa função carrega uma imagem PNG e cria uma textura OpenGL a partir dela.
 * Ela imprime mensagens no console para indicar sucesso ou falha no
 * carregamento.
 */

// Função para carregar textura usando STB
GLuint loadTexture(const char* filepath) {
  int width, height, channels;

  // Carrega a imagem (forçando 4 canais = RGBA)
  unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);

  if (!data) {
    printf("stb_image: erro ao carregar imagem %s (%s)\n", filepath,
           stbi_failure_reason());
    return 0;  // Retorna 0 em caso de falha
  }

  GLuint textureID = 0;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  // Envia os dados da imagem para o OpenGL
  // Como forçamos 4 canais no stbi_load, usamos GL_RGBA aqui com segurança
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);

  // --- MODIFICAÇÃO 2: Gerar Mipmaps ---
  // Cria versões menores da textura para quando o objeto estiver longe
  glGenerateMipmap(GL_TEXTURE_2D);

  // Define os filtros da textura
  // MIN_FILTER alterado para usar Mipmap (melhor qualidade à distância)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // --- MODIFICAÇÃO 3: Wrapping ---
  // GL_REPEAT é geralmente melhor para modelos 3D (obj) do que CLAMP
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  stbi_image_free(data);  // Libera a memória da imagem

  printf("Textura %s carregada com sucesso! id=%u (%dx%d)\n", filepath,
         textureID, width, height);

  return textureID;  // Retorna o ID da textura carregada
}

GLuint loadTexture_3D(const char* filename) {
  // 1. Configura para INVERTER (necessário para 3D: OpenGL Bottom-Up)
  stbi_set_flip_vertically_on_load(true);

  // 2. Chama a função principal
  GLuint textureID = loadTexture(filename);

  // 3. (Opcional, mas recomendado para evitar problemas futuros com texturas
  // 2D):
  //    Volta o flip para o estado padrão (Top-Down)
  stbi_set_flip_vertically_on_load(false);

  return textureID;
}

// Crie esta função se ainda não o fez, para carregar todas as texturas 2D
GLuint loadTexture_2D(const char* filename) {
  // Garante que o flip está DESATIVADO (padrão para 2D: Imagem Top-Down)
  stbi_set_flip_vertically_on_load(false);

  return loadTexture(filename);
}

/**
 * Essa função desenha um retângulo, mas é possível escolher a transparência da
 * sua cor. Os parâmetros passados são os mesmos para uma função comum de
 * desenho, como as variáveis onde o desenho será adicionado, a altura e a
 * largura, as cores (em RGB) e o coeficiente de transparência
 */
void drawRect(float initX, float initY, float width, float height, float r,
              float g, float b, float transparency) {
  if (transparency < 1.0f) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  glColor4f(r, g, b, transparency);
  glBegin(GL_QUADS);
  glVertex2f(initX, initY);
  glVertex2f(initX + width, initY);
  glVertex2f(initX + width, initY + height);
  glVertex2f(initX, initY + height);
  glEnd();
  if (transparency < 1.0f) {
    glDisable(GL_BLEND);
  }
}

void drawParallaxLayer(GLuint textureID, float cameraLeft, float cameraBottom,
                       float viewW, float viewH, float parallaxX,
                       float parallaxY, float tileWorldW, float tileWorldH) {
  if (textureID == 0) return;

  // Quantos tiles cabem na tela (em coordenadas UV)
  float uSpan = viewW / tileWorldW;
  float vSpan = viewH / tileWorldH;

  // Deslocamento UV conforme movimento da câmera
  float uOffset = fmodf((cameraLeft * parallaxX) / tileWorldW, 1.0f);
  float vOffset = fmodf((cameraBottom * parallaxY) / tileWorldH, 1.0f);
  if (uOffset < 0.0f) uOffset += 1.0f;
  if (vOffset < 0.0f) vOffset += 1.0f;

  float u0 = uOffset;
  float v0 = vOffset;
  float u1 = uOffset + uSpan;
  float v1 = vOffset + vSpan;

  // Configuração de textura
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1, 1, 1, 1);

  // --- FLIP USANDO MATRIZ ---
  glPushMatrix();

  // Move para o início da câmera
  glTranslatef(cameraLeft, cameraBottom + viewH, 0.0f);

  // Escala Y negativa: espelha verticalmente
  glScalef(1.0f, -1.0f, 1.0f);

  // Agora o quad é desenhado "virado" para o lado certo
  glBegin(GL_QUADS);
  glTexCoord2f(u0, v0);
  glVertex2f(0, 0);
  glTexCoord2f(u1, v0);
  glVertex2f(viewW, 0);
  glTexCoord2f(u1, v1);
  glVertex2f(viewW, viewH);
  glTexCoord2f(u0, v1);
  glVertex2f(0, viewH);
  glEnd();

  glPopMatrix();

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}

/**
 * Desenha um retângulo usando uma textura, com repetição (tiling) de UVs.
 * Isso é ideal para chão e plataformas.
 */
void drawRepeatingTexturedRect(float x, float y, float w, float h,
                               GLuint textureID, float textureWidth,
                               float textureHeight) {
  if (textureID == 0) {
    // Fallback: se a textura falhou, desenha um retângulo cinza
    glColor3f(0.5f, 0.5f, 0.5f);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
    return;
  }

  // Calcula quantas vezes a textura deve se repetir
  // textureWidth e textureHeight são as dimensões MUNDIAIS da textura base.
  float uTiles = w / textureWidth;
  float vTiles = h / textureHeight;

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, textureID);

  // Repetição (Tile)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glColor3f(1.0f, 1.0f, 1.0f);  // Garante que a textura não seja colorizada

  glBegin(GL_QUADS);
  // Canto Inferior Esquerdo (UV: 0, 0)
  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(x, y);

  // Canto Inferior Direito (UV: uTiles, 0)
  glTexCoord2f(uTiles, 0.0f);
  glVertex2f(x + w, y);

  // Canto Superior Direito (UV: uTiles, vTiles)
  glTexCoord2f(uTiles, vTiles);
  glVertex2f(x + w, y + h);

  // Canto Superior Esquerdo (UV: 0, vTiles)
  glTexCoord2f(0.0f, vTiles);
  glVertex2f(x, y + h);
  glEnd();

  glDisable(GL_TEXTURE_2D);
}

/**
 * Desenha um retângulo com uma textura aplicada, com opção de inverter
 * horizontalmente (flipH) e verticalmente (flipV).
 *
 * flipV = true (padrão): Corrige para o sistema de coordenadas Y-Up (Mundo do
 * Jogo). flipV = false: Mantém a orientação original da textura (Cordenadas de
 * Tela/Menu Y-Down).
 */
void drawTexturedRect(float x, float y, float w, float h, GLuint textureID,
                      bool flipH, bool flipV) {
  if (textureID == 0) return;

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glBindTexture(GL_TEXTURE_2D, textureID);

  // Coordenadas U (Horizontal)
  float u0 = flipH ? 1.0f : 0.0f;
  float u1 = flipH ? 0.0f : 1.0f;

  // Coordenadas V (Vertical)
  // Se flipV for true (Jogo Y-Up), V=1.0 vai para o topo (y) e V=0.0 vai para a
  // base (y+h). Se flipV for false (Menu Y-Down), V=0.0 vai para o topo (y) e
  // V=1.0 vai para a base (y+h).
  float v_TOP_EDGE_MAPPED = flipV ? 1.0f : 0.0f;
  float v_BOTTOM_EDGE_MAPPED = flipV ? 0.0f : 1.0f;

  glBegin(GL_QUADS);

  // Canto Superior Esquerdo (x, y)
  glTexCoord2f(u0, v_TOP_EDGE_MAPPED);
  glVertex2f(x, y);

  // Canto Superior Direito (x + w, y)
  glTexCoord2f(u1, v_TOP_EDGE_MAPPED);
  glVertex2f(x + w, y);

  // Canto Inferior Direito (x + w, y + h)
  glTexCoord2f(u1, v_BOTTOM_EDGE_MAPPED);
  glVertex2f(x + w, y + h);

  // Canto Inferior Esquerdo (x, y + h)
  glTexCoord2f(u0, v_BOTTOM_EDGE_MAPPED);
  glVertex2f(x, y + h);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
}

/**
 * Os "espinhos" são elementos do jogo responsáveis por eliminar o personagem. O
 * seu desenho se baseia em uma sequência de triângulos simples para simular o
 * formato real de espinhos
 */
void drawSpikes(float initX, float initY, float width, float height, float r,
                float g, float b) {
  glPushMatrix();  // Como parte dos requisitos, os desenhos são feitos por meio
                   // de pilhas de matriz
  glTranslatef(initX, initY, 0.0f);

  glColor3f(r, g, b);
  int numSpikes = (int)(width / height);
  if (numSpikes < 1) numSpikes = 1;
  float baseWidthOfSpike = width / numSpikes;

  for (int i = 0; i < numSpikes; i++) {
    float baseX = i * baseWidthOfSpike;
    glBegin(GL_TRIANGLES);
    glVertex2f(baseX, 0);
    glVertex2f(baseX + baseWidthOfSpike, 0);
    glVertex2f(baseX + baseWidthOfSpike / 2, height);
    glEnd();
  }
  glPopMatrix();
}

/**
 * Para o jogo, é possível escolher a fonte dos textos que serão escritos. Por
 * padrão, na própria definição da função, quando uma fonte não é escolhida, a
 * fonte usada será a mesma usada no menu.
 */
void drawText(float x, float y, const char* text, void* font) {
  glRasterPos2f(x, y);
  for (const char* c = text; *c; ++c) {
    glutBitmapCharacter(font, *c);
  }
}

/**
 * Como o tratamento das colisões do game é feito por meio de retângulos, essa
 * função verifica se um ponto está contido dentro de um retângulo e retorna um
 * booleano. Essa é uma função amplamente usada quando se trabalha com OpenGL.
 */
bool isPointInside(float pointX, float pointY, float rectX, float rectY,
                   float rectWidth, float rectHeight) {
  return (pointX >= rectX && pointX <= rectX + rectWidth && pointY >= rectY &&
          pointY <= rectY + rectHeight);
}

/**
 * Para trabalhar com questões de centralização e evitar que textos sejam
 * escritos em locais que irão dificultar a visualização do jogador, foi feita
 * uma função para verificar a largura do texto. Buscamos essa solução para
 * ajudar na visualização dos elementos.
 */
int getTextWidth(const char* text, void* font) {
  int width = 0;
  for (const char* c = text; *c; ++c) {
    width += glutBitmapWidth(font, *c);
  }
  return width;
}

/**
 * Como a dinâmica de movimentação do jogo é por um gancho (uma linha) que se
 * prende em um retângulo, essa função verifica se o segmento de reta formato
 * por dois pontos (x1, y1) e (x2 e y2) está contido dentro de um retângulo.
 * Nesse caso, não é verificado um retângulo comum, mas sim uma plataforma
 * (único objeto que o jogador pode se prender). Caso seja encontrada uma
 * interseção, retorna o exato ponto em que houve uma colisão.
 */
bool lineRectIntersection(float x1, float y1, float x2, float y2,
                          const Platform& plataform, float& hitX, float& hitY) {
  float t0 = 0.0f, t1 = 1.0f;
  float dx = x2 - x1, dy = y2 - y1;

  /**
   * Devido à dificuldade de encontrar uma solução eficiente, dado a
   * dificuladade da operação e a quantidade de vezes que é executada a cada
   * frame, para garantir a fluidez do jogo, buscamos uma função lambda
   * (algorítmo de Liang-Barsky) que consegue fazer a verificação de forma
   * performática
   */
  auto clipTest = [&](float p_val, float q_val, float& t_0, float& t_1) {
    if (fabs(p_val) < 1e-6) {
      return q_val >= 0;
    }
    float r = q_val / p_val;
    if (p_val < 0) {
      if (r > t_1) return false;
      if (r > t_0) t_0 = r;
    } else {
      if (r < t_0) return false;
      if (r < t_1) t_1 = r;
    }
    return true;
  };
  if (!clipTest(-dx, x1 - plataform.x, t0, t1)) return false;
  if (!clipTest(dx, plataform.x + plataform.w - x1, t0, t1)) return false;
  if (!clipTest(-dy, y1 - plataform.y, t0, t1)) return false;
  if (!clipTest(dy, plataform.y + plataform.h - y1, t0, t1)) return false;
  hitX = x1 + t0 * dx;
  hitY = y1 + t0 * dy;
  return true;
}

/**
 * Para garantir uma melhor experiência de usuário, a linha do grappling hook
 * não possui limitações de distância de acordo com o personagem. A única
 * limitação é o espaço visível da tela do jogador. Essa função verifica se a
 * linha (definida por suas tuplas de pontos iniciais e finais) ultrapassou os
 * limites da tela de visualização. Para isso, ela usa as variáveis globais
 * deinidas para visualização.
 */
void clipLineToView(float startX, float startY, float& endX, float& endY,
                    float VIEW_WIDTH, float VIEW_HEIGHT, float cameraLeft,
                    float cameraBottom) {
  float viewRight = cameraLeft + VIEW_WIDTH;
  float viewTop = cameraBottom + VIEW_HEIGHT;
  float dx = endX - startX;
  float dy = endY - startY;
  if (dx == 0 && dy == 0) return;
  float t = 1.0;
  if (endX < cameraLeft) t = std::min(t, (cameraLeft - startX) / dx);
  if (endX > viewRight) t = std::min(t, (viewRight - startX) / dx);
  if (endY < cameraBottom) t = std::min(t, (cameraBottom - startY) / dy);
  if (endY > viewTop) t = std::min(t, (viewTop - startY) / dy);
  endX = startX + t * dx;
  endY = startY + t * dy;
}

/**
 * Essa função é responsável por desenhar os vetores que serão usados no game.
 * Todos os vetores seguem o mesmo padrão e devem seguir, também, uma escala de
 * tamanho (definida nas constantes desse programa).
 */
void drawVector(float initX, float initY, float vX_physics, float vY_physics,
                float scale, float r, float g, float b,
                const char* vectorName) {
  /**
   * Para traduzir os valores de mundo dos vetores em escalas visíveis para o
   * usuário, as características físicas do vetor são multiplicadas
   * multiplicadas pela escala para gerar uma visualização correta e
   * padronizada.
   */
  float vX_visual = vX_physics * scale;
  float vY_visual = vY_physics * scale;
  float endX = initX + vX_visual;
  float endY = initY + vY_visual;

  glColor3f(r, g, b);
  glLineWidth(2.0f);
  glBegin(GL_LINES);
  glVertex2f(initX, initY);
  glVertex2f(endX, endY);
  glEnd();
  glLineWidth(1.0f);

  float angle = atan2(vY_visual, vX_visual);
  if (vX_visual == 0 && vY_visual == 0) angle = 0;
  float size = 10.0f;

  glBegin(GL_TRIANGLES);
  glVertex2f(endX, endY);
  glVertex2f(endX - size * cos(angle - 0.5f), endY - size * sin(angle - 0.5f));
  glVertex2f(endX - size * cos(angle + 0.5f), endY - size * sin(angle + 0.5f));
  glEnd();

  glColor3f(1.0f, 1.0f, 1.0f);  // Branco
  drawText(endX + 5, endY + 5, vectorName, GLUT_BITMAP_9_BY_15);
}

/**
 * Função para verificar a colisão entre dois retângulos. Ela usa a lógica de
 * "não colisão" por questões de eficiência e, caso não seja detectada nenhuma
 * das condicionais, uma colisão é detectada
 */
bool checkRectangleCollision(float x1, float y1, float w1, float h1, float x2,
                             float y2, float w2, float h2) {
  if (x1 > x2 + w2) {
    return false;
  }
  if (x1 + w1 < x2) {
    return false;
  }
  if (y1 > y2 + h2) {
    return false;
  }
  if (y1 + h1 < y2) {
    return false;
  }

  return true;
}

void drawTextCentered(float cx, float y, const char* text, void* font) {
  int textWidth = getTextWidth(text, font);
  drawText(cx - textWidth / 2.0f, y, text, font);
}

/**
 * Função auxiliar para desenhar texto em 3D
 */
void drawText3D(float x, float y, float z, const char* text) {
  glRasterPos3f(x, y, z);
  while (*text) {
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *text);
    text++;
  }
}

/**
 * Função responsável por desenhar vetores 3D (Linha + Cone/Ponta + Label).
 * @param start Posição de início do vetor (Ex: centro do player).
 * @param vector Componentes físicas do vetor (velocidade, força, etc.).
 * @param scale Fator de escala visual para traduzir o valor físico em tamanho
 * de mundo.
 * @param r Cor R (0.0 a 1.0).
 * @param g Cor G (0.0 a 1.0).
 * @param b Cor B (0.0 a 1.0).
 * @param label Nome do vetor para exibição.
 */
void drawVector_3D(Vector_3D start, Vector_3D vector, float scale, float r,
                   float g, float b, const char* label) {
  float vx = vector.x * scale;
  float vy = vector.y * scale;
  float vz = vector.z * scale;
  float len = sqrt(vx * vx + vy * vy + vz * vz);

  if (len < 0.1f) return;  // Muito pequeno

  Vector_3D end = {start.x + vx, start.y + vy, start.z + vz};

  glDisable(GL_LIGHTING);
  glLineWidth(2.0f);
  glColor3f(r, g, b);

  glBegin(GL_LINES);
  glVertex3f(start.x, start.y, start.z);
  glVertex3f(end.x, end.y, end.z);
  glEnd();

  glPushMatrix();
  glTranslatef(end.x, end.y, end.z);

  // O glutSolidCone aponta para +Z por padrão.
  float yaw = atan2(vx, vz) * 180.0f / M_PI;
  float pitch = -atan2(vy, sqrt(vx * vx + vz * vz)) * 180.0f /
                M_PI;  // Negativo pois Y é up

  glRotatef(yaw, 0, 1, 0);
  glRotatef(pitch, 1, 0, 0);  // Inclina para cima/baixo

  glutSolidCone(0.2f, 0.5f, 8, 2);
  glPopMatrix();

  if (label) {
    glDisable(GL_DEPTH_TEST);  // Texto sempre visível
    glColor3f(1, 1, 1);
    glRasterPos3f(end.x, end.y + 0.2f, end.z);
    while (*label) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *label);
      label++;
    }
    glEnable(GL_DEPTH_TEST);
  }

  glEnable(GL_LIGHTING);
}

// Verifica se dois objetos 3D (Box) colidem (AABB - Axis Aligned Bounding Box)
bool checkAABBCollision(float x1, float y1, float z1, float w1, float h1,
                        float d1, float x2, float y2, float z2, float w2,
                        float h2, float d2) {
  return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2 &&
          z1 < z2 + d2 && z1 + d1 > z2);
}

// Verifica se um ponto está dentro de uma caixa 3D
bool isPointInsideBox(float px, float py, float pz, float x, float y, float z,
                      float w, float h, float d) {
  return (px >= x && px <= x + w && py >= y && py <= y + h && pz >= z &&
          pz <= z + d);
}

/**
 * Interseção de segmento de reta com caixa (Simplificada para o Gancho)
 * Retorna true se bateu e preenche hitX, hitY, hitZ (usamos para debug)
 */
bool lineBoxIntersection(float x1, float y1, float z1, float x2, float y2,
                         float z2, Platform_3D p, float& hX, float& hY,
                         float& hZ) {
  if (isPointInsideBox(x2, y2, z2, p.x, p.y, p.z, p.w, p.h, p.d)) {
    hX = x2;
    hY = y2;
    hZ = z2;
    return true;
  }
  return false;
}