#include "equipe.h"
#include <assert.h>

void resetPlayerPosition(Jogador *jogador, float posX, float posY, int olhandoDireita)
{
    jogador->posX = posX;
    jogador->posY = posY;
    jogador->olhandoDireita = olhandoDireita;
    limparFila(&jogador->fila);
}

static void troca_rotaciona_fila_encadeada(void)
{
    int selecoes[TAM_EQUIPE] = {JOAO_CAMPOS, MAGRAO, KUKI};
    EquipeJogador equipe;

    inicializarEquipe(&equipe, selecoes, 10.0f, 20.0f, 1);

    assert(equipe.inicio == &equipe.membros[0]);
    assert(equipe.inicio->next == &equipe.membros[1]);
    assert(equipe.inicio->next->next == &equipe.membros[2]);
    assert(equipe.fim == &equipe.membros[2]);

    trocarParaProximoPersonagem(&equipe);

    assert(equipe.inicio == &equipe.membros[1]);
    assert(equipe.inicio->next == &equipe.membros[2]);
    assert(equipe.inicio->next->next == &equipe.membros[0]);
    assert(equipe.fim == &equipe.membros[0]);
}

static void ativo_morto_sai_da_fila_sem_reordenar_array(void)
{
    int selecoes[TAM_EQUIPE] = {JOAO_CAMPOS, MAGRAO, KUKI};
    EquipeJogador equipe;

    inicializarEquipe(&equipe, selecoes, 10.0f, 20.0f, 1);
    trocarParaProximoPersonagem(&equipe);
    equipe.inicio->jogador.hp = 0;

    trocarSeAtivoMorreu(&equipe);

    assert(equipe.inicio == &equipe.membros[2]);
    assert(equipe.inicio->next == &equipe.membros[0]);
    assert(equipe.fim == &equipe.membros[0]);
    assert(equipe.tamanho == 2);
}

int main(void)
{
    troca_rotaciona_fila_encadeada();
    ativo_morto_sai_da_fila_sem_reordenar_array();
    return 0;
}
