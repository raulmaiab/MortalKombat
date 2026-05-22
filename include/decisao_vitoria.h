#ifndef DECISAO_VITORIA_H
#define DECISAO_VITORIA_H

typedef enum
{
    OPCAO_POS_MENU,
    OPCAO_POS_SELECAO,
    OPCAO_POS_REINICIAR,
    TOTAL_OPCOES_POS_PARTIDA
} OpcaoPosPartida;

OpcaoPosPartida resolverDecisaoPosPartida(OpcaoPosPartida escolhaJ1, OpcaoPosPartida escolhaJ2);

#endif
