#ifndef JOGADOR_H
#define JOGADOR_H

#define MAX_HP 100
#define MAX_ENERGIA 100
#define MAX_NOME 50
#define ALTURA_PERSONAGEM 100
#define LARGURA_PERSONAGEM 60

typedef enum PlayerState
{
    IDLE,
    WALK,
    JUMP,
    CROUCH,
    DEFENSE,
    ATTACK,
    STUN,
    KNOCKDOWN
} PlayerState;

/* Atributos únicos de cada personagem */
typedef struct
{
    char nome[MAX_NOME];
    int hpMaximo;
    int danoNormal;
    int danoEspecial;
    int velocidade; /* influencia o tick de processamento */
} Personagem;

/* Estado do jogador durante a partida */
typedef struct
{
    Personagem personagem;
    int hp;
    int energia;
    int stunTicks;
    int ultimoGolpeTick; /* último tick em que o jogador levou dano */
    int golpesSeguidos;  /* contador de hits rápidos para hitstun */
    int roundsVencidos;
    float posX; /* posição na tela (Raylib) */
    float posY;
    float velY;
    int noChao;
    int defendendo;
    int agachado;
    int ataqueAgachadoTicks;
    int attackTicks;
    int stateTicks;
    int olhandoDireita;
    PlayerState state;
} Jogador;

/* ---- Personagens disponíveis ---- */
/* Índices para seleção no menu */
typedef enum
{
    JOAO_CAMPOS,
    MAGRAO,
    KUKI,
    GRAFITE,
    CLARISSE,
    ARIANO,
    TOJAL,
    TOTAL_PERSONAGENS
} IndicePersonagem;

/* ---- Funções de Jogador ---- */
void inicializarJogador(Jogador *jogador, IndicePersonagem indice, float posX, float posY, int olhandoDireita);
void resetarJogador(Jogador *jogador);
Personagem getPersonagem(IndicePersonagem indice);

#endif
