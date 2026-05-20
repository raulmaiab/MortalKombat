#include "selecao_personagens.h"
#include <assert.h>

static void confirmar_tres_slots_bloqueia_selecao(void)
{
    SelecaoPersonagens selecao;
    inicializarSelecaoPersonagens(&selecao, JOAO_CAMPOS);

    moverSelecaoPersonagens(&selecao, 1);
    assert(selecao.personagens[0] == MAGRAO);
    assert(selecao.slotAtual == 0);
    assert(selecao.confirmouTudo == 0);

    confirmarSelecaoPersonagens(&selecao);
    assert(selecao.slotAtual == 1);
    assert(selecao.confirmouTudo == 0);

    moverSelecaoPersonagens(&selecao, 2);
    assert(selecao.personagens[1] == KUKI);

    confirmarSelecaoPersonagens(&selecao);
    assert(selecao.slotAtual == 2);
    assert(selecao.confirmouTudo == 0);

    moverSelecaoPersonagens(&selecao, 3);
    assert(selecao.personagens[2] == GRAFITE);

    confirmarSelecaoPersonagens(&selecao);
    assert(selecao.slotAtual == 2);
    assert(selecao.confirmouTudo == 1);

    moverSelecaoPersonagens(&selecao, 1);
    assert(selecao.personagens[2] == GRAFITE);
}

int main(void)
{
    confirmar_tres_slots_bloqueia_selecao();
    return 0;
}
