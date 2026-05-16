#include "ui.h"
#include "combate.h"
#include "ordenacao.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* Cores temáticas */
#define COR_HP_J1       RED
#define COR_HP_J2       BLUE
#define COR_ENERGIA     YELLOW
#define COR_FUNDO_HUD   DARKGRAY

Texture2D bgMarcoZero;
Texture2D bgBoaViagem;
Texture2D bgJaqueira;
Texture2D bgMenu;
Texture2D bgPlayerSelect;

#define SELECT_J1_RETRATO_X 207
#define SELECT_J1_RETRATO_Y 234
#define SELECT_J2_RETRATO_X 816
#define SELECT_J2_RETRATO_Y 240
#define SELECT_RETRATO_LARGURA 258
#define SELECT_RETRATO_ALTURA 259
#define SELECT_J1_NOME_X 195
#define SELECT_J1_NOME_Y 580
#define SELECT_J2_NOME_X 804
#define SELECT_J2_NOME_Y 580
#define SELECT_NOME_LARGURA 288

static float distanciaJogadores(Jogador *jogador1, Jogador *jogador2) {
    float dx = jogador1->posX - jogador2->posX;
    float dy = jogador1->posY - jogador2->posY;
    return sqrtf(dx * dx + dy * dy);
}


/*
 * Desenha o HUD completo: barras de HP, energia, placar de rounds e round atual.
 */

void carregarCenarios() {
    bgMarcoZero = LoadTexture("assets/backgrounds/marco_zero.jpg");
    bgBoaViagem = LoadTexture("assets/backgrounds/boa_viagem.png");
    bgJaqueira  = LoadTexture("assets/backgrounds/jaqueira.png");
    bgMenu      = LoadTexture("assets/backgrounds/menu.png");
    bgPlayerSelect = LoadTexture("assets/backgrounds/player_select.png");
}

/* Chama isso no fechamento do jogo, antes do CloseWindow() */
void descarregarCenarios() {
    UnloadTexture(bgMarcoZero);
    UnloadTexture(bgBoaViagem);
    UnloadTexture(bgJaqueira);
    UnloadTexture(bgMenu);
    UnloadTexture(bgPlayerSelect);
}


void desenharHUD(Jogador *jogador1, Jogador *jogador2, int roundAtual, int segundosRestantes) {
    int larguraBarra = 400;
    int alturaBarra  = 25;
    int margem       = 20;

    /* -- HP Jogador 1 (esquerda) -- */
    DrawRectangle(margem, margem, larguraBarra, alturaBarra, DARKGRAY);
    int hpJ1 = (jogador1->hp * larguraBarra) / jogador1->personagem.hpMaximo;
    DrawRectangle(margem, margem, hpJ1, alturaBarra, COR_HP_J1);
    DrawText(jogador1->personagem.nome, margem, margem + alturaBarra + 5, 18, WHITE);

    /* -- HP Jogador 2 (direita) -- */
    int xJ2 = LARGURA_TELA - margem - larguraBarra;
    DrawRectangle(xJ2, margem, larguraBarra, alturaBarra, DARKGRAY);
    int hpJ2 = (jogador2->hp * larguraBarra) / jogador2->personagem.hpMaximo;
    DrawRectangle(xJ2 + (larguraBarra - hpJ2), margem, hpJ2, alturaBarra, COR_HP_J2);
    DrawText(jogador2->personagem.nome, xJ2, margem + alturaBarra + 5, 18, WHITE);

    /* -- Energia Jogador 1 -- */
    int larguraEnergia = 200;
    DrawRectangle(margem, margem + alturaBarra + 30, larguraEnergia, 15, DARKGRAY);
    int energiaJ1 = (jogador1->energia * larguraEnergia) / MAX_ENERGIA;
    DrawRectangle(margem, margem + alturaBarra + 30, energiaJ1, 15, COR_ENERGIA);
    DrawText(TextFormat("Energia: %d/%d", jogador1->energia, MAX_ENERGIA),
             margem, margem + alturaBarra + 48, 16, WHITE);

    /* -- Energia Jogador 2 -- */
    DrawRectangle(xJ2 + larguraBarra - larguraEnergia, margem + alturaBarra + 30, larguraEnergia, 15, DARKGRAY);
    int energiaJ2 = (jogador2->energia * larguraEnergia) / MAX_ENERGIA;
    DrawRectangle(xJ2 + larguraBarra - energiaJ2, margem + alturaBarra + 30, energiaJ2, 15, COR_ENERGIA);
    DrawText(TextFormat("Energia: %d/%d", jogador2->energia, MAX_ENERGIA),
             xJ2 + larguraBarra - larguraEnergia, margem + alturaBarra + 48, 16, WHITE);

    /* -- Round atual no centro -- */
    char textoRound[20];
    sprintf(textoRound, "ROUND %d", roundAtual);
    int larguraTexto = MeasureText(textoRound, 28);
    DrawText(textoRound, LARGURA_TELA / 2 - larguraTexto / 2, margem, 28, WHITE);

    char textoTempo[8];
    snprintf(textoTempo, sizeof(textoTempo), "%02d", segundosRestantes);
    int larguraTempo = MeasureText(textoTempo, 34);
    DrawText(textoTempo, LARGURA_TELA / 2 - larguraTempo / 2, margem + 28, 34,
             segundosRestantes <= 10 ? RED : WHITE);

    /* -- Placar de rounds -- */
    char placar[20];
    sprintf(placar, "%d  x  %d", jogador1->roundsVencidos, jogador2->roundsVencidos);
    int larguraPlacar = MeasureText(placar, 24);
    DrawText(placar, LARGURA_TELA / 2 - larguraPlacar / 2, margem + 62, 24, YELLOW);

    float distancia = distanciaJogadores(jogador1, jogador2);
    const char *alcance = distancia <= DISTANCIA_MAXIMA_ATAQUE ? "NO ALCANCE" : "LONGE";
    Color corAlcance = distancia <= DISTANCIA_MAXIMA_ATAQUE ? GREEN : RED;
    const char *textoAlcance = TextFormat("Distancia: %.0f px | %s", distancia, alcance);
    DrawText(textoAlcance, LARGURA_TELA / 2 - MeasureText(textoAlcance, 18) / 2, margem + 92, 18, corAlcance);

    DrawText(TextFormat("Fila: %d | Stun: %d | Esq: %d", jogador1->fila.tamanho, jogador1->stunTicks, jogador1->esquivaCooldown),
             margem, margem + alturaBarra + 68, 16, jogador1->stunTicks > 0 ? ORANGE : LIGHTGRAY);
    DrawText(TextFormat("Fila: %d | Stun: %d | Esq: %d", jogador2->fila.tamanho, jogador2->stunTicks, jogador2->esquivaCooldown),
             xJ2 + larguraBarra - larguraEnergia, margem + alturaBarra + 68, 16,
             jogador2->stunTicks > 0 ? ORANGE : LIGHTGRAY);
}

/*
 * Desenha o cenário de fundo.
 * TODO: carregar imagem de fundo (Marco Zero, Boa Viagem, etc.)
 */
void desenharCenario(Texture2D background) {
    DrawTexturePro(
        background,
        (Rectangle){ 0, 0, background.width, background.height },
        (Rectangle){ 0, 0, LARGURA_TELA, ALTURA_TELA },
        (Vector2){ 0, 0 },
        0.0f,
        WHITE
    );
}

/*
 * Desenha o menu principal com as opções de início.
 */
void desenharMenuPrincipal(Texture2D background) {
    DrawTexturePro(
        background,
        (Rectangle){ 0, 0, background.width, background.height },
        (Rectangle){ 0, 0, LARGURA_TELA, ALTURA_TELA },
        (Vector2){ 0, 0 }, 0.0f, WHITE
    );
    int cx = LARGURA_TELA / 2;
    DrawText("BATALHA DO PASSINHO", cx - MeasureText("BATALHA DO PASSINHO", 48) / 2, 150, 48, RED);
    DrawText("Na Vibe do Brega Funk Recifense", cx - MeasureText("Na Vibe do Brega Funk Recifense", 22) / 2, 220, 22, YELLOW);
    DrawText("Pressione ENTER para jogar", cx - MeasureText("Pressione ENTER para jogar", 24) / 2, 380, 24, WHITE);
    DrawText("ESC para sair", cx - MeasureText("ESC para sair", 20) / 2, 430, 20, GRAY);
}

static void desenharRetratoSelecao(const FighterAssets *assets, Rectangle destino, Color corFallback, const char *rotulo)
{
    Texture2D retrato = getPortraitForFighter(assets);

    if (retrato.id != 0)
    {
        Rectangle origem = (Rectangle){0, 0, (float)retrato.width, (float)retrato.height};

        if (assets->idle.totalFrames > 0 && retrato.id == assets->idle.frames[0].id)
            origem = assets->idle.sources[0];

        DrawTexturePro(
            retrato,
            origem,
            destino,
            (Vector2){0, 0},
            0.0f,
            WHITE
        );
        return;
    }

    DrawRectangleRec(destino, Fade(corFallback, 0.35f));
    DrawRectangleLinesEx(destino, 3, corFallback);
    DrawText(rotulo, (int)destino.x + 24, (int)destino.y + (int)destino.height / 2 - 16, 32, WHITE);
}

static void desenharNomeCentralizado(const char *nome, int x, int y, int largura, Color cor)
{
    int fonte = 30;
    int textoLargura = MeasureText(nome, fonte);

    while (textoLargura > largura && fonte > 18)
    {
        fonte -= 2;
        textoLargura = MeasureText(nome, fonte);
    }

    DrawText(nome, x + (largura - textoLargura) / 2, y, fonte, cor);
}

/*
 * Desenha a tela de seleção usando o fundo pronto e retratos dinâmicos.
 */
void desenharSelecaoPersonagem(int selecaoJ1, int selecaoJ2, int cenarioAtual) {
    const char *cenarios[] = { "Marco Zero", "Praia de Boa Viagem", "Parque da Jaqueira" };
    Personagem personagemJ1 = getPersonagem(selecaoJ1);
    Personagem personagemJ2 = getPersonagem(selecaoJ2);
    const FighterAssets *assetsJ1 = getFighterAssets(selecaoJ1);
    const FighterAssets *assetsJ2 = getFighterAssets(selecaoJ2);

    if (bgPlayerSelect.id != 0)
    {
        DrawTexturePro(
            bgPlayerSelect,
            (Rectangle){0, 0, (float)bgPlayerSelect.width, (float)bgPlayerSelect.height},
            (Rectangle){0, 0, LARGURA_TELA, ALTURA_TELA},
            (Vector2){0, 0},
            0.0f,
            WHITE
        );
    }
    else
    {
        ClearBackground(BLACK);
    }

    desenharRetratoSelecao(
        assetsJ1,
        (Rectangle){SELECT_J1_RETRATO_X, SELECT_J1_RETRATO_Y, SELECT_RETRATO_LARGURA, SELECT_RETRATO_ALTURA},
        RED,
        "J1"
    );
    desenharRetratoSelecao(
        assetsJ2,
        (Rectangle){SELECT_J2_RETRATO_X, SELECT_J2_RETRATO_Y, SELECT_RETRATO_LARGURA, SELECT_RETRATO_ALTURA},
        BLUE,
        "J2"
    );

    desenharNomeCentralizado(personagemJ1.nome, SELECT_J1_NOME_X, SELECT_J1_NOME_Y, SELECT_NOME_LARGURA, RED);
    desenharNomeCentralizado(personagemJ2.nome, SELECT_J2_NOME_X, SELECT_J2_NOME_Y, SELECT_NOME_LARGURA, BLUE);

    DrawText("<", SELECT_J1_NOME_X - 35, SELECT_J1_NOME_Y, 32, RED);
    DrawText(">", SELECT_J1_NOME_X + SELECT_NOME_LARGURA + 15, SELECT_J1_NOME_Y, 32, RED);
    DrawText("<", SELECT_J2_NOME_X - 35, SELECT_J2_NOME_Y, 32, BLUE);
    DrawText(">", SELECT_J2_NOME_X + SELECT_NOME_LARGURA + 15, SELECT_J2_NOME_Y, 32, BLUE);

    DrawText(TextFormat("Cenario: %s", cenarios[cenarioAtual]), 500, 650, 20, ORANGE);
}

/*
 * Desenha a tela de resultado do round com estatísticas ordenadas.
 */
void desenharResultadoRound(Jogador *vencedor, Estatistica *stats, int totalStats) {
    ClearBackground(BLACK);

    char titulo[80];
    if (vencedor != NULL)
        snprintf(titulo, sizeof(titulo), "%s VENCEU O ROUND!", vencedor->personagem.nome);
    else
        snprintf(titulo, sizeof(titulo), "ROUND EMPATADO!");
    DrawText(titulo, LARGURA_TELA / 2 - MeasureText(titulo, 30) / 2, 100, 30, YELLOW);
    DrawText("ESTATISTICAS", LARGURA_TELA / 2 - 80, 180, 26, WHITE);

    for (int i = 0; i < totalStats; i++) {
        int y = 230 + i * 100;
        DrawText(stats[i].nomePersonagem, 200, y,      22, i == 0 ? GOLD : LIGHTGRAY);
        DrawText(TextFormat("Dano: %d",    stats[i].danoTotal),          200, y + 28, 18, WHITE);
        DrawText(TextFormat("Combos: %d",  stats[i].combosExecutados),   200, y + 50, 18, WHITE);
        DrawText(TextFormat("Esquivas: %d",stats[i].esquivasRealizadas), 200, y + 72, 18, WHITE);
    }

    DrawText("Pressione ENTER para continuar", LARGURA_TELA / 2 - 180, ALTURA_TELA - 80, 22, GRAY);
}

/*
 * Desenha a tela final de vitória da partida.
 */
void desenharTelaVitoria(Jogador *vencedor) {
    ClearBackground(BLACK);
    char msg[80];
    sprintf(msg, "%s E O REI DO PASSINHO!", vencedor->personagem.nome);
    DrawText(msg, LARGURA_TELA / 2 - MeasureText(msg, 34) / 2, 250, 34, GOLD);
    DrawText("Pressione ENTER para jogar novamente", LARGURA_TELA / 2 - 220, 380, 22, WHITE);
    DrawText("ESC para sair", LARGURA_TELA / 2 - 70, 420, 22, GRAY);
}
