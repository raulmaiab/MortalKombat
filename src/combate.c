#include "combate.h"
#include <string.h>

/*
 * Processa um passinho atacante contra o alvo.
 * Verifica primeiro se o alvo tem ESQUIVA no topo da fila.
 * Se sim, bloqueia o dano e consome a esquiva.
 * Se não, aplica dano e atualiza energia do atacante.
 */
void processarPassinho(TipoPassinho passinho, Jogador *atacante, Jogador *alvo, Estatistica *stats) {
    /* Verifica se o alvo está esquivando */
    if (!filaVazia(&alvo->fila) && peekFila(&alvo->fila) == ESQUIVA) {
        desenfileirarPassinho(&alvo->fila); /* consome a esquiva */
        stats[1].esquivasRealizadas++;      /* stats[1] = alvo */
        return;
    }

    int dano = 0;
    switch (passinho) {
        case ATAQUE_LEVE:    dano = atacante->personagem.danoLeve;    break;
        case ATAQUE_MEDIO:   dano = atacante->personagem.danomedio;   break;
        case ATAQUE_ESPECIAL: dano = atacante->personagem.danoEspecial; break;
        default: break;
    }

    alvo->hp -= dano;
    if (alvo->hp < 0) alvo->hp = 0;

    atualizarEnergia(atacante, dano);
    stats[0].danoTotal += dano; /* stats[0] = atacante */
}

/*
 * Verifica se a sequência atual da fila corresponde a algum combo.
 * Retorna 1 se combo encontrado, 0 caso contrário.
 * A fila deve estar cheia (tamanho 3) antes de chamar essa função.
 */
int verificarCombo(FilaPassinhos *fila, Combo *combosPersonagem, int totalCombos) {
    for (int i = 0; i < totalCombos; i++) {
        int comboBateu = 1;
        for (int j = 0; j < TAM_MAX_FILA; j++) {
            int idx = (fila->inicio + j) % TAM_MAX_FILA;
            if (fila->elementos[idx] != combosPersonagem[i].sequencia[j]) {
                comboBateu = 0;
                break;
            }
        }
        if (comboBateu) return 1;
    }
    return 0;
}

/*
 * Incrementa a barra de energia do jogador proporcional ao dano causado.
 * Limita a energia ao máximo (MAX_ENERGIA).
 */
void atualizarEnergia(Jogador *jogador, int dano) {
    jogador->energia += dano / 2;
    if (jogador->energia > MAX_ENERGIA) jogador->energia = MAX_ENERGIA;
}

/*
 * Verifica se algum jogador zerou o HP.
 * Retorna 1 se jogador1 venceu, 2 se jogador2 venceu, 0 se ainda em curso.
 */
int verificarVencedor(Jogador *jogador1, Jogador *jogador2) {
    if (jogador2->hp <= 0) return 1;
    if (jogador1->hp <= 0) return 2;
    return 0;
}

/*
 * Encerra o round: incrementa rounds vencidos e limpa filas.
 */
void encerrarRound(Jogador *vencedor, Estatistica *stats) {
    vencedor->roundsVencidos++;
    limparFila(&vencedor->fila);
}