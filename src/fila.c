#include "fila.h"
#include <stddef.h>

void inicializarFilaPersonagens(FilaPersonagens *fila)
{
    fila->inicio = NULL;
    fila->fim = NULL;
    fila->tamanho = 0;
    fila->total = 0;

    for (int i = 0; i < TAM_EQUIPE; i++)
    {
        fila->membros[i].indicePersonagem = JOAO_CAMPOS;
        fila->membros[i].vivo = 0;
        fila->membros[i].next = NULL;
    }
}

int enfileirarPersonagem(FilaPersonagens *fila, IndicePersonagem indicePersonagem)
{
    if (fila->total >= TAM_EQUIPE)
        return 0;

    NoPersonagem *novo = &fila->membros[fila->total++];
    novo->indicePersonagem = indicePersonagem;
    novo->vivo = 1;
    novo->next = NULL;

    if (fila->fim != NULL)
        fila->fim->next = novo;
    else
        fila->inicio = novo;

    fila->fim = novo;
    fila->tamanho++;
    return 1;
}

NoPersonagem *noPersonagemAtivo(FilaPersonagens *fila)
{
    return fila->inicio;
}

const NoPersonagem *noPersonagemAtivoConst(const FilaPersonagens *fila)
{
    return fila->inicio;
}

int indicePersonagemAtivo(const FilaPersonagens *fila)
{
    if (fila->inicio == NULL)
        return 0;
    return fila->inicio->indicePersonagem;
}

int tamanhoFilaPersonagens(const FilaPersonagens *fila)
{
    return fila->tamanho;
}

int filaPersonagensTemVivos(const FilaPersonagens *fila)
{
    return fila->inicio != NULL;
}

int filaPersonagensPodeRotacionar(const FilaPersonagens *fila)
{
    return fila->tamanho > 1;
}

void rotacionarFilaPersonagens(FilaPersonagens *fila)
{
    if (!filaPersonagensPodeRotacionar(fila))
        return;

    NoPersonagem *antigoAtivo = fila->inicio;
    fila->inicio = antigoAtivo->next;
    antigoAtivo->next = NULL;
    fila->fim->next = antigoAtivo;
    fila->fim = antigoAtivo;
}

void removerPersonagemAtivo(FilaPersonagens *fila)
{
    if (fila->inicio == NULL)
        return;

    NoPersonagem *removido = fila->inicio;
    fila->inicio = removido->next;
    removido->vivo = 0;
    removido->next = NULL;
    fila->tamanho--;

    if (fila->inicio == NULL)
        fila->fim = NULL;
}
