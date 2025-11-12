#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NOME 64
#define MAX_PISTA 128
#define MAX_SUSPEITO 64
#define HASH_SIZE 101  // primo para hash


typedef struct Sala {
    char nome[MAX_NOME];
    char pista[MAX_PISTA]; // "" se não houver pista
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

        // Pista coletadas
typedef struct NodoPista {
    char pista[MAX_PISTA];
    struct NodoPista *esquerda;
    struct NodoPista *direita;
} NodoPista;

        // Tabela Pista -> Suspeito
typedef struct EntradaHash {
    char pista[MAX_PISTA];
    char suspeito[MAX_SUSPEITO];
    struct EntradaHash *proximo;
} EntradaHash;

typedef struct HashTable {
    EntradaHash *buckets[HASH_SIZE];
} HashTable;

void trim(char *s) {
    int n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r' || isspace((unsigned char)s[n-1]))) s[--n] = '\0';
    int i = 0;
    while (s[i] && isspace((unsigned char)s[i])) i++;
    if (i) memmove(s, s + i, strlen(s+i) + 1);
}

unsigned long hash_string(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

        // Criação de salas

Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        fprintf(stderr, "Erro de alocação para Sala\n");
        exit(EXIT_FAILURE);
    }
    strncpy(s->nome, nome, MAX_NOME-1);
    s->nome[MAX_NOME-1] = '\0';
    if (pista && strlen(pista) > 0) {
        strncpy(s->pista, pista, MAX_PISTA-1);
        s->pista[MAX_PISTA-1] = '\0';
    } else {
        s->pista[0] = '\0';
    }
    s->esquerda = s->direita = NULL;
    return s;
}

        // Insere ou busca as pistas

NodoPista* inserirPista(NodoPista *raiz, const char *pista) {
    if (raiz == NULL) {
        NodoPista *n = (NodoPista*) malloc(sizeof(NodoPista));
        if (!n) { fprintf(stderr, "Erro de alocação para NodoPista\n"); exit(EXIT_FAILURE); }
        strncpy(n->pista, pista, MAX_PISTA-1);
        n->pista[MAX_PISTA-1] = '\0';
        n->esquerda = n->direita = NULL;
        return n;
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0) raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (cmp > 0) raiz->direita = inserirPista(raiz->direita, pista);
        // Se igual, não insere duplicata
    return raiz;
}

        // busca se existe uma pista na BST (retorna 1 se existe)
int buscarPista(NodoPista *raiz, const char *pista) {
    if (raiz == NULL) return 0;
    int cmp = strcmp(pista, raiz->pista);
    if (cmp == 0) return 1;
    if (cmp < 0) return buscarPista(raiz->esquerda, pista);
    else return buscarPista(raiz->direita, pista);
}

        // Imprime as pistas em ordem alfabética

void exibirPistasInOrder(NodoPista *raiz) {
    if (!raiz) return;
    exibirPistasInOrder(raiz->esquerda);
    printf(" - %s\n", raiz->pista);
    exibirPistasInOrder(raiz->direita);
}

        // percorre BST e conta quantas pistas apontam para o suspeito acusado

int contarPistasParaSuspeito(NodoPista *raiz, HashTable *ht, const char *acusado);

        // Libera memória BST

void liberarBST(NodoPista *raiz) {
    if (!raiz) return;
    liberarBST(raiz->esquerda);
    liberarBST(raiz->direita);
    free(raiz);
}

HashTable* criarHash() {
    HashTable *ht = (HashTable*) malloc(sizeof(HashTable));
    if (!ht) { fprintf(stderr, "Erro alocacao HashTable\n"); exit(EXIT_FAILURE); }
    for (int i = 0; i < HASH_SIZE; ++i) ht->buckets[i] = NULL;
    return ht;
}

        // Insere a associação pista

void inserirNaHash(HashTable *ht, const char *pista, const char *suspeito) {
    unsigned long h = hash_string(pista) % HASH_SIZE;
    EntradaHash *atual = ht->buckets[h];
    while (atual) {
        if (strcmp(atual->pista, pista) == 0) {
            strncpy(atual->suspeito, suspeito, MAX_SUSPEITO-1);
            atual->suspeito[MAX_SUSPEITO-1] = '\0';
            return;
        }
        atual = atual->proximo;
    }
        // não encontrou: cria nova entrada e adiciona no início

    EntradaHash *novo = (EntradaHash*) malloc(sizeof(EntradaHash));
    if (!novo) { fprintf(stderr, "Erro alocar EntradaHash\n"); exit(EXIT_FAILURE); }
    strncpy(novo->pista, pista, MAX_PISTA-1);
    novo->pista[MAX_PISTA-1] = '\0';
    strncpy(novo->suspeito, suspeito, MAX_SUSPEITO-1);
    novo->suspeito[MAX_SUSPEITO-1] = '\0';
    novo->proximo = ht->buckets[h];
    ht->buckets[h] = novo;
}

        // Encontra o suspeito correspondete a pista

const char* encontrarSuspeito(HashTable *ht, const char *pista) {
    unsigned long h = hash_string(pista) % HASH_SIZE;
    EntradaHash *atual = ht->buckets[h];
    while (atual) {
        if (strcmp(atual->pista, pista) == 0) return atual->suspeito;
        atual = atual->proximo;
    }
    return NULL;
}

void liberarHash(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        EntradaHash *e = ht->buckets[i];
        while (e) {
            EntradaHash *prox = e->proximo;
            free(e);
            e = prox;
        }
    }
    free(ht);
}

        // Exploração das salas

void explorarSalas(Sala *salaAtual, NodoPista **arvorePistas, HashTable *ht) {
    char opcao;
    while (1) {
        printf("\nVocê está na sala: %s\n", salaAtual->nome);

        if (strlen(salaAtual->pista) > 0) {
            printf("  -> Pista encontrada: \"%s\"\n", salaAtual->pista);

        // adiciona apenas se ainda não coletada
        
            if (!buscarPista(*arvorePistas, salaAtual->pista)) {
                *arvorePistas = inserirPista(*arvorePistas, salaAtual->pista);
                const char *sus = encontrarSuspeito(ht, salaAtual->pista);
                if (sus)
                    printf("     (Essa pista aponta para o suspeito: %s)\n", sus);
                else
                    printf("     (Nenhum suspeito associado a esta pista)\n");
            } else {
                printf("     (Pista já coletada anteriormente)\n");
            }
        } else {
            printf("  -> Nenhuma pista nesta sala.\n");
        }

        // opções disponíveis

        printf("\nEscolha um caminho:\n");
        if (salaAtual->esquerda) printf(" (e) Esquerda: %s\n", salaAtual->esquerda->nome);
        if (salaAtual->direita)  printf(" (d) Direita: %s\n", salaAtual->direita->nome);
        printf(" (s) Sair e proceder ao julgamento\n");
        printf("Opção: ");

        // lê 1 caracter, ignorando whitespace

        if (scanf(" %c", &opcao) != 1) {

        // limpar stdin e continuar

            int ch; while ((ch = getchar()) != '\n' && ch != EOF);
            printf("Entrada inválida. Tente novamente.\n");
            continue;
        }

        if (opcao == 'e') {
            if (salaAtual->esquerda) salaAtual = salaAtual->esquerda;
            else printf("Caminho à esquerda indisponível. Tente outra opção.\n");
        } else if (opcao == 'd') {
            if (salaAtual->direita) salaAtual = salaAtual->direita;
            else printf("Caminho à direita indisponível. Tente outra opção.\n");
        } else if (opcao == 's') {
            printf("\nVocê encerrou a exploração.\n");
            break;
        } else {
            printf("Opção inválida. Use 'e', 'd' ou 's'.\n");
        }
    }
}

        // Verificação de suspeito

int verificarSuspeitoFinal(NodoPista *arvorePistas, HashTable *ht, const char *acusado) {
    int contador = contarPistasParaSuspeito(arvorePistas, ht, acusado);
    printf("\nEvidências encontradas que apontam para '%s': %d pista(s).\n", acusado, contador);
    if (contador >= 2) {
        printf("Resultado: Há evidências suficientes. Acusação procedente!\n");
        return 1;
    } else {
        printf("Resultado: Evidências insuficientes. Acusação improcedente.\n");
        return 0;
    }
}

        // Implementação auxiliar: percorre BST e conta pistas que apontam para acusado

int contarPistasParaSuspeito(NodoPista *raiz, HashTable *ht, const char *acusado) {
    if (!raiz) return 0;
    int cnt = 0;
    const char *sus = encontrarSuspeito(ht, raiz->pista);
    if (sus && strcmp(sus, acusado) == 0) cnt = 1;
    return cnt + contarPistasParaSuspeito(raiz->esquerda, ht, acusado)
               + contarPistasParaSuspeito(raiz->direita, ht, acusado);
}

        // Montagem fixa da mansão 

int main() {
    Sala *hall = criarSala("Hall de Entrada", "Pegadas de lama");
    Sala *salaEstar = criarSala("Sala de Estar", "Lenço com iniciais A");
    Sala *cozinha = criarSala("Cozinha", "Marca de queimadura no fogao");
    Sala *biblioteca = criarSala("Biblioteca", "Livro com páginas rasgadas");
    Sala *jardim = criarSala("Jardim", "Marcas de pneu");
    Sala *adega = criarSala("Adega", "Garrafa quebrada e cheiro de álcool");
    Sala *escritorio = criarSala("Escritório", "Carta ameaçadora assinada por B");
    Sala *quarto = criarSala("Quarto Principal", "fio de cabelo loiro");

        // Conexões (definindo a árvore)

    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    cozinha->esquerda = escritorio;
    cozinha->direita = adega;

    biblioteca->esquerda = quarto; // exemplo de ramificação adicional

        // Preencher a hash: mapa pista -> suspeito

    HashTable *ht = criarHash();
    inserirNaHash(ht, "Pegadas de lama", "Carlos");
    inserirNaHash(ht, "Lenço com iniciais A", "Ana");
    inserirNaHash(ht, "Marca de queimadura no fogao", "Bruno");
    inserirNaHash(ht, "Livro com páginas rasgadas", "Diana");
    inserirNaHash(ht, "Marcas de pneu", "Carlos");
    inserirNaHash(ht, "Garrafa quebrada e cheiro de álcool", "Bruno");
    inserirNaHash(ht, "Carta ameaçadora assinada por B", "Bruno");
    inserirNaHash(ht, "fio de cabelo loiro", "Ana");

        // BST de pistas coletadas (inicialmente vazia)

    NodoPista *arvorePistas = NULL;
    printf("=== Detective Quest: Jogo de Acusação ===\n");
    printf("Você inicia no Hall de Entrada. Explore a mansão e colete pistas.\n");
    printf("Comandos: (e) esquerda, (d) direita, (s) sair e acusar\n");

        // Inicia exploração interativa a partir do hall

    explorarSalas(hall, &arvorePistas, ht);

        // Exibe todas as pistas coletadas

    printf("\nPistas coletadas (em ordem alfabética):\n");
    if (arvorePistas == NULL) {
        printf(" Nenhuma pista coletada.\n");
    } else {
        exibirPistasInOrder(arvorePistas);
    }

        // Fase de acusação: pede ao jogador para digitar o nome do suspeito

    char acusado[MAX_SUSPEITO];

        // limpar leftover do buffer

    int ch; while ((ch = getchar()) != '\n' && ch != EOF);
    printf("\nInforme o nome do suspeito que deseja acusar: ");
    if (!fgets(acusado, sizeof(acusado), stdin)) acusado[0] = '\0';
    trim(acusado);

    if (strlen(acusado) == 0) {
        printf("Nenhum nome fornecido. Encerrando sem julgamento.\n");
    } else {

        // Verifica se existem ao menos 2 pistas apontando para o acusado

        verificarSuspeitoFinal(arvorePistas, ht, acusado);
    }

        // Limpeza de memória
    liberarBST(arvorePistas);
    liberarHash(ht);
    free(hall); 
    free(salaEstar);
    free(cozinha);
    free(biblioteca); 
    free(jardim);
    free(adega); 
    free(escritorio); 
    free(quarto);

    printf("\nFim do jogo. Obrigado por jogar!\n");
    return 0;
}
