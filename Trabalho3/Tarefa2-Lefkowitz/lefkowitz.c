//trabalho de ED2
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELEMENTS 8

// Estrutura para representar os registros
typedef struct {
    int ED;                    // Endereço de Disco (adicionado)
    int codcli;               // Código do Cliente (Chave Primária)
    int idade;                // Idade (Atributo não chave)
    int PROX;                 // Ponteiro para próximo registro com o mesmo nome (para A6)
} RegistroA2;

typedef struct {
    int ED;                    // Endereço de Disco (adicionado)
    int codcli;               // Código do Cliente (Chave Primária)
    int idade;                // Idade (Atributo não chave)
    char nome[20];            // Nome associado ao código do cliente
    int PROX;                 // Ponteiro para próximo registro com o mesmo nome (para A6)
} RegistroA3;

typedef struct {                   // Endereço de Disco (adicionado)
    int codcli;
    char nome[20];               // Código do Cliente (Chave Primária)
    int idade;                // Idade (Atributo não chave)
} tempA2;

typedef struct {
    int ignore;                   // Endereço de Disco (adicionado)
    int codcli;               // Código do Cliente (Chave Primária)
    int idade;                // Idade (Atributo não chave)
} tempA3;

// Passo 1: Criar o Arquivo A2 com ED, codcli, idade
void criarArquivoA2(FILE *arquivoDados, FILE *arquivoA2, char name_file[]) {
    RegistroA2 *temp = (RegistroA2 *)malloc(20 * sizeof(RegistroA2));
    tempA2 tempo;
    int temporario;
    int i = 0;
    fclose(arquivoDados);
    arquivoDados = fopen("dados copy.bin", "rb");
    while (!feof(arquivoDados))
    {
        fread(&tempo, sizeof(tempA2), 1, arquivoDados);
        temp[i].codcli = tempo.codcli;
        temp[i].idade = tempo.idade;
        printf("cod: %d idade: %d\n", temp[i].codcli, temp[i].idade);
        //fscanf(arquivoDados, "%d %*s %d", &temp[i].codcli, &temp[i].idade);
        i++;
    }

    i--;

    for(int j = 0; j < i; j++ )
    {
        temporario = j + 1;
        fwrite(&temporario, sizeof(int), 1, arquivoA2);
        fwrite(&temp[j].codcli, sizeof(int), 1, arquivoA2);
        fwrite(&temp[j].idade, sizeof(int), 1, arquivoA2);
        //fprintf(arquivoA2, "%d %d %d\n",j + 1, temp[j].codcli, temp[j].idade);
    }
    fclose(arquivoA2);

    arquivoA2 = fopen("A2.bin", "rb");

    printf("\n");
    printf("arquivo A2:\n");
    for(int j = 0; j < i; j++ )
    {
        fread(&temporario, sizeof(int), 1, arquivoA2);
        printf("id:%d ", temporario);
        fread(&temporario, sizeof(int), 1, arquivoA2);
        printf("codcli:%d ", temporario);
        fread(&temporario, sizeof(int), 1, arquivoA2);
        printf("idade:%d\n", temporario);
    }
    fclose(arquivoA2);
}

int compararIdade(const void *a, const void *b) {
    const RegistroA3 *reg1 = (const RegistroA3 *)a;
    const RegistroA3 *reg2 = (const RegistroA3 *)b;

    // Primeiro, compara pela idade
    if (reg1->idade != reg2->idade) {
        return reg1->idade - reg2->idade;
    } 
    // Se as idades são iguais, compara pelo ED
    return reg1->ED - reg2->ED;
}


RegistroA3 *criarArquivoA3(FILE *arquivoDados_base, FILE *arquivoA3) {
    rewind(arquivoDados_base);
    tempA2 tempo;
    tempA3 tempo_a3;
    FILE *arquivoDados = fopen("A2.bin", "rb");
    RegistroA3 *temp = (RegistroA3 *)malloc(20 * sizeof(RegistroA3));
    int i = 0;

    while (!feof(arquivoDados_base))
    {
        fread(&tempo, sizeof(tempA2), 1, arquivoDados_base);
        //fscanf(arquivoDados_base, "%*d %s %*d", temp[i].nome);
        strcpy(temp[i].nome, tempo.nome);
        i++;
        temp[i - 1].ED = i;
    }
    i=0;
    while(!feof(arquivoDados))
    {
        fread(&tempo_a3, sizeof(tempA3), 1, arquivoDados);
        temp[i].codcli = tempo_a3.codcli;
        temp[i].idade = tempo_a3.idade;
        //fscanf(arquivoDados, "%*d %d %d", &temp[i].codcli, &temp[i].idade);
        i++;
    }
    i--;
    qsort(temp, i, sizeof(RegistroA3), compararIdade);

    for(int j = 0; j < i; j++ )
    {
        tempo_a3.ignore = temp[j].ED;
        tempo_a3.codcli = temp[j].codcli;
        tempo_a3.idade = temp[j].idade;
        fwrite(&tempo_a3, sizeof(tempA3), 1, arquivoA3);
    } 

    fclose(arquivoA3);
    arquivoA3 = fopen("A3.bin", "rb");

    int temp_int;

    printf("\n");
    printf("arquivo A3:\n");
    for(int j = 0; j < i; j++ )
    {
        fread(&temp_int, sizeof(int), 1, arquivoA3);
        printf("ED: %d ", temp_int);
        fread(&temp_int, sizeof(int), 1, arquivoA3);
        printf("codcli: %d ", temp_int);
        fread(&temp_int, sizeof(int), 1, arquivoA3);
        printf("idade: %d\n", temp_int);
    }

    fclose(arquivoA3);

    return temp;
}

int jaExiste(int *vetor, int tamanho, int numero) {
    for (int i = 0; i < tamanho; i++) {
        if (vetor[i] == numero) {
            return 1; // Número encontrado, já existe
        }
    }
    return 0; // Número não encontrado
}

// Função para retornar números únicos e seus índices
void numerosUnicosEIndices(RegistroA3 *vetor, int tamanho, int *unicos, int *indices, int *novoTamanho) {
    int indice = 0;

    for (int i = 0; i < tamanho; i++) {
        // Verifica se o número já foi registrado
        if (!jaExiste(unicos, indice, vetor[i].idade)) {
            unicos[indice] = vetor[i].idade;  // Armazena o número único
            indices[indice] = vetor[i].ED;        // Armazena o índice da primeira ocorrência
            indice++;
        }
    }

    // Atualiza o tamanho do vetor de números únicos
    *novoTamanho = indice;
}

void criarArquivoA4( FILE *arquivoA4, FILE *arquivoA5, RegistroA3 *reg){
    int unicos[ELEMENTS];    // Vetor para armazenar números únicos
    int indices[ELEMENTS];   // Vetor para armazenar os índices correspondentes
    int contagem[ELEMENTS] = {0};
    int novoTamanho = 0;
    tempA3 tempo;

    numerosUnicosEIndices(reg, ELEMENTS, unicos, indices, &novoTamanho);

    for(int i = 0; i < novoTamanho; i++)
    {
        for (int j = 0; j < ELEMENTS; j++)
        {
            if(unicos[i] == reg[j].idade)
            {
                contagem[i]++;
            }
        }
        
    }

    for(int i = 0; i < novoTamanho; i++){
            tempo.ignore = unicos[i];
            tempo.codcli = indices[i];
            tempo.idade = contagem[i];
            fwrite(&tempo, sizeof(tempA3), 1, arquivoA5);
            //fprintf(arquivoA5, "%d %d %d\n", unicos[i], indices[i], contagem[i]);
    }


    for(int i = 0; i < ELEMENTS; i++)
    {
        reg[i].PROX = -1;
        for(int j = i + 1; j < ELEMENTS; j++)
        {
            if(reg[i].idade == reg[j].idade)
            {
                reg[i].PROX = reg[j].ED;
                break;
            }
        }
    }

    for(int j = 0; j < ELEMENTS; j++ )
    {
        fwrite(&reg[j].ED, sizeof(int), 1, arquivoA4);
        fwrite(&reg[j].codcli, sizeof(int), 1, arquivoA4);
        fwrite(&reg[j].idade, sizeof(int), 1, arquivoA4);
        fwrite(&reg[j].PROX, sizeof(int), 1, arquivoA4);
        //fprintf(arquivoA4, "%d %d %d %d\n",reg[j].ED, reg[j].codcli,reg[j].idade, reg[j].PROX);
    } 

    fclose(arquivoA4);
    arquivoA4 = fopen("A4.bin", "rb");

    int temp_int;

    printf("\n");
    printf("arquivo A4:\n");
    for(int j = 0; j < ELEMENTS; j++ )
    {
        fread(&temp_int, sizeof(int), 1, arquivoA4);
        printf("ED: %d ", temp_int);
        fread(&temp_int, sizeof(int), 1, arquivoA4);
        printf("codcli: %d ", temp_int);
        fread(&temp_int, sizeof(int), 1, arquivoA4);
        printf("idade: %d ", temp_int);
        fread(&temp_int, sizeof(int), 1, arquivoA4);
        printf("prox: %d\n", temp_int);
    }

    fclose(arquivoA5);

    arquivoA5 = fopen("A5.bin", "rb");

    printf("\n");
    printf("arquivo A5:\n");
    for(int i = 0; i < novoTamanho; i++)
    {
        fread(&tempo, sizeof(tempA3), 1, arquivoA5);
        printf("idade: %d id:%d qtnd:%d\n", tempo.ignore, tempo.codcli, tempo.idade);
    }

    fclose(arquivoA5);

    //return reg;
}

int compare(const void *a, const void *b) {
    RegistroA3 *regA = (RegistroA3 *)a;
    RegistroA3 *regB = (RegistroA3 *)b;
    return regA->codcli - regB->codcli;
}

void criarArquivoA6( FILE *arquivoA6, RegistroA3 *reg){
    qsort(reg, ELEMENTS, sizeof(RegistroA3), compare);
    for(int j = 0; j < ELEMENTS; j++ )
    {
                fwrite(&reg[j].ED, sizeof(int), 1, arquivoA6);
                fwrite(&reg[j].codcli, sizeof(int), 1, arquivoA6);
                fwrite(&reg[j].idade, sizeof(int), 1, arquivoA6);
                fwrite(&reg[j].PROX, sizeof(int), 1, arquivoA6);
                //fprintf(arquivoA6, "%d %d %d %d\n",reg[j].ED, reg[j].codcli,reg[j].idade, reg[j].PROX);
    } 

    fclose(arquivoA6);
    arquivoA6 = fopen("A6.bin", "rb");

    int temp_int;

    printf("\n");
    printf("arquivo A6:\n");
    for(int j = 0; j < ELEMENTS; j++ )
    {
        fread(&temp_int, sizeof(int), 1, arquivoA6);
        printf("ed: %d ", temp_int);
        fread(&temp_int, sizeof(int), 1, arquivoA6);
        printf("codcli: %d ", temp_int);
        fread(&temp_int, sizeof(int), 1, arquivoA6);
        printf("idade: %d ", temp_int);
        fread(&temp_int, sizeof(int), 1, arquivoA6);
        printf("prox: %d\n", temp_int);
    }

    fclose(arquivoA6);
}

void criarArquivoA7( FILE *arquivoDados_base,FILE *arquivoA7, RegistroA3 *reg){
    int i = 0;
    tempA2 tempo;
    while (!feof(arquivoDados_base))
    {
        fread(&tempo, sizeof(tempA2), 1, arquivoDados_base);
        strcpy(reg[i].nome, tempo.nome);
        //fscanf(arquivoDados_base, "%*d %s %*d", reg[i].nome);
        i++;
    }
    

    for(int j = 0; j < ELEMENTS; j++ )
    {
                fwrite(&reg[j].codcli, sizeof(int), 1, arquivoA7);
                fwrite(&reg[j].nome, sizeof(char), sizeof(reg[j].nome), arquivoA7);
                fwrite(&reg[j].idade, sizeof(int), 1, arquivoA7);
                fwrite(&reg[j].PROX, sizeof(int), 1, arquivoA7);
                //fprintf(arquivoA7, "%d %s %d %d\n", reg[j].codcli, reg[j].nome, reg[j].idade, reg[j].PROX);
    } 

    fclose(arquivoA7);

    arquivoA7 = fopen("A7.bin", "rb");

    int temp_int;

    printf("\n");
    printf("arquivo A7:\n");
    for(int j = 0; j < ELEMENTS; j++ )
    {
        fread(&temp_int, sizeof(int), 1, arquivoA7);
        printf("codcli: %d |", temp_int );

        fread(&tempo.nome, sizeof(char), sizeof(tempo.nome), arquivoA7);
        printf("nome: %s |", tempo.nome );

        fread(&temp_int, sizeof(int), 1, arquivoA7);
        printf("idade: %d |", temp_int );

        fread(&temp_int, sizeof(int), 1, arquivoA7);
        printf("prox: %d\n", temp_int );
    }

    fclose(arquivoA7);
}

int main() {
    FILE *arquivoDados = fopen("dados copy.bin", "wb");
    if (arquivoDados == NULL) { // Verificação de erro
        perror("Erro ao abrir o arquivo");
        return EXIT_FAILURE;
    }

    int codcli[8] = {2, 4, 5, 6, 7, 8, 9, 10};
    char name[8][20] = {"pedro", "joao_pedro", "joao_pedro", "sylvino", "gab", "gabriel", "joao", "sylv"};
    int idade[8] = {7, 16, 7, 6, 16, 23, 25, 23};

    for (int i = 0; i < 8; i++) {
        fwrite(&codcli[i], sizeof(int), 1, arquivoDados);              // Escreve o código
        fwrite(name[i], sizeof(char), sizeof(name[i]), arquivoDados);  // Escreve o nome
        fwrite(&idade[i], sizeof(int), 1, arquivoDados);               // Escreve a idade
    }
    /*2 pedro 7
4 joão_pedro 16
5 joão_pedro 7
6 sylvino 6
7 gab 16
8 gabriel 23
9 joao 25
10 sylv 23*/
    FILE *arquivoA2 = fopen("A2.bin", "wb");
    FILE *arquivoA3 = fopen("A3.bin", "wb");
    FILE *arquivoA4 = fopen("A4.bin", "wb");
    FILE *arquivoA5 = fopen("A5.bin", "wb");
    FILE *arquivoA6 = fopen("A6.bin", "wb");
    FILE *arquivoA7 = fopen("A7.bin", "wb");
    RegistroA3 *temp;
    criarArquivoA2(arquivoDados, arquivoA2, "A2.bin");
    temp = criarArquivoA3(arquivoDados ,arquivoA3);
    criarArquivoA4(arquivoA4, arquivoA5, temp);
    criarArquivoA6(arquivoA6, temp);
    criarArquivoA7(arquivoDados,arquivoA7, temp);
    return 0;
}
