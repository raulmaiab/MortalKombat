#ifndef UI_H
#define UI_H

#include "equipe.h"
#include "fighter_assets.h"
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
extern Texture2D bgPlayerSelect;

/* ---- Funções de UI (Raylib) ---- */
void carregarCenarios();
void descarregarCenarios();
void desenharHUD(const EquipeJogador *equipe1, const EquipeJogador *equipe2, int roundAtual, int segundosRestantes);
void desenharCenario(Texture2D background);
void desenharMenuPrincipal(Texture2D background);
void desenharSelecaoPersonagem(const int selecoesJ1[], int slotAtualJ1, int confirmouJ1,
                               const int selecoesJ2[], int slotAtualJ2, int confirmouJ2,
                               int cenarioAtual, int mostrarControles);
void desenharResultadoRound(Jogador *vencedor, Estatistica *stats, int totalStats);
void desenharTelaVitoria(Jogador *vencedor);

#endif
