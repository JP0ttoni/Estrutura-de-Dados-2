#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PARTITIONS 25
#define PARTITION_SIZE 7
#define MAX_NAME_LENGTH 50
#define MAX_DATE_LENGTH 20
#define MAX_INT 123456789

typedef struct REGISTRO {
    int num;
    int congelado;
    int zerado;
} Reg;

typedef struct CLIENTE {
    Reg cod_cli;
    char nome[MAX_NAME_LENGTH];
    char data_nascimento[MAX_DATE_LENGTH];
} Cliente;

void inicializar_particao(FILE *fl, Cliente part[]);
void imprimir_particao(int part_size, Cliente part[]);
void criar_arquivo(FILE *fl);
void escrever_particao(char nome_arquivo[], int num_part, FILE *p, int menor, char nome[], char data_nasc[]);
int verificar_congelados(int part_size, Cliente part[], int contador);

void inicializar_particao(FILE *fl, Cliente part[]) {
    for (int i = 0; i < PARTITION_SIZE; i++) {
        fscanf(fl, "%d %s %s\n", &part[i].cod_cli.num, part[i].nome, part[i].data_nascimento);
        part[i].cod_cli.congelado = 0;
        part[i].cod_cli.zerado = 0;
        printf("%d ", part[i].cod_cli.num);
    }
    printf("\n");
}

void imprimir_particao(int part_size, Cliente part[]) {
    for (int i = 0; i < part_size; i++) {
        printf("%d ", part[i].cod_cli.num);
    }
    printf("\n");
}

void escrever_particao(char nome_arquivo[], int num_part, FILE *p, int menor, char nome[], char data_nasc[]) {
    sprintf(nome_arquivo, "p%d", num_part);
    p = fopen(nome_arquivo, "ab");
    fprintf(p, "%d %s %s\n", menor, nome, data_nasc);
    fclose(p);
}

int verificar_congelados(int part_size, Cliente part[], int contador) {
    for (int i = 0; i < part_size; i++) {
        if (part[i].cod_cli.congelado == 1) {
            contador++;
        }
    }
    return contador;
}

void criar_arquivo(FILE *fl) {
    Cliente clientes[MAX_PARTITIONS];
    char data[MAX_DATE_LENGTH], nomes[3][MAX_NAME_LENGTH] = {"joao_pedro", "gabriel", "sylvino"};
    int count = 1, nome_index = 0;

    for (int i = 0; i < 24; i++, count++, nome_index++) {
        if (nome_index >= 3) {
            nome_index = 0;
        }
        strcpy(clientes[i].nome, nomes[nome_index]);
        sprintf(data, "%d/07/2004", count);
        strcpy(clientes[i].data_nascimento, data);
    }

    int codigos[] = {30, 14, 15, 75, 32, 6, 5, 81, 48, 41, 87, 18, 56, 20, 26, 4, 21, 65, 22, 49, 11, 16, 8, 12};
    for (int i = 0; i < 24; i++) {
        clientes[i].cod_cli.num = codigos[i];
        fprintf(fl, "%d %s %s\n", clientes[i].cod_cli.num, clientes[i].nome, clientes[i].data_nascimento);
    }
}

int main() {
    Cliente part[PARTITION_SIZE];
    int num_part = 1, pos_vazia, contador = 0, menor_part = MAX_INT;
    char nome_arquivo[4];
    FILE *fl, *p;

    // Criação do arquivo de entrada
    fl = fopen("dados.bin", "wb");
    criar_arquivo(fl);
    fclose(fl);

    fl = fopen("dados.bin", "rb");
    inicializar_particao(fl, part);

    // Processo de intercalação e criação das partições ordenadas
    while (1) {
        int menor = MAX_INT;

        // Encontrar o menor elemento da partição que não está congelado ou zerado
        for (int i = 0; i < PARTITION_SIZE; i++) {
            if (part[i].cod_cli.num < menor && part[i].cod_cli.congelado == 0 && part[i].cod_cli.zerado == 0) {
                menor = part[i].cod_cli.num;
                pos_vazia = i;
            }
        }

        // Início de um novo arquivo de partição
        if (menor_part == MAX_INT) {
            menor_part = menor;
            sprintf(nome_arquivo, "p%d", num_part);
            p = fopen(nome_arquivo, "wb");
        }

        // Verificação de congelamento
        if (menor < menor_part) {
            printf("\ncongelei %d em %d\n", menor, menor_part);
            part[pos_vazia].cod_cli.congelado = 1;
        } else {
            menor_part = menor;
            escrever_particao(nome_arquivo, num_part, p, menor, part[pos_vazia].nome, part[pos_vazia].data_nascimento);

            // Verifica se chegou ao final do arquivo
            if (feof(fl)) {
                part[pos_vazia].cod_cli.zerado = 1;
                part[pos_vazia].cod_cli.congelado = 1;
            } else {
                fscanf(fl, "%d %s %s\n", &part[pos_vazia].cod_cli.num, part[pos_vazia].nome, part[pos_vazia].data_nascimento);
                if (feof(fl)) {
                    part[pos_vazia].cod_cli.zerado = 1;
                    part[pos_vazia].cod_cli.congelado = 1;
                }
            }
        }

        // Verificação de final do arquivo
        if (feof(fl)) {
            contador = 0;
            for (int i = 0; i < PARTITION_SIZE; i++) {
                if (part[i].cod_cli.zerado == 1) {
                    contador++;
                }
            }
            if (contador == PARTITION_SIZE) {
                printf("Processo de criação de partições concluído.\n");
                break;
            }
        }

        // Reinicialização das partições congeladas
        if (verificar_congelados(PARTITION_SIZE, part, 0) == PARTITION_SIZE) {
            for (int i = 0; i < PARTITION_SIZE; i++) {
                part[i].cod_cli.congelado = 0;
            }
            num_part++;
            sprintf(nome_arquivo, "p%d", num_part);
            p = fopen(nome_arquivo, "wb");
            fclose(p);
            menor_part = MAX_INT;
        }

        imprimir_particao(PARTITION_SIZE, part);
    }

    fclose(fl);
    return 0;
}
