#ifndef FILA_H
#define FILA_H

#include "jogador.h"
#include "selecao_personagens.h"

typedef struct NoPersonagem
{
    Jogador jogador;
    IndicePersonagem indicePersonagem;
    int vivo;
    struct NoPersonagem *next;
} NoPersonagem;

typedef struct
{
    NoPersonagem membros[TAM_EQUIPE];
    NoPersonagem *inicio;
    NoPersonagem *fim;
    int tamanho;
    int total;
} FilaPersonagens;

void inicializarFilaPersonagens(FilaPersonagens *fila);
int enfileirarPersonagem(FilaPersonagens *fila, IndicePersonagem indicePersonagem);
NoPersonagem *noPersonagemAtivo(FilaPersonagens *fila);
const NoPersonagem *noPersonagemAtivoConst(const FilaPersonagens *fila);
int indicePersonagemAtivo(const FilaPersonagens *fila);
int tamanhoFilaPersonagens(const FilaPersonagens *fila);
int filaPersonagensTemVivos(const FilaPersonagens *fila);
int filaPersonagensPodeRotacionar(const FilaPersonagens *fila);
void rotacionarFilaPersonagens(FilaPersonagens *fila);
void removerPersonagemAtivo(FilaPersonagens *fila);

#endif
