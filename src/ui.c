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
}

/* Chama isso no fechamento do jogo, antes do CloseWindow() */
void descarregarCenarios() {
    UnloadTexture(bgMarcoZero);
    UnloadTexture(bgBoaViagem);
    UnloadTexture(bgJaqueira);
    UnloadTexture(bgMenu);
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

/*
 * Desenha a tela de seleção de personagem para os dois jogadores.
 */
void desenharSelecaoPersonagem(int selecaoJ1, int selecaoJ2, int cenarioAtual) {
    const char *nomes[TOTAL_PERSONAGENS] = {
        "Joao Campos", "Magrao", "Kuki",
        "Grafite", "Clarisse Lispector", "Ariano Suassuna", "Tojal"
    };
    const char *cenarios[] = { "Marco Zero", "Praia de Boa Viagem", "Parque da Jaqueira" };

    ClearBackground(BLACK);
    DrawText("ESCOLHA SEU PERSONAGEM", LARGURA_TELA / 2 - 200, 50, 32, YELLOW);

    /* Cenário selecionado */
    DrawText("Cenario:", 50, 120, 20, GRAY);
    DrawText(cenarios[cenarioAtual], 140, 120, 20, ORANGE);
    DrawText("J1: Q/E para mudar cenario", 50, 145, 16, GRAY);

    DrawText("J1: A/D para navegar | ENTER confirma", 50, ALTURA_TELA - 80, 18, RED);
    DrawText("J2: SETA ESQ/DIR para navegar | NUMPAD 1 confirma", 50, ALTURA_TELA - 50, 18, BLUE);

    for (int i = 0; i < TOTAL_PERSONAGENS; i++) {
        Color cor = WHITE;
        if (i == selecaoJ1) cor = RED;
        if (i == selecaoJ2) cor = BLUE;
        if (i == selecaoJ1 && i == selecaoJ2) cor = PURPLE;
        DrawText(nomes[i], 100 + (i % 4) * 280, 200 + (i / 4) * 120, 22, cor);
    }
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
