#include "raylib.h"
#include "fila.h"
#include "jogador.h"
#include "combate.h"
#include "ordenacao.h"
#include "ui.h"
#include <string.h>

typedef enum {
    ESTADO_MENU,
    ESTADO_SELECAO,
    ESTADO_COMBATE,
    ESTADO_RESULTADO_ROUND,
    ESTADO_VITORIA
} EstadoJogo;

typedef enum {
    CENARIO_MARCO_ZERO,
    CENARIO_BOA_VIAGEM,
    CENARIO_JAQUEIRA,
    TOTAL_CENARIOS
} IndiciceCenario;

#define TICK_ATAQUE 8

/* Texturas globais dos cenários — acessadas em ui.c via extern */
extern Texture2D bgMarcoZero;
extern Texture2D bgBoaViagem;
extern Texture2D bgJaqueira;
extern Texture2D bgMenu;

int main(void) {
    InitWindow(LARGURA_TELA, ALTURA_TELA, TITULO_JANELA);
    SetTargetFPS(FPS_ALVO);

    carregarCenarios();

    EstadoJogo    estado      = ESTADO_MENU;
    IndiciceCenario cenarioAtual = CENARIO_MARCO_ZERO;
    int tickAtual  = 0;
    int roundAtual = 1;

    int selecaoJ1  = JOAO_CAMPOS;
    int selecaoJ2  = GRAFITE;
    int confirmouJ1 = 0, confirmouJ2 = 0;

    Jogador jogador1, jogador2;
    Estatistica statsRound[2];
    Jogador *vencedorRound = NULL;

    while (!WindowShouldClose()) {
        tickAtual++;

        switch (estado) {

            case ESTADO_MENU:
                if (IsKeyPressed(KEY_ENTER)) estado = ESTADO_SELECAO;
                break;

            case ESTADO_SELECAO:
                if (IsKeyPressed(KEY_D) && selecaoJ1 < TOTAL_PERSONAGENS - 1) selecaoJ1++;
                if (IsKeyPressed(KEY_A) && selecaoJ1 > 0)                     selecaoJ1--;
                if (IsKeyPressed(KEY_ENTER)) confirmouJ1 = 1;

                if (IsKeyPressed(KEY_RIGHT) && selecaoJ2 < TOTAL_PERSONAGENS - 1) selecaoJ2++;
                if (IsKeyPressed(KEY_LEFT)  && selecaoJ2 > 0)                     selecaoJ2--;
                if (IsKeyPressed(KEY_KP_1)) confirmouJ2 = 1;

                /* Escolha de cenário — J1 usa W/S para selecionar */
                if (IsKeyPressed(KEY_E) && cenarioAtual < TOTAL_CENARIOS - 1) cenarioAtual++;
                if (IsKeyPressed(KEY_Q) && cenarioAtual > 0)                  cenarioAtual--;

                if (confirmouJ1 && confirmouJ2) {
                    inicializarJogador(&jogador1, selecaoJ1, 200, ALTURA_TELA / 2, 1);
                    inicializarJogador(&jogador2, selecaoJ2, LARGURA_TELA - 260, ALTURA_TELA / 2, 0);
                    memset(statsRound, 0, sizeof(statsRound));
                    strncpy(statsRound[0].nomePersonagem, jogador1.personagem.nome, MAX_NOME);
                    strncpy(statsRound[1].nomePersonagem, jogador2.personagem.nome, MAX_NOME);
                    roundAtual  = 1;
                    confirmouJ1 = 0;
                    confirmouJ2 = 0;
                    estado      = ESTADO_COMBATE;
                }
                break;

            case ESTADO_COMBATE:
                /* Inputs J1 */
                if (IsKeyPressed(KEY_G)) enfileirarPassinho(&jogador1.fila, ATAQUE_LEVE);
                if (IsKeyPressed(KEY_H)) enfileirarPassinho(&jogador1.fila, ATAQUE_MEDIO);
                if (IsKeyPressed(KEY_Y)) enfileirarPassinho(&jogador1.fila, ATAQUE_ESPECIAL);
                if (IsKeyPressed(KEY_S)) enfileirarPassinho(&jogador1.fila, ESQUIVA);

                if (IsKeyDown(KEY_A) && jogador1.posX > 0)                 jogador1.posX -= 4;
                if (IsKeyDown(KEY_D) && jogador1.posX < LARGURA_TELA - 60) jogador1.posX += 4;
                if (IsKeyDown(KEY_W) && jogador1.posY > ALTURA_TELA / 2)   jogador1.posY -= 4;

                /* Inputs J2 */
                if (IsKeyPressed(KEY_KP_1)) enfileirarPassinho(&jogador2.fila, ATAQUE_LEVE);
                if (IsKeyPressed(KEY_KP_2)) enfileirarPassinho(&jogador2.fila, ATAQUE_MEDIO);
                if (IsKeyPressed(KEY_KP_3)) enfileirarPassinho(&jogador2.fila, ATAQUE_ESPECIAL);
                if (IsKeyPressed(KEY_DOWN)) enfileirarPassinho(&jogador2.fila, ESQUIVA);

                if (IsKeyDown(KEY_LEFT)  && jogador2.posX > 0)                 jogador2.posX -= 4;
                if (IsKeyDown(KEY_RIGHT) && jogador2.posX < LARGURA_TELA - 60) jogador2.posX += 4;
                if (IsKeyDown(KEY_UP)    && jogador2.posY > ALTURA_TELA / 2)   jogador2.posY -= 4;

                /* Processamento da fila */
                if (tickAtual % TICK_ATAQUE == 0) {
                    if (!filaVazia(&jogador1.fila)) {
                        TipoPassinho p = desenfileirarPassinho(&jogador1.fila);
                        if (p != ESQUIVA)
                            processarPassinho(p, &jogador1, &jogador2, statsRound);
                    }
                    if (!filaVazia(&jogador2.fila)) {
                        TipoPassinho p = desenfileirarPassinho(&jogador2.fila);
                        if (p != ESQUIVA) {
                            Estatistica statsInvertido[2] = { statsRound[1], statsRound[0] };
                            processarPassinho(p, &jogador2, &jogador1, statsInvertido);
                            statsRound[0] = statsInvertido[1];
                            statsRound[1] = statsInvertido[0];
                        }
                    }
                }

                int resultado = verificarVencedor(&jogador1, &jogador2);
                if (resultado != 0) {
                    vencedorRound = (resultado == 1) ? &jogador1 : &jogador2;
                    encerrarRound(vencedorRound, statsRound);
                    ordenarEstatisticas(statsRound, 2);
                    estado = ESTADO_RESULTADO_ROUND;
                }
                break;

            case ESTADO_RESULTADO_ROUND:
                if (IsKeyPressed(KEY_ENTER)) {
                    if (jogador1.roundsVencidos >= 2 || jogador2.roundsVencidos >= 2) {
                        estado = ESTADO_VITORIA;
                    } else {
                        roundAtual++;
                        resetarJogador(&jogador1);
                        resetarJogador(&jogador2);
                        memset(statsRound, 0, sizeof(statsRound));
                        strncpy(statsRound[0].nomePersonagem, jogador1.personagem.nome, MAX_NOME);
                        strncpy(statsRound[1].nomePersonagem, jogador2.personagem.nome, MAX_NOME);
                        estado = ESTADO_COMBATE;
                    }
                }
                break;

            case ESTADO_VITORIA:
                if (IsKeyPressed(KEY_ENTER)) {
                    selecaoJ1   = JOAO_CAMPOS;
                    selecaoJ2   = GRAFITE;
                    cenarioAtual = CENARIO_MARCO_ZERO;
                    estado      = ESTADO_SELECAO;
                }
                break;
        }

        /* ===== DESENHO ===== */
        BeginDrawing();
        ClearBackground(BLACK);

        /* Seleciona o background do cenário atual */
        Texture2D bgAtual;
        switch (cenarioAtual) {
            case CENARIO_BOA_VIAGEM: bgAtual = bgBoaViagem; break;
            case CENARIO_JAQUEIRA:   bgAtual = bgJaqueira;  break;
            default:                 bgAtual = bgMarcoZero; break;
        }

        switch (estado) {
            case ESTADO_MENU:
                desenharMenuPrincipal(bgMenu);
                break;
            case ESTADO_SELECAO:
                desenharSelecaoPersonagem(selecaoJ1, selecaoJ2, cenarioAtual);
                break;
            case ESTADO_COMBATE:
                desenharCenario(bgAtual);
                desenharPersonagens(&jogador1, &jogador2);
                desenharHUD(&jogador1, &jogador2, roundAtual);
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

    descarregarCenarios();
    CloseWindow();
    return 0;
}