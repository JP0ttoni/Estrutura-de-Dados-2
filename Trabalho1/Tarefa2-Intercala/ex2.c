#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PARTITIONS 10
#define MAX_NAME_LENGTH 50 
#define MAX_DATE_LENGTH 20 

typedef struct CLIENTE {
    int cod_cliente;
    char nome[MAX_NAME_LENGTH];
    char data_nascimento[MAX_DATE_LENGTH];
} Cliente;

typedef struct NO {
    int index_particao; 
    Cliente cliente; 
} No;

void troca_nos(No *a, No *b) {
    No temp = *a;
    *a = *b;
    *b = temp;
}

// cria árvore
void reconstruir_arvore(No heap[], int n) {
    int i = (n - 2) / 2;
    while (i >= 0) {
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;
        int menor = i;

        if (esq < n && heap[esq].cliente.cod_cliente < heap[menor].cliente.cod_cliente) {
            menor = esq;
        }
        if (dir < n && heap[dir].cliente.cod_cliente < heap[menor].cliente.cod_cliente) {
            menor = dir;
        }

        if (menor != i) {
            troca_nos(&heap[i], &heap[menor]);
        }
        i--;
    }
}

void intercalar_particoes(char *particoes[], int num_particoes, char *arquivo_saida) {
    FILE *files[MAX_PARTITIONS];    // array de partições (arquivos)
    No heap[MAX_PARTITIONS];        // heap para os vencedores
    int num_ativos = 0;             // arquivos com algum número dentro

    // abre todos e coloca o primeiro elemento na heap
    for (int i = 0; i < num_particoes; i++) {
        files[i] = fopen(particoes[i], "rb");
        if (files[i] == NULL) {
            printf("Erro ao abrir a partição: %s\n", particoes[i]);
            exit(EXIT_FAILURE);
        }

        Cliente cliente;
        if (fread(&cliente, sizeof(Cliente), 1, files[i]) == 1) {
            heap[num_ativos].index_particao = i;
            heap[num_ativos].cliente = cliente;
            num_ativos++;
        }
    }

    reconstruir_arvore(heap, num_ativos);

    FILE *saida = fopen(arquivo_saida, "wb");
    if (saida == NULL) {
        printf("Erro ao criar o arquivo de saída.\n");
        exit(EXIT_FAILURE);
    }

    while (num_ativos > 0) {
        // primeiro elemento é o menor
        No vencedor = heap[0];

        fwrite(&vencedor.cliente, sizeof(Cliente), 1, saida);

        // lê o próximo elemento do arquivo vencedor
        Cliente cliente;
        if (fread(&cliente, sizeof(Cliente), 1, files[vencedor.index_particao]) == 1) {
            heap[0].cliente = cliente;
        } else {
            // se vazio, substitui o elemento vencedor pelo último do heap
            heap[0] = heap[--num_ativos];
        }

        reconstruir_arvore(heap, num_ativos);
    }

    for (int i = 0; i < num_particoes; i++) {
        if (files[i] != NULL) {
            fclose(files[i]);
        }
    }
    fclose(saida);
}

void exibir_arquivo_binario(const char *arquivo) {
    FILE *file = fopen(arquivo, "rb");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo binário para leitura: %s\n", arquivo);
        exit(EXIT_FAILURE);
    }

    printf("\nConteúdo do arquivo binário '%s':\n", arquivo);
    printf("=========================================\n");
    printf("| Código | Nome                     | Data       |\n");
    printf("=========================================\n");

    Cliente cliente;
    while (fread(&cliente, sizeof(Cliente), 1, file) == 1) {
        printf("| %-6d | %-24s | %-10s |\n", cliente.cod_cliente, cliente.nome, cliente.data_nascimento);
    }

    printf("=========================================\n");

    fclose(file);
}

int main() {
    char *particoes[] = {"p1", "p2", "p3"};
    int num_particoes = 3;

    char *arquivo_saida = "resultado_final.bin";

    intercalar_particoes(particoes, num_particoes, arquivo_saida);

    exibir_arquivo_binario(arquivo_saida);

    printf("Intercalação concluída com sucesso. Arquivo gerado: %s\n", arquivo_saida);

    return 0;
}
