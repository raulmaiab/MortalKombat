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
    if (jogador->ataqueAgachadoTicks > 0)
        jogador->ataqueAgachadoTicks--;
}

static TipoPassinho converterAtaqueAgachado(TipoPassinho passinho)
{
    switch (passinho)
    {
    case ATAQUE_LEVE:
        return ATAQUE_BAIXO_LEVE;
    case ATAQUE_MEDIO:
        return ATAQUE_BAIXO_MEDIO;
    case ATAQUE_ESPECIAL:
        return ATAQUE_BAIXO_ESPECIAL;
    default:
        return passinho;
    }
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
    PlayerState novoEstado;

    if (jogador->hp <= 0)
        novoEstado = KNOCKDOWN;
    else if (jogador->stunTicks > 0)
        novoEstado = STUN;
    else if (jogador->agachado || jogador->ataqueAgachadoTicks > 0)
        novoEstado = CROUCH;
    else if (jogador->defendendo)
        novoEstado = DEFENSE;
    else if (jogador->attackTicks > 0)
        novoEstado = ATTACK;
    else if (!jogador->noChao)
        novoEstado = JUMP;
    else if (moveu)
        novoEstado = WALK;
    else
        novoEstado = IDLE;

    if (jogador->state != novoEstado)
    {
        jogador->state = novoEstado;
        jogador->stateTicks = 0;
    }
    else
    {
        jogador->stateTicks++;
    }
}

static Color corDoEstado(const Jogador *jogador, Color corBase)
{
    switch (jogador->state)
    {
    case CROUCH:
        return SKYBLUE;
    case DEFENSE:
        return GREEN;
    case ATTACK:
        return GOLD;
    case JUMP:
        return SKYBLUE;
    case STUN:
        return ORANGE;
    case KNOCKDOWN:
        return GRAY;
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
    jogador->agachado = jogador->hp > 0 &&
                        jogador->stunTicks == 0 &&
                        jogador->noChao &&
                        IsKeyDown(controles.esquiva);
    jogador->defendendo = jogador->stunTicks == 0 &&
                          jogador->esquivaTicks == 0 &&
                          jogador->noChao &&
                          !jogador->agachado &&
                          (IsKeyDown(controles.defesa) ||
                           (controles.defesaAlternativa != 0 && IsKeyDown(controles.defesaAlternativa)));

    if (jogador->hp > 0 && jogador->stunTicks == 0)
    {
        if (!jogador->defendendo)
        {
            if (IsKeyPressed(controles.ataqueLeve))
            {
                enfileirarPassinho(&jogador->fila, jogador->agachado ? converterAtaqueAgachado(ATAQUE_LEVE) : ATAQUE_LEVE);
                jogador->attackTicks = jogador->agachado ? 0 : ATTACK_STATE_TICKS;
                jogador->ataqueAgachadoTicks = jogador->agachado ? CROUCH_ATTACK_STATE_TICKS : 0;
            }
            if (IsKeyPressed(controles.ataqueMedio))
            {
                enfileirarPassinho(&jogador->fila, jogador->agachado ? converterAtaqueAgachado(ATAQUE_MEDIO) : ATAQUE_MEDIO);
                jogador->attackTicks = jogador->agachado ? 0 : ATTACK_STATE_TICKS;
                jogador->ataqueAgachadoTicks = jogador->agachado ? CROUCH_ATTACK_STATE_TICKS : 0;
            }
            if (IsKeyPressed(controles.ataqueEspecial))
            {
                enfileirarPassinho(&jogador->fila, jogador->agachado ? converterAtaqueAgachado(ATAQUE_ESPECIAL) : ATAQUE_ESPECIAL);
                jogador->attackTicks = jogador->agachado ? 0 : ATTACK_STATE_TICKS;
                jogador->ataqueAgachadoTicks = jogador->agachado ? CROUCH_ATTACK_STATE_TICKS : 0;
            }
        }

        if (!jogador->agachado && jogador->ataqueAgachadoTicks == 0 && jogador->esquivaTicks == 0)
        {
            float velocidade = jogador->defendendo ? VELOCIDADE_DEFESA : VELOCIDADE_MOVIMENTO;

            if (IsKeyDown(controles.esquerda))
            {
                jogador->posX -= velocidade;
                moveu = 1;
            }
            if (IsKeyDown(controles.direita))
            {
                jogador->posX += velocidade;
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
    int ticksPorFrame;
    Texture2D textura;
    Rectangle origem;
    Rectangle destino;

    if (anim == NULL || anim->totalFrames == 0)
        return;

    ticksPorFrame = (int)(anim->frameDuration * FPS_ALVO + 0.5f);
    if (ticksPorFrame < 1)
        ticksPorFrame = 1;

    frameAtual = jogador->stateTicks / ticksPorFrame;
    if (jogador->state == IDLE || jogador->state == WALK || jogador->state == JUMP || jogador->state == DEFENSE || jogador->state == CROUCH)
    {
        frameAtual %= anim->totalFrames;
    }
    else if (frameAtual >= anim->totalFrames)
    {
        frameAtual = anim->totalFrames - 1;
    }

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
    jogador->agachado = 0;
    jogador->ataqueAgachadoTicks = 0;
    jogador->esquivaTicks = 0;
    jogador->esquivaCooldown = 0;
    jogador->attackTicks = 0;
    jogador->stateTicks = 0;
    jogador->olhandoDireita = olhandoDireita;
    jogador->state = IDLE;
    limparFila(&jogador->fila);
}
