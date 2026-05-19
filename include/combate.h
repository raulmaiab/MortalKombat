#ifndef COMBATE_H
#define COMBATE_H

#include "jogador.h"

#define DISTANCIA_MAXIMA_ATAQUE 200.0f
#define STUN_TICKS_PADRAO 60
#define HITSTUN_WINDOW_TICKS 20
#define BLOCKSTUN_TICKS 12

/* Combo: sequência de 3 passinhos que ativa o golpe especial */
typedef struct
{
    TipoPassinho sequencia[TAM_MAX_FILA];
} Combo;

extern const int energiaConsumida[3];

/* ---- Funções de Combate ---- */
void processarPassinho(TipoPassinho passinho, Jogador *atacante, Jogador *alvo, Estatistica *stats, int tickAtual);
int verificarCombo(FilaPassinhos *fila, Combo *combosPersonagem, int totalCombos);
void atualizarEnergia(Jogador *jogador, int dano);
int verificarVencedor(Jogador *jogador1, Jogador *jogador2);
void encerrarRound(Jogador *vencedor, Estatistica *stats);

#endif
