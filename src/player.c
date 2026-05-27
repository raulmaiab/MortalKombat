#include "player.h"
#include <stddef.h>

#define SPRITE_ALTURA_PADRAO 480.0f
#define SPRITE_ALTURA_AGACHADO 380.0f
#define SPRITE_ALTURA_CAIDO 260.0f

static void limitarPosicaoX(Jogador *jogador)
{
    if (jogador->posX < 0)
        jogador->posX = 0;
    if (jogador->posX > LARGURA_TELA - LARGURA_PERSONAGEM)
        jogador->posX = LARGURA_TELA - LARGURA_PERSONAGEM;
}

static void atualizarTimers(Jogador *jogador)
{
    int especialAtivo = jogador->specialAttackTicks > 0;

    if (jogador->stunTicks > 0)
        jogador->stunTicks--;
    if (jogador->attackTicks > 0)
        jogador->attackTicks--;
    if (jogador->specialAttackTicks > 0)
    {
        jogador->specialAttackTicks--;
        if (especialAtivo && jogador->specialAttackTicks == 0)
            jogador->energia = 0;
    }
    if (jogador->ataqueAgachadoTicks > 0)
        jogador->ataqueAgachadoTicks--;
    if (jogador->ataquePendente != PASSINHO_NENHUM)
        jogador->ataquePendenteTicks++;
}

static void registrarAtaquePendente(Jogador *jogador, TipoPassinho ataque)
{
    jogador->ataquePendente = ataque;
    jogador->ataquePendenteTicks = 0;
    jogador->ataquePendenteAplicado = 0;
}

static void cancelarAtaquePendente(Jogador *jogador)
{
    jogador->ataquePendente = PASSINHO_NENHUM;
    jogador->ataquePendenteTicks = 0;
    jogador->ataquePendenteAplicado = 0;
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
    else if (jogador->defendendo)
        novoEstado = DEFENSE;
    else if (jogador->stunTicks > 0)
        novoEstado = STUN;
    else if (jogador->agachado || jogador->ataqueAgachadoTicks > 0)
        novoEstado = CROUCH;
    else if (jogador->specialAttackTicks > 0)
        novoEstado = SPECIAL_ATTACK;
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
    case SPECIAL_ATTACK:
        return PURPLE;
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

static int keyPressedAlternativo(int teclaPrincipal, int teclaAlternativa)
{
    return IsKeyPressed(teclaPrincipal) ||
           (teclaAlternativa != 0 && IsKeyPressed(teclaAlternativa));
}

TipoPassinho updatePlayer(Jogador *jogador, Jogador *oponente, PlayerControls controles)
{
    int moveu = 0;
    TipoPassinho ataqueSolicitado = PASSINHO_NENHUM;

    atualizarTimers(jogador);
    if (jogador->hp <= 0 || jogador->stunTicks > 0)
        cancelarAtaquePendente(jogador);

    jogador->olhandoDireita = oponente->posX > jogador->posX;
    jogador->agachado = 0;
    jogador->defendendo = jogador->stunTicks == 0 &&
                          jogador->noChao &&
                          (IsKeyDown(controles.defesa) ||
                           (controles.defesaAlternativa != 0 && IsKeyDown(controles.defesaAlternativa)));

    if (jogador->hp > 0 && jogador->stunTicks == 0)
    {
        if (!jogador->defendendo && jogador->attackTicks == 0 &&
            jogador->specialAttackTicks == 0 && jogador->ataqueAgachadoTicks == 0)
        {
            if (IsKeyPressed(controles.agachar) && jogador->noChao)
            {
                if (jogador->energia >= energiaConsumida[1])
                {
                    ataqueSolicitado = ATAQUE_AGACHADO;
                    registrarAtaquePendente(jogador, ataqueSolicitado);
                    jogador->attackTicks = 0;
                    jogador->ataqueAgachadoTicks = CROUCH_ATTACK_STATE_TICKS;
                }
            }
            if (ataqueSolicitado == PASSINHO_NENHUM &&
                keyPressedAlternativo(controles.ataqueNormal, controles.ataqueNormalAlternativo))
            {
                if (jogador->energia >= energiaConsumida[0])
                {
                    ataqueSolicitado = ATAQUE_NORMAL;
                    registrarAtaquePendente(jogador, ataqueSolicitado);
                    jogador->attackTicks = ATTACK_STATE_TICKS;
                    jogador->ataqueAgachadoTicks = 0;
                }
            }
            if (ataqueSolicitado == PASSINHO_NENHUM &&
                keyPressedAlternativo(controles.ataqueEspecial, controles.ataqueEspecialAlternativo))
            {
                int custo = energiaConsumida[2];
                if (jogador->energia >= custo)
                {
                    ataqueSolicitado = ATAQUE_ESPECIAL;
                    registrarAtaquePendente(jogador, ataqueSolicitado);
                    jogador->attackTicks = 0;
                    jogador->specialAttackTicks = SPECIAL_ATTACK_STATE_TICKS;
                    jogador->ataqueAgachadoTicks = 0;
                }
            }
        }

        if (!jogador->agachado && jogador->attackTicks == 0 && jogador->specialAttackTicks == 0 &&
            jogador->ataqueAgachadoTicks == 0)
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
    return ataqueSolicitado;
}

TipoPassinho consumirAtaqueNoFrameDeImpacto(Jogador *jogador)
{
    int ticksImpacto;
    TipoPassinho ataque;

    if (jogador->ataquePendente == PASSINHO_NENHUM || jogador->ataquePendenteAplicado)
        return PASSINHO_NENHUM;

    if (jogador->ataquePendente == ATAQUE_ESPECIAL)
        ticksImpacto = SPECIAL_ATTACK_HIT_TICKS;
    else
        ticksImpacto = ATTACK_HIT_TICKS;

    if (jogador->ataquePendenteTicks < ticksImpacto)
        return PASSINHO_NENHUM;

    ataque = (TipoPassinho)jogador->ataquePendente;
    jogador->ataquePendenteAplicado = 1;
    return ataque;
}

static float alturaRenderEstado(PlayerState state)
{
    if (state == CROUCH)
        return SPRITE_ALTURA_AGACHADO;
    if (state == KNOCKDOWN)
        return SPRITE_ALTURA_CAIDO;
    return SPRITE_ALTURA_PADRAO;
}

static void renderSpriteAnimado(const Jogador *jogador, const FighterAnimation *anim, float escalaRender)
{
    int frameAtual;
    int ticksPorFrame;
    float alturaSprite;
    float larguraSprite;
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
    alturaSprite = alturaRenderEstado(jogador->state) * escalaRender;
    larguraSprite = alturaSprite * (origem.width / origem.height);
    destino = (Rectangle){
        jogador->posX + (LARGURA_PERSONAGEM / 2.0f) - (larguraSprite / 2.0f),
        jogador->posY + ALTURA_PERSONAGEM - alturaSprite,
        larguraSprite,
        alturaSprite
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
        renderSpriteAnimado(jogador, anim, assets->escalaRender);
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
    jogador->attackTicks = 0;
    jogador->specialAttackTicks = 0;
    cancelarAtaquePendente(jogador);
    jogador->stateTicks = 0;
    jogador->olhandoDireita = olhandoDireita;
    jogador->state = IDLE;
}

void resetPlayerEntradaPulando(Jogador *jogador, float posX, float posY, int olhandoDireita)
{
    resetPlayerPosition(jogador, posX, posY, olhandoDireita);
    jogador->velY = FORCA_PULO;
    jogador->noChao = 0;
    jogador->state = JUMP;
    jogador->stateTicks = 0;
}
