#ifndef COMBATE_H
#define COMBATE_H

#include "jogador.h"

/* Combo: sequência de 3 passinhos que ativa o golpe especial */
typedef struct {
    TipoPassinho sequencia[TAM_MAX_FILA];
} Combo;

/* ---- Funções de Combate ---- */
void processarPassinho(TipoPassinho passinho, Jogador *atacante, Jogador *alvo, Estatistica *stats);
int  verificarCombo(FilaPassinhos *fila, Combo *combosPersonagem, int totalCombos);
void atualizarEnergia(Jogador *jogador, int dano);
int  verificarVencedor(Jogador *jogador1, Jogador *jogador2);
void encerrarRound(Jogador *vencedor, Estatistica *stats);

#endif