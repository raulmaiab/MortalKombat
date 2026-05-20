#ifndef PLAYER_H
#define PLAYER_H

#include "fighter_assets.h"
#include "jogador.h"
#include "raylib.h"
#include "ui.h"

#define GRAVIDADE 0.65f
#define FORCA_PULO -14.0f
#define VELOCIDADE_MOVIMENTO 4.0f
#define VELOCIDADE_DEFESA 2.0f
#define ESQUIVA_TICKS 20
#define ESQUIVA_COOLDOWN_TICKS 45
#define ESQUIVA_DISTANCIA 70.0f
#define ATTACK_STATE_TICKS 40
#define CROUCH_ATTACK_STATE_TICKS 40
#define CHAO_Y (ALTURA_TELA - ALTURA_PERSONAGEM)

typedef struct
{
    int esquerda;
    int direita;
    int pulo;
    int defesa;
    int defesaAlternativa;
    int esquiva;
    int ataqueNormal;
    int ataqueNormalAlternativo;
    int ataqueEspecial;
    int ataqueEspecialAlternativo;
} PlayerControls;

void updatePlayer(Jogador *jogador, Jogador *oponente, PlayerControls controles);
void renderPlayer(const Jogador *jogador, const FighterAssets *assets, Color corBase, const char *rotulo);
void resetPlayerPosition(Jogador *jogador, float posX, float posY, int olhandoDireita);

#endif
