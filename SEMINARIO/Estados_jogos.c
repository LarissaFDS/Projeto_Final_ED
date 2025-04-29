#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <locale.h>

#define MAX_ESTADOS 100

typedef struct{
    char nome[50];  //representa um estado com um nome
}Estado;

typedef struct{
    Estado estados[MAX_ESTADOS]; //array de estado
    int topo; //indice pra saber o topo
}PilhaEstados;

// Funções da pilha
void inicializar(PilhaEstados *pilha){
    pilha -> topo = -1;
}

int vazia(PilhaEstados *pilha){
    return pilha -> topo == -1; //se tiver vazia retorna 1
}

int cheia(PilhaEstados *pilha){
    return pilha -> topo == MAX_ESTADOS - 1; //se tiver cheia retorna 1
}

void empilhar(PilhaEstados *pilha, const char *nomeEstado){
    if(cheia(pilha)){
        printf("\n\tErro: pilha cheia!\n");
        return;
    }
    pilha -> topo++; //avança o topo pro proximo indice
    strcpy(pilha -> estados[pilha -> topo].nome, nomeEstado); //copia o nome do novo estado pro topo
    printf("\n\tEntrou no estado: %s\n", nomeEstado);
}

void desempilhar(PilhaEstados *pilha){
    if(vazia(pilha)){
        printf("\n\tErro: pilha vazia!\n");
        return;
    }
    printf("\n\tSaindo do estado: %s", pilha -> estados[pilha -> topo].nome); //mostra o estado que vai ser removido
    pilha -> topo--; //decrementa o topo pra fazer a remoção lógica
}

void estadoAtual(PilhaEstados *pilha){
    if(vazia(pilha)){
        printf("\n\tJogando\n"); //estado inicial
        return;
    }
    printf("\n\tEstado atual: %s\n", pilha -> estados[pilha -> topo].nome);
}

int main(){
    setlocale(LC_ALL, "pt_BR.UTF-8");
    SetConsoleOutputCP(65001);

    PilhaEstados jogo;
    inicializar(&jogo);

    int opcao;
    char nomeEstado[50];

    do{
        printf("\nEscolha uma opção:\n");
        printf("1 - Empilhar novo estado\n");
        printf("2 - Desempilhar estado atual\n");
        printf("0 - Sair\n");
        printf("Opção: ");
        scanf("%d", &opcao);
        getchar(); // Consumir o '\n' deixado pelo scanf

        switch(opcao){
            case 1:
                printf("Digite o nome do novo estado: ");
                fgets(nomeEstado, sizeof(nomeEstado), stdin);
                nomeEstado[strcspn(nomeEstado, "\n")] = 0; // Remover o \n do final

                empilhar(&jogo, nomeEstado);
                estadoAtual(&jogo);
                break;
            case 2:
                desempilhar(&jogo);
                estadoAtual(&jogo);
                break;
            case 0:
                printf("Encerrando programa.\n");
                break;
            default:
                printf("Opção inválida!\n");
                break;
        }

    }while(opcao != 0);

    return 0;
}
