#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define LARGURA_TELA 930
#define ALTURA_TELA 600
#define RAIO 200
#define TAMANHO_FONTE 40

typedef struct
{
    float x, y, z;
} Vertice;

// Vértices do icosaedro (D20)
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
    {0, -1, 0}};

// Faces do icosaedro
const int faces[20][3] = {
    {0, 1, 2}, {0, 2, 3}, {0, 3, 4}, {0, 4, 5}, {0, 5, 1}, {1, 6, 2}, {2, 7, 3}, {3, 8, 4}, {4, 9, 5}, {5, 10, 1}, {6, 7, 2}, {7, 8, 3}, {8, 9, 4}, {9, 10, 5}, {10, 6, 1}, {6, 11, 7}, {7, 11, 8}, {8, 11, 9}, {9, 11, 10}, {10, 11, 6}};

void projetar_vertice(Vertice vertice, int *x, int *y)
{
    *x = LARGURA_TELA / 2 + (int)(vertice.x * RAIO);
    *y = ALTURA_TELA / 2 - (int)(vertice.y * RAIO);
}

void rotacionar_vertice(Vertice *vertice, float angulo_x, float angulo_y, float angulo_z)
{
    float radianos_x, radianos_y, radianos_z;
    float cos_x, sin_x, cos_y, sin_y, cos_z, sin_z;
    float y_temp, z_temp, x_temp, y_final;

    radianos_x = angulo_x * M_PI / 180.0;
    radianos_y = angulo_y * M_PI / 180.0;
    radianos_z = angulo_z * M_PI / 180.0;

    cos_x = cos(radianos_x);
    sin_x = sin(radianos_x);
    cos_y = cos(radianos_y);
    sin_y = sin(radianos_y);
    cos_z = cos(radianos_z);
    sin_z = sin(radianos_z);

    // Rotaciona no eixo X
    y_temp = vertice->y * cos_x - vertice->z * sin_x;
    z_temp = vertice->y * sin_x + vertice->z * cos_x;

    // Rotaciona no eixo Y
    x_temp = vertice->x * cos_y + z_temp * sin_y;
    z_temp = -vertice->x * sin_y + z_temp * cos_y;

    // Rotaciona no eixo Z
    vertice->x = x_temp * cos_z - y_temp * sin_z;
    y_final = x_temp * sin_z + y_temp * cos_z;

    vertice->y = y_final;
    vertice->z = z_temp;
}

Vertice calcular_normal(Vertice v1, Vertice v2, Vertice v3)
{
    Vertice normal;

    float ux = v2.x - v1.x;
    float uy = v2.y - v1.y;
    float uz = v2.z - v1.z;

    float vx = v3.x - v1.x;
    float vy = v3.y - v1.y;
    float vz = v3.z - v1.z;

    normal.x = uy * vz - uz * vy;
    normal.y = uz * vx - ux * vz;
    normal.z = ux * vy - uy * vx;

    return normal;
}

int verificar_face_visivel(Vertice normal, Vertice camera)
{
    return (normal.x * camera.x + normal.y * camera.y + normal.z * camera.z) < 0;
}

void desenhar_texto(SDL_Renderer *renderizador, TTF_Font *fonte, const char *texto, int x, int y, SDL_Color cor)
{
    SDL_Surface *superficie;
    SDL_Texture *textura;
    SDL_Rect retangulo;

    superficie = TTF_RenderText_Solid(fonte, texto, cor);
    textura = SDL_CreateTextureFromSurface(renderizador, superficie);

    retangulo.x = x - superficie->w / 2;
    retangulo.y = y - superficie->h / 2;
    retangulo.w = superficie->w;
    retangulo.h = superficie->h;

    SDL_RenderCopy(renderizador, textura, NULL, &retangulo);
    SDL_FreeSurface(superficie);
    SDL_DestroyTexture(textura);
}

void desenhar_triangulo(SDL_Renderer *renderizador, Vertice v1, Vertice v2, Vertice v3, SDL_Color cor)
{
    int x1, y1, x2, y2, x3, y3;

    projetar_vertice(v1, &x1, &y1);
    projetar_vertice(v2, &x2, &y2);
    projetar_vertice(v3, &x3, &y3);

    SDL_SetRenderDrawColor(renderizador, cor.r, cor.g, cor.b, 255);
    SDL_RenderDrawLine(renderizador, x1, y1, x2, y2);
    SDL_RenderDrawLine(renderizador, x2, y2, x3, y3);
    SDL_RenderDrawLine(renderizador, x3, y3, x1, y1);
}

void desenhar_faces(SDL_Renderer *renderizador, Vertice *vertices_rotacionados, SDL_Color cor_normal, Vertice camera)
{
    int i;

    for (i = 0; i < 20; i++)
    {
        Vertice v1 = vertices_rotacionados[faces[i][0]];
        Vertice v2 = vertices_rotacionados[faces[i][1]];
        Vertice v3 = vertices_rotacionados[faces[i][2]];

        Vertice normal = calcular_normal(v1, v2, v3);

        if (verificar_face_visivel(normal, camera))
        {
            desenhar_triangulo(renderizador, v1, v2, v3, cor_normal);
        }
    }
}

void rotacionar_vertices(Vertice *vertices_rotacionados, float angulo_x, float angulo_y, float angulo_z)
{
    int i;
    for (i = 0; i < 12; i++)
    {
        vertices_rotacionados[i] = vertices[i];
        rotacionar_vertice(&vertices_rotacionados[i], angulo_x, angulo_y, angulo_z);
    }
}

void exibir_texto(SDL_Renderer *renderizador, TTF_Font *fonte, int numero_sorteado, SDL_Color cor_texto)
{
    char texto[3];
    snprintf(texto, sizeof(texto), "%d", numero_sorteado);
    desenhar_texto(renderizador, fonte, texto, LARGURA_TELA / 2 + 9, ALTURA_TELA / 2 + 20, cor_texto);

    if (numero_sorteado == 1)
    {
        desenhar_texto(renderizador, fonte, "Falha Critica!", LARGURA_TELA / 2, ALTURA_TELA / 2 + 220, cor_texto);
    }
    else if (numero_sorteado == 20)
    {
        desenhar_texto(renderizador, fonte, "20 NATURAL!!!", LARGURA_TELA / 2, ALTURA_TELA / 2 + 220, cor_texto);
    }
}

void desenhar_d20(SDL_Renderer *renderizador, TTF_Font *fonte, int numero_sorteado)
{
    SDL_Color cor_normal = {0, 0, 255, 255};
    SDL_Color cor_texto = {255, 255, 255, 255};

    float angulo_x = 0.0;
    float angulo_y = 88.0;
    float angulo_z = 0.0;

    Vertice vertices_rotacionados[12];
    Vertice camera = {0, 0, -1};

    rotacionar_vertices(vertices_rotacionados, angulo_x, angulo_y, angulo_z);
    desenhar_faces(renderizador, vertices_rotacionados, cor_normal, camera);
    exibir_texto(renderizador, fonte, numero_sorteado, cor_texto);

    SDL_RenderPresent(renderizador);
}

void inicializar_sdl()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        exit(1);
    }

    if (TTF_Init() == -1)
    {
        printf("Erro ao inicializar SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }
}

SDL_Window *criar_janela()
{
    SDL_Window *janela = SDL_CreateWindow(
        "D20",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        LARGURA_TELA,
        ALTURA_TELA,
        0);

    if (!janela)
    {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }

    return janela;
}

SDL_Renderer *criar_renderizador(SDL_Window *janela)
{
    SDL_Renderer *renderizador = SDL_CreateRenderer(janela, -1, SDL_RENDERER_ACCELERATED);
    if (!renderizador)
    {
        printf("Erro ao criar renderizador: %s\n", SDL_GetError());
        SDL_DestroyWindow(janela);
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }

    return renderizador;
}

TTF_Font *carregar_fonte()
{
    TTF_Font *fonte = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", TAMANHO_FONTE);
    if (!fonte)
    {
        printf("Erro ao carregar fonte: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }

    return fonte;
}

void finalizar(SDL_Window *janela, SDL_Renderer *renderizador, TTF_Font *fonte)
{
    TTF_CloseFont(fonte);
    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(janela);
    TTF_Quit();
    SDL_Quit();
}

void loop_principal(SDL_Renderer *renderizador, TTF_Font *fonte)
{
    int em_execucao = 1;
    SDL_Event evento;
    int numero_sorteado = 20;

    srand(time(NULL));

    SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
    SDL_RenderClear(renderizador);
    desenhar_d20(renderizador, fonte, numero_sorteado);

    while (em_execucao)
    {
        while (SDL_PollEvent(&evento))
        {
            if (evento.type == SDL_QUIT)
            {
                em_execucao = 0;
            }
            else if (evento.type == SDL_KEYDOWN && evento.key.keysym.sym == SDLK_SPACE)
            {
                numero_sorteado = rand() % 20 + 1;
                SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
                SDL_RenderClear(renderizador);
                desenhar_d20(renderizador, fonte, numero_sorteado);
                printf("Resultado: %d\n", numero_sorteado);
            }
        }
    }
}

int main()
{
    inicializar_sdl();

    SDL_Window *janela = criar_janela();
    SDL_Renderer *renderizador = criar_renderizador(janela);
    TTF_Font *fonte = carregar_fonte();

    loop_principal(renderizador, fonte);

    finalizar(janela, renderizador, fonte);

    return 0;
}
