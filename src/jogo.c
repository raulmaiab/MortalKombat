#include "jogo.h"
#include "combate.h"
#include "fighter_assets.h"
#include "fila.h"
#include "jogador.h"
#include "ordenacao.h"
#include "player.h"
#include "raylib.h"
#include "ui.h"
#include <string.h>

typedef enum
{
    ESTADO_MENU,
    ESTADO_SELECAO,
    ESTADO_COMBATE,
    ESTADO_RESULTADO_ROUND,
    ESTADO_VITORIA
} EstadoJogo;

typedef enum
{
    CENARIO_MARCO_ZERO,
    CENARIO_BOA_VIAGEM,
    CENARIO_JAQUEIRA,
    TOTAL_CENARIOS
} IndiceCenario;

#define TICK_ATAQUE 8
#define TEMPO_ROUND_SEGUNDOS 99
#define POS_INICIAL_J1 200
#define POS_INICIAL_J2 (LARGURA_TELA - 260)

static const PlayerControls CONTROLES_J1 = {
    KEY_A, KEY_D, KEY_W, KEY_E, 0, KEY_S, KEY_G, KEY_H, KEY_Y
};

static const PlayerControls CONTROLES_J2 = {
    KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_INSERT, KEY_KP_0, KEY_DOWN, KEY_KP_1, KEY_KP_2, KEY_KP_3
};

static void prepararStats(Estatistica *statsRound, const Jogador *jogador1, const Jogador *jogador2)
{
    memset(statsRound, 0, sizeof(Estatistica) * 2);
    strncpy(statsRound[0].nomePersonagem, jogador1->personagem.nome, MAX_NOME - 1);
    strncpy(statsRound[1].nomePersonagem, jogador2->personagem.nome, MAX_NOME - 1);
}

static void iniciarPartida(Jogador *jogador1, Jogador *jogador2, Estatistica *statsRound,
                           int selecaoJ1, int selecaoJ2, int *roundAtual, int *ticksRestantesRound)
{
    inicializarJogador(jogador1, selecaoJ1, POS_INICIAL_J1, CHAO_Y, 1);
    inicializarJogador(jogador2, selecaoJ2, POS_INICIAL_J2, CHAO_Y, 0);
    prepararStats(statsRound, jogador1, jogador2);
    *roundAtual = 1;
    *ticksRestantesRound = TEMPO_ROUND_SEGUNDOS * FPS_ALVO;
}

static void iniciarProximoRound(Jogador *jogador1, Jogador *jogador2, Estatistica *statsRound,
                                int *roundAtual, int *ticksRestantesRound)
{
    (*roundAtual)++;
    resetarJogador(jogador1);
    resetarJogador(jogador2);
    resetPlayerPosition(jogador1, POS_INICIAL_J1, CHAO_Y, 1);
    resetPlayerPosition(jogador2, POS_INICIAL_J2, CHAO_Y, 0);
    prepararStats(statsRound, jogador1, jogador2);
    *ticksRestantesRound = TEMPO_ROUND_SEGUNDOS * FPS_ALVO;
}

static int compararHpPercentual(Jogador *jogador1, Jogador *jogador2)
{
    int hpJ1 = jogador1->hp * 1000 / jogador1->personagem.hpMaximo;
    int hpJ2 = jogador2->hp * 1000 / jogador2->personagem.hpMaximo;

    if (hpJ1 > hpJ2)
        return 1;
    if (hpJ2 > hpJ1)
        return 2;
    return 0;
}

static Texture2D selecionarCenario(IndiceCenario cenarioAtual)
{
    switch (cenarioAtual)
    {
    case CENARIO_BOA_VIAGEM:
        return bgBoaViagem;
    case CENARIO_JAQUEIRA:
        return bgJaqueira;
    case CENARIO_MARCO_ZERO:
    default:
        return bgMarcoZero;
    }
}

static void processarFilaAtaques(Jogador *jogador1, Jogador *jogador2, Estatistica *statsRound, int tickAtual)
{
    if (tickAtual % TICK_ATAQUE != 0)
        return;

    if (!filaVazia(&jogador1->fila))
    {
        TipoPassinho p = desenfileirarPassinho(&jogador1->fila);
        if (p != ESQUIVA)
            processarPassinho(p, jogador1, jogador2, statsRound, tickAtual);
    }

    if (!filaVazia(&jogador2->fila))
    {
        TipoPassinho p = desenfileirarPassinho(&jogador2->fila);
        if (p != ESQUIVA)
        {
            Estatistica statsInvertido[2] = {statsRound[1], statsRound[0]};
            processarPassinho(p, jogador2, jogador1, statsInvertido, tickAtual);
            statsRound[0] = statsInvertido[1];
            statsRound[1] = statsInvertido[0];
        }
    }
}

static void finalizarRound(int resultado, Jogador *jogador1, Jogador *jogador2,
                           Jogador **vencedorRound, Estatistica *statsRound)
{
    *vencedorRound = (resultado == 1) ? jogador1 : jogador2;
    encerrarRound(*vencedorRound, statsRound);
    ordenarEstatisticas(statsRound, 2);
}

int executarJogo(void)
{
    InitWindow(LARGURA_TELA, ALTURA_TELA, TITULO_JANELA);
    SetTargetFPS(FPS_ALVO);

    carregarCenarios();
    carregarAssetsLutadores();

    EstadoJogo estado = ESTADO_MENU;
    IndiceCenario cenarioAtual = CENARIO_MARCO_ZERO;
    int tickAtual = 0;
    int roundAtual = 1;
    int ticksRestantesRound = TEMPO_ROUND_SEGUNDOS * FPS_ALVO;

    int selecaoJ1 = JOAO_CAMPOS;
    int selecaoJ2 = GRAFITE;
    int confirmouJ1 = 0;
    int confirmouJ2 = 0;

    Jogador jogador1;
    Jogador jogador2;
    Estatistica statsRound[2];
    Jogador *vencedorRound = NULL;

    while (!WindowShouldClose())
    {
        tickAtual++;

        switch (estado)
        {
        case ESTADO_MENU:
            if (IsKeyPressed(KEY_ENTER))
                estado = ESTADO_SELECAO;
            break;

        case ESTADO_SELECAO:
            if (IsKeyPressed(KEY_D) && selecaoJ1 < TOTAL_PERSONAGENS - 1)
                selecaoJ1++;
            if (IsKeyPressed(KEY_A) && selecaoJ1 > 0)
                selecaoJ1--;
            if (IsKeyPressed(KEY_ENTER))
                confirmouJ1 = 1;

            if (IsKeyPressed(KEY_RIGHT) && selecaoJ2 < TOTAL_PERSONAGENS - 1)
                selecaoJ2++;
            if (IsKeyPressed(KEY_LEFT) && selecaoJ2 > 0)
                selecaoJ2--;
            if (IsKeyPressed(KEY_KP_1))
                confirmouJ2 = 1;

            if (IsKeyPressed(KEY_E) && cenarioAtual < TOTAL_CENARIOS - 1)
                cenarioAtual++;
            if (IsKeyPressed(KEY_Q) && cenarioAtual > 0)
                cenarioAtual--;

            if (confirmouJ1 && confirmouJ2)
            {
                iniciarPartida(&jogador1, &jogador2, statsRound, selecaoJ1, selecaoJ2,
                               &roundAtual, &ticksRestantesRound);
                confirmouJ1 = 0;
                confirmouJ2 = 0;
                estado = ESTADO_COMBATE;
            }
            break;

        case ESTADO_COMBATE:
            if (ticksRestantesRound > 0)
                ticksRestantesRound--;

            updatePlayer(&jogador1, &jogador2, CONTROLES_J1);
            updatePlayer(&jogador2, &jogador1, CONTROLES_J2);
            processarFilaAtaques(&jogador1, &jogador2, statsRound, tickAtual);

            {
                int resultado = verificarVencedor(&jogador1, &jogador2);
                if (resultado == 0 && ticksRestantesRound == 0)
                    resultado = compararHpPercentual(&jogador1, &jogador2);

                if (resultado != 0)
                {
                    finalizarRound(resultado, &jogador1, &jogador2, &vencedorRound, statsRound);
                    estado = ESTADO_RESULTADO_ROUND;
                }
                else if (ticksRestantesRound == 0)
                {
                    vencedorRound = NULL;
                    ordenarEstatisticas(statsRound, 2);
                    estado = ESTADO_RESULTADO_ROUND;
                }
            }
            break;

        case ESTADO_RESULTADO_ROUND:
            if (IsKeyPressed(KEY_ENTER))
            {
                if (jogador1.roundsVencidos >= 2 || jogador2.roundsVencidos >= 2)
                {
                    estado = ESTADO_VITORIA;
                }
                else
                {
                    iniciarProximoRound(&jogador1, &jogador2, statsRound, &roundAtual, &ticksRestantesRound);
                    estado = ESTADO_COMBATE;
                }
            }
            break;

        case ESTADO_VITORIA:
            if (IsKeyPressed(KEY_ENTER))
            {
                selecaoJ1 = JOAO_CAMPOS;
                selecaoJ2 = GRAFITE;
                cenarioAtual = CENARIO_MARCO_ZERO;
                estado = ESTADO_SELECAO;
            }
            break;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        switch (estado)
        {
        case ESTADO_MENU:
            desenharMenuPrincipal(bgMenu);
            break;
        case ESTADO_SELECAO:
            desenharSelecaoPersonagem(selecaoJ1, selecaoJ2, cenarioAtual);
            break;
        case ESTADO_COMBATE:
            desenharCenario(selecionarCenario(cenarioAtual));
            renderPlayer(&jogador1, getFighterAssets(selecaoJ1), RED, "J1");
            renderPlayer(&jogador2, getFighterAssets(selecaoJ2), BLUE, "J2");
            desenharHUD(&jogador1, &jogador2, roundAtual, (ticksRestantesRound + FPS_ALVO - 1) / FPS_ALVO);
            break;
        case ESTADO_RESULTADO_ROUND:
            desenharResultadoRound(vencedorRound, statsRound, 2);
            break;
        case ESTADO_VITORIA:
            desenharTelaVitoria(vencedorRound);
            break;
        }

        EndDrawing();
    }

    descarregarAssetsLutadores();
    descarregarCenarios();
    CloseWindow();
    return 0;
}
