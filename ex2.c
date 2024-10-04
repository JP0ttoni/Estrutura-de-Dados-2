#include<stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct CLIENTE
{
    int cod_cli;
    char nome[50];
    char Data_nasc[20];
} cli;

typedef struct ex1
{
    cli valor;
    FILE *f;
    struct ex1 *left, *right;
}no;

typedef struct ex2
{
    no *raiz;
}arv;

void inserir(arv *arv, no *left, no *right, cli valor, FILE *f)
{
    no *new = (no*) malloc(sizeof(no));
    new->valor = valor;
    new->left = left;
    new->right = right;
    new->f = f;
    arv->raiz = new;
}

no *criar(char name[20])
{
    FILE *f = fopen(name, "rb");
    if(f == NULL)
    {
        printf("erro");
    }else{
        printf("arquivo aberto\n");
    }
    no *new = (no*) malloc(sizeof(no));
    cli temp_cli;
    fscanf(f, "%d %s %s\n", &temp_cli.cod_cli, &temp_cli.nome, &temp_cli.Data_nasc);
    new->valor = temp_cli;
    new->f = f;
    new->right = NULL;
    new->left = NULL;
    return new;
}

int main()
{
    no *new = criar("p1");
    no *new2 = criar("p2");
    printf("%d %d", new->valor.cod_cli, new2->valor.cod_cli);
}