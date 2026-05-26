#ifndef PLAYER_H
#define PLAYER_H

#include "combate.h"
#include "fighter_assets.h"
#include "jogador.h"
#include "raylib.h"
#include "ui.h"

#define GRAVIDADE 0.65f
#define FORCA_PULO -14.0f
#define VELOCIDADE_MOVIMENTO 4.0f
#define VELOCIDADE_DEFESA 2.0f
#define ATTACK_STATE_TICKS RECUPERACAO_ATAQUE_TICKS
#define SPECIAL_ATTACK_STATE_TICKS 144
#define CROUCH_ATTACK_STATE_TICKS RECUPERACAO_ATAQUE_TICKS
#define ATTACK_HIT_TICKS 15
#define SPECIAL_ATTACK_HIT_TICKS 54
#define CHAO_Y (ALTURA_TELA - ALTURA_PERSONAGEM)

typedef struct
{
    int esquerda;
    int direita;
    int pulo;
    int defesa;
    int defesaAlternativa;
    int agachar;
    int ataqueNormal;
    int ataqueNormalAlternativo;
    int ataqueEspecial;
    int ataqueEspecialAlternativo;
} PlayerControls;

TipoPassinho updatePlayer(Jogador *jogador, Jogador *oponente, PlayerControls controles);
TipoPassinho consumirAtaqueNoFrameDeImpacto(Jogador *jogador);
void renderPlayer(const Jogador *jogador, const FighterAssets *assets, Color corBase, const char *rotulo);
void resetPlayerPosition(Jogador *jogador, float posX, float posY, int olhandoDireita);

#endif
