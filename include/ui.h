#ifndef UI_H
#define UI_H

#include "decisao_vitoria.h"
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
void desenharHUD(const EquipeJogador *equipe1, const EquipeJogador *equipe2,
                 const char *nomeJ1, const char *nomeJ2);
void desenharCenario(Texture2D background);
void desenharMenuPrincipal(Texture2D background);
void desenharEntradaNomes(Texture2D background, const char *nomeJ1, const char *nomeJ2, int jogadorAtual);
void desenharSelecaoPersonagem(const int selecoesJ1[], int slotAtualJ1, int confirmouJ1,
                               const int selecoesJ2[], int slotAtualJ2, int confirmouJ2,
                               int cenarioAtual, int mostrarControles);
void desenharResultadoRound(Jogador *vencedor, Estatistica *stats, int totalStats);
void desenharTelaVitoria(Jogador *vencedor, const char *nomeJ1, const char *nomeJ2,
                         OpcaoPosPartida escolhaJ1, OpcaoPosPartida escolhaJ2,
                         int confirmouJ1, int confirmouJ2);

#endif
