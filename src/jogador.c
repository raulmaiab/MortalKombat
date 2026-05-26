#include "jogador.h"
#include <string.h>

/*
 * Retorna os atributos do personagem pelo índice.
 * Cada personagem tem HP, danos e velocidade únicos.
 */
Personagem getPersonagem(IndicePersonagem indice)
{
    Personagem personagens[TOTAL_PERSONAGENS] = {
        /* Nome              hpMax  dNormal dBaixo  dEsp  vel */
        {"Alirio", 90, 9, 16, 28, 8},
        {"Anderson Neiff", 100, 10, 18, 35, 6},
        {"Ariano Suassuna", 100, 9, 16, 32, 7},
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
    jogador->energia = 0;
    jogador->stunTicks = 0;
    jogador->ultimoGolpeTick = -20;
    jogador->golpesSeguidos = 0;
    jogador->roundsVencidos = 0;
    jogador->posX = posX;
    jogador->posY = posY;
    jogador->velY = 0.0f;
    jogador->noChao = 1;
    jogador->defendendo = 0;
    jogador->agachado = 0;
    jogador->ataqueAgachadoTicks = 0;
    jogador->esquivaTicks = 0;
    jogador->esquivaCooldown = 0;
    jogador->attackTicks = 0;
    jogador->stateTicks = 0;
    jogador->olhandoDireita = olhandoDireita;
    jogador->state = IDLE;
    inicializarFila(&jogador->fila);
}

/*
 * Reseta HP, energia e fila para o próximo round.
 * Mantém personagem escolhido e rounds vencidos.
 */
void resetarJogador(Jogador *jogador)
{
    jogador->hp = jogador->personagem.hpMaximo;
    jogador->energia = 0;
    jogador->stunTicks = 0;
    jogador->ultimoGolpeTick = -20;
    jogador->golpesSeguidos = 0;
    jogador->velY = 0.0f;
    jogador->noChao = 1;
    jogador->defendendo = 0;
    jogador->agachado = 0;
    jogador->ataqueAgachadoTicks = 0;
    jogador->esquivaTicks = 0;
    jogador->esquivaCooldown = 0;
    jogador->attackTicks = 0;
    jogador->stateTicks = 0;
    jogador->state = IDLE;
    limparFila(&jogador->fila);
}
