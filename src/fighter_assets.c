#include "fighter_assets.h"
#include <stdio.h>

static FighterAssets lutadores[TOTAL_PERSONAGENS];

static const char *pastasPersonagens[TOTAL_PERSONAGENS] = {
    "joao_campos",
    "ds87",
    "kuki",
    "grafite",
    "clarisse",
    "ariano",
    "tojal"
};

static const char *pastaDisponivel(IndicePersonagem indice)
{
    static char caminho[128];

    snprintf(caminho, sizeof(caminho), "assets/fighters/%s", pastasPersonagens[indice]);
    if (DirectoryExists(caminho))
        return pastasPersonagens[indice];

    return indice == ARIANO ? "ariano" : "leigo";
}

static void carregarFrameSeExistir(FighterAnimation *anim, const char *path)
{
    if (anim->totalFrames >= MAX_ANIM_FRAMES || !FileExists(path))
        return;

    anim->frames[anim->totalFrames] = LoadTexture(path);
    anim->sources[anim->totalFrames] = (Rectangle){
        0,
        0,
        (float)anim->frames[anim->totalFrames].width,
        (float)anim->frames[anim->totalFrames].height
    };
    anim->totalFrames++;
}

static void adicionarFrameSpriteSheet(FighterAnimation *anim, Texture2D sheet, int coluna, int linha)
{
    if (anim->totalFrames >= MAX_ANIM_FRAMES || sheet.id == 0)
        return;

    anim->frames[anim->totalFrames] = sheet;
    anim->sources[anim->totalFrames] = (Rectangle){
        (float)(coluna * 256),
        (float)(linha * 256),
        256.0f,
        256.0f
    };
    anim->totalFrames++;
}

static int carregarSpriteSheetTeste(FighterAssets *assets, const char *pasta)
{
    char path[160];
    Texture2D sheet;

    snprintf(path, sizeof(path), "assets/fighters/%s/SpriteSheet.png", pasta);
    if (!FileExists(path))
        snprintf(path, sizeof(path), "assets/fighters/SpriteSheet.png");

    if (!FileExists(path))
        snprintf(path, sizeof(path), "assets/backgrounds/SpriteSheet.png");

    if (!FileExists(path))
        return 0;

    sheet = LoadTexture(path);

    assets->idle.totalFrames = 0;
    assets->idle.frameDuration = 0.13f;
    for (int i = 0; i < 5; i++)
        adicionarFrameSpriteSheet(&assets->idle, sheet, i, 0);

    assets->walk.totalFrames = 0;
    assets->walk.frameDuration = 0.09f;
    for (int i = 0; i < 8; i++)
        adicionarFrameSpriteSheet(&assets->walk, sheet, i, 1);

    assets->jump.totalFrames = 0;
    assets->jump.frameDuration = 0.10f;
    for (int i = 0; i < 8; i++)
        adicionarFrameSpriteSheet(&assets->jump, sheet, i, 2);

    assets->defense.totalFrames = 0;
    assets->defense.frameDuration = 0.12f;
    adicionarFrameSpriteSheet(&assets->defense, sheet, 0, 0);

    assets->attack.totalFrames = 0;
    assets->attack.frameDuration = 0.08f;
    adicionarFrameSpriteSheet(&assets->attack, sheet, 0, 2);

    assets->stun.totalFrames = 0;
    assets->stun.frameDuration = 0.14f;
    adicionarFrameSpriteSheet(&assets->stun, sheet, 0, 0);

    return 1;
}

static void carregarAnimacaoPadrao(FighterAnimation *anim, const char *pasta, const char *estado, float frameDuration)
{
    char path[160];

    anim->totalFrames = 0;
    anim->frameDuration = frameDuration;

    for (int i = 0; i < MAX_ANIM_FRAMES; i++)
    {
        snprintf(path, sizeof(path), "assets/fighters/%s/%s_%d.png", pasta, estado, i);
        carregarFrameSeExistir(anim, path);
    }

    snprintf(path, sizeof(path), "assets/fighters/%s/%s.png", pasta, estado);
    carregarFrameSeExistir(anim, path);
}

static void carregarFallbacksAtuais(FighterAssets *assets, const char *pasta)
{
    char path[160];

    if (assets->walk.totalFrames == 0)
    {
        snprintf(path, sizeof(path), "assets/fighters/%s/Andando.png", pasta);
        carregarFrameSeExistir(&assets->walk, path);
        snprintf(path, sizeof(path), "assets/fighters/%s/Andando (1).png", pasta);
        carregarFrameSeExistir(&assets->walk, path);
    }

    if (assets->defense.totalFrames == 0)
    {
        snprintf(path, sizeof(path), "assets/fighters/%s/Agachado_esquiva.png", pasta);
        carregarFrameSeExistir(&assets->defense, path);
        snprintf(path, sizeof(path), "assets/fighters/%s/Agachado.png", pasta);
        carregarFrameSeExistir(&assets->defense, path);
    }

    if (assets->jump.totalFrames == 0)
    {
        snprintf(path, sizeof(path), "assets/fighters/%s/golpes_pulo.png", pasta);
        carregarFrameSeExistir(&assets->jump, path);
        snprintf(path, sizeof(path), "assets/fighters/%s/Agachado.png", pasta);
        carregarFrameSeExistir(&assets->jump, path);
    }

    if (assets->attack.totalFrames == 0)
    {
        snprintf(path, sizeof(path), "assets/fighters/%s/golpes_pulo.png", pasta);
        carregarFrameSeExistir(&assets->attack, path);
        snprintf(path, sizeof(path), "assets/fighters/%s/MeuTrabalho.png", pasta);
        carregarFrameSeExistir(&assets->attack, path);
    }

    if (assets->idle.totalFrames == 0)
    {
        snprintf(path, sizeof(path), "assets/fighters/%s/idle.png", pasta);
        carregarFrameSeExistir(&assets->idle, path);
        snprintf(path, sizeof(path), "assets/fighters/%s/MeuTrabalho.png", pasta);
        carregarFrameSeExistir(&assets->idle, path);
        snprintf(path, sizeof(path), "assets/fighters/%s/Andando.png", pasta);
        carregarFrameSeExistir(&assets->idle, path);
        snprintf(path, sizeof(path), "assets/fighters/%s/Andando (1).png", pasta);
        carregarFrameSeExistir(&assets->idle, path);
    }

    if (assets->stun.totalFrames == 0 && assets->defense.totalFrames > 0)
    {
        assets->stun.frames[0] = assets->defense.frames[0];
        assets->stun.totalFrames = 1;
    }
}

static Texture2D carregarPortrait(const FighterAssets *assets, const char *pasta)
{
    char path[160];

    snprintf(path, sizeof(path), "assets/fighters/%s/portrait.png", pasta);
    if (FileExists(path))
        return LoadTexture(path);
        
    return (Texture2D){0};
}

static void carregarAssetsPersonagem(IndicePersonagem indice)
{
    FighterAssets *assets = &lutadores[indice];
    const char *pasta = pastaDisponivel(indice);

    if (!carregarSpriteSheetTeste(assets, pasta))
    {
        carregarAnimacaoPadrao(&assets->idle, pasta, "idle", 0.16f);
        carregarAnimacaoPadrao(&assets->walk, pasta, "walk", 0.10f);
        carregarAnimacaoPadrao(&assets->jump, pasta, "jump", 0.12f);
        carregarAnimacaoPadrao(&assets->defense, pasta, "defense", 0.12f);
        carregarAnimacaoPadrao(&assets->attack, pasta, "attack", 0.08f);
        carregarAnimacaoPadrao(&assets->stun, pasta, "stun", 0.14f);
        carregarFallbacksAtuais(assets, pasta);
    }

    assets->portrait = carregarPortrait(assets, pasta);
}

void carregarAssetsLutadores(void)
{
    for (int i = 0; i < TOTAL_PERSONAGENS; i++)
        carregarAssetsPersonagem((IndicePersonagem)i);
}

void descarregarAssetsLutadores(void)
{
    unsigned int texturasDescarregadas[TOTAL_PERSONAGENS * 7 * MAX_ANIM_FRAMES];
    int totalDescarregadas = 0;

    for (int i = 0; i < TOTAL_PERSONAGENS; i++)
    {
        FighterAssets *assets = &lutadores[i];
        FighterAnimation *animacoes[] = {
            &assets->idle, &assets->walk, &assets->jump,
            &assets->defense, &assets->attack, &assets->stun
        };

        for (int a = 0; a < 6; a++)
        {
            for (int f = 0; f < animacoes[a]->totalFrames; f++)
            {
                Texture2D textura = animacoes[a]->frames[f];
                int jaDescarregada = 0;

                for (int t = 0; t < totalDescarregadas; t++)
                {
                    if (texturasDescarregadas[t] == textura.id)
                    {
                        jaDescarregada = 1;
                        break;
                    }
                }

                if (textura.id != 0 && !jaDescarregada)
                {
                    UnloadTexture(textura);
                    texturasDescarregadas[totalDescarregadas++] = textura.id;
                }
            }
        }

        if (assets->portrait.id != 0)
        {
            int jaDescarregada = 0;
            for (int t = 0; t < totalDescarregadas; t++)
            {
                if (texturasDescarregadas[t] == assets->portrait.id)
                {
                    jaDescarregada = 1;
                    break;
                }
            }
            if (!jaDescarregada)
            {
                UnloadTexture(assets->portrait);
                texturasDescarregadas[totalDescarregadas++] = assets->portrait.id;
            }
        }
    }
}

const FighterAssets *getFighterAssets(IndicePersonagem indice)
{
    if (indice < 0 || indice >= TOTAL_PERSONAGENS)
        return &lutadores[0];
    return &lutadores[indice];
}

const FighterAnimation *getAnimationForState(const FighterAssets *assets, PlayerState state)
{
    switch (state)
    {
    case WALK:
        return &assets->walk;
    case JUMP:
        return &assets->jump;
    case DEFENSE:
        return &assets->defense;
    case ATTACK:
        return &assets->attack;
    case STUN:
        return &assets->stun;
    case IDLE:
    default:
        return &assets->idle;
    }
}

Texture2D getPortraitForFighter(const FighterAssets *assets)
{
    return assets->portrait;
}
