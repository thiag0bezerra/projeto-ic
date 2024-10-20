
#define Font XlibFont  // Renomeia o 'Font' do Xlib para evitar conflito

#include <X11/Xlib.h>  // Inclui o Xlib

#undef Font  // Remove a redefinição para evitar afetar o restante do código

#include <raylib.h>  // Inclui a raylib sem conflitos

#include <math.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Definições
//----------------------------------------------------------------------------------
#define PLAYER_BASE_SIZE    20.0f
#define PLAYER_SPEED        6.0f
#define PLAYER_MAX_SHOOTS   10

#define METEORS_SPEED       6
#define MAX_BIG_METEORS     4
#define MAX_MEDIUM_METEORS  8
#define MAX_SMALL_METEORS   16

//----------------------------------------------------------------------------------
// Estruturas de definição
//----------------------------------------------------------------------------------
typedef struct Player {
    Vector2 position;
    Vector2 speed;
    float acceleration;
    float rotation;
    Vector3 collider;
    Color color;
} Player;

typedef struct Shoot {
    Vector2 position;
    Vector2 speed;
    float radius;
    float rotation;
    int lifeSpawn;
    bool active;
    Color color;
} Shoot;

typedef struct Meteor {
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
    Color color;
} Meteor;

//------------------------------------------------------------------------------------
// Declaração de variáveis globais
//------------------------------------------------------------------------------------
static int screenWidth;
static int screenHeight;
Texture2D backgroundTexture;

int screen(void) {
    Display* display = XOpenDisplay(NULL);
    int screen = DefaultScreen(display);
    screenWidth = DisplayWidth(display, screen);   // Atribui o valor à variável global
    screenHeight = DisplayHeight(display, screen); // Atribui o valor à variável global
    
    XCloseDisplay(display);  // Fecha o display após o uso
    return 0;
}

static bool gameOver = false;
static bool pause = false;
static bool victory = false;

// Define um triangulo de 70 graus para ser a nave
static float shipHeight = 0.0f;

static Player player = { 0 };
static Shoot shoot[PLAYER_MAX_SHOOTS] = { 0 };
static Meteor bigMeteor[MAX_BIG_METEORS] = { 0 };
static Meteor mediumMeteor[MAX_MEDIUM_METEORS] = { 0 };
static Meteor smallMeteor[MAX_SMALL_METEORS] = { 0 };

static int midMeteorsCount = 0;
static int smallMeteorsCount = 0;
static int destroyedMeteorsCount = 0;

//------------------------------------------------------------------------------------
// Declaração dos modulos
//------------------------------------------------------------------------------------
static void InitGame(void);         // Inicializa o jogo
static void UpdateGame(void);       // Atualiza o jogo a cada frame
static void DrawGame(void);         // Desenha o jogo a cada frame
static void UnloadGame(void);       // Descarrega o jogo
static void UpdateDrawFrame(void);  // Atualiza e desenha a cada frame

//------------------------------------------------------------------------------------
// função principal
//------------------------------------------------------------------------------------
int main(void)
{
    screen();
    // Inicialização da janela
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "classic game: asteroid");

    InitGame();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Loop principal do jogo
    while (!WindowShouldClose())    // Detecta se o 'Esc' foi clicado para sair do jogo
    {
        // Atualiza e desenha
        //----------------------------------------------------------------------------------
        UpdateDrawFrame();
        //----------------------------------------------------------------------------------
    }
#endif
    
    //--------------------------------------------------------------------------------------
    UnloadGame();         // Descarregamento das informações de sons e texturas

    CloseWindow();        // Fecha a janela
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Definição dos modulos locais
//------------------------------------------------------------------------------------

// Inicializar as variáveis do jogo
void InitGame(void)
{
    int posx, posy;
    int velx, vely;
    bool correctRange = false;
    victory = false;
    pause = false;

    backgroundTexture = LoadTexture("assets/universe.png");

    shipHeight = (PLAYER_BASE_SIZE/2)/tanf(20*DEG2RAD);

    // Inicialização do jogador
    player.position = (Vector2){screenWidth/2, screenHeight/2 - shipHeight/2};
    player.speed = (Vector2){0, 0};
    player.acceleration = 0;
    player.rotation = 0;
    player.collider = (Vector3){player.position.x + sin(player.rotation*DEG2RAD)*(shipHeight/2.5f), player.position.y - cos(player.rotation*DEG2RAD)*(shipHeight/2.5f), 12};
    player.color = LIGHTGRAY;

    destroyedMeteorsCount = 0;

    // Inicialização do tiro
    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
    {
        shoot[i].position = (Vector2){0, 0};
        shoot[i].speed = (Vector2){0, 0};
        shoot[i].radius = 2;
        shoot[i].active = false;
        shoot[i].lifeSpawn = 0;
        shoot[i].color = WHITE;
    }

    for (int i = 0; i < MAX_BIG_METEORS; i++)
    {
        posx = GetRandomValue(0, screenWidth);

        while (!correctRange)
        {
            if (posx > screenWidth/2 - 150 && posx < screenWidth/2 + 150) posx = GetRandomValue(0, screenWidth);
            else correctRange = true;
        }

        correctRange = false;

        posy = GetRandomValue(0, screenHeight);

        while (!correctRange)
        {
            if (posy > screenHeight/2 - 150 && posy < screenHeight/2 + 150)  posy = GetRandomValue(0, screenHeight);
            else correctRange = true;
        }

        bigMeteor[i].position = (Vector2){posx, posy};

        correctRange = false;
        velx = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);
        vely = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);

        while (!correctRange)
        {
            if (velx == 0 && vely == 0)
            {
                velx = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);
                vely = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);
            }
            else correctRange = true;
        }

        bigMeteor[i].speed = (Vector2){velx, vely};
        bigMeteor[i].radius = 40;
        bigMeteor[i].active = true;
        bigMeteor[i].color = BLUE;
    }

    for (int i = 0; i < MAX_MEDIUM_METEORS; i++)
    {
        mediumMeteor[i].position = (Vector2){-100, -100};
        mediumMeteor[i].speed = (Vector2){0,0};
        mediumMeteor[i].radius = 20;
        mediumMeteor[i].active = false;
        mediumMeteor[i].color = BLUE;
    }

    for (int i = 0; i < MAX_SMALL_METEORS; i++)
    {
        smallMeteor[i].position = (Vector2){-100, -100};
        smallMeteor[i].speed = (Vector2){0,0};
        smallMeteor[i].radius = 10;
        smallMeteor[i].active = false;
        smallMeteor[i].color = BLUE;
    }

    midMeteorsCount = 0;
    smallMeteorsCount = 0;
}

// Atualização do jogo
void UpdateGame(void)
{
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            // Logica do movimento do jogador
            if (IsKeyDown(KEY_LEFT)) player.rotation -= 5;
            if (IsKeyDown(KEY_RIGHT)) player.rotation += 5;

            // Logica da velocidade do jogador
            player.speed.x = sin(player.rotation*DEG2RAD)*PLAYER_SPEED;
            player.speed.y = cos(player.rotation*DEG2RAD)*PLAYER_SPEED;

            // Aceleração do jogador
            if (IsKeyDown(KEY_UP))
            {
                if (player.acceleration < 1) player.acceleration += 0.04f;
            }
            else
            {
                if (player.acceleration > 0) player.acceleration -= 0.02f;
                else if (player.acceleration < 0) player.acceleration = 0;
            }
            if (IsKeyDown(KEY_DOWN))
            {
                if (player.acceleration > 0) player.acceleration -= 0.04f;
                else if (player.acceleration < 0) player.acceleration = 0;
            }

            // Movimentação
            player.position.x += (player.speed.x*player.acceleration);
            player.position.y -= (player.speed.y*player.acceleration);

            // Colisão
            if (player.position.x > screenWidth + shipHeight) player.position.x = -(shipHeight);
            else if (player.position.x < -(shipHeight)) player.position.x = screenWidth + shipHeight;
            if (player.position.y > (screenHeight + shipHeight)) player.position.y = -(shipHeight);
            else if (player.position.y < -(shipHeight)) player.position.y = screenHeight + shipHeight;

            // Tiro
            if (IsKeyPressed(KEY_SPACE))
            {
                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if (!shoot[i].active)
                    {
                        shoot[i].position = (Vector2){ player.position.x + sin(player.rotation*DEG2RAD)*(shipHeight), player.position.y - cos(player.rotation*DEG2RAD)*(shipHeight) };
                        shoot[i].active = true;
                        shoot[i].speed.x = 1.5*sin(player.rotation*DEG2RAD)*PLAYER_SPEED;
                        shoot[i].speed.y = 1.5*cos(player.rotation*DEG2RAD)*PLAYER_SPEED;
                        shoot[i].rotation = player.rotation;
                        break;
                    }
                }
            }

            // Tempo de vida do tiro
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].active) shoot[i].lifeSpawn++;
            }

            // logica do tiro
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].active)
                {
                    // Movimento
                    shoot[i].position.x += shoot[i].speed.x;
                    shoot[i].position.y -= shoot[i].speed.y;

                    // Colisão do tiro
                    if  (shoot[i].position.x > screenWidth + shoot[i].radius)
                    {
                        shoot[i].active = false;
                        shoot[i].lifeSpawn = 0;
                    }
                    else if (shoot[i].position.x < 0 - shoot[i].radius)
                    {
                        shoot[i].active = false;
                        shoot[i].lifeSpawn = 0;
                    }
                    if (shoot[i].position.y > screenHeight + shoot[i].radius)
                    {
                        shoot[i].active = false;
                        shoot[i].lifeSpawn = 0;
                    }
                    else if (shoot[i].position.y < 0 - shoot[i].radius)
                    {
                        shoot[i].active = false;
                        shoot[i].lifeSpawn = 0;
                    }

                    // Vida do tiro
                    if (shoot[i].lifeSpawn >= 60)
                    {
                        shoot[i].position = (Vector2){0, 0};
                        shoot[i].speed = (Vector2){0, 0};
                        shoot[i].lifeSpawn = 0;
                        shoot[i].active = false;
                    }
                }
            }

            // Colisão com meteoros
            player.collider = (Vector3){player.position.x + sin(player.rotation*DEG2RAD)*(shipHeight/2.5f), player.position.y - cos(player.rotation*DEG2RAD)*(shipHeight/2.5f), 12};

            for (int a = 0; a < MAX_BIG_METEORS; a++)
            {
                if (CheckCollisionCircles((Vector2){player.collider.x, player.collider.y}, player.collider.z, bigMeteor[a].position, bigMeteor[a].radius) && bigMeteor[a].active) gameOver = true;
            }

            for (int a = 0; a < MAX_MEDIUM_METEORS; a++)
            {
                if (CheckCollisionCircles((Vector2){player.collider.x, player.collider.y}, player.collider.z, mediumMeteor[a].position, mediumMeteor[a].radius) && mediumMeteor[a].active) gameOver = true;
            }

            for (int a = 0; a < MAX_SMALL_METEORS; a++)
            {
                if (CheckCollisionCircles((Vector2){player.collider.x, player.collider.y}, player.collider.z, smallMeteor[a].position, smallMeteor[a].radius) && smallMeteor[a].active) gameOver = true;
            }

            // Meteoros grandes
            for (int i = 0; i < MAX_BIG_METEORS; i++)
            {
                if (bigMeteor[i].active)
                {
                    // Movimento
                    bigMeteor[i].position.x += bigMeteor[i].speed.x;
                    bigMeteor[i].position.y += bigMeteor[i].speed.y;

                    // Colisão meteoro e parede
                    if  (bigMeteor[i].position.x > screenWidth + bigMeteor[i].radius) bigMeteor[i].position.x = -(bigMeteor[i].radius);
                    else if (bigMeteor[i].position.x < 0 - bigMeteor[i].radius) bigMeteor[i].position.x = screenWidth + bigMeteor[i].radius;
                    if (bigMeteor[i].position.y > screenHeight + bigMeteor[i].radius) bigMeteor[i].position.y = -(bigMeteor[i].radius);
                    else if (bigMeteor[i].position.y < 0 - bigMeteor[i].radius) bigMeteor[i].position.y = screenHeight + bigMeteor[i].radius;
                }
            }

            // Meteoro medio
            for (int i = 0; i < MAX_MEDIUM_METEORS; i++)
            {
                if (mediumMeteor[i].active)
                {
                    // Movimento
                    mediumMeteor[i].position.x += mediumMeteor[i].speed.x;
                    mediumMeteor[i].position.y += mediumMeteor[i].speed.y;

                    // Colisão meteoro e parede
                    if  (mediumMeteor[i].position.x > screenWidth + mediumMeteor[i].radius) mediumMeteor[i].position.x = -(mediumMeteor[i].radius);
                    else if (mediumMeteor[i].position.x < 0 - mediumMeteor[i].radius) mediumMeteor[i].position.x = screenWidth + mediumMeteor[i].radius;
                    if (mediumMeteor[i].position.y > screenHeight + mediumMeteor[i].radius) mediumMeteor[i].position.y = -(mediumMeteor[i].radius);
                    else if (mediumMeteor[i].position.y < 0 - mediumMeteor[i].radius) mediumMeteor[i].position.y = screenHeight + mediumMeteor[i].radius;
                }
            }

            // Meteoro pequeno
            for (int i = 0; i < MAX_SMALL_METEORS; i++)
            {
                if (smallMeteor[i].active)
                {
                    // Movimento
                    smallMeteor[i].position.x += smallMeteor[i].speed.x;
                    smallMeteor[i].position.y += smallMeteor[i].speed.y;

                    // Meteoro e parede
                    if  (smallMeteor[i].position.x > screenWidth + smallMeteor[i].radius) smallMeteor[i].position.x = -(smallMeteor[i].radius);
                    else if (smallMeteor[i].position.x < 0 - smallMeteor[i].radius) smallMeteor[i].position.x = screenWidth + smallMeteor[i].radius;
                    if (smallMeteor[i].position.y > screenHeight + smallMeteor[i].radius) smallMeteor[i].position.y = -(smallMeteor[i].radius);
                    else if (smallMeteor[i].position.y < 0 - smallMeteor[i].radius) smallMeteor[i].position.y = screenHeight + smallMeteor[i].radius;
                }
            }

            // Colisão tiro meteoro
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if ((shoot[i].active))
                {
                    for (int a = 0; a < MAX_BIG_METEORS; a++)
                    {
                        if (bigMeteor[a].active && CheckCollisionCircles(shoot[i].position, shoot[i].radius, bigMeteor[a].position, bigMeteor[a].radius))
                        {
                            shoot[i].active = false;
                            shoot[i].lifeSpawn = 0;
                            bigMeteor[a].active = false;
                            destroyedMeteorsCount++;

                            for (int j = 0; j < 2; j ++)
                            {
                                if (midMeteorsCount%2 == 0)
                                {
                                    mediumMeteor[midMeteorsCount].position = (Vector2){bigMeteor[a].position.x, bigMeteor[a].position.y};
                                    mediumMeteor[midMeteorsCount].speed = (Vector2){cos(shoot[i].rotation*DEG2RAD)*METEORS_SPEED*-1, sin(shoot[i].rotation*DEG2RAD)*METEORS_SPEED*-1};
                                }
                                else
                                {
                                    mediumMeteor[midMeteorsCount].position = (Vector2){bigMeteor[a].position.x, bigMeteor[a].position.y};
                                    mediumMeteor[midMeteorsCount].speed = (Vector2){cos(shoot[i].rotation*DEG2RAD)*METEORS_SPEED, sin(shoot[i].rotation*DEG2RAD)*METEORS_SPEED};
                                }

                                mediumMeteor[midMeteorsCount].active = true;
                                midMeteorsCount ++;
                            }
                            //bigMeteor[a].position = (Vector2){-100, -100};
                            bigMeteor[a].color = RED;
                            a = MAX_BIG_METEORS;
                        }
                    }

                    for (int b = 0; b < MAX_MEDIUM_METEORS; b++)
                    {
                        if (mediumMeteor[b].active && CheckCollisionCircles(shoot[i].position, shoot[i].radius, mediumMeteor[b].position, mediumMeteor[b].radius))
                        {
                            shoot[i].active = false;
                            shoot[i].lifeSpawn = 0;
                            mediumMeteor[b].active = false;
                            destroyedMeteorsCount++;

                            for (int j = 0; j < 2; j ++)
                            {
                                 if (smallMeteorsCount%2 == 0)
                                {
                                    smallMeteor[smallMeteorsCount].position = (Vector2){mediumMeteor[b].position.x, mediumMeteor[b].position.y};
                                    smallMeteor[smallMeteorsCount].speed = (Vector2){cos(shoot[i].rotation*DEG2RAD)*METEORS_SPEED*-1, sin(shoot[i].rotation*DEG2RAD)*METEORS_SPEED*-1};
                                }
                                else
                                {
                                    smallMeteor[smallMeteorsCount].position = (Vector2){mediumMeteor[b].position.x, mediumMeteor[b].position.y};
                                    smallMeteor[smallMeteorsCount].speed = (Vector2){cos(shoot[i].rotation*DEG2RAD)*METEORS_SPEED, sin(shoot[i].rotation*DEG2RAD)*METEORS_SPEED};
                                }

                                smallMeteor[smallMeteorsCount].active = true;
                                smallMeteorsCount ++;
                            }
                            //mediumMeteor[b].position = (Vector2){-100, -100};
                            mediumMeteor[b].color = GREEN;
                            b = MAX_MEDIUM_METEORS;
                        }
                    }

                    for (int c = 0; c < MAX_SMALL_METEORS; c++)
                    {
                        if (smallMeteor[c].active && CheckCollisionCircles(shoot[i].position, shoot[i].radius, smallMeteor[c].position, smallMeteor[c].radius))
                        {
                            shoot[i].active = false;
                            shoot[i].lifeSpawn = 0;
                            smallMeteor[c].active = false;
                            destroyedMeteorsCount++;
                            smallMeteor[c].color = YELLOW;
                           // smallMeteor[c].position = (Vector2){-100, -100};
                            c = MAX_SMALL_METEORS;
                        }
                    }
                }
            }
        }

        if (destroyedMeteorsCount == MAX_BIG_METEORS + MAX_MEDIUM_METEORS + MAX_SMALL_METEORS) victory = true;
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

// Desenhar jogo
void DrawGame(void)
{
    BeginDrawing();

        ClearBackground(RAYWHITE);
        // Define a área de origem e destino da imagem
        Rectangle source = { 0.0f, 0.0f, (float)backgroundTexture.width, (float)backgroundTexture.height };
        Rectangle dest = { 0.0f, 0.0f, (float)screenWidth, (float)screenHeight };
        Vector2 origin = { 0.0f, 0.0f };

        // Desenha a imagem redimensionada para ocupar a tela inteira    
        DrawTexturePro(backgroundTexture, source, dest, origin, 0.0f, WHITE);

        if (!gameOver)
        {
            // Desenha a nave
            Vector2 v1 = { player.position.x + sinf(player.rotation*DEG2RAD)*(shipHeight), player.position.y - cosf(player.rotation*DEG2RAD)*(shipHeight) };
            Vector2 v2 = { player.position.x - cosf(player.rotation*DEG2RAD)*(PLAYER_BASE_SIZE/2), player.position.y - sinf(player.rotation*DEG2RAD)*(PLAYER_BASE_SIZE/2) };
            Vector2 v3 = { player.position.x + cosf(player.rotation*DEG2RAD)*(PLAYER_BASE_SIZE/2), player.position.y + sinf(player.rotation*DEG2RAD)*(PLAYER_BASE_SIZE/2) };
            DrawTriangle(v1, v2, v3, MAROON);

            // Desenha os meteoros
            for (int i = 0; i < MAX_BIG_METEORS; i++)
            {
                if (bigMeteor[i].active){
                    Image image = LoadImage("assets/bigMeteor.png");     // Carregado na RAM
                    Texture2D texture = LoadTextureFromImage(image);          // Convertido em textura (VRAM)
                    UnloadImage(image);   // Descarregar da RAM
                    DrawTexture(texture, bigMeteor[i].position.x - 45, bigMeteor[i].position.y - 45, WHITE);

                } 
                else DrawCircleV(bigMeteor[i].position, bigMeteor[i].radius, Fade(LIGHTGRAY, 0.3f));
            }

            for (int i = 0; i < MAX_MEDIUM_METEORS; i++)
            {
                if (mediumMeteor[i].active){
                    Image image = LoadImage("assets/mediumMeteor.png");     // Carregado na RAM
                    Texture2D texture = LoadTextureFromImage(image);          // Convertido em textura (VRAM)
                    UnloadImage(image);   // Descarregar da RAM
                    DrawTexture(texture, mediumMeteor[i].position.x - 20, mediumMeteor[i].position.y - 20, WHITE);
                } 
                else DrawCircleV(mediumMeteor[i].position, mediumMeteor[i].radius, Fade(LIGHTGRAY, 0.3f));
            }

            for (int i = 0; i < MAX_SMALL_METEORS; i++)
            {
                if (smallMeteor[i].active){
                    Image image = LoadImage("assets/smallMeteor.png");     // Carregado na RAM
                    Texture2D texture = LoadTextureFromImage(image);          // Convertido em textura (VRAM)
                    UnloadImage(image);   // Descarregar da RAM
                    DrawTexture(texture, smallMeteor[i].position.x - 10, smallMeteor[i].position.y - 10, WHITE);
                } 
                else DrawCircleV(smallMeteor[i].position, smallMeteor[i].radius, Fade(LIGHTGRAY, 0.3f));
            }

            // Desenhar tiro
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].active) DrawCircleV(shoot[i].position, shoot[i].radius, WHITE);
            }

            if (victory) DrawText("VICTORY", screenWidth/2 - MeasureText("VICTORY", 20)/2, screenHeight/2, 20, LIGHTGRAY);

            if (pause) DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, GRAY);
        }
        else{
            DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
        }    
                
    EndDrawing();
}

// Descarregar variaveis
void UnloadGame(void)
{
    UnloadTexture(backgroundTexture);
}

// Atualizar os desenhos do jogo
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}