// #include <GL/glew.h>
// #include <GL/freeglut.h>
// #include <stdio.h>

// // --- Funções de Callback ---

// void display() {
//     // Limpa o buffer de cor
//     glClear(GL_COLOR_BUFFER_BIT);

//     // Lógica de desenho aqui...
//     // Ex: desenharMenu(), desenharJogo(), etc.

//     // Troca os buffers (desenho duplo)
//     glutSwapBuffers();
// }

// void reshape(int width, int height) {
//     // Configura a viewport para a janela inteira
//     glViewport(0, 0, width, height);
    
//     // Mais configurações de projeção podem vir aqui
// }

// void keyboard(unsigned char key, int x, int y) {
//     // Trata pressionamento de teclas normais
//     // ex: if (key == 'w') { ... }
//     printf("Tecla pressionada: %c\n", key);
// }

// // Função para ser chamada quando não há eventos
// void idle() {
//     // Lógica de atualização do jogo (física, animação, IA)
//     // ...

//     // Redesenha a tela
//     glutPostRedisplay();
// }


// // --- Função Principal ---

// int main(int argc, char** argv) {
//     // 1. Inicializa o GLUT
//     glutInit(&argc, argv);
//     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // Habilita buffer duplo e cores RGBA
//     glutInitWindowSize(800, 600);
//     glutInitWindowPosition(100, 100);
//     glutCreateWindow("Projeto de Computacao Grafica - Gancho");

//     // 2. Inicializa o GLEW
//     GLenum err = glewInit();
//     if (GLEW_OK != err) {
//         // Problema: glewInit falhou, algo está muito errado.
//         fprintf(stderr, "Erro: %s\n", glewGetErrorString(err));
//         return 1;
//     }
//     printf("Status: Usando GLEW %s\n", glewGetString(GLEW_VERSION));

//     // 3. Registra as funções de callback
//     glutDisplayFunc(display);
//     glutReshapeFunc(reshape);
//     glutKeyboardFunc(keyboard);
//     glutIdleFunc(idle);
//     // Outros callbacks: glutMouseFunc, glutMotionFunc, etc.

//     // 4. Configurações iniciais do OpenGL (cor de fundo, etc.)
//     glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Fundo cinza escuro

//     // 5. Inicia o loop principal do GLUT
//     glutMainLoop();

//     return 0;
// }

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

// Função de callback para desenho
void display() {
    // Limpa a tela com a cor de fundo definida
    glClear(GL_COLOR_BUFFER_BIT);

    // Começa a desenhar um triângulo
    glBegin(GL_TRIANGLES);
        // Vértice 1 (inferior esquerdo) - Vermelho
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(-0.5f, -0.5f);

        // Vértice 2 (inferior direito) - Verde
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f(0.5f, -0.5f);

        // Vértice 3 (superior central) - Azul
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex2f(0.0f, 0.5f);
    glEnd(); // Finaliza o desenho do triângulo

    // Troca os buffers para exibir o que foi desenhado
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    // Inicializa o GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Teste OpenGL com Makefile");

    // Inicializa o GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        // Se falhar, exibe o erro e encerra
        std::cerr << "Erro ao inicializar GLEW: " << glewGetErrorString(err) << std::endl;
        return 1;
    }

    // Registra a função de display
    glutDisplayFunc(display);

    // Define a cor de fundo (cinza escuro)
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // Inicia o loop principal do GLUT
    std::cout << "Iniciando o loop principal do GLUT..." << std::endl;
    glutMainLoop();

    return 0;
}
