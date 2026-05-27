#ifndef COMBATE_H
#define COMBATE_H

#include "jogador.h"

#define DISTANCIA_MAXIMA_ATAQUE 200.0f
#define STUN_TICKS_PADRAO 14
#define BLOCKSTUN_TICKS 10
#define RECUPERACAO_ATAQUE_TICKS 22
#define GANHO_ENERGIA_TEMPO 1
#define GANHO_ENERGIA_ACERTO 5
#define GANHO_ENERGIA_DEFESA 3
#define ENERGIA_ESPECIAL 100
#define DANO_NORMAL_PERCENTUAL 4
#define DANO_ESPECIAL_PERCENTUAL 40

typedef enum
{
    PASSINHO_NENHUM = -1,
    ATAQUE_NORMAL,
    ATAQUE_AGACHADO,
    ATAQUE_ESPECIAL
} TipoPassinho;

extern const int energiaConsumida[3];

/* ---- Funções de Combate ---- */
void processarPassinho(TipoPassinho passinho, Jogador *atacante, Jogador *alvo, int *energiaAtacante);
void adicionarEnergia(int *energia, int quantidade);

#endif
