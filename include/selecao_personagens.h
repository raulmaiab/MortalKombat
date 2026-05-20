#ifndef SELECAO_PERSONAGENS_H
#define SELECAO_PERSONAGENS_H

#include "jogador.h"

#define TAM_EQUIPE 3

typedef struct
{
    int personagens[TAM_EQUIPE];
    int slotAtual;
    int confirmouTudo;
} SelecaoPersonagens;

void inicializarSelecaoPersonagens(SelecaoPersonagens *selecao, IndicePersonagem personagemInicial);
void moverSelecaoPersonagens(SelecaoPersonagens *selecao, int direcao);
void confirmarSelecaoPersonagens(SelecaoPersonagens *selecao);

#endif
