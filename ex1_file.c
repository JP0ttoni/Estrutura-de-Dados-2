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
    char Data_nasc[11];
} cli;

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
    for (int i = 0; i < 7; i++)
    {
        fscanf(fl, "%d %s %s\n", &part[i].cod_cli.num, part[i].nome, part[i].Data_nasc);
        part[i].cod_cli.congelado = 0;
        part[i].cod_cli.zerado = 0;
        printf("%d ", part[i].cod_cli.num);
    }
    printf("\n");
}
void write(char name_file[], int num_part, FILE *p, int menor, char name[], char data_nasc[])
{
    sprintf(name_file, "p%d", num_part);
    p = fopen(name_file, "ab");
    fprintf(p, "%d %s %s\n", menor, name, data_nasc);
    fclose(p);
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
void create_file(FILE *fl)
{
        cli vec[25];
        char date[20], name[3][20] = {"joão_pedro","gabriel", "sylvino"};
        int count = 1, count_name = 0;

        for (int i = 0; i < 24; i++, count++, count_name++)
        {
            if(count_name >= 3)
            {
                count_name = 0;
            }
            strcpy(vec[i].nome, name[count_name]);
            sprintf(date, "%d/07/2004", count);
            strcpy(vec[i].Data_nasc, date);
        }

        vec[0].cod_cli.num = 30;
        vec[1].cod_cli.num = 14;
        vec[2].cod_cli.num = 15;
        vec[3].cod_cli.num = 75;
        vec[4].cod_cli.num = 32;
        vec[5].cod_cli.num = 6;
        vec[6].cod_cli.num = 5;
        vec[7].cod_cli.num = 81;
        vec[8].cod_cli.num = 48;
        vec[9].cod_cli.num = 41;
        vec[10].cod_cli.num = 87;
        vec[11].cod_cli.num = 18;
        vec[12].cod_cli.num = 56;
        vec[13].cod_cli.num = 20;
        vec[14].cod_cli.num = 26;
        vec[15].cod_cli.num = 4;
        vec[16].cod_cli.num = 21;
        vec[17].cod_cli.num = 65;
        vec[18].cod_cli.num = 22;
        vec[19].cod_cli.num = 49;
        vec[20].cod_cli.num = 11;
        vec[21].cod_cli.num = 16;
        vec[22].cod_cli.num = 8;
        vec[23].cod_cli.num = 12;
        int vect[25] = {30, 14, 15, 75, 32, 6, 5, 81, 48, 41, 87, 18, 56, 20, 26, 4, 21, 65, 22, 49, 11, 16, 8, 12};
        for (int i = 0; vect[i] != '\0'; i++)
        {
            fprintf(fl, "%d %s %s\n", vec[i].cod_cli.num, vec[i].nome, vec[i].Data_nasc);
        }
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
    fclose(fl);
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
            printf("\ncongelei %d em %d\n", menor, menor_part);
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
                fscanf(fl, "%d %s %s\n", &part[pos_empty].cod_cli.num, part[pos_empty].nome, part[pos_empty].Data_nasc);
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
                printf("acabou");
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
            num_part++;
            sprintf(name_file, "p%d", num_part);
            p = fopen(name_file, "wb");
            fclose(p);
            menor_part = 123456789;
        }
        imprime(part_size, part);
    }

}