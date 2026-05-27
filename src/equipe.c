#include "equipe.h"
#include "player.h"
#include <stddef.h>

void inicializarEquipe(EquipeJogador *equipe, const int selecoes[], float posX, float posY, int olhandoDireita)
{
    equipe->energia = 0;
    equipe->roundsVencidos = 0;
    equipe->olhandoDireitaEntrada = olhandoDireita;
    inicializarFilaPersonagens(&equipe->personagens);

    for (int i = 0; i < TAM_EQUIPE; i++)
    {
        enfileirarPersonagem(&equipe->personagens, (IndicePersonagem)selecoes[i]);
        inicializarJogador(&equipe->personagens.membros[i].jogador,
                           (IndicePersonagem)selecoes[i], posX, posY, olhandoDireita);
    }
}

Jogador *jogadorAtivo(EquipeJogador *equipe)
{
    NoPersonagem *ativo = noPersonagemAtivo(&equipe->personagens);
    if (ativo == NULL)
        return NULL;
    return &ativo->jogador;
}

const Jogador *jogadorAtivoConst(const EquipeJogador *equipe)
{
    const NoPersonagem *ativo = noPersonagemAtivoConst(&equipe->personagens);
    if (ativo == NULL)
        return NULL;
    return &ativo->jogador;
}

int indiceAtivoEquipe(const EquipeJogador *equipe)
{
    return indicePersonagemAtivo(&equipe->personagens);
}

int equipeTemVivos(const EquipeJogador *equipe)
{
    return filaPersonagensTemVivos(&equipe->personagens);
}

int equipePodeTrocar(const EquipeJogador *equipe)
{
    return filaPersonagensPodeRotacionar(&equipe->personagens);
}

void trocarParaProximoPersonagem(EquipeJogador *equipe)
{
    if (!equipePodeTrocar(equipe))
        return;

    float posX = equipe->olhandoDireitaEntrada ? 0.0f : (float)(LARGURA_TELA - LARGURA_PERSONAGEM);

    rotacionarFilaPersonagens(&equipe->personagens);

    resetPlayerEntradaPulando(jogadorAtivo(equipe), posX, CHAO_Y, equipe->olhandoDireitaEntrada);
}

void trocarSeAtivoMorreu(EquipeJogador *equipe)
{
    NoPersonagem *morto = noPersonagemAtivo(&equipe->personagens);
    if (morto == NULL || morto->jogador.hp > 0)
        return;

    float posX = equipe->olhandoDireitaEntrada ? 0.0f : (float)(LARGURA_TELA - LARGURA_PERSONAGEM);

    removerPersonagemAtivo(&equipe->personagens);

    if (!equipeTemVivos(equipe))
        return;

    resetPlayerEntradaPulando(jogadorAtivo(equipe), posX, CHAO_Y, equipe->olhandoDireitaEntrada);
}

void resetarEquipeParaNovoRound(EquipeJogador *equipe, float posX, float posY, int olhandoDireita)
{
    int roundsVencidos = equipe->roundsVencidos;
    IndicePersonagem indices[TAM_EQUIPE];

    for (int i = 0; i < TAM_EQUIPE; i++)
    {
        IndicePersonagem indice = equipe->personagens.membros[i].indicePersonagem;
        indices[i] = indice;
        inicializarJogador(&equipe->personagens.membros[i].jogador, indice, posX, posY, olhandoDireita);
    }

    equipe->energia = 0;
    equipe->roundsVencidos = roundsVencidos;
    inicializarFilaPersonagens(&equipe->personagens);
    for (int i = 0; i < TAM_EQUIPE; i++)
    {
        enfileirarPersonagem(&equipe->personagens, indices[i]);
        inicializarJogador(&equipe->personagens.membros[i].jogador, indices[i], posX, posY, olhandoDireita);
    }
}
