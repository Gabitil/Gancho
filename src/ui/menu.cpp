#include "menu.h" // Já inclui GL/glew.h e freeglut.h na ordem certa

#include <cstring>
#include <stdio.h> // Para printf se necessário

#include "utils.h"
// #include "audio.h"

// ----------------------------------------------------------------------------------------------------------------

// Puxa os IDs de textura globais (definidos em main.cpp)
extern GLuint texMenuBackground;
extern GLuint texButtonNormal;
extern GLuint texButtonHover;
extern GLuint texButtonDisabled;

/**
 * Os botões do jogo também serão padronizados.
 * Para implementar o esquema de "mouse sobre o botão" solicitado nos requisitos
 * técnicos do trabalho, foi usada uma variável de controle dentro da definição
 * da estrutura do botão chamada de "hovered" e ela define se o mouse está ou
 * não sobre o botão. Além disso, para indicar se o botão está disponível para
 * clique, também há a variável "enabled"
 */
// Em menu.cpp

void drawButton(const Button &btn)
{
  GLuint textureToUse = texButtonNormal; // Textura padrão

  if (!btn.enabled)
  {
    textureToUse = texButtonDisabled; // Usa textura de desabilitado
  }
  else if (btn.hovered)
  {
    textureToUse = texButtonHover; // Usa textura de hover (mouse sobre)
  }

  // 1. Desenha o retângulo do botão usando a textura
  drawTexturedRect(btn.x, btn.y, btn.w, btn.h, textureToUse, false, false);

  // 2. O código para desenhar o TEXTO por cima do botão permanece o mesmo
  if (btn.enabled)
  {
    glColor3f(1.0f, 1.0f, 1.0f); // Texto branco (ativado)
  }
  else
  {
    glColor3f(0.8f, 0.8f, 0.8f); // Texto cinza (desativado)
  }

  // ... (resto da função para centralizar o texto, não precisa mudar)
  int textWidth = 0;
  for (const char *caracter = btn.label; *caracter; ++caracter)
  {
    textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *caracter);
  }

  float textX = btn.x + (btn.w - textWidth) / 2.0f;
  float textY = btn.y + (btn.h / 2.0f) + 6.0f;

  drawText(textX, textY, btn.label, GLUT_BITMAP_HELVETICA_18);
}

/**
 * Função responsável por desenhar a quantidade de botões solicitada. Para isso,
 * ela deve limpar o buffer de cor "GL_COLOR_BUFFER_BIT", desenhar os botões e
 * trocar os buffers de desenho para perpetuar as alterações na cena
 */
// Em menu.cpp

void renderMenu(Button buttons[], int count)
{
  glClear(GL_COLOR_BUFFER_BIT); // Limpa a tela

  // 1. Desenha o fundo texturizado
  float winW = (float)glutGet(GLUT_WINDOW_WIDTH);
  float winH = (float)glutGet(GLUT_WINDOW_HEIGHT);
  drawTexturedRect(0, 0, winW, winH,
                   texMenuBackground, false, false); // Usa o ID da textura de fundo

  // 2. Desenha os botões (agora texturizados)
  for (int i = 0; i < count; i++)
  {
    drawButton(buttons[i]);
  }

  const char *text = "Desenvolvedores: Gabriel Augusto, Geovane da Silva e Otavio Andrade. Agradecimentos: Professor Andre Rodrigues";
  void *font = GLUT_BITMAP_HELVETICA_10;
  int padding = 10;

  int textWidth = getTextWidth(text, font);
  int textHeight = 17;

  float boxW = (float)(textWidth + padding * 2);
  float boxH = (float)(textHeight + padding * 2);

  float x = winW - boxW - 20.0f;
  float y = winH - boxH - 20.0f;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
  glRectf(x, y, x + boxW, y + boxH);

  glDisable(GL_BLEND);

  glColor3f(1.0f, 1.0f, 1.0f);

  drawText(x + padding, y + padding + 4, text, font);

  glutSwapBuffers();
}

/**
 * Para verificar qual botão do menu foi selecionado, a lógica é parecida com a
 * de colisão de quadrados (amplamente usada em atividades anteriores e nesse
 * trabalho). Ao escolher, será retornada a opção de menu que foi escolhida
 */
MenuOption handleMenuInput(int x, int y, Button buttons[], int count)
{
  for (int i = 0; i < count; i++)
  {
    if (buttons[i].enabled && x >= buttons[i].x &&
        x <= buttons[i].x + buttons[i].w && y >= buttons[i].y &&
        y <= buttons[i].y + buttons[i].h)
    {
      return (MenuOption)(i + 1);
    }
  }
  return NONE;
}