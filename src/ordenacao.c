#include "ordenacao.h"
#include <string.h>

/*
 * Ordena o ranking geral por vitórias acumuladas em ordem decrescente.
 * Algoritmo exigido para o projeto: Insertion Sort.
 */
void ordenarRankingPorVitorias(RegistroRanking ranking[], int total)
{
    for (int i = 1; i < total; i++)
    {
        RegistroRanking chave = ranking[i];
        int j = i - 1;

        while (j >= 0 && ranking[j].vitorias < chave.vitorias)
        {
            ranking[j + 1] = ranking[j];
            j--;
        }
        ranking[j + 1] = chave;
    }
}

void registrarVitoriaRanking(RegistroRanking ranking[], int *total, int capacidade, const char *nomeJogador)
{
    for (int i = 0; i < *total; i++)
    {
        if (strncmp(ranking[i].nomeJogador, nomeJogador, MAX_NOME_RANKING) == 0)
        {
            ranking[i].vitorias++;
            ordenarRankingPorVitorias(ranking, *total);
            return;
        }
    }

    if (*total >= capacidade)
        return;

    strncpy(ranking[*total].nomeJogador, nomeJogador, MAX_NOME_RANKING - 1);
    ranking[*total].nomeJogador[MAX_NOME_RANKING - 1] = '\0';
    ranking[*total].vitorias = 1;
    (*total)++;
    ordenarRankingPorVitorias(ranking, *total);
}
