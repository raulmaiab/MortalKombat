#include "selecao_personagens.h"

void inicializarSelecaoPersonagens(SelecaoPersonagens *selecao, IndicePersonagem personagemInicial)
{
    for (int i = 0; i < TAM_EQUIPE; i++)
        selecao->personagens[i] = personagemInicial;

    selecao->slotAtual = 0;
    selecao->confirmouTudo = 0;
}

void moverSelecaoPersonagens(SelecaoPersonagens *selecao, int direcao)
{
    if (selecao->confirmouTudo)
        return;

    int personagem = selecao->personagens[selecao->slotAtual] + direcao;

    while (personagem < 0)
        personagem += TOTAL_PERSONAGENS;
    personagem %= TOTAL_PERSONAGENS;

    selecao->personagens[selecao->slotAtual] = personagem;
}

void confirmarSelecaoPersonagens(SelecaoPersonagens *selecao)
{
    if (selecao->confirmouTudo)
        return;

    if (selecao->slotAtual < TAM_EQUIPE - 1)
    {
        selecao->slotAtual++;
        return;
    }

    selecao->confirmouTudo = 1;
}
