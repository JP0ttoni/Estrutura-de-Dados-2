#include<stdio.h>
#include<stdlib.h>

int main()
{
    int i;
    char string[30];
    FILE *f = fopen("dados.bin", "rb");
    while (!feof(f))
    {
        fscanf(f, "%d %s\n", &i, string);
        printf("%d %s\n", i, string);
    }
    
}