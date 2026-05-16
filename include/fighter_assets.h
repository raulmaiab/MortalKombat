#ifndef FIGHTER_ASSETS_H
#define FIGHTER_ASSETS_H

#include "jogador.h"
#include "raylib.h"

#define MAX_ANIM_FRAMES 8

typedef struct
{
    Texture2D frames[MAX_ANIM_FRAMES];
    Rectangle sources[MAX_ANIM_FRAMES];
    int totalFrames;
    float frameDuration;
} FighterAnimation;

typedef struct
{
    FighterAnimation idle;
    FighterAnimation walk;
    FighterAnimation jump;
    FighterAnimation defense;
    FighterAnimation attack;
    FighterAnimation stun;
    Texture2D portrait;
} FighterAssets;

void carregarAssetsLutadores(void);
void descarregarAssetsLutadores(void);
const FighterAssets *getFighterAssets(IndicePersonagem indice);
const FighterAnimation *getAnimationForState(const FighterAssets *assets, PlayerState state);
Texture2D getPortraitForFighter(const FighterAssets *assets);

#endif
