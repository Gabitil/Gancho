#ifndef UTILS_H
#define UTILS_H

#include <cstddef>
#include <cstdint>

// Ordem Crítica:
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "GL/stb_image.h"
#include <cmath>

// Headers do projeto (opcionais aqui, dependendo se utils.h precisa deles)
// Se utils.h apenas declara funções utilitárias, evite incluir game.h aqui para reduzir acoplamento.
// Mas se for necessário:
#include "game.h" 
#include "game_structs.h" 
// #include "game_3D.h"

// ----------------------------------------------------------------------------------------------------------------

/**
 * @brief Desenha um retângulo preenchido.
 *
 * Desenha um retângulo na posição especificada por (initX, initY) com a largura e altura
 * fornecidas, utilizando a cor definida pelos componentes RGB e um valor de transparência (alpha).
 * Valores de cor e transparência devem ser passados no intervalo [0.0, 1.0]. O parâmetro transparency
 * tem valor padrão 1.0 (completamente opaco).
 *
 * @param initX Coordenada X do ponto inicial do retângulo (interpretação do ponto inicial depende do sistema de coordenadas do contexto).
 * @param initY Coordenada Y do ponto inicial do retângulo (interpretação do ponto inicial depende do sistema de coordenadas do contexto).
 * @param width Largura do retângulo (espera-se valor positivo).
 * @param height Altura do retângulo (espera-se valor positivo).
 * @param r Componente vermelha da cor, no intervalo [0, 1].
 * @param g Componente verde da cor, no intervalo [0, 1].
 * @param b Componente azul da cor, no intervalo [0, 1].
 * @param transparency Valor alpha (transparência) no intervalo [0, 1]. 0 = totalmente transparente, 1 = totalmente opaco.
 *
 * @note A interpretação exata de (initX, initY) — por exemplo, se corresponde ao canto superior esquerdo,
 *       inferior esquerdo ou ao centro do retângulo — depende do sistema de coordenadas usado pelo contexto de desenho.
 * @note Para que a transparência funcione corretamente, certifique-se de que o blending esteja habilitado no contexto gráfico.
 */
void drawRect(float initX, float initY, float width, float height, float r,
              float g, float b, float transparency = 1.0f);

/**
 * @brief Desenha uma sequência de "spikes" (picos triangulares) preenchidos.
 *
 * Desenha uma série de picos triangulares contíguos dentro do retângulo
 * definido pelo ponto inicial (initX, initY) e pelas dimensões (width, height).
 * Os picos ocupam toda a largura especificada e têm altura limitada por height.
 * A cor utilizada para preenchimento é determinada pelos componentes r, g e b.
 *
 * @param initX Coordenada X do ponto inicial (unidades do sistema de coordenadas atual).
 * @param initY Coordenada Y do ponto inicial (unidades do sistema de coordenadas atual).
 * @param width Largura total da área onde os spikes serão desenhados.
 * @param height Altura máxima de cada spike.
 * @param r Componente vermelha da cor, normalmente no intervalo [0.0, 1.0].
 * @param g Componente verde da cor, normalmente no intervalo [0.0, 1.0].
 * @param b Componente azul da cor, normalmente no intervalo [0.0, 1.0].
 *
 * @note A função utiliza o contexto gráfico atual (por exemplo OpenGL) e pode
 *       modificar estados gráficos (como cor). Se for necessário preservar o
 *       estado, salve-o antes e restaure após a chamada.
 * @warning Valores de cor fora do intervalo esperado ou dimensões não positivas
 *          podem produzir resultados inesperados.
 */
void drawSpikes(float initX, float initY, float width, float height, float r,
                float g, float b);

/**
 * @brief Desenha texto na tela usando fontes bitmap do GLUT.
 *
 * Desenha a string fornecida na posição especificada por (x, y). As coordenadas
 * são interpretadas no sistema de coordenadas corrente (dependem da projeção e
 * do viewport ativos), portanto podem corresponder a pixels da janela ou a
 * unidades normalizadas conforme a configuração atual do OpenGL.
 *
 * @param x Coordenada X do ponto de referência onde o texto será renderizado.
 * @param y Coordenada Y do ponto de referência onde o texto será renderizado.
 * @param text Ponteiro para uma string C terminada em '\0' contendo o texto a ser desenhado.
 * @param font Ponteiro para a fonte bitmap do GLUT a ser utilizada. Valor padrão:
 *             GLUT_BITMAP_HELVETICA_18.
 */
void drawText(float x, float y, const char* text,
    void* font = GLUT_BITMAP_HELVETICA_18);

/**
 * @brief Verifica se um ponto está dentro de um retângulo alinhado aos eixos.
 *
 * @details
 * Determina se o ponto (pointX, pointY) encontra-se dentro do retângulo ou sobre suas bordas.
 * O retângulo é considerado alinhado aos eixos e definido pelo canto superior-esquerdo em
 * (rectX, rectY) com largura rectWidth e altura rectHeight. Espera-se que rectWidth e rectHeight
 * sejam valores não-negativos.
 *
 * @param pointX Coordenada X do ponto a testar.
 * @param pointY Coordenada Y do ponto a testar.
 * @param rectX Coordenada X do canto superior-esquerdo do retângulo.
 * @param rectY Coordenada Y do canto superior-esquerdo do retângulo.
 * @param rectWidth Largura do retângulo (>= 0).
 * @param rectHeight Altura do retângulo (>= 0).
 * @return true se o ponto estiver dentro do retângulo ou exatamente sobre sua borda; false caso contrário.
 */
bool isPointInside(float pointX, float pointY, float rectX, float rectY,
                   float rectWidth, float rectHeight);

/**
 * @brief Calcula a largura em pixels de um texto renderizado com uma fonte.
 *
 * Calcula e retorna a largura total (em pixels) da string C terminada em '\0'
 * passada em 'text' quando renderizada usando a fonte indicada por 'font'.
 * O ponteiro 'font' é opaco e a sua interpretação depende da implementação do
 * sistema de renderização usado pelo projeto.
 *
 * @param text Ponteiro para uma string C (null-terminated) cujo comprimento se
 *             deseja medir. Deve apontar para uma sequência válida de caracteres.
 * @param font Ponteiro opaco para o objeto/estrutura que representa a fonte a
 *             ser usada para a medição. Deve ser válido conforme a implementação.
 *
 * @return Largura do texto em pixels como inteiro não negativo.
 *
 * @pre text != nullptr && font != nullptr
 * @note Comportamento em caso de ponteiros inválidos, codificações de entrada
 *       (ex.: UTF-8 vs ASCII) ou fontes não suportadas depende da implementação
 *       concreta. Esta função apenas calcula a largura; não altera o estado da
 *       fonte nem realiza operações de desenho.
 */
int getTextWidth(const char* text, void* font);

/**
 * @brief Verifica a interseção entre um segmento de reta e um retângulo (Platform).
 *
 * Avalia se o segmento definido pelos pontos (x1, y1) e (x2, y2) intersecta o retângulo
 * representado por 'plataform'. Quando há interseção, as coordenadas do ponto de impacto
 * são escritas em hitX e hitY.
 *
 * @param x1 Coordenada X do primeiro ponto do segmento.
 * @param y1 Coordenada Y do primeiro ponto do segmento.
 * @param x2 Coordenada X do segundo ponto do segmento.
 * @param y2 Coordenada Y do segundo ponto do segmento.
 * @param plataform Referência ao objeto Platform que descreve o retângulo (assume-se uma AABB - axis-aligned bounding box).
 * @param[out] hitX Recebe a coordenada X do ponto de interseção (válida somente se a função retornar true).
 * @param[out] hitY Recebe a coordenada Y do ponto de interseção (válida somente se a função retornar true).
 *
 * @return true se existir interseção entre o segmento e o retângulo; false caso contrário.
 *
 * @note Se o segmento intersectar o retângulo em mais de um ponto, o ponto retornado corresponde
 *       ao primeiro ponto de interseção encontrado ao avançar de (x1, y1) em direção a (x2, y2).
 * @note hitX e hitY não são garantidos quando a função retorna false.
 */
bool lineRectIntersection(float x1, float y1, float x2, float y2,
                          const Platform& plataform, float& hitX, float& hitY);

/**
 * @brief Corta (clipa) um segmento de linha à janela de visualização.
 *
 * Esta função ajusta o ponto final (endX, endY) do segmento definido por
 * (startX, startY) -> (endX, endY) de modo que o segmento resultante fique
 * dentro do retângulo de visualização definido pelo canto inferior-esquerdo
 * (cameraLeft, cameraBottom) e pelas dimensões VIEW_WIDTH x VIEW_HEIGHT.
 *
 * - Se o segmento já estiver totalmente dentro da janela de visualização,
 *   endX e endY permanecem inalterados.
 * - Se o segmento cruzar a borda da janela, endX e endY são movidos para o
 *   ponto de interseção entre o segmento original e a borda da janela, de forma
 *   que o novo segmento seja contido na janela.
 * - Se o segmento não intersectar a janela (isto é, estiver completamente
 *   fora), o ponto final será ajustado de modo a produzir um segmento vazio
 *   ou não visível (por exemplo, igualando-se ao ponto inicial), dependendo da
 *   implementação concreta.
 *
 * @param startX       Coordenada X do ponto inicial do segmento (não é modificada).
 * @param startY       Coordenada Y do ponto inicial do segmento (não é modificada).
 * @param[out] endX    Referência para a coordenada X do ponto final; será atualizada
 *                     para garantir que o segmento fique dentro da janela.
 * @param[out] endY    Referência para a coordenada Y do ponto final; será atualizada
 *                     para garantir que o segmento fique dentro da janela.
 * @param VIEW_WIDTH   Largura da janela de visualização (unidades do mesmo sistema de coordenadas).
 * @param VIEW_HEIGHT  Altura da janela de visualização (unidades do mesmo sistema de coordenadas).
 * @param cameraLeft   Coordenada X do canto inferior-esquerdo da janela de visualização.
 * @param cameraBottom Coordenada Y do canto inferior-esquerdo da janela de visualização.
 *
 * @note Todas as coordenadas e dimensões devem estar no mesmo sistema de
 *       coordenadas (por exemplo, coordenadas de mundo ou de tela), e não há
 *       normalização implícita. Consulte a implementação caso precise de um
 *       comportamento determinístico para o caso de segmento totalmente fora.
 */
void clipLineToView(float startX, float startY, float& endX, float& endY,
                    float VIEW_WIDTH, float VIEW_HEIGHT, float cameraLeft,
                    float cameraBottom);

/**
 * @brief Desenha um vetor 2D (seta) na cena a partir de uma posição inicial.
 *
 * Desenha uma representação visual de um vetor como uma linha com ponta de
 * seta, aplicando um fator de escala às componentes físicas do vetor e usando
 * a cor especificada. Opcionalmente desenha um rótulo próximo à ponta do vetor.
 *
 * @param initX Coordenada X do ponto inicial (origem) do vetor, em unidades de tela.
 * @param initY Coordenada Y do ponto inicial (origem) do vetor, em unidades de tela.
 * @param vX_physics Componente X do vetor em unidades físicas (será multiplicada por scale).
 * @param vY_physics Componente Y do vetor em unidades físicas (será multiplicada por scale).
 * @param scale Fator de escala aplicado às componentes do vetor antes do desenho.
 *              Valores maiores tornam o vetor mais longo na representação gráfica.
 * @param r Valor do canal vermelho da cor do vetor (esperado entre 0.0 e 1.0).
 * @param g Valor do canal verde da cor do vetor (esperado entre 0.0 e 1.0).
 * @param b Valor do canal azul da cor do vetor (esperado entre 0.0 e 1.0).
 * @param vectorName Ponteiro para string NUL-terminated usada como rótulo. 
 *                   Se for nullptr ou string vazia, nenhum rótulo é desenhado.
 *
 * @details
 * - A linha é desenhada de (initX, initY) até (initX + vX_physics*scale, initY + vY_physics*scale).
 * - É desenhada uma ponta de seta orientada ao longo da direção do vetor.
 * - Espera-se que o contexto gráfico (por exemplo OpenGL) já esteja configurado pelo chamador.
 * - Uso típico: visualização de vetores físicos (velocidade, força, etc.) para depuração ou ilustração.
 */
void drawVector(float initX, float initY, float vX_physics, float vY_physics,
                float scale, float r, float g, float b, const char* vectorName);

/**
 * Verifica colisão entre dois retângulos alinhados aos eixos (axis-aligned).
 *
 * O parâmetro (x, y) de cada retângulo representa a posição do canto superior esquerdo,
 * e w/h representam, respectivamente, a largura e a altura do retângulo.
 *
 * Observações:
 * - A função assume retângulos alinhados aos eixos (sem rotação).
 * - Recomenda-se que w e h sejam valores não-negativos.
 * - Retorna true se as áreas dos retângulos se intersectam ou chegam a se tocar nas bordas.
 *
 * @param x1 Posição X (canto superior esquerdo) do retângulo 1
 * @param y1 Posição Y (canto superior esquerdo) do retângulo 1
 * @param w1 Largura do retângulo 1
 * @param h1 Altura do retângulo 1
 * @param x2 Posição X (canto superior esquerdo) do retângulo 2
 * @param y2 Posição Y (canto superior esquerdo) do retângulo 2
 * @param w2 Largura do retângulo 2
 * @param h2 Altura do retângulo 2
 * @return true se houver interseção (ou contato de borda) entre os dois retângulos; false caso contrário
 *
 * Complexidade: O(1)
 */
bool checkRectangleCollision(float x1, float y1, float w1, float h1, float x2,
                             float y2, float w2, float h2);

/**
 * @brief Desenha uma string com alinhamento horizontal centralizado.
 *
 * Renderiza o texto de modo que o centro horizontal da string coincida com a
 * coordenada especificada por 'cx'. A coordenada 'y' indica a posição vertical
 * onde o texto será desenhado (normalmente a linha de base, dependendo da
 * implementação da fonte).
 *
 * Observações:
 * - Se 'text' for nullptr ou uma string vazia, a função não realiza desenho.
 * - 'font' é um ponteiro para o recurso de fonte usado pela rotina de renderização
 *   (o tipo concreto e o formato aceito dependem da biblioteca gráfica utilizada,
 *   por exemplo fontes GLUT ou estruturas de FreeType).
 * - É esperado que um contexto de renderização válido (por exemplo um contexto OpenGL)
 *   esteja ativo ao chamar esta função.
 * - A função centraliza apenas no eixo X; o alinhamento vertical efetivo depende da
 *   métrica da fonte e da interpretação de 'y' pela implementação.
 *
 * @param cx Coordenada X do ponto que deverá corresponder ao centro horizontal do texto.
 * @param y  Coordenada Y onde o texto será posicionado (linha de base ou referência vertical).
 * @param text Ponteiro para a string terminada em '\0' a ser desenhada (codificação conforme suporte da implementação).
 * @param font Ponteiro para a fonte a ser utilizada (tipo específico da biblioteca de renderização).
 */
void drawTextCentered(float cx, float y, const char* text, void* font);

/**
 * @brief Carrega uma textura a partir de um ficheiro e cria uma textura OpenGL.
 *
 * @param filepath Caminho para o ficheiro de imagem (por exemplo PNG, JPEG, BMP).
 * @return GLuint Identificador da textura criada (nome OpenGL). Retorna 0 em caso de erro
 *               (falha ao abrir/decodificar o ficheiro ou ao enviar os dados para a GPU).
 *
 * Detalhes:
 * - Os dados da imagem são carregados e enviados para a GPU como uma textura 2D.
 * - Parâmetros de textura (filtragem, modo de repetição) e geração de mipmaps são aplicados
 *   conforme a implementação desta função.
 * - É necessário ter um contexto OpenGL válido no momento da chamada.
 * - O chamador é responsável por libertar a textura com glDeleteTextures quando não for mais necessária.
 */
GLuint loadTexture(const char* filepath);

/**
 * @brief Desenha um retângulo texturizado 2D.
 *
 * Desenha um quad na posição (x, y) com largura w e altura h, aplicando a textura
 * especificada por textureID. Os parâmetros flipH e flipV invertem, respectivamente,
 * as coordenadas de textura no eixo horizontal e vertical, permitindo ajustar a
 * orientação da imagem (útil para compensar diferenças de origem entre imagens e
 * coordenadas de textura OpenGL).
 *
 * @param x        Coordenada X da origem do retângulo (unidades da projeção/viewport).
 * @param y        Coordenada Y da origem do retângulo (unidades da projeção/viewport).
 * @param w        Largura do retângulo.
 * @param h        Altura do retângulo.
 * @param textureID ID da textura OpenGL a ser utilizada; deve estar previamente carregada.
 * @param flipH    Se true, inverte as coordenadas de textura horizontalmente.
 * @param flipV    Se true, inverte as coordenadas de textura verticalmente. O valor
 *                 padrão é true para compensar a origem de textura típica do OpenGL.
 *
 * @note É necessário que um contexto OpenGL válido esteja ativo. A função vincula a
 *       textura (glBindTexture) e emite os vértices/coords do quad; ela pode alterar
 *       estados OpenGL (binding de textura, unidade ativa, etc.) e não necessariamente
 *       restaura o estado anterior.
 * @warning Certifique-se de que o shader/programa, blend e matrizes de projeção/modelo
 *          estejam configurados conforme necessário antes de chamar esta função.
 */
void drawTexturedRect(float x, float y, float w, float h, GLuint textureID,
                      bool flipH = false, bool flipV = true);

void drawRepeatingTexturedRect(float x, float y, float w, float h,
                               GLuint textureID, float textureWidth,
                               float textureHeight);

void drawParallaxLayer(GLuint textureID,
                       float cameraLeft, float cameraBottom,
                       float viewW, float viewH,
                       float parallaxX, float parallaxY,
                       float tileWorldW, float tileWorldH);

void drawCubeLegacy(float x, float y, float z, float w, float h, float d, float r, float g, float b, float alpha = 1.0f);

void drawVector_3D(Vector_3D start, Vector_3D vector, float scale, float r, float g, float b, const char* label);

bool checkAABBCollision(float x1, float y1, float z1, float w1, float h1, float d1,
                        float x2, float y2, float z2, float w2, float h2, float d2);

bool isPointInsideBox(float px, float py, float pz, 
                      float x, float y, float z, float w, float h, float d);

bool lineBoxIntersection(float x1, float y1, float z1, 
                         float x2, float y2, float z2, 
                         Platform_3D p, float& hX, float& hY, float& hZ);


void drawText3D(float x, float y, float z, const char* text);
                         
#endif