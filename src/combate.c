#include "combate.h"
#include <math.h>
#include <string.h>

const int energiaConsumida[3] = {10, 25, 80};

static float calcularDistancia(const Jogador *a, const Jogador *b)
{
    float dx = a->posX - b->posX;
    float dy = a->posY - b->posY;
    return sqrtf(dx * dx + dy * dy);
}

static void aplicarStun(Jogador *alvo, int tickAtual)
{
    alvo->stunTicks = STUN_TICKS_PADRAO;
    limparFila(&alvo->fila);
    alvo->ultimoGolpeTick = tickAtual;
    alvo->golpesSeguidos = 0;
}

/*
 * Processa um passinho atacante contra o alvo.
 * Verifica distância, energia e stun antes de aplicar dano.
 */
void processarPassinho(TipoPassinho passinho, Jogador *atacante, Jogador *alvo, Estatistica *stats, int tickAtual)
{
    if (atacante->stunTicks > 0)
        return;

    int custo = 0;
    int dano = 0;

    switch (passinho)
    {
    case ATAQUE_LEVE:
        custo = energiaConsumida[0];
        dano = atacante->personagem.danoLeve;
        break;
    case ATAQUE_MEDIO:
        custo = energiaConsumida[1];
        dano = atacante->personagem.danomedio;
        break;
    case ATAQUE_ESPECIAL:
        custo = energiaConsumida[2];
        dano = atacante->personagem.danoEspecial;
        break;
    default:
        break;
    }

    if ((passinho == ATAQUE_LEVE || passinho == ATAQUE_MEDIO || passinho == ATAQUE_ESPECIAL) && atacante->energia < custo)
    {
        return;
    }

    if (calcularDistancia(atacante, alvo) > DISTANCIA_MAXIMA_ATAQUE)
    {
        return;
    }

    if (passinho == ATAQUE_LEVE || passinho == ATAQUE_MEDIO || passinho == ATAQUE_ESPECIAL)
    {
        atacante->energia -= custo;
        if (atacante->energia < 0)
            atacante->energia = 0;
    }

    if (alvo->esquivaTicks > 0)
    {
        stats[1].esquivasRealizadas++;
        return;
    }

    if (!filaVazia(&alvo->fila) && peekFila(&alvo->fila) == ESQUIVA)
    {
        desenfileirarPassinho(&alvo->fila);
        stats[1].esquivasRealizadas++;
        return;
    }

    if (alvo->defendendo && alvo->noChao)
    {
        alvo->stunTicks = BLOCKSTUN_TICKS;
        limparFila(&alvo->fila);
        alvo->ultimoGolpeTick = tickAtual;
        return;
    }

    alvo->hp -= dano;
    if (alvo->hp < 0)
        alvo->hp = 0;

    atualizarEnergia(atacante, dano);
    stats[0].danoTotal += dano;

    if (tickAtual - alvo->ultimoGolpeTick < HITSTUN_WINDOW_TICKS)
    {
        alvo->golpesSeguidos++;
    }
    else
    {
        alvo->golpesSeguidos = 1;
    }

    if (alvo->golpesSeguidos >= 3)
    {
        aplicarStun(alvo, tickAtual);
    }
    else
    {
        alvo->stunTicks = STUN_TICKS_PADRAO;
        limparFila(&alvo->fila);
        alvo->ultimoGolpeTick = tickAtual;
    }
}

/*
 * Verifica se a sequência atual da fila corresponde a algum combo.
 * Retorna 1 se combo encontrado, 0 caso contrário.
 * A fila deve estar cheia (tamanho 3) antes de chamar essa função.
 */
int verificarCombo(FilaPassinhos *fila, Combo *combosPersonagem, int totalCombos)
{
    for (int i = 0; i < totalCombos; i++)
    {
        int comboBateu = 1;
        for (int j = 0; j < TAM_MAX_FILA; j++)
        {
            int idx = (fila->inicio + j) % TAM_MAX_FILA;
            if (fila->elementos[idx] != combosPersonagem[i].sequencia[j])
            {
                comboBateu = 0;
                break;
            }
        }
        if (comboBateu)
            return 1;
    }
    return 0;
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
 * Encerra o round: incrementa rounds vencidos e limpa filas.
 */
void encerrarRound(Jogador *vencedor, Estatistica *stats)
{
    vencedor->roundsVencidos++;
    limparFila(&vencedor->fila);
}
