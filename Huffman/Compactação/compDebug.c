#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <locale.h>

#define TAMANHO_MAX 256

// Estrutura de Dados principal 
typedef struct Base {
    void* dados;
    void* frequencia;
    struct Base *proximo;
    struct Base *direita;
    struct Base *esquerda;
} Base;

// Estrutura de Dados auxiliar
typedef struct Lista{
    Base *head;
    int tamanho;
}Lista;

// ------------------------- Compactação ------------------------- //

// Etapa 1 : Tabela de Frequências ------------------------- //

// Cria a tabela de frequências e inicializa com zero
void Inicializar_Tabela(unsigned int *tabela){
    for (int i = 0; i < TAMANHO_MAX; i++) {
        tabela[i] = 0;
    }
}

// Vai receber o arquivo e a tabela inicializada com zero
void Definir_Tabela_Freq(FILE *arquivo, unsigned int *tab_frequencia) {
    unsigned char index = 0; 
    while (fread(&index, 1, 1, arquivo) == 1) { // Lê o arquivo byte a byte
        tab_frequencia[(unsigned char)index]++; // Incrementa a frequência do símbolo lido, preechendo a tabela
    }
}

// Função para testes
void imprimir_tabela(unsigned int *tabela) {
    printf("Tabela de Frequencias:\n");
    for (int i = 0; i < TAMANHO_MAX; i++) {
        if (tabela[i] > 0) {
            printf("Simbolo: %c, Frequencia: %u\n", i, tabela[i]);
        }
    }
}

// Etapa 2 : Listas Encadeadas Ordenadas ------------------------- //

// Inicializa a lista
void Criar_Lista_Encadeada(Lista *l) {
    l->head = NULL; // Inicializa a cabeça da lista como NULL
    l->tamanho = 0; // Inicializa o tamanho da lista como 0  
}
// Vai inserir os nós da árvore na lista encadeada de forma ordenada. Recebe o nó a ser inserido e a lista encadeada
// No final de tudo, a head vai apontar para o no com menor frequencia.
void Inserir_em_Ordem(Lista *l, Base *base){
    if(l->head == NULL || *(long long int *)base->frequencia < *(long long int *)l->head->frequencia){
         // Se caso a frequencia for menor que a frequencia do inicio da lista ou se a lista estiver vazia
         base->proximo = l->head; // Adiciona o nó no início da lista, se caso head apontar pra nulo agora o novo nó vai apontar pra nulo. Se caso apontar pra outro no, faz a troca de ponteiros.
         l->head = base; // Agora head vai apontar pra o novo nó adicionado
         l->tamanho++; // Incrementa o tamanho da lista
         return;
    }  
 
    Base *atual = l->head; // Inicializa um ponteiro auxiliar para percorrer a lista
    while(atual->proximo != NULL && *(long long int *)atual->proximo->frequencia < *(long long int *)base->frequencia){
        atual = atual->proximo; // Percorre a lista até encontrar a posição correta
    }
    base->proximo = atual->proximo; // Insere o nó na posição correta
    atual->proximo = base; // Atualiza o ponteiro do nó anterior
    l->tamanho++; // Incrementa o tamanho da lista
}
// Vai preencher a lista encadeada com os nós criados a partir da tabela de frequências. Recebe a lista encadeada vazia e a tabela de frequências
// Alerta nessa função
void Preencher_Lista_Encadeada(Lista *l, unsigned int *tabela_frequencia) {
    for (int i = 0; i < TAMANHO_MAX; i++) {
        if (tabela_frequencia[i] > 0) { // Verifica se a frequência é maior que zero. As frequencias maiores que zero que importam

            Base *novo_nodo = malloc(sizeof(Base)); // Aloca memória para o novo nó

            if (!novo_nodo) { // Verifica se houve algum erro na alocação
                perror("Erro ao alocar memória para o nó da lista encadeada.\n");
                exit(EXIT_FAILURE); // Encerra o programa se a alocação falhar
            }
            // VERIFICAR SE VAI PRECISAR MANTER COM CASTING DESSA FORMA MAIS PRA FRENTE
            // Aloca memória para o símbolo
            unsigned char *simbolo = malloc(sizeof(unsigned char));
            *simbolo = (unsigned char)i;
            novo_nodo->dados = simbolo;

            // Aloca memória para a frequência 
            long long int *frequencia = malloc(sizeof(long long int));
            *frequencia = (long long int)tabela_frequencia[i];
            novo_nodo->frequencia = frequencia;
            // Inicializa os ponteiros para nulo pois não sabemos onde eles vão apontar ainda
            novo_nodo->proximo = NULL; 
            novo_nodo->direita = NULL; 
            novo_nodo->esquerda = NULL; 
            Inserir_em_Ordem(l, novo_nodo); // Vai garantir que a lista esteja ordenada.
        }
    }
}
// Função para testar a lista encadeada
void imprimir_lista(Lista *l) {
    Base *atual = l->head; // Inicializa um ponteiro auxiliar para percorrer a lista
    printf("Lista Encadeada:\n");
    printf("Tamanho da lista: %d\n", l->tamanho); // Imprime o tamanho da lista
    while (atual != NULL) { // Enquanto o ponteiro atual não for NULL
        printf("Simbolo: %c, Frequencia: %I64d\n", *(unsigned char *)atual->dados, *(long long int *)atual->frequencia);// Imprime o símbolo e a frequência
        atual = atual->proximo; // Avança para o próximo nó
    }
}


// Etapa 3 : Montar a árvore de Huffman ------------------------- //

/* 
A árvore de Huffman é uma árvore binária onde cada nó interno tem dois filhos e cada folha representa um símbolo.
A árvore é construída a partir da tabela de frequências, onde os símbolos mais frequentes estão mais próximos da raiz e os menos frequentes estão mais distantes.
*/

// Vai receber a lista e vai retornar o inicio da lista, o no com a menor frequencia.
Base* Pegar_Node_Inicial(Lista *l) {
    Base *temp = NULL; // Inicializa um ponteiro temporário para armazenar o nó a ser removido

    if (l->head != NULL) { // Verifica se a lista não está vazia
        temp = l->head; // Vai receber o nó que queremos pegar para remoção
        l->head = l->head->proximo; // O novo inicio vai ser o proximo node já que vamos remover esse nó
        l->tamanho--; // Decrementa o tamanho da lista, visto que removemos um nó
        temp->proximo = NULL; // O nó removido não deve apontar para nada, já que ele foi removido da lista
    }
    return temp; // Retorna o nó da cabeça da lista
}
// Recebe a lista e constrói a árvore de Huffman, retornando a raiz da árvore   
Base* Construir_Arvore_de_Huffman(Lista *no){
    Base *node1;
    Base *node2; 
    while(no->tamanho > 1) {
         node1 = Pegar_Node_Inicial(no); 
         node2 = Pegar_Node_Inicial(no); 

        // Cria um novo nó que vai ser a raiz da árvore
        Base *raiz_temporaria = malloc(sizeof(Base)); // Aloca memória para o novo nó

        if (!raiz_temporaria) { // Verifica se houve algum erro na alocação
            perror("Erro ao alocar memória para o nó da árvore de Huffman.\n");
            exit(EXIT_FAILURE); // Encerra o programa se a alocação falhar
        }
        // Aloca memória para a frequência e soma as frequências dos nós filhos
        long long int *frequencia = malloc(sizeof(long long int));
        *frequencia = *(long long int *)node1->frequencia + *(long long int *)node2->frequencia;


        //printf("Frequência: %lld\n", *frequencia); // Imprime a frequência do nó pai
        raiz_temporaria->frequencia = frequencia; // A frequência do novo nó é a soma das frequências dos nós filhos
        unsigned char *simbolo = malloc(sizeof(unsigned char));
        *simbolo = '*';
        raiz_temporaria->dados = simbolo;
        // Os filhos do novo nó são os nós que foram removidos da lista
        raiz_temporaria->esquerda = node1; 
        raiz_temporaria->direita = node2; 
        raiz_temporaria->proximo = NULL; // O próximo nó é NULL, já que ele vai ser a nova cabeça da lista

        Inserir_em_Ordem(no, raiz_temporaria); // Vai inserir o Nó pai na posição correta
        
    }
    Base *raiz_final = no->head; // A raiz da árvore é o único nó restante na lista encadeada
    no->head = NULL; // A lista encadeada agora está vazia
    no->tamanho = 0; // O tamanho da lista encadeada agora é zero
   return raiz_final;
}

// Função para testar a arvore
void imprimir_folhas_pre_ordem(Base *raiz, int nivel) {
    if (raiz == NULL) return;

    // Se for folha, imprime
    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        printf("Simbolo: %c | Frequencia: %I64d | Altura: %d\n", *(unsigned char *)raiz->dados, *(long long int *)raiz->frequencia, nivel);
    }
    else{
        // Percorre esquerda e direita (em ordem de pré-ordem)
        imprimir_folhas_pre_ordem(raiz->esquerda, nivel + 1);
        imprimir_folhas_pre_ordem(raiz->direita, nivel + 1);
    }  
}
// Etapa 4 : Gerar Dicionário de Códigos ------------------------- //


// A árvore de Huffman é percorrida em pré-ordem para gerar os códigos binários para cada símbolo.

// A função recebe a raiz da árvore e retorna a altura da mesma (Entender melhor essa função)
int Calcular_Altura_Arvore(Base *raiz){
    if (raiz == NULL) return 0; // Se a raiz for nula, retorna 0
    // Processo recursivo para verificar a altura da árvore
    int altura_esquerda = Calcular_Altura_Arvore(raiz->esquerda) + 1; // Soma 1 porque foi percorrida uma aresta pra fazer isso
    int altura_direita = Calcular_Altura_Arvore(raiz->direita) + 1; 
    // Verifica qual é a maior altura e retorna ela
    return (altura_esquerda > altura_direita) ? altura_esquerda : altura_direita;
}

// rever essa função depois (VER O PRIMEIRO CALLOC)
// recebe a altura da arvore somada mais 1 que resulta no numero total de colunas da matriz do dicionario. Vai retornar o dicionario inicializado antes de preenche-lo.
char** Definir_Dicionario(int colunas){
    char **dicionario = calloc(TAMANHO_MAX, sizeof(char*)); // Aloca memória para 256 ponteiros de char (um para cada símbolo ASCII possível)
    for (int i = 0; i < TAMANHO_MAX; i++) {
        dicionario[i] = calloc(colunas, sizeof(char)); // Aloca memória para a string do código binário do símbolo i (com espaço para colunas caracteres + '\0'
    }
    return dicionario; // Retorna o dicionário inicializado
}
// Essa função vai preencher o dicionario com os simbolos e os respectivos codigos binarios. Ela vai receber a raiz da arvore, o dicionario, o codigo a ser formado(caminho) e o numero de colunas
// A função retorna o dicionário preenchido. (Entender bem essa função depois)
void Completar_Dicionario(Base* raiz, char** dicionario, char *codigo, int colunas){
    if (raiz == NULL) return; 

    // Se for folha, preenche o dicionário com o símbolo e o código binário
    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        strcpy(dicionario[*(unsigned char *)raiz->dados], codigo); // Copia o código binário para uma linha do dicionário
    } else {
        // Percorre a árvore para gerar os códigos binários
        char codigo_esquerda[colunas]; // Array auxiliar para armazenar o código da esquerda
        char codigo_direita[colunas]; // Array auxiliar para armazenar o código da direita
        // Se andarmos pra esquerda, adicionamos '0' ao código, se andarmos pra direita, adicionamos '1'
        snprintf(codigo_esquerda, sizeof(codigo_esquerda), "%s0", codigo); // Adiciona '0' ao código da esquerda
        snprintf(codigo_direita, sizeof(codigo_direita), "%s1", codigo); // Adiciona '1' ao código da direita
        Completar_Dicionario(raiz->esquerda, dicionario, codigo_esquerda, colunas); // Chama recursivamente para a esquerda
        Completar_Dicionario(raiz->direita, dicionario, codigo_direita, colunas); // Chama recursivamente para a direita
    }
}

void imprime_dicionario(char** dicionario){
    printf("Dicionario:\n");
    for (int i = 0; i < TAMANHO_MAX; i++) {
        if (dicionario[i][0] != '\0') { // Verifica se o símbolo tem um código associado
            printf("Simbolo: %c, Codigo: %s\n", i, dicionario[i]); // Imprime o símbolo e o código binário
        }
    }
}

// Etapa 5 : Funções para o cabeçalho ----------------------------- //

char* Decimal_para_Binario(int Decimal, int tamanho_necessario){
    char *binario = malloc(tamanho_necessario + 1); // Aloca memória para n bits + '\0'
    if (!binario) {
        perror("Erro ao alocar memória para a conversão de decimal para binário.\n");
        exit(EXIT_FAILURE); // Encerra o programa se a alocação falhar
    }
    binario[tamanho_necessario] = '\0'; // Adiciona o terminador de string
    for (int i = tamanho_necessario - 1; i >= 0; i--) { // Converte o decimal para binário
        binario[i] = (Decimal % 2) + '0'; // Adiciona '0' ou '1' ao array
        Decimal /= 2; // Divide por 2 para obter o próximo bit
    }
    return binario; // Retorna a string binária
}

// Recebe o tamanho do codigo binario produzido pela codificacao e retorna o tamanho do lixo em decimal que vai ser adicionado no cabeçalho
int Calcular_Lixo(long long tamanho_bits) {
    return (8 - (tamanho_bits % 8)) % 8;
}

bool Caracter_Especial(unsigned char c) {
    if(c == '*' || c == '\\') // Verifica se o caractere é especial
        return true; // Se for, retorna verdadeiro
    else 
        return false; // Se não for, retorna falso    
}

void Escrever_Pre_Ordem(Base* no, char* buffer, int* posicao) {
    if (no == NULL) return;

    // Folha
    if (!no->esquerda && !no->direita) {
        unsigned char dado = *(unsigned char*)no->dados;

        // Escreve caractere com escape, se necessário
        if (Caracter_Especial(dado)) {
            buffer[(*posicao)++] = '\\';
        }

        buffer[(*posicao)++] = dado;
        return;
    }

    // Nó interno
    buffer[(*posicao)++] = '*';
    // Pré-ordem: escreve o nó atual, depois a subárvore esquerda e depois a subárvore direita
    Escrever_Pre_Ordem(no->esquerda, buffer, posicao);
    Escrever_Pre_Ordem(no->direita, buffer, posicao);
}

int Calcular_Tamanho_Arvore(Base* raiz) {
    if (raiz == NULL) return 0;

    // Se for folha
    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        unsigned char c = *(unsigned char*)raiz->dados;
        if (c == '*' || c == '\\') {
            return 2; // Usa escape (\*) ou (\\), necessita de 2 espaços pra ser representado
        } else {
            return 1; // Só o caractere normal
        }
    }
    // Para os nós não-folha, conta 1 para o nó e soma os tamanhos das subárvores
    return 1 + Calcular_Tamanho_Arvore(raiz->esquerda) + Calcular_Tamanho_Arvore(raiz->direita);
}

char* Arvore_Pre_Ordem(Base* raiz) {
    int tamanho = Calcular_Tamanho_Arvore(raiz);
    char *saida = calloc(tamanho + 1, sizeof(char)); // +1 só por segurança pro '\0'
    if (!saida) {
        perror("Erro ao alocar memória para árvore serializada");
        exit(EXIT_FAILURE);
    }

    int pos = 0;
    Escrever_Pre_Ordem(raiz, saida, &pos);
    saida[pos] = '\0'; // Adiciona o terminador de string
    
    return saida;
}

// Etapa 6 : Codificação e Compactação ------------------------- //
// Recebe o arquivo e o dicionário e retorna o código binário gerado
// A função lê o arquivo byte a byte e concatena os códigos binários correspondentes no dicionário
char* Codificar_Arquivo(unsigned char* dados, long long tamanhoArquivo, char** dicionario) {
    
    long long tamanho_bits = 0;
    for (long long i = 0; i < tamanhoArquivo; i++) {
        tamanho_bits += strlen(dicionario[dados[i]]);
    }

    char *codigo_binario = calloc(tamanho_bits + 1, sizeof(char));
    if (!codigo_binario) {
        perror("Erro ao alocar memória para código binário");
        exit(EXIT_FAILURE);
    }

    char *ptr = codigo_binario;
    for (long long i = 0; i < tamanhoArquivo; i++) {
        const char* cod = dicionario[dados[i]];
        memcpy(ptr, cod, strlen(cod));
        ptr += strlen(cod);
    }

    *ptr = '\0';
    return codigo_binario;
}


// (teste)
void decodificar(char *codigo_binario, Base *raiz) {
    int i = 0; // Inicializa o índice do código binário
       Base *atual = raiz; // Inicializa o ponteiro atual na raiz da árvore
       while(codigo_binario[i] != '\0') { // Enquanto não chegar ao final do código binário
           if (codigo_binario[i] == '0') { // Se o bit for '0', vai para a esquerda
               atual = atual->esquerda;
           } else { // Se o bit for '1', vai para a direita
               atual = atual->direita;
           }
           // Se for uma folha, imprime o símbolo e volta para a raiz
           if (atual->esquerda == NULL && atual->direita == NULL) {
               printf("%c", *(unsigned char *)atual->dados); // Imprime o símbolo encontrado
               atual = raiz; // Volta para a raiz da árvore
           }
           i++; // Avança para o próximo bit do código binário
       }
   printf("\n"); // Imprime uma nova linha ao final
}
// Função para testes
void imprimir_codigo_binario(char *codigo_binario) {
    printf("Codigo Binario:\n%s\n", codigo_binario); // Imprime o código binário
}

void Printar_Byte_em_Binario(unsigned char byte) {
    for (int i = 7; i >= 0; i--) {
        putchar((byte & (1 << i)) ? '1' : '0');
    }
}

// Parte 6 : COMPACTAR (Provavelmente errado)
// Recebe o código binário e o tamanho em bits e compacta o arquivo
// A função escreve o código binário em um arquivo binário, byte a byte
// Manipulação de bits
void Compactar_Arquivo(char *codigo_binario, long long int tamanho_bits, Base *HuffTree) {
    FILE *arquivo = fopen("C:\\Huffman\\compactado.huff", "wb");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para escrita.\n");
        return;
    }
    //Antes de escrever a parte compactada, precisamos escrever o cabeçalho
    // O cabeçalho vai consistir em 3 bits para o tamanho do lixo, 13 bits para o tamanho da arvore e a arvore serializada em pre ordem.
    // Vamos usar fwrite para escrever os bytes correspondentes ao cabeçalho no começo do arquivo.

    // Cabeçalho:
    int lixo = Calcular_Lixo(tamanho_bits);
    int tam_arvore = Calcular_Tamanho_Arvore(HuffTree);
    char *arvore_serializada = Arvore_Pre_Ordem(HuffTree);

    // Monta os dois primeiros bytes (16 bits)
    // 3 bits para o lixo + 13 bits para o tamanho da árvore
    unsigned short cabecalho = (lixo << 13) | tam_arvore;

    // Quebra o cabeçalho em 2 bytes (big endian)
    unsigned char byte1 = cabecalho >> 8;      // bits mais significativos
    unsigned char byte2 = cabecalho % 256;    // bits menos significativos

    // Agora escreve no arquivo:
    fwrite(&byte1, 1, 1, arquivo);
    fwrite(&byte2, 1, 1, arquivo);
    fwrite(arvore_serializada, 1, tam_arvore, arquivo);

    printf("\n--- DEBUG DO CABEÇALHO ---\n");
    printf("Byte 1 (lixo + parte alta do tam_arvore): ");
    Printar_Byte_em_Binario(byte1);
    printf("  (0x%02X)\n", byte1);

    printf("Byte 2 (parte baixa do tam_arvore):        ");
    Printar_Byte_em_Binario(byte2);
    printf("  (0x%02X)\n", byte2);

// Mostra os primeiros caracteres da árvore serializada
printf("Início da árvore serializada: \"");
for (int i = 0; i < 10 && i < tam_arvore; i++) {
    if (arvore_serializada[i] == '\n') printf("\\n");
    else if (arvore_serializada[i] == '\r') printf("\\r");
    else if (arvore_serializada[i] == '\\') printf("\\\\");
    else if (arvore_serializada[i] == '*') printf("*");
    else printf("%c", arvore_serializada[i]);
}
printf("\"\n");

// Mostrar os primeiros bits do código do conteúdo
printf("Início dos bits do conteúdo: ");
for (int i = 0; i < 40 && codigo_binario[i] != '\0'; i++) {
    putchar(codigo_binario[i]);
}
printf("...\n");



    int j = 7;
    unsigned char byte = 0;

    for (long long int i = 0; i < tamanho_bits; i++) {
        if (codigo_binario[i] == '1') {
            byte |= (1 << j);
        }
        j--;

        if (j < 0) {
            fwrite(&byte, 1, 1, arquivo);
            byte = 0;
            j = 7;
        }
    }

    if (j != 7) {
        fwrite(&byte, 1, 1, arquivo); // Escreve o último byte incompleto
    }

    fclose(arquivo);

    printf("Arquivo compactado com sucesso!\n");
    printf("Tamanho real esperado do código em bytes: %I64d bytes\n", (tamanho_bits + 7) / 8);
    printf("Bits totais: %I64d | Bits lixo no final: %d\n", tamanho_bits, lixo);
    printf("Tamanho da árvore: %d \n", tam_arvore);
}

// SÓ FUNCIONA PARA TEXTO POR ENQUANTO. ESTÁ SEM O CABEÇALHO TBM, OU SEJA, NÃO DÁ PRA DESCOMPACTAR AINDA
int main() {
    setlocale(LC_ALL, "pt_BR.UTF-8");

    char nome_arquivo[FILENAME_MAX];
    printf("Digite o nome do arquivo a ser compactado: ");
    scanf(" %[^\n]", nome_arquivo);

    char caminho_completo[FILENAME_MAX];
    snprintf(caminho_completo, FILENAME_MAX, "C:\\Huffman\\%s", nome_arquivo);

    FILE *arquivo = fopen(caminho_completo, "rb");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    // Passo 1: Frequência
    unsigned int tabela_frequencia[TAMANHO_MAX];
    Inicializar_Tabela(tabela_frequencia);
    Definir_Tabela_Freq(arquivo, tabela_frequencia);
    imprimir_tabela(tabela_frequencia); // Imprime a tabela de frequências para testes
    rewind(arquivo);

    // Passo 2: Lista Encadeada
    Lista lista;
    Criar_Lista_Encadeada(&lista);
    Preencher_Lista_Encadeada(&lista, tabela_frequencia);

    // Passo 3: Árvore de Huffman
    Base *HuffTree = Construir_Arvore_de_Huffman(&lista);
    printf("\n--- Árvores folhas em pré-ordem ---\n");
    imprimir_folhas_pre_ordem(HuffTree, 0);


    // Passo 4: Dicionário
    int altura = Calcular_Altura_Arvore(HuffTree);
    char **dicionario = Definir_Dicionario(altura + 1);
    Completar_Dicionario(HuffTree, dicionario, "", altura + 1);
    for (int i = 0; i < 256; i++) {
        if (dicionario[i][0] != '\0') {
            printf("Byte %3d | Huffman: %s | %I64u bits\n", i, dicionario[i], strlen(dicionario[i]));
        }
    }

    // Passo 5: Leitura dos dados
    fseek(arquivo, 0, SEEK_END);
    long long tamanhoArquivo = ftell(arquivo);
    rewind(arquivo);
    unsigned char *dados = malloc(tamanhoArquivo);
    fread(dados, 1, tamanhoArquivo, arquivo);
    fclose(arquivo);

    // Passo 6: Codificação e Compactação
    char *codigo_binario = Codificar_Arquivo(dados, tamanhoArquivo, dicionario);
    long long tamanho_bits = strlen(codigo_binario);
    Compactar_Arquivo(codigo_binario, tamanho_bits, HuffTree);

    return 0;
}
