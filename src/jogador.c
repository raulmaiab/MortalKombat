#include "jogador.h"
#include <string.h>

/*
 * Retorna os atributos do personagem pelo índice.
 * Cada personagem tem HP, danos e velocidade únicos.
 */
Personagem getPersonagem(IndicePersonagem indice) {
    Personagem personagens[TOTAL_PERSONAGENS] = {
        /* Nome              hpMax  dLeve  dMedio  dEsp  vel */
        {"Joao Campos",       120,    8,     15,     30,   5 },
        {"Magrao",            110,    7,     13,     25,   4 },
        {"Kuki",               90,    9,     16,     28,   8 },
        {"Grafite",           100,   10,     18,     35,   6 },
        {"Clarisse Lispector", 95,    7,     14,     40,   5 },
        {"Ariano Suassuna",   105,    8,     15,     45,   4 },
        {"Tojal",             100,    9,     16,     32,   7 },
    };
    return personagens[indice];
}

/*
 * Inicializa o jogador com o personagem escolhido e posição na tela.
 * Chamada na seleção de personagem antes do round começar.
 */
void inicializarJogador(Jogador *jogador, IndicePersonagem indice, float posX, float posY, int olhandoDireita) {
    jogador->personagem      = getPersonagem(indice);
    jogador->hp              = jogador->personagem.hpMaximo;
    jogador->energia         = MAX_ENERGIA;
    jogador->stunTicks       = 0;
    jogador->ultimoGolpeTick = -20;
    jogador->golpesSeguidos  = 0;
    jogador->roundsVencidos  = 0;
    jogador->posX            = posX;
    jogador->posY            = posY;
    jogador->olhandoDireita  = olhandoDireita;
    inicializarFila(&jogador->fila);
}

/*
 * Reseta HP, energia e fila para o próximo round.
 * Mantém personagem escolhido e rounds vencidos.
 */
void resetarJogador(Jogador *jogador) {
    jogador->hp              = jogador->personagem.hpMaximo;
    jogador->energia         = MAX_ENERGIA;
    jogador->stunTicks       = 0;
    jogador->ultimoGolpeTick = -20;
    jogador->golpesSeguidos  = 0;
    limparFila(&jogador->fila);
}
