#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void create_file(FILE *fl);

typedef struct REGISTRO
{
    int num;
    int congelado;
    int zerado;
} Reg;

typedef struct CLIENTE
{
    Reg cod_cli;
    char nome[50];
    char Data_nasc[20];
} cli;

typedef struct TEMPORARIO
{
    int num;
    char nome[50];
    char Data_nasc[20];
} temp;

void imprime(int part_size, cli part[])
{
    for (int i = 0; i < part_size; i++)
    {
        printf("%d ", part[i].cod_cli.num);
    }
    printf("\n");
}
void inicializando(FILE *fl, cli part[])
{
    temp temporario;
    printf("\n");
    printf("vetor intermediario: \n");
    printf("\n");
    for (int i = 0; i < 7; i++)
    {
        fread(&temporario, sizeof(temp), 1, fl);
        part[i].cod_cli.num = temporario.num;
        strcpy(part[i].nome, temporario.nome);
        strcpy(part[i].Data_nasc, temporario.Data_nasc);
        //fscanf(fl, "%d %s %s\n", &part[i].cod_cli.num, part[i].nome, part[i].Data_nasc);
        part[i].cod_cli.congelado = 0;
        part[i].cod_cli.zerado = 0;

        printf("%d %s %s\n", part[i].cod_cli.num, part[i].nome, part[i].Data_nasc);
    }
    printf("\n");
}
void write(char name_file[], int num_part, FILE *p, int menor, char name[], char data_nasc[])
{
    temp tempo;
    temp read;
    tempo.num = menor;
    strcpy(tempo.nome, name);
    strcpy(tempo.Data_nasc, data_nasc);
    sprintf(name_file, "p%d", num_part);
    p = fopen(name_file, "ab");
    fwrite(&tempo, sizeof(temp), 1, p);
    //fprintf(p, "%d %s %s\n", menor, name, data_nasc);
    fclose(p);
    /*p = fopen(name_file, "rb");

    fread(&read, sizeof(temp), 1, p);

    printf("read - numero: %d nome: %s data: %s\n", read.num, read.nome, read.Data_nasc);
    fclose(p);*/
}

int frozen(int part_size, cli part[], int count)
{
    for (int i = 0; i < part_size; i++)
    {
        if (part[i].cod_cli.congelado == 1)
        {
            count++;
        }
    }
    return count;
}
void create_file(FILE *fl) {
    temp tempor[24];  // Variável para armazenar os dados (não precisa de malloc)
    temp read[24];    // Variável para armazenar os dados lidos do arquivo
    int vec[24] = {30, 14, 15, 75, 32, 6, 5, 81, 48, 41, 87, 18, 56, 20, 26, 4, 21, 65, 22, 49, 11, 16, 8, 12};
    char date[20], name[3][20] = {"joao_pedro", "gabriel", "sylvino"};
    int count = 1, count_name = 0;

    // Gravar os números no arquivo binário
    for (int i = 0; i < 24; i++, count_name++) {
        if (count_name > 2) {
            count_name = 0;  // Resetar para 0, para ciclar entre os nomes
        }

        // Preencher os dados fictícios para cada pessoa
        tempor[i].num = vec[i];
        strcpy(tempor[i].nome, name[count_name]);
        sprintf(date, "%d/07/2004", count++);
        strcpy(tempor[i].Data_nasc, date);
        // Escrever a estrutura 'tempor[i]' no arquivo
        fwrite(&tempor[i], sizeof(temp), 1, fl);
    }

    // Reposicionar o ponteiro para o início do arquivo para leitura
    fclose(fl);
    fl = fopen("dados.bin", "rb");

    printf("arquivo:\n");
    fread(read, sizeof(temp), 24, fl);
    // Ler os dados do arquivo e imprimir
    for (int i = 0; i < 24; i++) {
        printf("Numero: %d, Nome: %s, Data de Nascimento: %s\n", read[i].num, read[i].nome, read[i].Data_nasc);
    }

    // Fechar o arquivo após a operação
    fclose(fl);
}

void imprimir(FILE *fl, char name_file[])
{
    fclose(fl);
    fl = fopen(name_file, "rb");
    if (fl == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    temp tempo;
    printf("Arquivo:\n");

    // Lê o arquivo até o final
    while (fread(&tempo, sizeof(temp), 1, fl) == 1) {
        // Imprime somente se a leitura for bem-sucedida
        printf("Numero: %d, Nome: %s, Data de Nascimento: %s\n", tempo.num, tempo.nome, tempo.Data_nasc);
    }

    printf("\n");

    fclose(fl);  // Fechar o arquivo após a leitura
}

int main()
{
    // create file
    cli part[7];
    int num_part = 1, pos_empty, count = 0, part_size = 7;
    char name_file[4];
    FILE *fl, *p;
    // limpando o arquivo
    fl = fopen("dados.bin", "wb");
    create_file(fl);
    fl = fopen("dados.bin", "rb");
    inicializando(fl, part);
    int menor_part = 123456789;
    while (1)
    {
        
        // resetando o valor inical de menor
        int menor = 123456789;
        for (int i = 0; i < part_size; i++)
        {
            if (part[i].cod_cli.num < menor && part[i].cod_cli.congelado == 0 && part[i].cod_cli.zerado == 0)
            {
                menor = part[i].cod_cli.num;
                pos_empty = i;
            }
        }
        // inicio de um novo arquivo
        if (menor_part == 123456789)
        {
            menor_part = menor;
            sprintf(name_file, "p%d", num_part);
            p = fopen(name_file, "wb");
        }
        // checando se vai congelar ou n
        if (menor < menor_part)
        {
            printf("\n");
            printf("congelei %d em %d\n", menor, menor_part);
            printf("\n");
            part[pos_empty].cod_cli.congelado = 1;
        }
        else
        {
            menor_part = menor;
            write(name_file, num_part, p, menor, part[pos_empty].nome, part[pos_empty].Data_nasc);
            if (feof(fl))
            {
                part[pos_empty].cod_cli.zerado = 1;
                part[pos_empty].cod_cli.congelado = 1;
            }
            else
            {
                temp tempo;
                fread(&tempo, sizeof(temp), 1, fl);
                part[pos_empty].cod_cli.num = tempo.num;
                strcpy(part[pos_empty].nome, tempo.nome);
                strcpy(part[pos_empty].Data_nasc, tempo.Data_nasc);
                //fscanf(fl, "%d %s %s\n", &part[pos_empty].cod_cli.num, part[pos_empty].nome, part[pos_empty].Data_nasc);
                // para zerar quando chegar no final do aqruivoi
                if (feof(fl))
                {
                    part[pos_empty].cod_cli.zerado = 1;
                    part[pos_empty].cod_cli.congelado = 1;
                }
            }
        }
        if (feof(fl))
        {
            count = 0;
            // verifica se ta zerado
            for (int i = 0; i < part_size; i++)
            {
                if (part[i].cod_cli.zerado == 1)
                {
                    count++;
                }
            }
            if (count == part_size)
            {
                printf("acabou\n");
                printf("\n");
                printf("terminou arquivo %d\n", num_part);
                printf("\n");
                imprimir(p, name_file);
                break;
            }
        }
        // reseta
        if (frozen(part_size, part, 0) == part_size)
        {
            for (int i = 0, count = 0; i < part_size; i++)
            {
                part[i].cod_cli.congelado = 0;
            }
            printf("\n");
            printf("terminou arquivo %d\n", num_part);
            printf("\n");
            imprimir(p, name_file);
            num_part++;
            sprintf(name_file, "p%d", num_part);
            p = fopen(name_file, "wb");
            fclose(p);
            menor_part = 123456789;
        }
        imprime(part_size, part);
    }

    printf("\n");

    /*printf("imprimindo todos os arquivos\n");
    imprimir(p, "p2");
    imprimir(p, "p3");*/

}