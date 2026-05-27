#ifndef JOGADOR_H
#define JOGADOR_H

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
    SPECIAL_ATTACK,
    STUN,
    KNOCKDOWN
} PlayerState;

/* Atributos únicos de cada personagem */
typedef struct
{
    char nome[MAX_NOME];
    int hpMaximo;
} Personagem;

/* Estado do jogador durante a partida */
typedef struct
{
    Personagem personagem;
    int hp;
    int stunTicks;
    float posX; /* posição na tela (Raylib) */
    float posY;
    float velY;
    int noChao;
    int defendendo;
    int agachado;
    int ataqueAgachadoTicks;
    int attackTicks;
    int specialAttackTicks;
    int ataquePendente;
    int ataquePendenteTicks;
    int ataquePendenteAplicado;
    int stateTicks;
    int olhandoDireita;
    PlayerState state;
} Jogador;

/* ---- Personagens disponíveis ---- */
/* Índices para seleção no menu */
typedef enum
{
    ANDERSON_NEIFF,
    ALIRIO,
    ARIANO,
    TOTAL_PERSONAGENS
} IndicePersonagem;

/* ---- Funções de Jogador ---- */
void inicializarJogador(Jogador *jogador, IndicePersonagem indice, float posX, float posY, int olhandoDireita);
Personagem getPersonagem(IndicePersonagem indice);

#endif
