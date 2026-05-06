#ifndef JOGADOR_H
#define JOGADOR_H

#include "fila.h"

#define MAX_HP      100
#define MAX_ENERGIA 100
#define MAX_NOME    50

/* Atributos únicos de cada personagem */
typedef struct {
    char nome[MAX_NOME];
    int  hpMaximo;
    int  danoLeve;
    int  danomedio;
    int  danoEspecial;
    int  velocidade;    /* influencia o tick de processamento */
} Personagem;

/* Estado do jogador durante a partida */
typedef struct {
    Personagem    personagem;
    FilaPassinhos fila;         /* fila de inputs — ED central */
    int           hp;
    int           energia;
    int           roundsVencidos;
    float         posX;         /* posição na tela (Raylib) */
    float         posY;
    int           olhandoDireita;
} Jogador;

/* Estatísticas ao fim de cada round */
typedef struct {
    char nomePersonagem[MAX_NOME];
    int  danoTotal;
    int  combosExecutados;
    int  esquivasRealizadas;
} Estatistica;

/* ---- Personagens disponíveis ---- */
/* Índices para seleção no menu */
typedef enum {
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
void resetarJogador(Jogador *jogador);   /* reseta HP/energia/fila para novo round */
Personagem getPersonagem(IndicePersonagem indice);

#endif