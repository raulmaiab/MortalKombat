#include "fila.h"
#include <stdio.h>

/*
 * Inicializa a fila de passinnhos do jogador.
 * Deve ser chamada antes de qualquer operação na fila.
 */
void inicializarFila(FilaPassinhos *fila) {
    fila->inicio  = 0;
    fila->fim     = 0;
    fila->tamanho = 0;
}

/*
 * Enfileira um passinho na fila do jogador.
 * Chamada a cada IsKeyPressed() no game loop.
 * Não enfileira se a fila estiver cheia.
 */
void enfileirarPassinho(FilaPassinhos *fila, TipoPassinho passinho) {
    if (filaCheia(fila)) return;

    fila->elementos[fila->fim] = passinho;

    fila->fim = (fila->fim + 1) % TAM_MAX_FILA;
    
    fila->tamanho++;
}

/*
 * Remove e retorna o próximo passinho a ser processado (FIFO).
 * Chamada a cada tick de processamento no game loop.
 * Retorna -1 se a fila estiver vazia.
 */
TipoPassinho desenfileirarPassinho(FilaPassinhos *fila) {
    if (filaVazia(fila)) return -1;

    TipoPassinho passinho = fila->elementos[fila->inicio];
    fila->inicio = (fila->inicio + 1) % TAM_MAX_FILA;
    fila->tamanho--;
    return passinho;
}

/*
 * Retorna o próximo elemento da fila SEM removê-lo.
 * Útil para verificar esquiva antes de aplicar dano.
 */
TipoPassinho peekFila(FilaPassinhos *fila) {
    if (filaVazia(fila)) return -1;
    return fila->elementos[fila->inicio];
}

/*
 * Retorna 1 se a fila não possui elementos, 0 caso contrário.
 */
int filaVazia(FilaPassinhos *fila) {
    return fila->tamanho == 0;
}

/*
 * Retorna 1 se a fila atingiu o tamanho máximo (TAM_MAX_FILA).
 * Quando cheia, o sistema verifica se há combo antes de processar.
 */
int filaCheia(FilaPassinhos *fila) {
    return fila->tamanho == TAM_MAX_FILA;
}

/*
 * Limpa todos os elementos da fila.
 * Chamada no início de cada round ou ao tomar um combo.
 */
void limparFila(FilaPassinhos *fila) {
    fila->inicio  = 0;
    fila->fim     = 0;
    fila->tamanho = 0;
}