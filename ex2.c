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

arv *inserir(arv *arvore, no *left, no *right, cli valor, FILE *f)
{
    arv *new = (arv*) malloc(sizeof(no));
    new->raiz->valor = valor;
    new->raiz->left = left;
    new->raiz->right = right;
    new->raiz->f = f;
    return new;
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
    cli new_cli;
    no *new = criar("p1");
    no *new2 = criar("p2");
    FILE *f = fopen("p1", "rb");
    arv *new_arv = inserir(new_arv, new, new2, new_cli, f);
    printf("%d %d", new_arv->raiz->left->valor.cod_cli, new_arv->raiz->right->valor.cod_cli);

    fclose(new->f);
    fclose(new2->f);
    fclose(f);

    free(new);
    free(new2);
    free(new_arv->raiz);
    free(new_arv);
    
    return 0;
}