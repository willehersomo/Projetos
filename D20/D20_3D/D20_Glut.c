#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define NUMERO_TRIANGULOS 20

/*
Este projeto utiliza a biblioteca stb_image para carregar as texturas. 
A biblioteca é desenvolvida por Sean Barrett e disponibilizada sob licença pública de domínio ou MIT. 
É uma ferramenta incrível para trabalhar com imagens de maneira simples em projetos C/C++.
*/

typedef struct
{
    float x, y, z;
} Vertice;

const Vertice vertices[] = {
    {0, 1, 0},
    {0.894, 0.447, 0},
    {0.276, 0.447, 0.851},
    {-0.724, 0.447, 0.526},
    {-0.724, 0.447, -0.526},
    {0.276, 0.447, -0.851},
    {0.724, -0.447, 0.526},
    {-0.276, -0.447, 0.851},
    {-0.894, -0.447, 0},
    {-0.276, -0.447, -0.851},
    {0.724, -0.447, -0.526},
    {0, -1, 0},
};

const int faces[NUMERO_TRIANGULOS][3] = {
    {0, 1, 2}, 
    {0, 2, 3}, 
    {0, 3, 4}, 
    {0, 4, 5}, 
    {0, 5, 1}, 
    {1, 6, 2}, 
    {2, 7, 3}, 
    {3, 8, 4}, 
    {4, 9, 5}, 
    {5, 10, 1}, 
    {6, 7, 2}, 
    {7, 8, 3}, 
    {8, 9, 4}, 
    {9, 10, 5}, 
    {10, 6, 1}, 
    {6, 11, 7}, 
    {7, 11, 8}, 
    {8, 11, 9}, 
    {9, 11, 10}, 
    {10, 11, 6}};

float rotacao_x = 30.0f;
float rotacao_y = 30.0f;
float ultima_posicao_mouse_x = -1;
float ultima_posicao_mouse_y = -1;

GLuint texturas[NUMERO_TRIANGULOS];

void carregar_textura(int indice, const char *nome_arquivo)
{
    int largura, altura, canais;
    unsigned char *dados = stbi_load(nome_arquivo, &largura, &altura, &canais, 0);

    if (dados)
    {
        glGenTextures(1, &texturas[indice]);
        glBindTexture(GL_TEXTURE_2D, texturas[indice]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, largura, altura, 0, GL_RGB, GL_UNSIGNED_BYTE, dados);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(dados);
    }
    else
    {
        printf("Erro ao carregar %s\n", nome_arquivo);
    }
}

void carregar_texturas()
{
    char nome_arquivo[10];
    int i;
    for (i = 0; i < NUMERO_TRIANGULOS; i++)
    {
        sprintf(nome_arquivo, "%d.jpg", i + 1);
        carregar_textura(i, nome_arquivo);
    }
}

void desenhar_triangulo(int indice_face)
{
    int i;
    float coordenada_textura_x, coordenada_textura_y;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturas[indice_face]);

    glBegin(GL_TRIANGLES);
    for (i = 0; i < 3; i++)
    {
        Vertice vertice = vertices[faces[indice_face][i]];
        coordenada_textura_x = (i == 0) ? 0.5f : (i == 1) ? 0.0f: 1.0f;                                               
        coordenada_textura_y = (i == 0) ? 1.0f : 0.0f;
        glTexCoord2f(coordenada_textura_x, coordenada_textura_y);
        glVertex3f(vertice.x, vertice.y, vertice.z);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void desenhar_d20()
{
    int i;
    for (i = 0; i < NUMERO_TRIANGULOS; i++)
    {
        desenhar_triangulo(i);
    }
}

void exibir()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, -3.0f);
    glRotatef(rotacao_x, 1.0f, 0.0f, 0.0f);
    glRotatef(rotacao_y, 0.0f, 1.0f, 0.0f);

    desenhar_d20();

    glutSwapBuffers();
}

void movimentar_mouse(int x, int y)
{
    if (ultima_posicao_mouse_x < 0 || ultima_posicao_mouse_y < 0)
    {
        ultima_posicao_mouse_x = x;
        ultima_posicao_mouse_y = y;
    }

    rotacao_x += (y - ultima_posicao_mouse_y) * 0.5f;
    rotacao_y += (x - ultima_posicao_mouse_x) * 0.5f;

    ultima_posicao_mouse_x = x;
    ultima_posicao_mouse_y = y;

    glutPostRedisplay();
}

void clique_mouse(int botao, int estado, int x, int y)
{
    if (estado == GLUT_UP)
    {
        ultima_posicao_mouse_x = -1;
        ultima_posicao_mouse_y = -1;
    }
}

void inicializar_opengl()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    carregar_texturas();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("D20");

    inicializar_opengl();

    glutDisplayFunc(exibir);
    glutMotionFunc(movimentar_mouse);
    glutMouseFunc(clique_mouse);

    glutMainLoop();
    return 0;
}
