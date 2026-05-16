#include "player.h"
#include "fila.h"
#include <stddef.h>

#define SPRITE_ALTURA 504
#define SPRITE_LARGURA 504

static void limitarPosicaoX(Jogador *jogador)
{
    if (jogador->posX < 0)
        jogador->posX = 0;
    if (jogador->posX > LARGURA_TELA - LARGURA_PERSONAGEM)
        jogador->posX = LARGURA_TELA - LARGURA_PERSONAGEM;
}

static void atualizarTimers(Jogador *jogador)
{
    if (jogador->stunTicks > 0)
        jogador->stunTicks--;
    if (jogador->esquivaTicks > 0)
        jogador->esquivaTicks--;
    if (jogador->esquivaCooldown > 0)
        jogador->esquivaCooldown--;
    if (jogador->attackTicks > 0)
        jogador->attackTicks--;
}

static void iniciarEsquiva(Jogador *jogador, const Jogador *oponente)
{
    float direcao = (oponente->posX > jogador->posX) ? -1.0f : 1.0f;

    jogador->esquivaTicks = ESQUIVA_TICKS;
    jogador->esquivaCooldown = ESQUIVA_COOLDOWN_TICKS;
    jogador->defendendo = 0;
    jogador->posX += direcao * ESQUIVA_DISTANCIA;
    limitarPosicaoX(jogador);
    limparFila(&jogador->fila);
}

static void atualizarFisica(Jogador *jogador)
{
    if (!jogador->noChao)
    {
        jogador->velY += GRAVIDADE;
        jogador->posY += jogador->velY;

        if (jogador->posY >= CHAO_Y)
        {
            jogador->posY = CHAO_Y;
            jogador->velY = 0.0f;
            jogador->noChao = 1;
        }
    }
}

static void atualizarEstado(Jogador *jogador, int moveu)
{
    if (jogador->stunTicks > 0)
        jogador->state = STUN;
    else if (jogador->defendendo)
        jogador->state = DEFENSE;
    else if (jogador->attackTicks > 0)
        jogador->state = ATTACK;
    else if (!jogador->noChao)
        jogador->state = JUMP;
    else if (moveu)
        jogador->state = WALK;
    else
        jogador->state = IDLE;
}

static Color corDoEstado(const Jogador *jogador, Color corBase)
{
    switch (jogador->state)
    {
    case DEFENSE:
        return GREEN;
    case ATTACK:
        return GOLD;
    case JUMP:
        return SKYBLUE;
    case STUN:
        return ORANGE;
    case WALK:
        return Fade(corBase, 0.85f);
    case IDLE:
    default:
        return corBase;
    }
}

void updatePlayer(Jogador *jogador, Jogador *oponente, PlayerControls controles)
{
    int moveu = 0;

    atualizarTimers(jogador);
    jogador->olhandoDireita = oponente->posX > jogador->posX;
    jogador->defendendo = jogador->stunTicks == 0 &&
                          jogador->esquivaTicks == 0 &&
                          jogador->noChao &&
                          (IsKeyDown(controles.defesa) ||
                           (controles.defesaAlternativa != 0 && IsKeyDown(controles.defesaAlternativa)));

    if (jogador->stunTicks == 0)
    {
        if (!jogador->defendendo)
        {
            if (IsKeyPressed(controles.ataqueLeve))
            {
                enfileirarPassinho(&jogador->fila, ATAQUE_LEVE);
                jogador->attackTicks = ATTACK_STATE_TICKS;
            }
            if (IsKeyPressed(controles.ataqueMedio))
            {
                enfileirarPassinho(&jogador->fila, ATAQUE_MEDIO);
                jogador->attackTicks = ATTACK_STATE_TICKS;
            }
            if (IsKeyPressed(controles.ataqueEspecial))
            {
                enfileirarPassinho(&jogador->fila, ATAQUE_ESPECIAL);
                jogador->attackTicks = ATTACK_STATE_TICKS;
            }
            if (IsKeyPressed(controles.esquiva) && jogador->esquivaCooldown == 0)
                iniciarEsquiva(jogador, oponente);
        }

        if (!jogador->defendendo && jogador->esquivaTicks == 0)
        {
            if (IsKeyDown(controles.esquerda))
            {
                jogador->posX -= VELOCIDADE_MOVIMENTO;
                moveu = 1;
            }
            if (IsKeyDown(controles.direita))
            {
                jogador->posX += VELOCIDADE_MOVIMENTO;
                moveu = 1;
            }
            if (IsKeyPressed(controles.pulo) && jogador->noChao)
            {
                jogador->velY = FORCA_PULO;
                jogador->noChao = 0;
            }
        }
    }

    limitarPosicaoX(jogador);
    atualizarFisica(jogador);
    atualizarEstado(jogador, moveu);
}

static void renderSpriteAnimado(const Jogador *jogador, const FighterAnimation *anim)
{
    int frameAtual;
    Texture2D textura;
    Rectangle origem;
    Rectangle destino;

    if (anim == NULL || anim->totalFrames == 0)
        return;

    frameAtual = (int)(GetTime() / anim->frameDuration) % anim->totalFrames;
    textura = anim->frames[frameAtual];
    origem = anim->sources[frameAtual];
    destino = (Rectangle){
        jogador->posX + (LARGURA_PERSONAGEM / 2.0f) - (SPRITE_LARGURA / 2.0f),
        jogador->posY + ALTURA_PERSONAGEM - SPRITE_ALTURA,
        SPRITE_LARGURA,
        SPRITE_ALTURA
    };

    if (!jogador->olhandoDireita)
    {
        origem.width *= -1;
    }

    DrawTexturePro(textura, origem, destino, (Vector2){0, 0}, 0.0f, WHITE);
}

void renderPlayer(const Jogador *jogador, const FighterAssets *assets, Color corBase, const char *rotulo)
{
    int x = (int)jogador->posX;
    int y = (int)jogador->posY;
    Color cor = corDoEstado(jogador, corBase);
    const FighterAnimation *anim = getAnimationForState(assets, jogador->state);

    if (anim != NULL && anim->totalFrames > 0)
        renderSpriteAnimado(jogador, anim);
    else
        DrawRectangle(x, y, LARGURA_PERSONAGEM, ALTURA_PERSONAGEM, cor);

    DrawText(rotulo, x + 18, y - 22, 18, WHITE);
}

void resetPlayerPosition(Jogador *jogador, float posX, float posY, int olhandoDireita)
{
    jogador->posX = posX;
    jogador->posY = posY;
    jogador->velY = 0.0f;
    jogador->noChao = 1;
    jogador->defendendo = 0;
    jogador->esquivaTicks = 0;
    jogador->esquivaCooldown = 0;
    jogador->attackTicks = 0;
    jogador->olhandoDireita = olhandoDireita;
    jogador->state = IDLE;
    limparFila(&jogador->fila);
}
