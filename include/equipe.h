#ifndef EQUIPE_H
#define EQUIPE_H

#include "jogador.h"
#include "selecao_personagens.h"

typedef struct NoPersonagem
{
    Jogador jogador;
    int indicePersonagem;
    int vivo;
    struct NoPersonagem *next;
} NoPersonagem;

typedef struct
{
    NoPersonagem membros[TAM_EQUIPE];
    NoPersonagem *inicio;
    NoPersonagem *fim;
    int tamanho;
    int roundsVencidos;
} EquipeJogador;

void inicializarEquipe(EquipeJogador *equipe, const int selecoes[], float posX, float posY, int olhandoDireita);
Jogador *jogadorAtivo(EquipeJogador *equipe);
const Jogador *jogadorAtivoConst(const EquipeJogador *equipe);
int indiceAtivoEquipe(const EquipeJogador *equipe);
int equipeTemVivos(const EquipeJogador *equipe);
int equipePodeTrocar(const EquipeJogador *equipe);
void trocarParaProximoPersonagem(EquipeJogador *equipe);
void trocarSeAtivoMorreu(EquipeJogador *equipe);
void resetarEquipeParaNovoRound(EquipeJogador *equipe, float posX, float posY, int olhandoDireita);

#endif
