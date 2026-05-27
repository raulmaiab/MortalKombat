#include "fila.h"
#include "ordenacao.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void testarFilaPersonagens(void)
{
    FilaPersonagens fila;

    inicializarFilaPersonagens(&fila);
    enfileirarPersonagem(&fila, ANDERSON_NEIFF);
    enfileirarPersonagem(&fila, ALIRIO);
    enfileirarPersonagem(&fila, ARIANO);

    assert(tamanhoFilaPersonagens(&fila) == 3);
    assert(indicePersonagemAtivo(&fila) == ANDERSON_NEIFF);

    rotacionarFilaPersonagens(&fila);
    assert(indicePersonagemAtivo(&fila) == ALIRIO);

    removerPersonagemAtivo(&fila);
    assert(tamanhoFilaPersonagens(&fila) == 2);
    assert(indicePersonagemAtivo(&fila) == ARIANO);
}

static void testarRankingPorVitorias(void)
{
    RegistroRanking ranking[3] = {
        {"Ana", 2},
        {"Bia", 5},
        {"Caio", 3},
    };

    ordenarRankingPorVitorias(ranking, 3);

    assert(strcmp(ranking[0].nomeJogador, "Bia") == 0);
    assert(strcmp(ranking[1].nomeJogador, "Caio") == 0);
    assert(strcmp(ranking[2].nomeJogador, "Ana") == 0);
}

static void testarRankingArquivo(void)
{
    const char *caminho = "tests/ranking_teste.txt";
    RegistroRanking ranking[3] = {
        {"Ana", 2},
        {"Bia", 5},
        {"Caio", 3},
    };
    RegistroRanking carregado[3];
    int total;

    assert(salvarRankingArquivo(caminho, ranking, 3));
    total = carregarRankingArquivo(caminho, carregado, 3);

    assert(total == 3);
    assert(strcmp(carregado[0].nomeJogador, "Bia") == 0);
    assert(carregado[0].vitorias == 5);
    assert(strcmp(carregado[2].nomeJogador, "Ana") == 0);

    remove(caminho);
}

int main(void)
{
    testarFilaPersonagens();
    testarRankingPorVitorias();
    testarRankingArquivo();
    return 0;
}
