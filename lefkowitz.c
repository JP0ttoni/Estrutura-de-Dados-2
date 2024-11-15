#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELEMENTS 5

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

// Passo 1: Criar o Arquivo A2 com ED, codcli, idade
void criarArquivoA2(FILE *arquivoDados, FILE *arquivoA2) {
    RegistroA2 *temp = (RegistroA2 *)malloc(20 * sizeof(RegistroA2));
    int i = 0;
    while (!feof(arquivoDados))
    {
        fscanf(arquivoDados, "%d %*s %d", &temp[i].codcli, &temp[i].idade);
        i++;
    }



    for(int j = 0; j < i; j++ )
    {
        if(j + 1 != i)
        {
            fprintf(arquivoA2, "%d %d %d\n",j + 1, temp[j].codcli, temp[j].idade);
        }else{
            fprintf(arquivoA2, "%d %d %d",j + 1, temp[j].codcli, temp[j].idade);
        }
    }
    fclose(arquivoA2);
    
    
}

int compararNomes(const void *a, const void *b) {
    const RegistroA3 *reg1 = (const RegistroA3 *)a;
    const RegistroA3 *reg2 = (const RegistroA3 *)b;
    return strcmp(reg1->nome, reg2->nome);
}


RegistroA3 *criarArquivoA3(FILE *arquivoDados_base, FILE *arquivoA3) {
    rewind(arquivoDados_base);
    FILE *arquivoDados = fopen("A2.bin", "rb");
    RegistroA3 *temp = (RegistroA3 *)malloc(20 * sizeof(RegistroA3));
    int i = 0;

    while (!feof(arquivoDados_base))
    {
        fscanf(arquivoDados_base, "%*d %s %*d", temp[i].nome);
        i++;
        temp[i - 1].ED = i;
    }
    i=0;
    while(!feof(arquivoDados))
    {
        fscanf(arquivoDados, "%*d %d %d", &temp[i].codcli, &temp[i].idade);
        i++;
    }
    qsort(temp, i, sizeof(RegistroA3), compararNomes);

    for(int j = 0; j < i; j++ )
    {
            if(j + 1 != i)
            {
                fprintf(arquivoA3, "%d %d %d\n",temp[j].ED, temp[j].codcli, temp[j].idade);
            }else{
                fprintf(arquivoA3, "%d %d %d",temp[j].ED, temp[j].codcli, temp[j].idade);
            }
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

    numerosUnicosEIndices(reg, ELEMENTS, unicos, indices, &novoTamanho);

    printf("Numeros unicos e seus indices:\n");
    for (int i = 0; i < novoTamanho; i++) {
        printf("Numero: %d, indice: %d\n", unicos[i], indices[i]);
    }

    for(int i = 0; i < novoTamanho; i++)
    {
        for (int j = 0; j < ELEMENTS; j++)
        {
            printf("contagem: %d %d\n", unicos[i], reg[j].idade);
            if(unicos[i] == reg[j].idade)
            {
                contagem[i]++;
                printf("entrou: %d\n", contagem[i]);
            }
        }
        
    }

    for(int i = 0; i < novoTamanho; i++){
        if(i + 1 != novoTamanho)
        {
            fprintf(arquivoA5, "%d %d %d\n", unicos[i], indices[i], contagem[i]);
        }else{
            fprintf(arquivoA5, "%d %d %d", unicos[i], indices[i], contagem[i]);
        }
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
        
        printf("%d ", reg[i].PROX);
    }

    for(int j = 0; j < ELEMENTS; j++ )
    {
            if(j + 1 != ELEMENTS)
            {
                fprintf(arquivoA4, "%d %d %d %d\n",reg[j].ED, reg[j].codcli,reg[j].idade, reg[j].PROX);
            }else{
                fprintf(arquivoA4, "%d %d %d %d",reg[j].ED, reg[j].codcli, reg[j].idade, reg[j].PROX);
            }
    } 

    //return reg;
}

int main() {
    FILE *arquivoDados = fopen("dados copy.bin", "rb");
    FILE *arquivoA2 = fopen("A2.bin", "wb");
    FILE *arquivoA3 = fopen("A3.bin", "wb");
    FILE *arquivoA4 = fopen("A4.bin", "wb");
    FILE *arquivoA5 = fopen("A5.bin", "wb");
    RegistroA3 *temp;
    criarArquivoA2(arquivoDados, arquivoA2);
    temp = criarArquivoA3(arquivoDados ,arquivoA3);
    criarArquivoA4(arquivoA4, arquivoA5, temp);
    return 0;
}
