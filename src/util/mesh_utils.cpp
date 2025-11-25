#include "mesh_utils.h" // Traz cstdint, cstddef e glew.h

#include <GL/freeglut.h> // Agora sim pode incluir freeglut
// gl.h e glu.h geralmente já vêm com freeglut, mas se precisar:
#include <GL/gl.h>
#include <GL/glu.h>

#include <cstdlib>
#include <cstring>
#include <cstdio>

// Layout: pos(3) + cor(4) + tex(2) + normal(3) = 12 floats por vértice
static const int FLOATS_POS    = 3;
static const int FLOATS_COR    = 4;
static const int FLOATS_TEX    = 2;
static const int FLOATS_NORMAL = 3;
static const int FLOATS_TOTAL  = FLOATS_POS + FLOATS_COR + FLOATS_TEX + FLOATS_NORMAL; // 12

// === Função interna: monta o "arrayzão" intercalado na RAM ===
// retorna um ponteiro que PRECISA de free() depois
/**
 * Inicializa e preenche um array linear (raw bytes) contendo os dados de vértices
 * para um conjunto de triângulos, pronto para uso em APIs gráficas que esperam
 * arrays intercalados de atributos (position, color, texcoord, normal).
 *
 * Layout por vértice (FLOATS_TOTAL = 12):
 *  - posição (pos)   : 3 GLfloats
 *  - cor   (cor)     : 4 GLfloats
 *  - texcoord (tex)  : 2 GLfloats
 *  - normal (normal) : 3 GLfloats
 *
 * Cada triângulo contribui com 3 vértices contíguos seguindo o layout acima.
 *
 * Parâmetros:
 *  - quantosTris : número de triângulos a serem convertidos para o array linear.
 *  - triangulos  : ponteiro para o primeiro elemento de um array de triângulos
 *                  (tipo `tri`) contendo, para cada triângulo, 3 vértices com os
 *                  arrays/atributos esperados (v[i].pos, v[i].cor, v[i].tex, v[i].normal).
 *
 * Retorno:
 *  - Retorna um ponteiro (char*) para um bloco de memória alocado com malloc()
 *    contendo os dados como uma sequência de GLfloats. O tamanho alocado é
 *    quantosTris * 3 (vértices por tri) * FLOATS_TOTAL * sizeof(GLfloat).
 *
 * Observações importantes:
 *  - A função usa malloc() para alocar a memória; o chamador é responsável por
 *    liberar essa memória usando free().
 *  - O ponteiro retornado é tipado como char* (raw bytes); entretanto os dados
 *    são armazenados como GLfloat consecutivos e podem ser reinterpretados como
 *    GLfloat* para uso posterior.
 *  - A função assume que cada triângulo em `triangulos` possui os atributos nas
 *    localizações esperadas e que FLOATS_TOTAL corresponde ao layout descrito
 *    (no código atual, espera-se 12 floats por vértice).
 *  - A função é static (escopo de arquivo), ou seja, visível apenas dentro do
 *    ficheiro em que está definida.
 *
 * Avisos/erros potenciais:
 *  - Não há verificação de ponteiro nulo para `triangulos` nem tratamento de
 *    falha de alocação (malloc pode retornar NULL). O chamador deve assegurar
 *    argumentos válidos ou adaptar a função para tratar erros conforme necessário.
 */
static char* inicializaVertexArrayTriangulos(int quantosTris, const tri* triangulos) {
    int quantosFloatsPorVertice = FLOATS_TOTAL; // 12
    int quantosVertices         = quantosTris * 3;

    char* arrayzao = (char*) std::malloc(
        quantosVertices *
        quantosFloatsPorVertice *
        sizeof(GLfloat)
    );

    GLfloat* p = (GLfloat*) arrayzao;

    // para cada triângulo, colocar dados de seus 3 vértices
    for (int i = 0; i < quantosTris; i++, p += quantosFloatsPorVertice * 3) {
        const tri* t = &triangulos[i];

        // vértice 0
        std::memcpy(&p[0],                         t->v[0].pos,    sizeof(GLfloat) * 3);
        std::memcpy(&p[3],                         t->v[0].cor,    sizeof(GLfloat) * 4);
        std::memcpy(&p[7],                         t->v[0].tex,    sizeof(GLfloat) * 2);
        std::memcpy(&p[9],                         t->v[0].normal, sizeof(GLfloat) * 3);

        // vértice 1
        std::memcpy(&p[12],                        t->v[1].pos,    sizeof(GLfloat) * 3);
        std::memcpy(&p[15],                        t->v[1].cor,    sizeof(GLfloat) * 4);
        std::memcpy(&p[19],                        t->v[1].tex,    sizeof(GLfloat) * 2);
        std::memcpy(&p[21],                        t->v[1].normal, sizeof(GLfloat) * 3);

        // vértice 2
        std::memcpy(&p[24],                        t->v[2].pos,    sizeof(GLfloat) * 3);
        std::memcpy(&p[27],                        t->v[2].cor,    sizeof(GLfloat) * 4);
        std::memcpy(&p[31],                        t->v[2].tex,    sizeof(GLfloat) * 2);
        std::memcpy(&p[33],                        t->v[2].normal, sizeof(GLfloat) * 3);
    }

    return arrayzao;
}

// === Função interna: configura ponteiros de atributo pro layout intercalado ===
// Usa VBO já bound em GL_ARRAY_BUFFER
/**
 * @brief Configura ponteiros de atributos intercalados para uso com VBOs (arrays de vértices).
 *
 * Esta função configura os ponteiros de atributo do pipeline fixo do OpenGL (funções gl*Pointer)
 * assumindo que os dados dos vértices estão organizados de forma intercalada em um único buffer.
 * Ela calcula o stride a partir da constante FLOATS_TOTAL (nº total de floats por vértice) e
 * define deslocamentos (offsets) em bytes para cada atributo dentro de um vértice.
 *
 * Layout esperado por vértice (em floats):
 *  - posição:    3 floats  (offset 0)
 *  - cor:        4 floats  (offset 3)
 *  - texCoord:   2 floats  (offset 7)
 *  - normal:     3 floats  (offset 9)
 *  => total esperado: 12 floats por vértice (FLOATS_TOTAL deve refletir isso)
 *
 * Observações importantes:
 *  - O último parâmetro das chamadas gl*Pointer() é tratado como um OFFSET em bytes porque se
 *    presume que um VBO (GL_ARRAY_BUFFER) está atualmente ligado. Se nenhum VBO estiver ligado,
 *    esse parâmetro seria interpretado como um ponteiro para memória cliente.
 *  - A função habilita os estados de cliente correspondentes:
 *      glEnableClientState(GL_VERTEX_ARRAY);
 *      glEnableClientState(GL_COLOR_ARRAY);
 *      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
 *      glEnableClientState(GL_NORMAL_ARRAY);
 *    Caberá ao chamador desabilitá-los se necessário ou restaurar o estado anterior.
 *  - Deve haver um contexto OpenGL válido e o VBO contendo os dados intercalados deve estar
 *    vinculado antes de chamar esta função.
 *
 * Efeitos colaterais:
 *  - Modifica o estado de ponteiros de atributos e habilita os arrays de cliente no contexto GL.
 *
 * @note Esta função usa a API de arrays do pipeline fixo (glVertexPointer, glColorPointer, etc.),
 *       adequada para compatibilidade com OpenGL legado. Para OpenGL moderno, prefira usar
 *       atributos de vértice (glVertexAttribPointer) e shaders.
 */
static void configuraPonteirosAtributosIntercalados() {
    GLsizei stride = sizeof(GLfloat) * FLOATS_TOTAL; // 12 floats por vértice

    // Como estamos usando VBO, o último parâmetro é um OFFSET em bytes
    const GLvoid* offsetPos   = (const GLvoid*)(0 * sizeof(GLfloat));  // 0
    const GLvoid* offsetColor = (const GLvoid*)(3 * sizeof(GLfloat));  // depois de 3 floats (pos)
    const GLvoid* offsetTex   = (const GLvoid*)(7 * sizeof(GLfloat));  // 3+4 = 7
    const GLvoid* offsetNorm  = (const GLvoid*)(9 * sizeof(GLfloat));  // 3+4+2 = 9

    glVertexPointer(3, GL_FLOAT, stride, offsetPos);
    glColorPointer (4, GL_FLOAT, stride, offsetColor);
    glTexCoordPointer(2, GL_FLOAT, stride, offsetTex);
    glNormalPointer(GL_FLOAT,  stride, offsetNorm);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
}

// === Implementação pública: desenhar direto da RAM (sem VBO) ===
/**
 * @brief Desenha um conjunto de triângulos usando Vertex Arrays do OpenGL.
 *
 * Esta função monta um array intercalado de atributos (posição, cor, coordenada
 * de textura e normal) chamando inicializaVertexArrayTriangulos(...) e em seguida
 * configura os ponteiros de atributo (glVertexPointer, glColorPointer,
 * glTexCoordPointer, glNormalPointer) usando um stride baseado em FLOATS_TOTAL.
 * Depois habilita os estados de cliente correspondentes e chama glDrawArrays
 * com o modo GL_TRIANGLES para desenhar todos os vértices.
 *
 * @param quantosTris Número de triângulos a desenhar. Cada triângulo é composto
 * de 3 vértices; o número total de vértices enviados a glDrawArrays é
 * quantosTris * 3.
 *
 * @param triangulos Ponteiro para um buffer de tri (estrutura tri). A função
 * passa esses dados para inicializaVertexArrayTriangulos que produz o layout
 * de floats esperado.
 *
 * Pré-condições:
 * - Deve haver um contexto OpenGL válido e ativo no thread que chama esta função.
 * - FLOATS_TOTAL e a função inicializaVertexArrayTriangulos(...) devem estar
 *   definidos de forma consistente com o layout esperado: 3 floats de posição,
 *   4 de cor, 2 de texcoord e 3 de normal (ou equivalente).
 *
 * Efeitos colaterais e comportamento:
 * - A função aloca um buffer temporário (via inicializaVertexArrayTriangulos)
 *   e libera essa memória antes de retornar (std::free).
 * - Habilita os client states: GL_VERTEX_ARRAY, GL_COLOR_ARRAY,
 *   GL_TEXTURE_COORD_ARRAY e GL_NORMAL_ARRAY. OBS: ela não os desabilita antes
 *   de retornar; se for necessário restaurar o estado anterior, o chamador deve
 *   fazê-lo.
 * - Não realiza verificação explícita de erros OpenGL nem validações dos
 *   ponteiros recebidos.
 *
 * Considerações de desempenho:
 * - A cada chamada é feita uma alocação temporária seguida de liberação; para
 *   cenários de alto desempenho é recomendado usar VBOs persistentes em vez de
 *   construir arrays temporários por chamada.
 *
 * Observações adicionais:
 * - Os ponteiros passados às gl*Pointer são calculados a partir do buffer de
 *   floats (offsets em número de GLfloat). Em implementações que usam VBOs,
 *   esses parâmetros devem ser reinterpretados como offsets (pointers para
 *   NULL + offset) conforme apropriado.
 */
void desenhaTrisComVertexArrays(int quantosTris, const tri* triangulos) {
    char* arrayzao = inicializaVertexArrayTriangulos(quantosTris, triangulos);
    const GLfloat* p = (const GLfloat*) arrayzao;

    int stride = sizeof(GLfloat) * FLOATS_TOTAL;

    glVertexPointer(3, GL_FLOAT, stride, p + 0);
    glColorPointer (4, GL_FLOAT, stride, p + 3);
    glTexCoordPointer(2, GL_FLOAT, stride, p + 7);
    glNormalPointer( GL_FLOAT, stride, p + 9);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    int quantosVertices = quantosTris * 3;
    glDrawArrays(GL_TRIANGLES, 0, quantosVertices);

    std::free(arrayzao);
}

// === Implementação pública: criar VBO a partir de tris ===
/**
 * criaVBODeTris
 *
 * Cria e inicializa um Vertex Buffer Object (VBO) contendo os dados de vértices
 * correspondentes a um conjunto de triângulos fornecidos.
 *
 * Esta função:
 *  - Gera um identificador de buffer OpenGL (glGenBuffers).
 *  - Constrói um array linear de floats com os dados de vértice chamando
 *    `inicializaVertexArrayTriangulos(quantosTris, triangulos)`. Espera-se que
 *    essa função retorne um bloco alocado dinamicamente (ponteiro para char)
 *    contendo valores do tipo GLfloat organizados conforme a constante
 *    FLOATS_TOTAL (layout/interleaving de atributos por vértice).
 *  - Calcula o tamanho do buffer em bytes com base no número de triângulos,
 *    no número de vértices por triângulo (3) e no stride (sizeof(GLfloat) * FLOATS_TOTAL).
 *  - Realiza o bind em GL_ARRAY_BUFFER e carrega os dados para a GPU usando
 *    glBufferData(..., GL_STATIC_DRAW).
 *  - Libera o array temporário alocado (std::free).
 *  - Opcionalmente desfaz o bind de GL_ARRAY_BUFFER (glBindBuffer(..., 0)).
 *
 * Parâmetros:
 *  - quantosTris: número de triângulos cuja geometria será transferida para o VBO.
 *  - triangulos: ponteiro para o primeiro elemento de um array de estruturas `tri`
 *                que descrevem os triângulos (formato esperado pelos utilitários
 *                de conversão para array de vértices).
 *
 * Retorno:
 *  - Retorna o identificador GLuint do VBO criado. Se a geração falhar, pode
 *    retornar 0 (comportamento dependente da implementação do driver/GL).
 *
 * Observações/Considerações:
 *  - A função `inicializaVertexArrayTriangulos` é responsável por criar o bloco
 *    de memória com os dados de vértice; aqui ele é liberado com std::free após
 *    o upload para GPU. Certifique-se de que a alocação realizada por
 *    `inicializaVertexArrayTriangulos` é compatível com std::free.
 *  - O layout dos dados por vértice (ordem e quantidade de floats) deve corresponder
 *    ao valor de FLOATS_TOTAL e às chamadas subsequentes de glVertexAttribPointer
 *    usadas quando esse VBO for ligado para renderização.
 *  - O VBO criado permanece propriedade do contexto OpenGL; é responsabilidade do
 *    chamador liberar o buffer quando não for mais necessário, usando glDeleteBuffers.
 *  - Esta função usa GL_STATIC_DRAW ao criar o buffer; ajuste se os dados forem
 *    atualizados dinamicamente em tempo de execução.
 */
GLuint criaVBODeTris(int quantosTris, const tri* triangulos) {
    GLuint idBuffer = 0;
    glGenBuffers(1, &idBuffer);

    char* arrayzao = inicializaVertexArrayTriangulos(quantosTris, triangulos);

    int stride          = sizeof(GLfloat) * FLOATS_TOTAL;
    int quantosVertices = 3 * quantosTris;
    int tamanhoBuffer   = stride * quantosVertices;

    glBindBuffer(GL_ARRAY_BUFFER, idBuffer);
    glBufferData(GL_ARRAY_BUFFER, tamanhoBuffer, arrayzao, GL_STATIC_DRAW);

    std::free(arrayzao);

    // Desvincula se quiser
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return idBuffer;
}

// === Implementação pública: desenhar usando VBO já criado ===
/**
 * @brief Desenha uma sequência de triângulos a partir de um VBO (Vertex Buffer Object).
 *
 * Esta função vincula o VBO passado ao alvo GL_ARRAY_BUFFER, configura os ponteiros
 * de atributos assumindo que os dados estão intercalados (através de
 * configuraPonteirosAtributosIntercalados()), calcula o número total de vértices
 * com base na quantidade de triângulos e dispara o desenho com glDrawArrays usando
 * o modo GL_TRIANGLES. Ao final, o buffer é desassociado do alvo GL_ARRAY_BUFFER.
 *
 * Efeitos colaterais / estado OpenGL:
 * - Chama glBindBuffer(GL_ARRAY_BUFFER, vbo), portanto o VBO especificado passa a
 *   ser o buffer ativo para atributos de vértice.
 * - Chama configuraPonteirosAtributosIntercalados(), que deve habilitar e definir
 *   os ponteiros de atributos (posições, normais, UVs, etc.) de acordo com o
 *   layout intercalado de dados no VBO.
 * - Executa glDrawArrays(GL_TRIANGLES, 0, quantosTris * 3) para emitir os vértices.
 * - No final chama glBindBuffer(GL_ARRAY_BUFFER, 0) para desassociar o buffer.
 * - Não desabilita explicitamente arrays de vértices nem altera VAOs ativos; o
 *   estado de habilitação dos atributos permanece como deixado por
 *   configuraPonteirosAtributosIntercalados() ou pelo contexto anterior.
 *
 * Pré-condições / suposições:
 * - Deve existir um contexto OpenGL válido no thread chamador.
 * - O identificador 'vbo' deve ser um buffer de vértices válido com dados organizados
 *   conforme os ponteiros configurados por configuraPonteirosAtributosIntercalados().
 * - Os dados no VBO devem conter pelo menos quantosTris * 3 vértices.
 * - 'quantosTris' deve ser um valor não-negativo; valores negativos resultarão em comportamento
 *   indefinido ao passar um tamanho inválido para glDrawArrays.
 * - O programa (shader) apropriado deve estar ligado e pronto para receber os atributos
 *   definidos pelos ponteiros.
 *
 * Uso esperado:
 * - Chamar antes de desenhar: carregar dados no VBO e garantir que o layout de atributos
 *   corresponda ao que configuraPonteirosAtributosIntercalados() espera.
 *
 * Observações de desempenho:
 * - Evitar ligar/desligar estados OpenGL desnecessariamente entre várias chamadas de
 *   desenho para reduzir overhead. Se for desenhar muitos VBOs/objetos, considerar
 *   uso de VAOs que armazenem os ponteiros de atributo para reaproveitamento.
 *
 * @param vbo Handle (GLuint) do Vertex Buffer Object contendo os dados dos vértices.
 * @param quantosTris Número de triângulos a serem desenhados (cada triângulo usa 3 vértices).
 */
void desenhaTrisComVBO(GLuint vbo, int quantosTris) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    configuraPonteirosAtributosIntercalados();

    int quantosVertices = quantosTris * 3;
    glDrawArrays(GL_TRIANGLES, 0, quantosVertices);

    // opcional: desligar estados / desbindar
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * Gera um array de triângulos representando uma caixa (cubo/paralelepípedo).
 * Retorna um ponteiro para o array alocado (deve ser liberado com free).
 */
tri* createBoxTris(float x, float y, float z, float w, float h, float d, float r, float g, float b, int& outTriCount) {
    outTriCount = 12;
    tri* tris = (tri*)malloc(outTriCount * sizeof(tri));

    // Coordenadas
    float x0 = x,     y0 = y,     z0 = z + d;
    float x1 = x + w, y1 = y,     z1 = z + d;
    float x2 = x + w, y2 = y + h, z2 = z + d;
    float x3 = x,     y3 = y + h, z3 = z + d;
    float x4 = x,     y4 = y,     z4 = z;
    float x5 = x + w, y5 = y,     z5 = z;
    float x6 = x + w, y6 = y + h, z6 = z;
    float x7 = x,     y7 = y + h, z7 = z;

    // Função auxiliar atualizada para preencher a struct 'vert'
    // Agora aceita a Normal (nx, ny, nz)
    auto setV = [&](vert& v, float vx, float vy, float vz, float nx, float ny, float nz) {
        // Posição
        v.pos[0] = vx; v.pos[1] = vy; v.pos[2] = vz;
        // Cor (R, G, B, Alpha=1.0)
        v.cor[0] = r;  v.cor[1] = g;  v.cor[2] = b; v.cor[3] = 1.0f; 
        // Normal
        v.normal[0] = nx; v.normal[1] = ny; v.normal[2] = nz;
        // Textura (Zero por enquanto)
        v.tex[0] = 0.0f; v.tex[1] = 0.0f;
    };

    int i = 0;

    // --- FACE FRONTAL (Normal aponta para +Z: 0, 0, 1) ---
    setV(tris[i].v[0], x0, y0, z0, 0, 0, 1);
    setV(tris[i].v[1], x1, y1, z1, 0, 0, 1);
    setV(tris[i].v[2], x2, y2, z2, 0, 0, 1);
    i++;
    setV(tris[i].v[0], x0, y0, z0, 0, 0, 1);
    setV(tris[i].v[1], x2, y2, z2, 0, 0, 1);
    setV(tris[i].v[2], x3, y3, z3, 0, 0, 1);
    i++;

    // --- FACE TRASEIRA (Normal aponta para -Z: 0, 0, -1) ---
    setV(tris[i].v[0], x5, y5, z5, 0, 0, -1);
    setV(tris[i].v[1], x4, y4, z4, 0, 0, -1);
    setV(tris[i].v[2], x7, y7, z7, 0, 0, -1);
    i++;
    setV(tris[i].v[0], x5, y5, z5, 0, 0, -1);
    setV(tris[i].v[1], x7, y7, z7, 0, 0, -1);
    setV(tris[i].v[2], x6, y6, z6, 0, 0, -1);
    i++;

    // --- FACE ESQUERDA (Normal aponta para -X: -1, 0, 0) ---
    setV(tris[i].v[0], x4, y4, z4, -1, 0, 0);
    setV(tris[i].v[1], x0, y0, z0, -1, 0, 0);
    setV(tris[i].v[2], x3, y3, z3, -1, 0, 0);
    i++;
    setV(tris[i].v[0], x4, y4, z4, -1, 0, 0);
    setV(tris[i].v[1], x3, y3, z3, -1, 0, 0);
    setV(tris[i].v[2], x7, y7, z7, -1, 0, 0);
    i++;

    // --- FACE DIREITA (Normal aponta para +X: 1, 0, 0) ---
    setV(tris[i].v[0], x1, y1, z1, 1, 0, 0);
    setV(tris[i].v[1], x5, y5, z5, 1, 0, 0);
    setV(tris[i].v[2], x6, y6, z6, 1, 0, 0);
    i++;
    setV(tris[i].v[0], x1, y1, z1, 1, 0, 0);
    setV(tris[i].v[1], x6, y6, z6, 1, 0, 0);
    setV(tris[i].v[2], x2, y2, z2, 1, 0, 0);
    i++;

    // --- FACE SUPERIOR (Normal aponta para +Y: 0, 1, 0) ---
    setV(tris[i].v[0], x3, y3, z3, 0, 1, 0);
    setV(tris[i].v[1], x2, y2, z2, 0, 1, 0);
    setV(tris[i].v[2], x6, y6, z6, 0, 1, 0);
    i++;
    setV(tris[i].v[0], x3, y3, z3, 0, 1, 0);
    setV(tris[i].v[1], x6, y6, z6, 0, 1, 0);
    setV(tris[i].v[2], x7, y7, z7, 0, 1, 0);
    i++;

    // --- FACE INFERIOR (Normal aponta para -Y: 0, -1, 0) ---
    setV(tris[i].v[0], x4, y4, z4, 0, -1, 0);
    setV(tris[i].v[1], x5, y5, z5, 0, -1, 0);
    setV(tris[i].v[2], x1, y1, z1, 0, -1, 0);
    i++;
    setV(tris[i].v[0], x4, y4, z4, 0, -1, 0);
    setV(tris[i].v[1], x1, y1, z1, 0, -1, 0);
    setV(tris[i].v[2], x0, y0, z0, 0, -1, 0);
    i++;

    return tris;
}