#ifndef UI_H
#define UI_H

#include "jogador.h"
#include "raylib.h"

#define LARGURA_TELA  1280
#define ALTURA_TELA   720
#define TITULO_JANELA "Batalha do Passinho"
#define FPS_ALVO      60

/* Texturas globais dos cenários */
extern Texture2D bgMarcoZero;
extern Texture2D bgBoaViagem;
extern Texture2D bgJaqueira;
extern Texture2D bgMenu;

/* ---- Funções de UI (Raylib) ---- */
void carregarCenarios();
void descarregarCenarios();
void desenharHUD(Jogador *jogador1, Jogador *jogador2, int roundAtual);
void desenharPersonagens(Jogador *jogador1, Jogador *jogador2);
void desenharCenario(Texture2D background);
void desenharMenuPrincipal(Texture2D background);
void desenharSelecaoPersonagem(int selecaoJ1, int selecaoJ2, int cenarioAtual);
void desenharResultadoRound(Jogador *vencedor, Estatistica *stats, int totalStats);
void desenharTelaVitoria(Jogador *vencedor);

#endif