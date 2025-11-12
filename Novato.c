#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Sala da mansão
typedef struct Sala {
    char nome[50];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;


Sala* criarSala(const char *nome) {
    Sala *novaSala = (Sala*) malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

    // Exploração das salas
void explorarSalas(Sala *salaAtual) {
    char opcao;

    while (1) {
        printf("\nVocê está na sala: %s\n", salaAtual->nome);

        // Se a sala não tiver caminhos
        if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
            printf("Não há mais caminhos a seguir. Fim da exploração!\n");
            break;
        }

            // Se a sala tiver caminhos
        printf("Escolha um caminho:\n");
        if (salaAtual->esquerda != NULL)
            printf(" (e) Ir para a esquerda: %s\n", salaAtual->esquerda->nome);
        if (salaAtual->direita != NULL)
            printf(" (d) Ir para a direita: %s\n", salaAtual->direita->nome);
        printf(" (s) Sair do jogo\n");
        printf("Opção: ");
        scanf(" %c", &opcao);

        if (opcao == 'e' && salaAtual->esquerda != NULL) {
            salaAtual = salaAtual->esquerda;
        } else if (opcao == 'd' && salaAtual->direita != NULL) {
            salaAtual = salaAtual->direita;
        } else if (opcao == 's') {
            printf("Você decidiu encerrar a exploração.\n");
            break;
        } else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}


    // Cria a estrutura da mansão e inicia a exploração.
int main() {
    
    // Criação manual da árvore binária da mansão
    Sala *hall = criarSala("Hall de Entrada");
    Sala *salaEstar = criarSala("Sala de Estar");
    Sala *cozinha = criarSala("Cozinha");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *jardim = criarSala("Jardim");
    Sala *adega = criarSala("Adega");

    // Conectando as salas
    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    cozinha->direita = adega;

    return 0;
}
