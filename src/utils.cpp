#define STB_IMAGE_IMPLEMENTATION
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/stb_image.h>

#include <cstdio>

#include "game.h"
// #include <stdio.h>
#include <math.h>
// #include <string>
// #include <vector>
// #include <algorithm>

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
  // Carrega a imagem
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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);

  // Define os filtros da textura
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Define como a textura se comporta nas bordas (opcional, mas recomendado)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  stbi_image_free(data);  // Libera a memória da imagem

  printf("Textura %s carregada com sucesso! id=%u (%dx%d)\n", filepath,
         textureID, width, height);

  return textureID;  // Retorna o ID da textura carregada
}

/**
 * Essa função desenha um retângulo, mas é possível escolher a transparência da
 * sua cor. Os parâmetros passados são os mesmos para uma função comum de
 * desenho, como as variáveis onde o desenho será adicionado, a altura e a
 * largura, as cores (em RGB) e o coeficiente de transparência
 */
void drawRect(float initX, float initY, float width, float height, float r,
              float g, float b, float transparency = 1.0f) {
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
                      bool flipH = false,
                      bool flipV = true) {  // flipV = true é o novo padrão
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
void drawText(float x, float y, const char* text,
              void* font = GLUT_BITMAP_HELVETICA_18) {
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