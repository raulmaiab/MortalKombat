#include "fighter_assets.h"
#include <stdio.h>

static FighterAssets lutadores[TOTAL_PERSONAGENS][2];

static const char *pastasPersonagens[TOTAL_PERSONAGENS] = {
    "alirio",
    "odiferenciado",
    "ariano",
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
    SetTextureFilter(anim->frames[anim->totalFrames], TEXTURE_FILTER_BILINEAR);
    anim->sources[anim->totalFrames] = (Rectangle){
        0,
        0,
        (float)anim->frames[anim->totalFrames].width,
        (float)anim->frames[anim->totalFrames].height
    };
    anim->totalFrames++;
}

static int pastaExiste(const char *pasta)
{
    char caminho[160];

    snprintf(caminho, sizeof(caminho), "assets/fighters/%s", pasta);
    return DirectoryExists(caminho);
}

static int pastaTemAssets(const char *pasta)
{
    char caminho[180];
    const char *arquivos[] = {
        "SpriteSheet.png", "portrait.png", "idle.png", "walk.png",
        "attack.png", "stun.png", "MeuTrabalho.png", "Andando.png"
    };

    for (int i = 0; i < 8; i++)
    {
        snprintf(caminho, sizeof(caminho), "assets/fighters/%s/%s", pasta, arquivos[i]);
        if (FileExists(caminho))
            return 1;
    }

    return 0;
}

static const char *pastaJogador(const char *pastaBase, int jogador)
{
    static char pastaComJogador[2][160];
    int indiceBuffer = jogador == 2 ? 1 : 0;

    snprintf(pastaComJogador[indiceBuffer], sizeof(pastaComJogador[indiceBuffer]),
             "%s/p%d", pastaBase, jogador);

    if (pastaExiste(pastaComJogador[indiceBuffer]) && pastaTemAssets(pastaComJogador[indiceBuffer]))
        return pastaComJogador[indiceBuffer];

    return pastaBase;
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

static int frameSpriteSheetVazio(const Image *image, int coluna, int linha)
{
    int inicioX = coluna * 256;
    int inicioY = linha * 256;

    if (inicioX >= image->width || inicioY >= image->height)
        return 1;

    Color *pixels = LoadImageColors(*image);
    int limiteY = inicioY + 256;
    int limiteX = inicioX + 256;

    if (limiteY > image->height)
        limiteY = image->height;
    if (limiteX > image->width)
        limiteX = image->width;

    for (int y = inicioY; y < limiteY; y++)
    {
        for (int x = inicioX; x < limiteX; x++)
        {
            if (pixels[y * image->width + x].a > 0)
            {
                UnloadImageColors(pixels);
                return 0;
            }
        }
    }

    UnloadImageColors(pixels);
    return 1;
}

static void carregarLinhaSpriteSheet(FighterAnimation *anim, Texture2D sheet, const Image *image,
                                     int linha, float frameDuration)
{
    anim->totalFrames = 0;
    anim->frameDuration = frameDuration;

    for (int coluna = 0; coluna < MAX_ANIM_FRAMES; coluna++)
    {
        if (frameSpriteSheetVazio(image, coluna, linha))
            break;
        adicionarFrameSpriteSheet(anim, sheet, coluna, linha);
    }
}

static int carregarLinhaSpriteSheetArquivo(FighterAnimation *anim, const char *path,
                                           int linha, float frameDuration)
{
    Image image;
    Texture2D sheet;

    if (!FileExists(path))
        return 0;

    image = LoadImage(path);
    sheet = LoadTextureFromImage(image);
    SetTextureFilter(sheet, TEXTURE_FILTER_BILINEAR);
    carregarLinhaSpriteSheet(anim, sheet, &image, linha, frameDuration);
    UnloadImage(image);

    return anim->totalFrames > 0;
}

static int carregarSpriteSheetTeste(FighterAssets *assets, const char *pasta)
{
    char path[160];
    Image image;
    Texture2D sheet;

    snprintf(path, sizeof(path), "assets/fighters/%s/SpriteSheet.png", pasta);
    if (!FileExists(path))
        snprintf(path, sizeof(path), "assets/fighters/SpriteSheet.png");

    if (!FileExists(path))
        snprintf(path, sizeof(path), "assets/backgrounds/SpriteSheet.png");

    if (!FileExists(path))
        return 0;

    image = LoadImage(path);
    sheet = LoadTextureFromImage(image);
    SetTextureFilter(sheet, TEXTURE_FILTER_BILINEAR);

    carregarLinhaSpriteSheet(&assets->idle, sheet, &image, 0, 0.13f);
    carregarLinhaSpriteSheet(&assets->walk, sheet, &image, 1, 0.09f);
    carregarLinhaSpriteSheet(&assets->jump, sheet, &image, 2, 0.10f);
    carregarLinhaSpriteSheet(&assets->special, sheet, &image, 3, 0.24f);
    carregarLinhaSpriteSheet(&assets->stun, sheet, &image, 4, 0.10f);
    carregarLinhaSpriteSheet(&assets->knockdown, sheet, &image, 5, 0.12f);
    carregarLinhaSpriteSheet(&assets->defense, sheet, &image, 6, 0.10f);
    carregarLinhaSpriteSheet(&assets->attack, sheet, &image, 7, 0.08f);

    UnloadImage(image);

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

    if (assets->lowattack.totalFrames == 0)
    {
        snprintf(path, sizeof(path), "assets/fighters/%s/Agachado.png", pasta);
        carregarLinhaSpriteSheetArquivo(&assets->lowattack, path, 0, 0.08f);
    }

    if (assets->lowattack.totalFrames == 0)
    {
        snprintf(path, sizeof(path), "assets/fighters/%s/Agachado_chute.png", pasta);
        carregarFrameSeExistir(&assets->lowattack, path);
    }

    if (assets->defense.totalFrames == 0)
    {
        snprintf(path, sizeof(path), "assets/fighters/%s/Defesa.png", pasta);
        carregarFrameSeExistir(&assets->defense, path);
        snprintf(path, sizeof(path), "assets/fighters/%s/defense.png", pasta);
        carregarFrameSeExistir(&assets->defense, path);
        snprintf(path, sizeof(path), "assets/fighters/%s/Agachado.png", pasta);
        carregarFrameSeExistir(&assets->defense, path);
    }

    if (assets->lowattack.totalFrames == 0 && assets->defense.totalFrames > 0)
    {
        assets->lowattack.frames[0] = assets->defense.frames[0];
        assets->lowattack.sources[0] = assets->defense.sources[0];
        assets->lowattack.totalFrames = 1;
        assets->lowattack.frameDuration = 0.08f;
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

    if (assets->special.totalFrames == 0 && assets->attack.totalFrames > 0)
    {
        assets->special.frames[0] = assets->attack.frames[0];
        assets->special.sources[0] = assets->attack.sources[0];
        assets->special.totalFrames = 1;
        assets->special.frameDuration = 0.24f;
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
        assets->stun.sources[0] = assets->defense.sources[0];
        assets->stun.totalFrames = 1;
        assets->stun.frameDuration = 0.14f;
    }

    if (assets->knockdown.totalFrames == 0 && assets->stun.totalFrames > 0)
    {
        assets->knockdown.frames[0] = assets->stun.frames[0];
        assets->knockdown.sources[0] = assets->stun.sources[0];
        assets->knockdown.totalFrames = 1;
        assets->knockdown.frameDuration = 0.12f;
    }
}

static Texture2D carregarPortrait(const char *pasta)
{
    char path[160];

    snprintf(path, sizeof(path), "assets/fighters/%s/portrait.png", pasta);
    if (FileExists(path))
        return LoadTexture(path);
        
    return (Texture2D){0};
}

static void carregarAssetsPersonagem(IndicePersonagem indice, int jogador)
{
    FighterAssets *assets = &lutadores[indice][jogador - 1];
    const char *pastaBase = pastaDisponivel(indice);
    const char *pasta = pastaJogador(pastaBase, jogador);

    assets->escalaRender = indice == ALIRIO ? 0.62f : 1.0f;

    if (!carregarSpriteSheetTeste(assets, pasta))
    {
        carregarAnimacaoPadrao(&assets->idle, pasta, "idle", 0.16f);
        carregarAnimacaoPadrao(&assets->walk, pasta, "walk", 0.10f);
        carregarAnimacaoPadrao(&assets->jump, pasta, "jump", 0.12f);
        carregarAnimacaoPadrao(&assets->lowattack, pasta, "lowattack", 0.08f);
        carregarAnimacaoPadrao(&assets->special, pasta, "special", 0.24f);
        carregarAnimacaoPadrao(&assets->defense, pasta, "defense", 0.12f);
        carregarAnimacaoPadrao(&assets->attack, pasta, "attack", 0.08f);
        carregarAnimacaoPadrao(&assets->stun, pasta, "stun", 0.14f);
        carregarAnimacaoPadrao(&assets->knockdown, pasta, "knockdown", 0.12f);
    }
    carregarFallbacksAtuais(assets, pasta);

    assets->portrait = carregarPortrait(pastaBase);
}

void carregarAssetsLutadores(void)
{
    for (int i = 0; i < TOTAL_PERSONAGENS; i++)
    {
        carregarAssetsPersonagem((IndicePersonagem)i, 1);
        carregarAssetsPersonagem((IndicePersonagem)i, 2);
    }
}

void descarregarAssetsLutadores(void)
{
    unsigned int texturasDescarregadas[TOTAL_PERSONAGENS * 2 * (9 * MAX_ANIM_FRAMES + 1)];
    int totalDescarregadas = 0;

    for (int i = 0; i < TOTAL_PERSONAGENS; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            FighterAssets *assets = &lutadores[i][j];
            FighterAnimation *animacoes[] = {
                &assets->idle, &assets->walk, &assets->jump,
                &assets->lowattack, &assets->special, &assets->defense, &assets->attack,
                &assets->stun, &assets->knockdown
            };

            for (int a = 0; a < 9; a++)
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
}

const FighterAssets *getFighterAssets(IndicePersonagem indice)
{
    return getFighterAssetsJogador(indice, 1);
}

const FighterAssets *getFighterAssetsJogador(IndicePersonagem indice, int jogador)
{
    int indiceJogador = jogador == 2 ? 1 : 0;

    if (indice < 0 || indice >= TOTAL_PERSONAGENS)
        return &lutadores[0][indiceJogador];
    return &lutadores[indice][indiceJogador];
}

const FighterAnimation *getAnimationForState(const FighterAssets *assets, PlayerState state)
{
    switch (state)
    {
    case WALK:
        return &assets->walk;
    case JUMP:
        return &assets->jump;
    case CROUCH:
        return &assets->lowattack;
    case DEFENSE:
        return &assets->defense;
    case ATTACK:
        return &assets->attack;
    case SPECIAL_ATTACK:
        return &assets->special;
    case STUN:
        return &assets->stun;
    case KNOCKDOWN:
        return &assets->knockdown;
    case IDLE:
    default:
        return &assets->idle;
    }
}

Texture2D getPortraitForFighter(const FighterAssets *assets)
{
    return assets->portrait;
}
