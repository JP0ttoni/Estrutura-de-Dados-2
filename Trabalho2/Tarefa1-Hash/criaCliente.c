#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define bool int
#define false 0
#define true 1


typedef struct
{
    int cod;
    char nome[100];
    int prox;
    bool status;
} Cliente;

static size_t escreveCliente(Cliente *cliente, FILE *arquivo)
{
    // Escreve o cliente no arquivo e retorna a quantidade de registros escritos
    return fwrite(cliente, sizeof(Cliente), 1, arquivo);
}

static Cliente *leCliente(FILE *arquivo)
{
    // Cria um espaço para armazenar um cliente lido do arquivo
    Cliente *cliente = malloc(sizeof(Cliente));

    if (cliente == NULL)
    {
        fprintf(stderr, "Erro ao alocar memória para Cliente\n");
        return NULL;
    }

    if (fread(cliente, sizeof(Cliente), 1, arquivo) != 1)
    {
        if (feof(arquivo))
        {
            return NULL; // Chegou ao final do arquivo
        }
        perror("Erro ao ler Cliente do arquivo");
        // Libera memória se a leitura falhar
        free(cliente);
        exit(EXIT_FAILURE); // Sai do programa com código de erro
    }

    return cliente;
}

const char *nomes[] = {
    "Allan",
    "Mariana",
    "Raquel",
    "Elienar",
    "João Pedro",
    "Sylvino",
    "Ubiratam",
    "Marcel",
    "Filipe Braida",
    "Juliana",
    "Marcelo",
    "Lucas",
    "Rafael",
    "Gustavo",
    "Luis",
    "Pedro",
    "Paulo",
    "Maria",
    "João",
    "José",
    "Carlos",
    "Antônio",
    "Francisco",
    "Ana",
    "Luiza",
    "Luiz",
    "Fernando",
    "Clara",
    "Thiago",
    "Gabriel",
    "Camila",
    "Larissa",
    "Beatriz",
    "Vinicius",
    "Rogério",
    "Natália",
    "Sofia",
    "Eduardo",
    "Henrique",
    "Patrícia",
    "Renata",
    "Mateus",
    "André",
    "Vera",
    "Silvia",
    "Leandro",
    "Bruna",
    "Marcos",
    "Karen",
    "Débora",
    "Rosa",
    "Joaquim",
    "Felipe",
    "Alexandre",
};

int main(int argc, char const *argv[])
{
    const int clientesContador = argc > 1 ? atoi(argv[1]) : 15;
    const char *arquivoNome = argc > 2 ? argv[2] : "listaclientes.txt";

    if (clientesContador <= 0)
    {
        printf("Tamanho de partição inválida\n");
        return 1;
    }

    printf("\n Configuração do Programa:\n");
    printf(" > Clientes Contador: %d\n", clientesContador);
    printf(" > Nome do Arquivo: \"%s\"\n", arquivoNome);

    FILE *arquivo = fopen(arquivoNome, "wb+");

    if (arquivo == NULL)
    {
        printf("Error abrindo o arquivo \"%s\"\n", arquivoNome);
        return 1;
    }

    printf("Arquivo \"%s\" aberto, iniciando criação dos %d clientes...\n", arquivoNome, clientesContador);

    srand(time(NULL));
    Cliente *cliente = (Cliente *)malloc(sizeof(Cliente));

    for (int i = 0; i < clientesContador; i++)
    {
        cliente->cod = rand() % 10000;
        cliente->prox = -1;
        cliente->status = true;
        strcpy(cliente->nome, nomes[rand() % (sizeof(nomes) / sizeof(nomes[0]))]);
        escreveCliente(cliente, arquivo);
    }

    printf("%d clientes criados com sucesso, listando itens de todos clientes...\n\n", clientesContador);
    rewind(arquivo);

    while ((cliente = leCliente(arquivo)) != NULL)
    {
        printf("Cliente criado %d %s\n", cliente->cod, cliente->nome);
    }

    printf("\nTodos %d clientes listados\n", clientesContador);

    fclose(arquivo);
    printf("Arquivo \"%s\" fechado\n", arquivoNome);
    return 0;
}
