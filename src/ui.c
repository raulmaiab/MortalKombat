#include "ui.h"
#include "combate.h"
#include "ordenacao.h"
#include "raylib.h"
#include "selecao_personagens.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* Cores temáticas */
#define COR_HP_J1       BLUE
#define COR_HP_J2       RED
#define COR_ENERGIA     YELLOW
#define COR_FUNDO_HUD   DARKGRAY

Texture2D bgMarcoZero;
Texture2D bgBoaViagem;
Texture2D bgJaqueira;
Texture2D bgMenu;
Texture2D bgPlayerSelect;

#define SELECT_SLOTS_POR_JOGADOR 3
#define SELECT_RETRATO_LARGURA 292
#define SELECT_RETRATO_ALTURA 122
#define SELECT_NOME_LARGURA 270

static const Rectangle SELECT_SLOTS_J1[SELECT_SLOTS_POR_JOGADOR] = {
    {67, 88, SELECT_RETRATO_LARGURA, SELECT_RETRATO_ALTURA},
    {67, 309, SELECT_RETRATO_LARGURA, SELECT_RETRATO_ALTURA},
    {67, 525, SELECT_RETRATO_LARGURA, SELECT_RETRATO_ALTURA}
};

static const Rectangle SELECT_SLOTS_J2[SELECT_SLOTS_POR_JOGADOR] = {
    {960, 88, SELECT_RETRATO_LARGURA, SELECT_RETRATO_ALTURA},
    {960, 309, SELECT_RETRATO_LARGURA, SELECT_RETRATO_ALTURA},
    {960, 525, SELECT_RETRATO_LARGURA, SELECT_RETRATO_ALTURA}
};

static const Vector2 SELECT_NOMES_J1[SELECT_SLOTS_POR_JOGADOR] = {
    {73, 234},
    {73, 453},
    {73, 670}
};

static const Vector2 SELECT_NOMES_J2[SELECT_SLOTS_POR_JOGADOR] = {
    {965, 234},
    {965, 453},
    {965, 670}
};

#define COR_AMARELO_ESCURO (Color){150, 132, 0, 255}
#define COR_LARANJA_ESCURO (Color){150, 82, 0, 255}
#define COR_CINZA_ESCURO   (Color){105, 105, 105, 255}

static void desenharTextoComSombra(const char *texto, int x, int y, int tamanho, Color cor)
{
    DrawText(texto, x + 3, y + 3, tamanho, Fade(BLACK, 0.9f));
    DrawText(texto, x, y, tamanho, cor);
}

static void desenharTextoCentralizadoComSombra(const char *texto, int centroX, int y, int tamanho, Color cor)
{
    int largura = MeasureText(texto, tamanho);
    desenharTextoComSombra(texto, centroX - largura / 2, y, tamanho, cor);
}

static float distanciaJogadores(const Jogador *jogador1, const Jogador *jogador2) {
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
    bgPlayerSelect = LoadTexture("assets/backgrounds/player_select3.png");
}

/* Chama isso no fechamento do jogo, antes do CloseWindow() */
void descarregarCenarios() {
    UnloadTexture(bgMarcoZero);
    UnloadTexture(bgBoaViagem);
    UnloadTexture(bgJaqueira);
    UnloadTexture(bgMenu);
    UnloadTexture(bgPlayerSelect);
}


static void desenharPortraitPequeno(int indicePersonagem, Rectangle destino)
{
    const FighterAssets *assets = getFighterAssets(indicePersonagem);
    Texture2D retrato = getPortraitForFighter(assets);

    if (retrato.id == 0)
    {
        DrawRectangleRec(destino, DARKGRAY);
        return;
    }

    DrawTexturePro(
        retrato,
        (Rectangle){0, 0, (float)retrato.width, (float)retrato.height},
        destino,
        (Vector2){0, 0},
        0.0f,
        WHITE
    );
}

static void desenharBarraPersonagemEquipe(const NoPersonagem *no, int x, int y, int largura,
                                          int altura, Color cor, int alinhadoDireita, int ativo)
{
    int portrait = ativo ? 34 : 26;
    int barraX = alinhadoDireita ? x : x + portrait + 6;
    int retratoX = alinhadoDireita ? x + largura - portrait : x;
    int larguraBarra = largura - portrait - 6;
    int hp = no->jogador.hp * larguraBarra / no->jogador.personagem.hpMaximo;
    Color corHp = no->vivo ? cor : GRAY;

    desenharPortraitPequeno(no->indicePersonagem, (Rectangle){retratoX, y, portrait, altura});
    DrawRectangle(barraX, y, larguraBarra, altura, DARKGRAY);

    if (alinhadoDireita)
        DrawRectangle(barraX + (larguraBarra - hp), y, hp, altura, corHp);
    else
        DrawRectangle(barraX, y, hp, altura, corHp);

    DrawRectangleLines(barraX, y, larguraBarra, altura, ativo ? WHITE : LIGHTGRAY);
    if (ativo)
        DrawText(no->jogador.personagem.nome, barraX, y + altura + 3, 16, WHITE);
}

static void desenharHUDJogador(const EquipeJogador *equipe, int x, int y, Color cor, int alinhadoDireita)
{
    int reservaY = y;
    int reservasDesenhadas = 0;

    for (int i = 0; i < TAM_EQUIPE; i++)
    {
        const NoPersonagem *no = &equipe->membros[i];
        if (&equipe->membros[i] == equipe->inicio)
            continue;

        desenharBarraPersonagemEquipe(no, x, reservaY + reservasDesenhadas * 30, 300, 22, cor, alinhadoDireita, 0);
        reservasDesenhadas++;
    }

    if (equipe->inicio != NULL)
    {
        desenharBarraPersonagemEquipe(equipe->inicio, x, y + 64, 400, 28, cor, alinhadoDireita, 1);
        DrawRectangle(x + (alinhadoDireita ? 120 : 0), y + 112, 200, 15, DARKGRAY);
        int energia = equipe->inicio->jogador.energia * 200 / MAX_ENERGIA;
        if (alinhadoDireita)
            DrawRectangle(x + 120 + (200 - energia), y + 112, energia, 15, COR_ENERGIA);
        else
            DrawRectangle(x, y + 112, energia, 15, COR_ENERGIA);
        DrawText(TextFormat("Energia: %d%%", equipe->inicio->jogador.energia),
                 x + (alinhadoDireita ? 120 : 0), y + 130, 16, WHITE);
    }
}

void desenharHUD(const EquipeJogador *equipe1, const EquipeJogador *equipe2) {
    int margem = 20;

    desenharHUDJogador(equipe1, margem, margem, COR_HP_J1, 0);
    desenharHUDJogador(equipe2, LARGURA_TELA - margem - 400, margem, COR_HP_J2, 1);

    desenharTextoCentralizadoComSombra("ELIMINE OS 3 PERSONAGENS", LARGURA_TELA / 2, margem, 22, WHITE);
    desenharTextoCentralizadoComSombra(TextFormat("%d vivos  x  %d vivos", equipe1->tamanho, equipe2->tamanho),
                                       LARGURA_TELA / 2, margem + 30, 20, WHITE);

    if (equipe1->inicio == NULL || equipe2->inicio == NULL)
        return;

    float distancia = distanciaJogadores(&equipe1->inicio->jogador, &equipe2->inicio->jogador);
    const char *alcance = distancia <= DISTANCIA_MAXIMA_ATAQUE ? "NO ALCANCE" : "LONGE";
    Color corAlcance = distancia <= DISTANCIA_MAXIMA_ATAQUE ? GREEN : RED;
    const char *textoAlcance = TextFormat("Distancia: %.0f px | %s", distancia, alcance);
    desenharTextoCentralizadoComSombra(textoAlcance, LARGURA_TELA / 2, margem + 62, 18, corAlcance);

    DrawText(TextFormat("Fila: %d | Stun: %d", equipe1->tamanho, equipe1->inicio->jogador.stunTicks),
             margem, margem + 150, 16, equipe1->inicio->jogador.stunTicks > 0 ? ORANGE : LIGHTGRAY);
    DrawText(TextFormat("Fila: %d | Stun: %d", equipe2->tamanho, equipe2->inicio->jogador.stunTicks),
             LARGURA_TELA - margem - 190, margem + 150, 16,
             equipe2->inicio->jogador.stunTicks > 0 ? ORANGE : LIGHTGRAY);
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
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, Fade(BLACK, 0.5f));

    int cx = LARGURA_TELA / 2;
    DrawRectangleRounded((Rectangle){310, 125, 660, 335}, 0.05f, 10, Fade(BLACK, 0.35f));
    desenharTextoCentralizadoComSombra("BATALHA DO PASSINHO", cx, 150, 48, RED);
    desenharTextoCentralizadoComSombra("Na Vibe do Brega Funk Recifense", cx, 220, 22, COR_AMARELO_ESCURO);
    desenharTextoCentralizadoComSombra("Pressione ENTER para jogar", cx, 380, 24, WHITE);
    desenharTextoCentralizadoComSombra("ESC para sair", cx, 430, 20, COR_CINZA_ESCURO);
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

    desenharTextoComSombra(nome, x + (largura - textoLargura) / 2, y, fonte, cor);
}

static void desenharSlotSelecao(int indicePersonagem, Rectangle slot, Vector2 posNome,
                                Color cor, const char *rotulo, int ativo, int bloqueado)
{
    Personagem personagem = getPersonagem(indicePersonagem);
    const FighterAssets *assets = getFighterAssets(indicePersonagem);

    desenharRetratoSelecao(assets, slot, cor, rotulo);
    desenharNomeCentralizado(personagem.nome, (int)posNome.x, (int)posNome.y, SELECT_NOME_LARGURA, cor);

    if (ativo && !bloqueado)
        DrawRectangleLinesEx(slot, 4, WHITE);
    else if (bloqueado)
        DrawRectangleLinesEx(slot, 3, Fade(GRAY, 0.75f));
}

static void desenharSlotsSelecao(const int selecoes[],
                                 const Rectangle *slots, const Vector2 *nomes,
                                 int slotAtual, int confirmouTudo,
                                 Color cor, const char *rotulo)
{
    for (int i = 0; i < SELECT_SLOTS_POR_JOGADOR; i++)
    {
        desenharSlotSelecao(selecoes[i], slots[i], nomes[i], cor, rotulo,
                            i == slotAtual, confirmouTudo);
    }
}

static void desenharAbaControles(void)
{
    Rectangle aba = {545, 675, 190, 34};

    DrawRectangleRounded(aba, 0.18f, 8, Fade(BLACK, 0.82f));
    DrawRectangleRoundedLines(aba, 0.18f, 8, Fade(COR_CINZA_ESCURO, 0.75f));
    desenharTextoComSombra("I", 567, 683, 18, COR_AMARELO_ESCURO);
    desenharTextoComSombra("Controles", 595, 683, 18, WHITE);
}

static void desenharPainelControles(void)
{
    Rectangle painel = {260, 105, 760, 495};
    int y = 170;

    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, Fade(BLACK, 0.68f));
    DrawRectangleRounded(painel, 0.04f, 10, Fade((Color){8, 8, 12, 255}, 0.98f));
    DrawRectangleRoundedLines(painel, 0.04f, 10, Fade(COR_AMARELO_ESCURO, 0.7f));

    desenharTextoComSombra("CONTROLES", 535, 130, 28, COR_AMARELO_ESCURO);
    desenharTextoComSombra("Pressione I para fechar", 548, 560, 18, WHITE);

    desenharTextoComSombra("JOGADOR 1", 330, y, 22, BLUE);
    desenharTextoComSombra("Mover: A / D", 330, y + 40, 20, WHITE);
    desenharTextoComSombra("Pular: W", 330, y + 72, 20, WHITE);
    desenharTextoComSombra("Agachar: S", 330, y + 104, 20, WHITE);
    desenharTextoComSombra("Defender: F ou E", 330, y + 136, 20, WHITE);
    desenharTextoComSombra("Ataques: G / T", 330, y + 168, 20, WHITE);
    desenharTextoComSombra("Trocar: R", 330, y + 200, 20, WHITE);
    desenharTextoComSombra("Confirmar: ENTER", 330, y + 232, 20, WHITE);

    desenharTextoComSombra("JOGADOR 2", 705, y, 22, RED);
    desenharTextoComSombra("Mover: SETAS", 705, y + 40, 20, WHITE);
    desenharTextoComSombra("Pular: CIMA", 705, y + 72, 20, WHITE);
    desenharTextoComSombra("Agachar: BAIXO", 705, y + 104, 20, WHITE);
    desenharTextoComSombra("Defender: SHIFT DIR", 705, y + 136, 20, WHITE);
    desenharTextoComSombra("Ataques: J / L", 705, y + 168, 20, WHITE);
    desenharTextoComSombra("Trocar: P", 705, y + 200, 20, WHITE);
    desenharTextoComSombra("Confirmar: L ou SHIFT", 705, y + 232, 20, WHITE);

    desenharTextoComSombra("Cenario: Q / E", 545, 470, 20, COR_LARANJA_ESCURO);
    desenharTextoComSombra("Alternativas J2: INSERT e KP_1 / KP_3", 405, 510, 18, WHITE);
}

/*
 * Desenha a tela de seleção usando o fundo pronto e retratos dinâmicos.
 */
void desenharSelecaoPersonagem(const int selecoesJ1[], int slotAtualJ1, int confirmouJ1,
                               const int selecoesJ2[], int slotAtualJ2, int confirmouJ2,
                               int cenarioAtual, int mostrarControles) {
    const char *cenarios[] = { "Marco Zero", "Praia de Boa Viagem", "Parque da Jaqueira" };

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

    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, Fade(BLACK, 0.18f));

    desenharSlotsSelecao(selecoesJ1, SELECT_SLOTS_J1, SELECT_NOMES_J1, slotAtualJ1, confirmouJ1, BLUE, "J1");
    desenharSlotsSelecao(selecoesJ2, SELECT_SLOTS_J2, SELECT_NOMES_J2, slotAtualJ2, confirmouJ2, RED, "J2");

    if (!confirmouJ1)
    {
        desenharTextoComSombra("<", (int)SELECT_NOMES_J1[slotAtualJ1].x - 25, (int)SELECT_NOMES_J1[slotAtualJ1].y, 32, BLUE);
        desenharTextoComSombra(">", (int)SELECT_NOMES_J1[slotAtualJ1].x + SELECT_NOME_LARGURA + 15, (int)SELECT_NOMES_J1[slotAtualJ1].y, 32, BLUE);
    }
    if (!confirmouJ2)
    {
        desenharTextoComSombra("<", (int)SELECT_NOMES_J2[slotAtualJ2].x - 25, (int)SELECT_NOMES_J2[slotAtualJ2].y, 32, RED);
        desenharTextoComSombra(">", (int)SELECT_NOMES_J2[slotAtualJ2].x + SELECT_NOME_LARGURA + 15, (int)SELECT_NOMES_J2[slotAtualJ2].y, 32, RED);
    }

    {
        const char *textoCenario = TextFormat("Cenario: %s", cenarios[cenarioAtual]);
        int larguraCenario = MeasureText(textoCenario, 20);
        desenharTextoComSombra(textoCenario, LARGURA_TELA / 2 - larguraCenario / 2, 645, 20, WHITE);
    }
    desenharAbaControles();

    if (mostrarControles)
        desenharPainelControles();
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
