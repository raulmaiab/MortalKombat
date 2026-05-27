#include "jogador.h"

/*
 * Retorna os atributos do personagem pelo índice.
 * Cada personagem tem HP e nome próprios.
 */
Personagem getPersonagem(IndicePersonagem indice)
{
    Personagem personagens[TOTAL_PERSONAGENS] = {
        {"Diferenciado", 100},
        {"Alirio", 100},
        {"Adriano", 100},
    };
    return personagens[indice];
}

/*
 * Inicializa o jogador com o personagem escolhido e posição na tela.
 * Chamada na seleção de personagem antes do round começar.
 */
void inicializarJogador(Jogador *jogador, IndicePersonagem indice, float posX, float posY, int olhandoDireita)
{
    jogador->personagem = getPersonagem(indice);
    jogador->hp = jogador->personagem.hpMaximo;
    jogador->stunTicks = 0;
    jogador->posX = posX;
    jogador->posY = posY;
    jogador->velY = 0.0f;
    jogador->noChao = 1;
    jogador->defendendo = 0;
    jogador->agachado = 0;
    jogador->ataqueAgachadoTicks = 0;
    jogador->attackTicks = 0;
    jogador->specialAttackTicks = 0;
    jogador->ataquePendente = -1;
    jogador->ataquePendenteTicks = 0;
    jogador->ataquePendenteAplicado = 0;
    jogador->stateTicks = 0;
    jogador->olhandoDireita = olhandoDireita;
    jogador->state = IDLE;
}
