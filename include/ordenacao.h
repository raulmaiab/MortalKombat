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
int carregarRankingArquivo(const char *caminho, RegistroRanking ranking[], int capacidade);
int salvarRankingArquivo(const char *caminho, const RegistroRanking ranking[], int total);

#endif
