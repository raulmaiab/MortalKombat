#include "ordenacao.h"
#include <stdio.h>
#include <string.h>

/*
 * Ordena o ranking geral por vitórias acumuladas em ordem decrescente.
 * Insertion Sort
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

int carregarRankingArquivo(const char *caminho, RegistroRanking ranking[], int capacidade)
{
    FILE *arquivo = fopen(caminho, "r");
    int total = 0;

    if (arquivo == NULL)
        return 0;

    while (total < capacidade)
    {
        char nome[MAX_NOME_RANKING];
        int vitorias;

        if (fscanf(arquivo, "%31[^;];%d\n", nome, &vitorias) != 2)
            break;

        strncpy(ranking[total].nomeJogador, nome, MAX_NOME_RANKING - 1);
        ranking[total].nomeJogador[MAX_NOME_RANKING - 1] = '\0';
        ranking[total].vitorias = vitorias;
        total++;
    }

    fclose(arquivo);
    ordenarRankingPorVitorias(ranking, total);
    return total;
}

int salvarRankingArquivo(const char *caminho, const RegistroRanking ranking[], int total)
{
    FILE *arquivo = fopen(caminho, "w");

    if (arquivo == NULL)
        return 0;

    for (int i = 0; i < total; i++)
        fprintf(arquivo, "%s;%d\n", ranking[i].nomeJogador, ranking[i].vitorias);

    fclose(arquivo);
    return 1;
}
