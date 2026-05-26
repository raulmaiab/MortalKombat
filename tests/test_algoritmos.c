#include "fila.h"
#include "ordenacao.h"
#include <assert.h>
#include <string.h>

static void testarFilaPersonagens(void)
{
    FilaPersonagens fila;

    inicializarFilaPersonagens(&fila);
    enfileirarPersonagem(&fila, JOAO_CAMPOS);
    enfileirarPersonagem(&fila, MAGRAO);
    enfileirarPersonagem(&fila, KUKI);

    assert(tamanhoFilaPersonagens(&fila) == 3);
    assert(indicePersonagemAtivo(&fila) == JOAO_CAMPOS);

    rotacionarFilaPersonagens(&fila);
    assert(indicePersonagemAtivo(&fila) == MAGRAO);

    removerPersonagemAtivo(&fila);
    assert(tamanhoFilaPersonagens(&fila) == 2);
    assert(indicePersonagemAtivo(&fila) == KUKI);
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

int main(void)
{
    testarFilaPersonagens();
    testarRankingPorVitorias();
    return 0;
}
