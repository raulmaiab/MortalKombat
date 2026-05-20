#include "jogo.h"
#include "combate.h"
#include "equipe.h"
#include "fighter_assets.h"
#include "fila.h"
#include "jogador.h"
#include "ordenacao.h"
#include "player.h"
#include "raylib.h"
#include "selecao_personagens.h"
#include "ui.h"
#include <string.h>

typedef enum
{
    ESTADO_MENU,
    ESTADO_SELECAO,
    ESTADO_COMBATE,
    ESTADO_FIM_ROUND,
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
#define KNOCKDOWN_DISPLAY_TICKS 60
#define POS_INICIAL_J1 200
#define POS_INICIAL_J2 (LARGURA_TELA - 260)
#define TICK_GANHO_ENERGIA FPS_ALVO

static const PlayerControls CONTROLES_J1 = {
    KEY_A, KEY_D, KEY_W,
    KEY_F, KEY_E,
    KEY_S,
    KEY_G, 0,
    KEY_T, KEY_Y
};

static const PlayerControls CONTROLES_J2 = {
    KEY_LEFT, KEY_RIGHT, KEY_UP,
    KEY_RIGHT_SHIFT, KEY_INSERT,
    KEY_DOWN,
    KEY_J, KEY_KP_1,
    KEY_L, KEY_KP_3
};

static void prepararStats(Estatistica *statsRound, const Jogador *jogador1, const Jogador *jogador2)
{
    memset(statsRound, 0, sizeof(Estatistica) * 2);
    strncpy(statsRound[0].nomePersonagem, jogador1->personagem.nome, MAX_NOME - 1);
    strncpy(statsRound[1].nomePersonagem, jogador2->personagem.nome, MAX_NOME - 1);
}

static void iniciarPartida(EquipeJogador *equipe1, EquipeJogador *equipe2, Estatistica *statsRound,
                           const int selecoesJ1[], const int selecoesJ2[],
                           int *roundAtual, int *ticksRestantesRound)
{
    inicializarEquipe(equipe1, selecoesJ1, POS_INICIAL_J1, CHAO_Y, 1);
    inicializarEquipe(equipe2, selecoesJ2, POS_INICIAL_J2, CHAO_Y, 0);
    prepararStats(statsRound, jogadorAtivo(equipe1), jogadorAtivo(equipe2));
    *roundAtual = 1;
    *ticksRestantesRound = TEMPO_ROUND_SEGUNDOS * FPS_ALVO;
}

static void iniciarProximoRound(EquipeJogador *equipe1, EquipeJogador *equipe2, Estatistica *statsRound,
                                int *roundAtual, int *ticksRestantesRound)
{
    (*roundAtual)++;
    resetarEquipeParaNovoRound(equipe1, POS_INICIAL_J1, CHAO_Y, 1);
    resetarEquipeParaNovoRound(equipe2, POS_INICIAL_J2, CHAO_Y, 0);
    prepararStats(statsRound, jogadorAtivo(equipe1), jogadorAtivo(equipe2));
    *ticksRestantesRound = TEMPO_ROUND_SEGUNDOS * FPS_ALVO;
}

static int hpTotalEquipe(const EquipeJogador *equipe)
{
    int total = 0;
    for (int i = 0; i < TAM_EQUIPE; i++)
        total += equipe->membros[i].jogador.hp;
    return total;
}

static int hpMaximoTotalEquipe(const EquipeJogador *equipe)
{
    int total = 0;
    for (int i = 0; i < TAM_EQUIPE; i++)
        total += equipe->membros[i].jogador.personagem.hpMaximo;
    return total;
}

static int compararHpPercentual(const EquipeJogador *equipe1, const EquipeJogador *equipe2)
{
    int hpJ1 = hpTotalEquipe(equipe1) * 1000 / hpMaximoTotalEquipe(equipe1);
    int hpJ2 = hpTotalEquipe(equipe2) * 1000 / hpMaximoTotalEquipe(equipe2);

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

static int verificarVencedorEquipes(EquipeJogador *equipe1, EquipeJogador *equipe2)
{
    if (!equipeTemVivos(equipe2))
        return 1;
    if (!equipeTemVivos(equipe1))
        return 2;
    return 0;
}

static void finalizarRound(int resultado, EquipeJogador *equipe1, EquipeJogador *equipe2,
                           Jogador **vencedorRound, Estatistica *statsRound)
{
    EquipeJogador *equipeVencedora = (resultado == 1) ? equipe1 : equipe2;
    Jogador *perdedor = (resultado == 1) ? jogadorAtivo(equipe2) : jogadorAtivo(equipe1);

    *vencedorRound = jogadorAtivo(equipeVencedora);
    if (perdedor != NULL && perdedor->hp <= 0)
    {
        perdedor->state = KNOCKDOWN;
        perdedor->stateTicks = 0;
    }

    equipeVencedora->roundsVencidos++;
    if (*vencedorRound != NULL)
        limparFila(&(*vencedorRound)->fila);
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
    int ticksFimRound = 0;

    SelecaoPersonagens selecaoJ1;
    SelecaoPersonagens selecaoJ2;
    int mostrarControles = 0;

    EquipeJogador equipe1;
    EquipeJogador equipe2;
    Estatistica statsRound[2];
    Jogador *vencedorRound = NULL;

    inicializarSelecaoPersonagens(&selecaoJ1, JOAO_CAMPOS);
    inicializarSelecaoPersonagens(&selecaoJ2, GRAFITE);

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
            if (IsKeyPressed(KEY_I))
                mostrarControles = !mostrarControles;

            if (IsKeyPressed(KEY_D))
                moverSelecaoPersonagens(&selecaoJ1, 1);
            if (IsKeyPressed(KEY_A))
                moverSelecaoPersonagens(&selecaoJ1, -1);
            if (IsKeyPressed(KEY_ENTER))
                confirmarSelecaoPersonagens(&selecaoJ1);

            if (IsKeyPressed(KEY_RIGHT))
                moverSelecaoPersonagens(&selecaoJ2, 1);
            if (IsKeyPressed(KEY_LEFT))
                moverSelecaoPersonagens(&selecaoJ2, -1);
            if (IsKeyPressed(KEY_L) || IsKeyPressed(KEY_RIGHT_SHIFT))
                confirmarSelecaoPersonagens(&selecaoJ2);

            if (IsKeyPressed(KEY_E) && cenarioAtual < TOTAL_CENARIOS - 1)
                cenarioAtual++;
            if (IsKeyPressed(KEY_Q) && cenarioAtual > 0)
                cenarioAtual--;

            if (selecaoJ1.confirmouTudo && selecaoJ2.confirmouTudo)
            {
                iniciarPartida(&equipe1, &equipe2, statsRound,
                               selecaoJ1.personagens, selecaoJ2.personagens,
                               &roundAtual, &ticksRestantesRound);
                mostrarControles = 0;
                estado = ESTADO_COMBATE;
            }
            break;

        case ESTADO_COMBATE:
            if (ticksRestantesRound > 0)
                ticksRestantesRound--;

            if (tickAtual % TICK_GANHO_ENERGIA == 0)
            {
                adicionarEnergia(jogadorAtivo(&equipe1), GANHO_ENERGIA_TEMPO);
                adicionarEnergia(jogadorAtivo(&equipe2), GANHO_ENERGIA_TEMPO);
            }

            if (IsKeyPressed(KEY_R))
                trocarParaProximoPersonagem(&equipe1);
            if (IsKeyPressed(KEY_P))
                trocarParaProximoPersonagem(&equipe2);

            updatePlayer(jogadorAtivo(&equipe1), jogadorAtivo(&equipe2), CONTROLES_J1);
            updatePlayer(jogadorAtivo(&equipe2), jogadorAtivo(&equipe1), CONTROLES_J2);
            processarFilaAtaques(jogadorAtivo(&equipe1), jogadorAtivo(&equipe2), statsRound, tickAtual);
            trocarSeAtivoMorreu(&equipe1);
            trocarSeAtivoMorreu(&equipe2);

            {
                int resultado = verificarVencedorEquipes(&equipe1, &equipe2);
                if (resultado == 0 && ticksRestantesRound == 0)
                    resultado = compararHpPercentual(&equipe1, &equipe2);

                if (resultado != 0)
                {
                    finalizarRound(resultado, &equipe1, &equipe2, &vencedorRound, statsRound);
                    ticksFimRound = KNOCKDOWN_DISPLAY_TICKS;
                    estado = ESTADO_FIM_ROUND;
                }
                else if (ticksRestantesRound == 0)
                {
                    vencedorRound = NULL;
                    ordenarEstatisticas(statsRound, 2);
                    estado = ESTADO_RESULTADO_ROUND;
                }
            }
            break;

        case ESTADO_FIM_ROUND:
            if (jogadorAtivo(&equipe1) != NULL)
                jogadorAtivo(&equipe1)->stateTicks++;
            if (jogadorAtivo(&equipe2) != NULL)
                jogadorAtivo(&equipe2)->stateTicks++;
            if (ticksFimRound > 0)
                ticksFimRound--;
            else
                estado = ESTADO_RESULTADO_ROUND;
            break;

        case ESTADO_RESULTADO_ROUND:
            if (IsKeyPressed(KEY_ENTER))
            {
                if (equipe1.roundsVencidos >= 2 || equipe2.roundsVencidos >= 2)
                {
                    estado = ESTADO_VITORIA;
                }
                else
                {
                    iniciarProximoRound(&equipe1, &equipe2, statsRound, &roundAtual, &ticksRestantesRound);
                    estado = ESTADO_COMBATE;
                }
            }
            break;

        case ESTADO_VITORIA:
            if (IsKeyPressed(KEY_ENTER))
            {
                inicializarSelecaoPersonagens(&selecaoJ1, JOAO_CAMPOS);
                inicializarSelecaoPersonagens(&selecaoJ2, GRAFITE);
                cenarioAtual = CENARIO_MARCO_ZERO;
                mostrarControles = 0;
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
            desenharSelecaoPersonagem(selecaoJ1.personagens, selecaoJ1.slotAtual, selecaoJ1.confirmouTudo,
                                      selecaoJ2.personagens, selecaoJ2.slotAtual, selecaoJ2.confirmouTudo,
                                      cenarioAtual, mostrarControles);
            break;
        case ESTADO_COMBATE:
        case ESTADO_FIM_ROUND:
            desenharCenario(selecionarCenario(cenarioAtual));
            if (jogadorAtivo(&equipe1) != NULL)
                renderPlayer(jogadorAtivo(&equipe1), getFighterAssets(indiceAtivoEquipe(&equipe1)), RED, "J1");
            if (jogadorAtivo(&equipe2) != NULL)
                renderPlayer(jogadorAtivo(&equipe2), getFighterAssets(indiceAtivoEquipe(&equipe2)), BLUE, "J2");
            desenharHUD(&equipe1, &equipe2, roundAtual, (ticksRestantesRound + FPS_ALVO - 1) / FPS_ALVO);
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
