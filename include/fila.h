#ifndef FILA_H
#define FILA_H

#define TAM_MAX_FILA 3

/* Tipos de passinho/ação que o jogador pode executar */
typedef enum {
    ATAQUE_LEVE,
    ATAQUE_MEDIO,
    ATAQUE_ESPECIAL,
    ESQUIVA
} TipoPassinho;

/* Fila circular de inputs do jogador (ED central do jogo) */
typedef struct {
    TipoPassinho elementos[TAM_MAX_FILA];
    int inicio;
    int fim;
    int tamanho;
} FilaPassinhos;

/* ---- Funções da Fila ---- */
void        inicializarFila(FilaPassinhos *fila);
void        enfileirarPassinho(FilaPassinhos *fila, TipoPassinho passinho);
TipoPassinho desenfileirarPassinho(FilaPassinhos *fila);
TipoPassinho peekFila(FilaPassinhos *fila);
int         filaVazia(FilaPassinhos *fila);
int         filaCheia(FilaPassinhos *fila);
void        limparFila(FilaPassinhos *fila);

#endif