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
    ESTADO_PAUSA,
    ESTADO_FIM_ROUND,
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
#define MAX_RANKING 10
#define ARQUIVO_RANKING "ranking.txt"
#define BOTAO_PAUSA_LARGURA 96
#define BOTAO_PAUSA_ALTURA 34
#define AUDIO_FUNDO "assets/audio/fundo.ogg"
#define AUDIO_VITORIA "assets/audio/vitoria.wav"
#define AUDIO_ESPECIAL "assets/audio/especial.wav"

typedef struct
{
    Music fundo;
    Sound vitoria;
    Sound especial;
    int temFundo;
    int temVitoria;
    int temEspecial;
} AudioJogo;

static const PlayerControls CONTROLES_J1 = {
    KEY_A, KEY_D, KEY_W,
    KEY_F, KEY_E,
    KEY_S,
    KEY_G, 0,
    KEY_LEFT_SHIFT, 0};

static const PlayerControls CONTROLES_J2 = {
    KEY_LEFT, KEY_RIGHT, KEY_UP,
    KEY_RIGHT_SHIFT, KEY_INSERT,
    KEY_DOWN,
    KEY_J, KEY_KP_1,
    KEY_ENTER, KEY_KP_ENTER};

static void desenharFiltroEspecial(const EquipeJogador *equipe1, const EquipeJogador *equipe2);

static void carregarAudioJogo(AudioJogo *audio)
{
    audio->temFundo = 0;
    audio->temVitoria = 0;
    audio->temEspecial = 0;

    InitAudioDevice();

    if (FileExists(AUDIO_FUNDO))
    {
        audio->fundo = LoadMusicStream(AUDIO_FUNDO);
        audio->fundo.looping = 1;
        audio->temFundo = 1;
        PlayMusicStream(audio->fundo);
    }

    if (FileExists(AUDIO_VITORIA))
    {
        audio->vitoria = LoadSound(AUDIO_VITORIA);
        audio->temVitoria = 1;
    }

    if (FileExists(AUDIO_ESPECIAL))
    {
        audio->especial = LoadSound(AUDIO_ESPECIAL);
        audio->temEspecial = 1;
    }
}

static void atualizarAudioJogo(AudioJogo *audio)
{
    if (audio->temFundo)
        UpdateMusicStream(audio->fundo);
}

static void descarregarAudioJogo(AudioJogo *audio)
{
    if (audio->temFundo)
        UnloadMusicStream(audio->fundo);
    if (audio->temVitoria)
        UnloadSound(audio->vitoria);
    if (audio->temEspecial)
        UnloadSound(audio->especial);

    CloseAudioDevice();
}

static void tocarSomEspecial(const AudioJogo *audio)
{
    if (audio->temEspecial)
        PlaySound(audio->especial);
}

static void tocarSomVitoria(const AudioJogo *audio)
{
    if (audio->temVitoria)
        PlaySound(audio->vitoria);
}

static void iniciarPartida(EquipeJogador *equipe1, EquipeJogador *equipe2,
                           const int selecoesJ1[], const int selecoesJ2[])
{
    inicializarEquipe(equipe1, selecoesJ1, POS_INICIAL_J1, CHAO_Y, 1);
    inicializarEquipe(equipe2, selecoesJ2, POS_INICIAL_J2, CHAO_Y, 0);
}

static void reiniciarSelecoes(SelecaoPersonagens *selecaoJ1, SelecaoPersonagens *selecaoJ2)
{
    inicializarSelecaoPersonagens(selecaoJ1, ALIRIO);
    inicializarSelecaoPersonagens(selecaoJ2, ADRIANO);
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

static Rectangle retanguloBotaoPausa(void)
{
    return (Rectangle){LARGURA_TELA / 2.0f - BOTAO_PAUSA_LARGURA / 2.0f, 58.0f,
                       BOTAO_PAUSA_LARGURA, BOTAO_PAUSA_ALTURA};
}

static Rectangle retanguloBotaoContinuar(void)
{
    return (Rectangle){LARGURA_TELA / 2.0f - 145.0f, 310.0f, 290.0f, 48.0f};
}

static Rectangle retanguloBotaoMenu(void)
{
    return (Rectangle){LARGURA_TELA / 2.0f - 145.0f, 378.0f, 290.0f, 48.0f};
}

static int botaoClicado(Rectangle botao)
{
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
           CheckCollisionPointRec(GetMousePosition(), botao);
}

static void desenharBotaoTexto(Rectangle botao, const char *texto, Color fundo, Color borda, int tamanhoFonte)
{
    int larguraTexto = MeasureText(texto, tamanhoFonte);

    DrawRectangleRounded(botao, 0.12f, 8, fundo);
    DrawRectangleRoundedLines(botao, 0.12f, 8, borda);
    DrawText(texto,
             (int)(botao.x + botao.width / 2.0f - larguraTexto / 2.0f),
             (int)(botao.y + botao.height / 2.0f - tamanhoFonte / 2.0f),
             tamanhoFonte, WHITE);
}

static void desenharBotaoPausa(void)
{
    desenharBotaoTexto(retanguloBotaoPausa(), "PAUSA", Fade(BLACK, 0.65f), Fade(WHITE, 0.75f), 18);
}

static void desenharModalPausa(void)
{
    Rectangle painel = {LARGURA_TELA / 2.0f - 190.0f, 220.0f, 380.0f, 260.0f};
    int larguraTitulo = MeasureText("JOGO PAUSADO", 30);

    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, Fade(BLACK, 0.58f));
    DrawRectangleRounded(painel, 0.05f, 10, Fade((Color){8, 8, 12, 255}, 0.96f));
    DrawRectangleRoundedLines(painel, 0.05f, 10, Fade(WHITE, 0.65f));
    DrawText("JOGO PAUSADO", LARGURA_TELA / 2 - larguraTitulo / 2, 250, 30, WHITE);
    desenharBotaoTexto(retanguloBotaoContinuar(), "Continuar", Fade(GREEN, 0.45f), Fade(GREEN, 0.9f), 22);
    desenharBotaoTexto(retanguloBotaoMenu(), "Voltar ao Menu", Fade(RED, 0.45f), Fade(RED, 0.9f), 22);
}

static void desenharCombateAtual(IndiceCenario cenarioAtual, EquipeJogador *equipe1, EquipeJogador *equipe2,
                                 const char *nomeJ1, const char *nomeJ2)
{
    desenharCenario(selecionarCenario(cenarioAtual));
    if (jogadorAtivo(equipe1) != NULL)
        renderPlayer(jogadorAtivo(equipe1), getFighterAssetsJogador(indiceAtivoEquipe(equipe1), 1), BLUE, "J1");
    if (jogadorAtivo(equipe2) != NULL)
        renderPlayer(jogadorAtivo(equipe2), getFighterAssetsJogador(indiceAtivoEquipe(equipe2), 2), RED, "J2");
    desenharHUD(equipe1, equipe2, nomeJ1, nomeJ2);
    desenharFiltroEspecial(equipe1, equipe2);
    desenharBotaoPausa();
}

static int verificarVencedorEquipes(EquipeJogador *equipe1, EquipeJogador *equipe2)
{
    if (!equipeTemVivos(equipe2))
        return 1;
    if (!equipeTemVivos(equipe1))
        return 2;
    return 0;
}

static void desenharFiltroEspecial(const EquipeJogador *equipe1, const EquipeJogador *equipe2)
{
    const Jogador *ativoJ1 = jogadorAtivoConst(equipe1);
    const Jogador *ativoJ2 = jogadorAtivoConst(equipe2);

    if (ativoJ1 != NULL && ativoJ1->specialAttackTicks > 0)
        DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, Fade(BLUE, 0.24f));

    if (ativoJ2 != NULL && ativoJ2->specialAttackTicks > 0)
        DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, Fade(RED, 0.24f));
}

static void processarAtaquesNoFrameDeImpacto(EquipeJogador *equipe1, EquipeJogador *equipe2)
{
    Jogador *ativoJ1 = jogadorAtivo(equipe1);
    Jogador *ativoJ2 = jogadorAtivo(equipe2);
    TipoPassinho ataqueJ1 = consumirAtaqueNoFrameDeImpacto(ativoJ1);
    TipoPassinho ataqueJ2 = consumirAtaqueNoFrameDeImpacto(ativoJ2);

    if (ataqueJ1 != PASSINHO_NENHUM)
        processarPassinho(ataqueJ1, ativoJ1, ativoJ2, &equipe1->energia);

    if (ataqueJ2 != PASSINHO_NENHUM)
        processarPassinho(ataqueJ2, ativoJ2, ativoJ1, &equipe2->energia);
}

static void finalizarPartida(int resultado, EquipeJogador *equipe1, EquipeJogador *equipe2,
                             Jogador **vencedorPartida, RegistroRanking ranking[],
                             int *totalRanking, const char *nomeJ1, const char *nomeJ2)
{
    EquipeJogador *equipeVencedora = (resultado == 1) ? equipe1 : equipe2;
    Jogador *perdedor = (resultado == 1) ? jogadorAtivo(equipe2) : jogadorAtivo(equipe1);
    const char *nomeVencedor = (resultado == 1) ? nomeJ1 : nomeJ2;

    *vencedorPartida = jogadorAtivo(equipeVencedora);
    if (perdedor != NULL && perdedor->hp <= 0)
    {
        perdedor->state = KNOCKDOWN;
        perdedor->stateTicks = 0;
    }

    registrarVitoriaRanking(ranking, totalRanking, MAX_RANKING, nomeVencedor);
    salvarRankingArquivo(ARQUIVO_RANKING, ranking, *totalRanking);
}

int executarJogo(void)
{
    InitWindow(LARGURA_TELA, ALTURA_TELA, TITULO_JANELA);
    SetTargetFPS(FPS_ALVO);

    AudioJogo audio;
    carregarAudioJogo(&audio);
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
    RegistroRanking ranking[MAX_RANKING];
    int totalRanking = carregarRankingArquivo(ARQUIVO_RANKING, ranking, MAX_RANKING);
    Jogador *vencedorRound = NULL;
    OpcaoPosPartida escolhaVitoriaJ1 = OPCAO_POS_REINICIAR;
    OpcaoPosPartida escolhaVitoriaJ2 = OPCAO_POS_REINICIAR;
    int confirmouVitoriaJ1 = 0;
    int confirmouVitoriaJ2 = 0;

    reiniciarSelecoes(&selecaoJ1, &selecaoJ2);

    while (!WindowShouldClose())
    {
        if (estado != ESTADO_PAUSA)
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

            if (IsKeyPressed(KEY_E))
                cenarioAtual = (IndiceCenario)((cenarioAtual + 1) % TOTAL_CENARIOS);
            if (IsKeyPressed(KEY_Q))
                cenarioAtual = (IndiceCenario)((cenarioAtual + TOTAL_CENARIOS - 1) % TOTAL_CENARIOS);

            if (selecaoJ1.confirmouTudo && selecaoJ2.confirmouTudo)
            {
                iniciarPartida(&equipe1, &equipe2, selecaoJ1.personagens, selecaoJ2.personagens);
                mostrarControles = 0;
                estado = ESTADO_COMBATE;
            }
            break;

        case ESTADO_COMBATE:
            if (botaoClicado(retanguloBotaoPausa()))
            {
                estado = ESTADO_PAUSA;
                break;
            }

            if (tickAtual % TICK_GANHO_ENERGIA == 0)
            {
                adicionarEnergia(&equipe1.energia, GANHO_ENERGIA_TEMPO);
                adicionarEnergia(&equipe2.energia, GANHO_ENERGIA_TEMPO);
            }

            if (IsKeyPressed(KEY_R))
                trocarParaProximoPersonagem(&equipe1);
            if (IsKeyPressed(KEY_P))
                trocarParaProximoPersonagem(&equipe2);

            {
                Jogador *ativoJ1 = jogadorAtivo(&equipe1);
                Jogador *ativoJ2 = jogadorAtivo(&equipe2);
                TipoPassinho ataqueSolicitadoJ1 = updatePlayer(ativoJ1, ativoJ2, CONTROLES_J1, &equipe1.energia);
                TipoPassinho ataqueSolicitadoJ2 = updatePlayer(ativoJ2, ativoJ1, CONTROLES_J2, &equipe2.energia);

                if (ataqueSolicitadoJ1 == ATAQUE_ESPECIAL || ataqueSolicitadoJ2 == ATAQUE_ESPECIAL)
                    tocarSomEspecial(&audio);
                processarAtaquesNoFrameDeImpacto(&equipe1, &equipe2);
            }
            trocarSeAtivoMorreu(&equipe1);
            trocarSeAtivoMorreu(&equipe2);

            {
                int resultado = verificarVencedorEquipes(&equipe1, &equipe2);

                if (resultado != 0)
                {
                    tocarSomVitoria(&audio);
                    finalizarPartida(resultado, &equipe1, &equipe2, &vencedorRound,
                                     ranking, &totalRanking, nomeJ1, nomeJ2);
                    ticksFimRound = KNOCKDOWN_DISPLAY_TICKS;
                    estado = ESTADO_FIM_ROUND;
                }
            }
            break;

        case ESTADO_PAUSA:
            if (botaoClicado(retanguloBotaoContinuar()))
                estado = ESTADO_COMBATE;
            else if (botaoClicado(retanguloBotaoMenu()))
                estado = ESTADO_MENU;
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
                    iniciarPartida(&equipe1, &equipe2, selecaoJ1.personagens, selecaoJ2.personagens);
                    vencedorRound = NULL;
                    estado = ESTADO_COMBATE;
                }
            }
            break;
        }

        atualizarAudioJogo(&audio);
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
            desenharCombateAtual(cenarioAtual, &equipe1, &equipe2, nomeJ1, nomeJ2);
            break;
        case ESTADO_PAUSA:
            desenharCombateAtual(cenarioAtual, &equipe1, &equipe2, nomeJ1, nomeJ2);
            desenharModalPausa();
            break;
        case ESTADO_VITORIA:
            desenharTelaVitoria(vencedorRound, nomeJ1, nomeJ2,
                                escolhaVitoriaJ1, escolhaVitoriaJ2,
                                confirmouVitoriaJ1, confirmouVitoriaJ2,
                                ranking, totalRanking);
            break;
        }

        EndDrawing();
    }

    descarregarAssetsLutadores();
    descarregarCenarios();
    descarregarAudioJogo(&audio);
    CloseWindow();
    return 0;
}
