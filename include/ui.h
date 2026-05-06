#ifndef UI_H
#define UI_H

#include "jogador.h"

#define LARGURA_TELA  1280
#define ALTURA_TELA   720
#define TITULO_JANELA "Batalha do Passinho"
#define FPS_ALVO      60

/* ---- Funções de UI (Raylib) ---- */
void desenharHUD(Jogador *jogador1, Jogador *jogador2, int roundAtual);
void desenharPersonagens(Jogador *jogador1, Jogador *jogador2);
void desenharCenario();
void desenharMenuPrincipal();
void desenharSelecaoPersonagem(int selecaoJ1, int selecaoJ2);
void desenharResultadoRound(Jogador *vencedor, Estatistica *stats, int totalStats);
void desenharTelaVitoria(Jogador *vencedor);

#endif