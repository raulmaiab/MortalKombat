#include "jogo.h"
#include "combate.h"
#include "decisao_vitoria.h"
#include "equipe.h"
#include "fighter_assets.h"
#include "jogador.h"
#include "ordenacao.h"
#include "player.h"
#include "raylib.h"
#include "selecao_personagens.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>

typedef enum
{
    ESTADO_MENU,
    ESTADO_NOMES,
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

#define KNOCKDOWN_DISPLAY_TICKS 60
#define POS_INICIAL_J1 200
#define POS_INICIAL_J2 (LARGURA_TELA - 260)
#define TICK_GANHO_ENERGIA FPS_ALVO
#define MAX_NOME_JOGADOR 32

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
                           const int selecoesJ1[], const int selecoesJ2[])
{
    inicializarEquipe(equipe1, selecoesJ1, POS_INICIAL_J1, CHAO_Y, 1);
    inicializarEquipe(equipe2, selecoesJ2, POS_INICIAL_J2, CHAO_Y, 0);
    prepararStats(statsRound, jogadorAtivo(equipe1), jogadorAtivo(equipe2));
}

static void reiniciarSelecoes(SelecaoPersonagens *selecaoJ1, SelecaoPersonagens *selecaoJ2)
{
    inicializarSelecaoPersonagens(selecaoJ1, JOAO_CAMPOS);
    inicializarSelecaoPersonagens(selecaoJ2, GRAFITE);
}

static void preencherNomePadrao(char *nome, int jogador)
{
    if (nome[0] == '\0')
        snprintf(nome, MAX_NOME_JOGADOR, "Jogador %d", jogador);
}

static void processarDigitacaoNome(char *nome)
{
    int tecla = GetCharPressed();

    while (tecla > 0)
    {
        int tamanho = (int)strlen(nome);
        if (tecla >= 32 && tecla <= 126 && tamanho < MAX_NOME_JOGADOR - 1)
        {
            nome[tamanho] = (char)tecla;
            nome[tamanho + 1] = '\0';
        }

        tecla = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        int tamanho = (int)strlen(nome);
        if (tamanho > 0)
            nome[tamanho - 1] = '\0';
    }
}

static void moverOpcaoPosPartida(OpcaoPosPartida *opcao, int direcao)
{
    int novaOpcao = (int)(*opcao) + direcao;

    if (novaOpcao < 0)
        novaOpcao = TOTAL_OPCOES_POS_PARTIDA - 1;
    else if (novaOpcao >= TOTAL_OPCOES_POS_PARTIDA)
        novaOpcao = 0;

    *opcao = (OpcaoPosPartida)novaOpcao;
}

static void reiniciarDecisoesVitoria(OpcaoPosPartida *escolhaJ1, OpcaoPosPartida *escolhaJ2,
                                     int *confirmouJ1, int *confirmouJ2)
{
    *escolhaJ1 = OPCAO_POS_REINICIAR;
    *escolhaJ2 = OPCAO_POS_REINICIAR;
    *confirmouJ1 = 0;
    *confirmouJ2 = 0;
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

static int verificarVencedorEquipes(EquipeJogador *equipe1, EquipeJogador *equipe2)
{
    if (!equipeTemVivos(equipe2))
        return 1;
    if (!equipeTemVivos(equipe1))
        return 2;
    return 0;
}

static void finalizarPartida(int resultado, EquipeJogador *equipe1, EquipeJogador *equipe2,
                             Jogador **vencedorPartida, Estatistica *statsRound)
{
    EquipeJogador *equipeVencedora = (resultado == 1) ? equipe1 : equipe2;
    Jogador *perdedor = (resultado == 1) ? jogadorAtivo(equipe2) : jogadorAtivo(equipe1);

    *vencedorPartida = jogadorAtivo(equipeVencedora);
    if (perdedor != NULL && perdedor->hp <= 0)
    {
        perdedor->state = KNOCKDOWN;
        perdedor->stateTicks = 0;
    }

    if (*vencedorPartida != NULL)
        limparFila(&(*vencedorPartida)->fila);
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
    int ticksFimRound = 0;

    SelecaoPersonagens selecaoJ1;
    SelecaoPersonagens selecaoJ2;
    int mostrarControles = 0;
    char nomeJ1[MAX_NOME_JOGADOR] = "";
    char nomeJ2[MAX_NOME_JOGADOR] = "";
    int jogadorDigitandoNome = 1;

    EquipeJogador equipe1;
    EquipeJogador equipe2;
    Estatistica statsRound[2];
    Jogador *vencedorRound = NULL;
    OpcaoPosPartida escolhaVitoriaJ1 = OPCAO_POS_REINICIAR;
    OpcaoPosPartida escolhaVitoriaJ2 = OPCAO_POS_REINICIAR;
    int confirmouVitoriaJ1 = 0;
    int confirmouVitoriaJ2 = 0;

    reiniciarSelecoes(&selecaoJ1, &selecaoJ2);

    while (!WindowShouldClose())
    {
        tickAtual++;

        switch (estado)
        {
        case ESTADO_MENU:
            if (IsKeyPressed(KEY_ENTER))
            {
                nomeJ1[0] = '\0';
                nomeJ2[0] = '\0';
                jogadorDigitandoNome = 1;
                estado = ESTADO_NOMES;
            }
            break;

        case ESTADO_NOMES:
            processarDigitacaoNome(jogadorDigitandoNome == 1 ? nomeJ1 : nomeJ2);

            if (IsKeyPressed(KEY_ENTER))
            {
                if (jogadorDigitandoNome == 1)
                {
                    preencherNomePadrao(nomeJ1, 1);
                    jogadorDigitandoNome = 2;
                }
                else
                {
                    preencherNomePadrao(nomeJ2, 2);
                    reiniciarSelecoes(&selecaoJ1, &selecaoJ2);
                    cenarioAtual = CENARIO_MARCO_ZERO;
                    mostrarControles = 0;
                    estado = ESTADO_SELECAO;
                }
            }
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
                               selecaoJ1.personagens, selecaoJ2.personagens);
                mostrarControles = 0;
                estado = ESTADO_COMBATE;
            }
            break;

        case ESTADO_COMBATE:
            if (tickAtual % TICK_GANHO_ENERGIA == 0)
            {
                adicionarEnergia(jogadorAtivo(&equipe1), GANHO_ENERGIA_TEMPO);
                adicionarEnergia(jogadorAtivo(&equipe2), GANHO_ENERGIA_TEMPO);
            }

            if (IsKeyPressed(KEY_R))
                trocarParaProximoPersonagem(&equipe1);
            if (IsKeyPressed(KEY_P))
                trocarParaProximoPersonagem(&equipe2);

            {
                Jogador *ativoJ1 = jogadorAtivo(&equipe1);
                Jogador *ativoJ2 = jogadorAtivo(&equipe2);
                TipoPassinho ataqueJ1 = updatePlayer(ativoJ1, ativoJ2, CONTROLES_J1);
                TipoPassinho ataqueJ2 = updatePlayer(ativoJ2, ativoJ1, CONTROLES_J2);

                if (ataqueJ1 != PASSINHO_NENHUM)
                    processarPassinho(ataqueJ1, ativoJ1, ativoJ2, statsRound, tickAtual);

                if (ataqueJ2 != PASSINHO_NENHUM)
                {
                    Estatistica statsInvertido[2] = {statsRound[1], statsRound[0]};
                    processarPassinho(ataqueJ2, ativoJ2, ativoJ1, statsInvertido, tickAtual);
                    statsRound[0] = statsInvertido[1];
                    statsRound[1] = statsInvertido[0];
                }
            }
            trocarSeAtivoMorreu(&equipe1);
            trocarSeAtivoMorreu(&equipe2);

            {
                int resultado = verificarVencedorEquipes(&equipe1, &equipe2);

                if (resultado != 0)
                {
                    finalizarPartida(resultado, &equipe1, &equipe2, &vencedorRound, statsRound);
                    ticksFimRound = KNOCKDOWN_DISPLAY_TICKS;
                    estado = ESTADO_FIM_ROUND;
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
            {
                reiniciarDecisoesVitoria(&escolhaVitoriaJ1, &escolhaVitoriaJ2,
                                         &confirmouVitoriaJ1, &confirmouVitoriaJ2);
                estado = ESTADO_VITORIA;
            }
            break;

        case ESTADO_RESULTADO_ROUND:
            if (IsKeyPressed(KEY_ENTER))
                estado = ESTADO_VITORIA;
            break;

        case ESTADO_VITORIA:
            if (!confirmouVitoriaJ1)
            {
                if (IsKeyPressed(KEY_W))
                    moverOpcaoPosPartida(&escolhaVitoriaJ1, -1);
                if (IsKeyPressed(KEY_S))
                    moverOpcaoPosPartida(&escolhaVitoriaJ1, 1);
                if (IsKeyPressed(KEY_ENTER))
                    confirmouVitoriaJ1 = 1;
            }

            if (!confirmouVitoriaJ2)
            {
                if (IsKeyPressed(KEY_UP))
                    moverOpcaoPosPartida(&escolhaVitoriaJ2, -1);
                if (IsKeyPressed(KEY_DOWN))
                    moverOpcaoPosPartida(&escolhaVitoriaJ2, 1);
                if (IsKeyPressed(KEY_L) || IsKeyPressed(KEY_RIGHT_SHIFT))
                    confirmouVitoriaJ2 = 1;
            }

            if (confirmouVitoriaJ1 && confirmouVitoriaJ2)
            {
                OpcaoPosPartida decisao = resolverDecisaoPosPartida(escolhaVitoriaJ1, escolhaVitoriaJ2);
                reiniciarDecisoesVitoria(&escolhaVitoriaJ1, &escolhaVitoriaJ2,
                                         &confirmouVitoriaJ1, &confirmouVitoriaJ2);

                if (decisao == OPCAO_POS_MENU)
                {
                    estado = ESTADO_MENU;
                }
                else if (decisao == OPCAO_POS_SELECAO)
                {
                    reiniciarSelecoes(&selecaoJ1, &selecaoJ2);
                    cenarioAtual = CENARIO_MARCO_ZERO;
                    mostrarControles = 0;
                    estado = ESTADO_SELECAO;
                }
                else
                {
                    iniciarPartida(&equipe1, &equipe2, statsRound,
                                   selecaoJ1.personagens, selecaoJ2.personagens);
                    vencedorRound = NULL;
                    estado = ESTADO_COMBATE;
                }
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
        case ESTADO_NOMES:
            desenharEntradaNomes(bgMenu, nomeJ1, nomeJ2, jogadorDigitandoNome);
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
                renderPlayer(jogadorAtivo(&equipe1), getFighterAssetsJogador(indiceAtivoEquipe(&equipe1), 1), BLUE, "J1");
            if (jogadorAtivo(&equipe2) != NULL)
                renderPlayer(jogadorAtivo(&equipe2), getFighterAssetsJogador(indiceAtivoEquipe(&equipe2), 2), RED, "J2");
            desenharHUD(&equipe1, &equipe2, nomeJ1, nomeJ2);
            break;
        case ESTADO_RESULTADO_ROUND:
            desenharResultadoRound(vencedorRound, statsRound, 2);
            break;
        case ESTADO_VITORIA:
            desenharTelaVitoria(vencedorRound, nomeJ1, nomeJ2,
                                escolhaVitoriaJ1, escolhaVitoriaJ2,
                                confirmouVitoriaJ1, confirmouVitoriaJ2);
            break;
        }

        EndDrawing();
    }

    descarregarAssetsLutadores();
    descarregarCenarios();
    CloseWindow();
    return 0;
}
