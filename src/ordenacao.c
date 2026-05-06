#include "ordenacao.h"
#include <stdio.h>

/*
 * Ordena o array de estatísticas por danoTotal (decrescente).
 * Algoritmo: Insertion Sort.
 * Chamada ao fim de cada round para exibir resultado.
 *
 * Justificativa: array sempre pequeno (2 jogadores),
 * Insertion Sort é eficiente para n pequeno e simples de implementar em C.
 */
void ordenarEstatisticas(Estatistica *stats, int n) {
    for (int i = 1; i < n; i++) {
        Estatistica chave = stats[i];
        int j = i - 1;

        /* Move elementos maiores para frente (ordem decrescente) */
        while (j >= 0 && stats[j].danoTotal < chave.danoTotal) {
            stats[j + 1] = stats[j];
            j--;
        }
        stats[j + 1] = chave;
    }
}

/*
 * Exibe as estatísticas ordenadas no terminal.
 * Versão texto — pode ser substituída pela versão Raylib em ui.c.
 */
void exibirEstatisticas(Estatistica *stats, int n) {
    printf("\n===== RESULTADO DO ROUND =====\n");
    for (int i = 0; i < n; i++) {
        printf("%d. %s\n",          i + 1, stats[i].nomePersonagem);
        printf("   Dano causado:   %d\n", stats[i].danoTotal);
        printf("   Combos feitos:  %d\n", stats[i].combosExecutados);
        printf("   Esquivas:       %d\n", stats[i].esquivasRealizadas);
    }
    printf("==============================\n\n");
}