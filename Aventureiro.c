#include <stdio.h>
#include <stdlib.h>
#include <string.h>

    // Estrutura que representa um cômodo da mansão

typedef struct Sala {
    char nome[50];
    char pista[100];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

    // Estrutura que representa um nó da árvore BST de pistas

typedef struct NodoPista {
    char pista[100];
    struct NodoPista *esquerda;
    struct NodoPista *direita;
} NodoPista;

    // Cria uma sala (nó da árvore da mansão) de forma dinâmica,

Sala* criarSala(const char *nome, const char *pista) {
    Sala *novaSala = (Sala*) malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro ao alocar memória para sala!\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    if (pista != NULL)
        strcpy(novaSala->pista, pista);
    else
        strcpy(novaSala->pista, "");
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

    // Cria um novo nó da árvore de pistas.

NodoPista* criarNodoPista(const char *pista) {
    NodoPista *novo = (NodoPista*) malloc(sizeof(NodoPista));
    if (novo == NULL) {
        printf("Erro ao alocar memória para pista!\n");
        exit(1);
    }
    strcpy(novo->pista, pista);
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}

    // Insere uma nova pista na árvore BST de forma ordenada (alfabética).

NodoPista* inserirPista(NodoPista *raiz, const char *pista) {
    if (raiz == NULL)
        return criarNodoPista(pista);

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);

    return raiz;
}

    // Exibe as pistas da BST em ordem alfabética

void exibirPistas(NodoPista *raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("- %s\n", raiz->pista);
        exibirPistas(raiz->direita);
    }
}

    // Permite ao jogador explorar a mansão, coletando pistas automaticamente

void explorarSalasComPistas(Sala *salaAtual, NodoPista **arvorePistas) {
    char opcao;

    while (1) {
        printf("\nVocê está na sala: %s\n", salaAtual->nome);

        // Se houver pista, coleta automaticamente

        if (strlen(salaAtual->pista) > 0) {
            printf("Você encontrou uma pista: \"%s\"\n", salaAtual->pista);
            *arvorePistas = inserirPista(*arvorePistas, salaAtual->pista);
        } else {
            printf("Nenhuma pista nesta sala.\n");
        }

        // Mostra opções de caminho

        printf("\nEscolha um caminho:\n");
        if (salaAtual->esquerda != NULL)
            printf(" (e) Ir para a esquerda: %s\n", salaAtual->esquerda->nome);
        if (salaAtual->direita != NULL)
            printf(" (d) Ir para a direita: %s\n", salaAtual->direita->nome);
        printf(" (s) Sair da exploração\n");
        printf("Opção: ");
        scanf(" %c", &opcao);

        if (opcao == 'e' && salaAtual->esquerda != NULL) {
            salaAtual = salaAtual->esquerda;
        } else if (opcao == 'd' && salaAtual->direita != NULL) {
            salaAtual = salaAtual->direita;
        } else if (opcao == 's') {
            printf("\nVocê decidiu encerrar a exploração.\n");
            break;
        } else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

    // Cria a estrutura da mansão, inicia a exploração e exibe as pistas coletadas

int main() {

    // Construção manual da árvore da mansão

    Sala *hall = criarSala("Hall de Entrada", "Pegadas de lama no chão");
    Sala *salaEstar = criarSala("Sala de Estar", "Um lenço com iniciais misteriosas");
    Sala *cozinha = criarSala("Cozinha", "");
    Sala *biblioteca = criarSala("Biblioteca", "Um livro aberto na página 47");
    Sala *jardim = criarSala("Jardim", "Marcas de pneu frescas");
    Sala *adega = criarSala("Adega", "Cheiro forte de álcool derramado");

    // Conexões (estrutura da árvore)

    hall->esquerda = salaEstar;
    hall->direita = cozinha;
    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;
    cozinha->direita = adega;

    // Árvore de pistas (inicialmente vazia)

    NodoPista *arvorePistas = NULL;

    // Inicia a exploração

    explorarSalasComPistas(hall, &arvorePistas);

    // Exibe pistas coletadas

    printf("\n Pistas coletadas em ordem alfabética:\n");
    exibirPistas(arvorePistas);

    // Liberação de memória

    free(biblioteca);
    free(jardim);
    free(salaEstar);
    free(adega);
    free(cozinha);
    free(hall);

    return 0;
}
