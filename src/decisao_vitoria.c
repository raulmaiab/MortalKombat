#include "decisao_vitoria.h"

OpcaoPosPartida resolverDecisaoPosPartida(OpcaoPosPartida escolhaJ1, OpcaoPosPartida escolhaJ2)
{
    if (escolhaJ1 == OPCAO_POS_MENU || escolhaJ2 == OPCAO_POS_MENU)
        return OPCAO_POS_MENU;

    if (escolhaJ1 == OPCAO_POS_SELECAO || escolhaJ2 == OPCAO_POS_SELECAO)
        return OPCAO_POS_SELECAO;

    return OPCAO_POS_REINICIAR;
}
