#include "combate.h"
#include <math.h>
#include <string.h>

const int energiaConsumida[3] = {0, 0, ENERGIA_ESPECIAL};

static float calcularDistancia(const Jogador *a, const Jogador *b)
{
    float dx = a->posX - b->posX;
    float dy = a->posY - b->posY;
    return sqrtf(dx * dx + dy * dy);
}

static void aplicarRecuperacaoAtaque(Jogador *atacante, TipoPassinho passinho)
{
    if (passinho == ATAQUE_AGACHADO)
    {
        if (atacante->ataqueAgachadoTicks <= 0)
            atacante->ataqueAgachadoTicks = RECUPERACAO_ATAQUE_TICKS;
    }
    else if (passinho == ATAQUE_ESPECIAL)
    {
        if (atacante->specialAttackTicks <= 0)
            atacante->specialAttackTicks = RECUPERACAO_ATAQUE_TICKS;
    }
    else if (atacante->attackTicks <= 0)
    {
        atacante->attackTicks = RECUPERACAO_ATAQUE_TICKS;
    }
}

static int ataqueAgachado(TipoPassinho passinho)
{
    return passinho == ATAQUE_AGACHADO;
}

static int ataqueValido(TipoPassinho passinho)
{
    return passinho == ATAQUE_NORMAL ||
           passinho == ATAQUE_AGACHADO ||
           passinho == ATAQUE_ESPECIAL;
}

static int defesaDeFrente(const Jogador *defensor, const Jogador *atacante)
{
    if (atacante->posX > defensor->posX)
        return defensor->olhandoDireita;
    return !defensor->olhandoDireita;
}

static int calcularDanoPercentual(const Jogador *alvo, int percentual)
{
    int dano = alvo->personagem.hpMaximo * percentual / 100;
    if (dano < 1)
        dano = 1;
    return dano;
}

/*
 * Processa um passinho atacante contra o alvo.
 * Verifica distância, energia e stun antes de aplicar dano.
 */
void processarPassinho(TipoPassinho passinho, Jogador *atacante, Jogador *alvo, int tickAtual)
{
    if (atacante->stunTicks > 0)
        return;

    int custo = 0;
    int dano = 0;

    switch (passinho)
    {
    case ATAQUE_NORMAL:
        custo = energiaConsumida[0];
        dano = calcularDanoPercentual(alvo, DANO_NORMAL_PERCENTUAL);
        break;
    case ATAQUE_AGACHADO:
        custo = energiaConsumida[1];
        dano = calcularDanoPercentual(alvo, DANO_NORMAL_PERCENTUAL);
        break;
    case ATAQUE_ESPECIAL:
        custo = 0;
        dano = calcularDanoPercentual(alvo, DANO_ESPECIAL_PERCENTUAL);
        break;
    default:
        break;
    }

    if (ataqueValido(passinho) && atacante->energia < custo)
    {
        return;
    }

    if (passinho != ATAQUE_ESPECIAL && calcularDistancia(atacante, alvo) > DISTANCIA_MAXIMA_ATAQUE)
    {
        return;
    }

    if (ataqueValido(passinho))
    {
        atacante->energia -= custo;
        if (atacante->energia < 0)
            atacante->energia = 0;
    }

    if (!ataqueAgachado(passinho) && (alvo->agachado || alvo->ataqueAgachadoTicks > 0))
    {
        return;
    }

    if (ataqueAgachado(passinho) && !alvo->noChao)
    {
        return;
    }

    if (!ataqueAgachado(passinho) && alvo->defendendo && alvo->noChao && defesaDeFrente(alvo, atacante))
    {
        adicionarEnergia(atacante, GANHO_ENERGIA_DEFESA);
        aplicarRecuperacaoAtaque(atacante, passinho);
        alvo->stunTicks = BLOCKSTUN_TICKS;
        alvo->ultimoGolpeTick = tickAtual;
        return;
    }

    alvo->hp -= dano;
    if (alvo->hp < 0)
        alvo->hp = 0;

    adicionarEnergia(atacante, GANHO_ENERGIA_ACERTO);
    aplicarRecuperacaoAtaque(atacante, passinho);
    alvo->stunTicks = STUN_TICKS_PADRAO;
    alvo->ultimoGolpeTick = tickAtual;
    alvo->golpesSeguidos = 0;
}

/*
 * Incrementa a barra de energia do jogador proporcional ao dano causado.
 * Limita a energia ao máximo (MAX_ENERGIA).
 */
void atualizarEnergia(Jogador *jogador, int dano)
{
    jogador->energia += dano / 2;
    if (jogador->energia > MAX_ENERGIA)
        jogador->energia = MAX_ENERGIA;
}

void adicionarEnergia(Jogador *jogador, int quantidade)
{
    jogador->energia += quantidade;
    if (jogador->energia > MAX_ENERGIA)
        jogador->energia = MAX_ENERGIA;
}

/*
 * Verifica se algum jogador zerou o HP.
 * Retorna 1 se jogador1 venceu, 2 se jogador2 venceu, 0 se ainda em curso.
 */
int verificarVencedor(Jogador *jogador1, Jogador *jogador2)
{
    if (jogador2->hp <= 0)
        return 1;
    if (jogador1->hp <= 0)
        return 2;
    return 0;
}

/*
 * Encerra o round: incrementa rounds vencidos.
 */
void encerrarRound(Jogador *vencedor)
{
    vencedor->roundsVencidos++;
}
