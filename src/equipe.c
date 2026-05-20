#include "equipe.h"
#include "player.h"
#include <stddef.h>

static void atualizarLinks(EquipeJogador *equipe)
{
    equipe->inicio = NULL;
    equipe->fim = NULL;
    equipe->tamanho = 0;

    for (int i = 0; i < TAM_EQUIPE; i++)
    {
        equipe->membros[i].next = NULL;
        if (!equipe->membros[i].vivo)
            continue;

        if (equipe->fim != NULL)
            equipe->fim->next = &equipe->membros[i];
        else
            equipe->inicio = &equipe->membros[i];

        equipe->fim = &equipe->membros[i];
        equipe->tamanho++;
    }
}

void inicializarEquipe(EquipeJogador *equipe, const int selecoes[], float posX, float posY, int olhandoDireita)
{
    equipe->roundsVencidos = 0;

    for (int i = 0; i < TAM_EQUIPE; i++)
    {
        equipe->membros[i].indicePersonagem = selecoes[i];
        equipe->membros[i].vivo = 1;
        inicializarJogador(&equipe->membros[i].jogador, selecoes[i], posX, posY, olhandoDireita);
    }

    atualizarLinks(equipe);
}

Jogador *jogadorAtivo(EquipeJogador *equipe)
{
    if (equipe->inicio == NULL)
        return NULL;
    return &equipe->inicio->jogador;
}

const Jogador *jogadorAtivoConst(const EquipeJogador *equipe)
{
    if (equipe->inicio == NULL)
        return NULL;
    return &equipe->inicio->jogador;
}

int indiceAtivoEquipe(const EquipeJogador *equipe)
{
    if (equipe->inicio == NULL)
        return 0;
    return equipe->inicio->indicePersonagem;
}

int equipeTemVivos(const EquipeJogador *equipe)
{
    return equipe->inicio != NULL;
}

int equipePodeTrocar(const EquipeJogador *equipe)
{
    return equipe->tamanho > 1;
}

void trocarParaProximoPersonagem(EquipeJogador *equipe)
{
    if (!equipePodeTrocar(equipe))
        return;

    NoPersonagem *antigoAtivo = equipe->inicio;
    float posX = antigoAtivo->jogador.posX;
    float posY = antigoAtivo->jogador.posY;
    int olhandoDireita = antigoAtivo->jogador.olhandoDireita;

    equipe->inicio = antigoAtivo->next;
    antigoAtivo->next = NULL;
    equipe->fim->next = antigoAtivo;
    equipe->fim = antigoAtivo;

    resetPlayerPosition(&equipe->inicio->jogador, posX, posY, olhandoDireita);
}

void trocarSeAtivoMorreu(EquipeJogador *equipe)
{
    if (equipe->inicio == NULL || equipe->inicio->jogador.hp > 0)
        return;

    NoPersonagem *morto = equipe->inicio;
    float posX = morto->jogador.posX;
    float posY = morto->jogador.posY;
    int olhandoDireita = morto->jogador.olhandoDireita;

    morto->vivo = 0;
    equipe->inicio = morto->next;
    morto->next = NULL;
    equipe->tamanho--;

    if (equipe->inicio == NULL)
    {
        equipe->fim = NULL;
        return;
    }

    resetPlayerPosition(&equipe->inicio->jogador, posX, posY, olhandoDireita);
}

void resetarEquipeParaNovoRound(EquipeJogador *equipe, float posX, float posY, int olhandoDireita)
{
    int roundsVencidos = equipe->roundsVencidos;

    for (int i = 0; i < TAM_EQUIPE; i++)
    {
        equipe->membros[i].vivo = 1;
        resetarJogador(&equipe->membros[i].jogador);
        resetPlayerPosition(&equipe->membros[i].jogador, posX, posY, olhandoDireita);
    }

    equipe->roundsVencidos = roundsVencidos;
    atualizarLinks(equipe);
}
