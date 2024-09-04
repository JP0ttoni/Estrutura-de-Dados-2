#include <stdio.h>
#include<stdlib.h>

void create_file(FILE *fl);

typedef struct REGISTRO
{
    int num;
    int congelado;
    int zerado;
} Reg;

int main()
{
    //create file
    Reg part[7];
    int num_part = 1, pos_empty, count = 0, part_size = 7;
    char name_file[4];
    FILE *fl, *p;
    //verifica se existe o arquivo ou n
    fl = fopen("dados.bin", "wb");
    create_file(fl);
    fclose(fl);
    fl = fopen("dados.bin", "rb");

    for(int i = 0; i<7; i++)
    {
        fscanf(fl, "%d", &part[i].num);
        part[i].congelado = 0;
        part[i].zerado = 0;
        printf("%d ", part[i].num);
    }

    int menor_part = 123456789;
    while(1)
    {
        if(num_part > 3)
        {
            break;
        }
        int menor = 123456789;
        //printf("o menor é: %d\n", menor);
        //acha o menor do vetor
        for(int i = 0; i<part_size;i++)
        {
            if(part[i].num < menor && part[i].congelado == 0 && part[i].zerado == 0)
            {
                menor = part[i].num;
                pos_empty = i;
            }
        }

        if(menor_part == 123456789)
        {
            menor_part = menor;
            sprintf(name_file, "p%d", num_part);
            p = fopen(name_file, "wb");
        }

        if(menor < menor_part)
        {
            printf("\ncongelei %d %d\n", menor, menor_part);
            part[pos_empty].congelado = 1;
        } else{
                menor_part = menor;
                sprintf(name_file, "p%d", num_part);
                p = fopen(name_file,"ab");
                fprintf(p, "%d ", menor);
                fclose(p);
                if(feof(fl))
                {
                    part[pos_empty].zerado = 1;
                    part[pos_empty].congelado = 1;
                } else{
                    fscanf(fl, "%d", &part[pos_empty].num);
                    //para zerar quando chegar no final do aqruivo
                    if(feof(fl))
                    {
                        part[pos_empty].zerado = 1;
                        part[pos_empty].congelado = 1;
                    }
                }
                
        }

        if(feof(fl))
        {
            count = 0;
            //verifica se ta zerado
            for(int i = 0; i<part_size; i++)
            {
                if(part[i].zerado == 1)
                {
                    //printf("congelado[%d]\n", part[i].num);
                    count++;
                }
            }

            if(count == part_size)
            {
                printf("acabou");
                break;
            }
            
        }

        count = 0;
        //verifica se ta congelado
        for(int i = 0; i<part_size; i++)
        {
            if(part[i].congelado == 1)
            {
                printf("congelado[%d]\n", part[i].num);
                count++;
            }
        }

        //reseta
        if(count == part_size)
        {
            printf("entrou\n");
            for(int i = 0, count = 0; i<part_size; i++)
            {
                part[i].congelado = 0;
            }
            num_part++;
            sprintf(name_file, "p%d", num_part);
            p = fopen(name_file,"wb");
            fclose(p);
            menor_part = 123456789;
        }

        //printf("\n%d\n", pos_empty);
        for (int i = 0; i < part_size; i++)
        {
            printf("%d ", part[i].num);
        }
        printf("\n");
    }

    

}

void create_file(FILE *fl)
{
    int vect[25] = {30,14,15,75,32,6,5,81,48,41,87,18,56,20,26,4,21,65,22,49,11,16,8,12};
    for(int i = 0; vect[i] != '\0'; i++)
    {
        fprintf(fl, "%d ", vect[i]);
    }
}
