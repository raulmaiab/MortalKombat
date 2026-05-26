#ifndef ORDENACAO_H
#define ORDENACAO_H

#define MAX_NOME_RANKING 32

typedef struct
{
    char nomeJogador[MAX_NOME_RANKING];
    int vitorias;
} RegistroRanking;

void ordenarRankingPorVitorias(RegistroRanking ranking[], int total);
void registrarVitoriaRanking(RegistroRanking ranking[], int *total, int capacidade, const char *nomeJogador);

#endif
