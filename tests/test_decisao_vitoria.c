#include "decisao_vitoria.h"
#include <assert.h>

static void menu_tem_prioridade_sobre_qualquer_escolha(void)
{
    assert(resolverDecisaoPosPartida(OPCAO_POS_MENU, OPCAO_POS_REINICIAR) == OPCAO_POS_MENU);
    assert(resolverDecisaoPosPartida(OPCAO_POS_SELECAO, OPCAO_POS_MENU) == OPCAO_POS_MENU);
}

static void selecao_tem_prioridade_sobre_reiniciar(void)
{
    assert(resolverDecisaoPosPartida(OPCAO_POS_SELECAO, OPCAO_POS_REINICIAR) == OPCAO_POS_SELECAO);
    assert(resolverDecisaoPosPartida(OPCAO_POS_REINICIAR, OPCAO_POS_SELECAO) == OPCAO_POS_SELECAO);
}

static void reinicia_direto_quando_os_dois_escolhem_reiniciar(void)
{
    assert(resolverDecisaoPosPartida(OPCAO_POS_REINICIAR, OPCAO_POS_REINICIAR) == OPCAO_POS_REINICIAR);
}

int main(void)
{
    menu_tem_prioridade_sobre_qualquer_escolha();
    selecao_tem_prioridade_sobre_reiniciar();
    reinicia_direto_quando_os_dois_escolhem_reiniciar();
    return 0;
}
