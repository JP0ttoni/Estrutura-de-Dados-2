
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>

#define bool int
#define false 0
#define true 1

#define OFFSET_PROX (sizeof(int) + 10) // cod (4 bytes) + nome (10 bytes)


// Struct Cliente
typedef struct
{
    int cod;
    char nome[10];
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
        // Libera memória se a leitura falhar
        free(cliente);
        if (feof(arquivo))
        {
            return NULL; // Chegou ao final do arquivo
        }
        perror("Erro ao ler Cliente do arquivo");
        exit(EXIT_FAILURE); // Sai do programa com código de erro
    }

    return cliente;
}

int verificar_fatorCarga(FILE *tabClientes, int tam_tabela, float limite_fator_carga)
{
    Cliente cliente;
    int ativos = 0;

    // Reposiciona no início do arquivo
    rewind(tabClientes);

    // Conta quantos clientes estão ativos
    while (fread(&cliente, sizeof(Cliente), 1, tabClientes) == 1)
    {
        if (cliente.status)
        {
            ativos++;
        }
    }

    float fator_carga = (float)ativos / tam_tabela;
    return (fator_carga >= limite_fator_carga);
}

void exibir_tabHash(const char *caminho_tabHash, int tam_tabela)
{
    FILE *tabHash = fopen(caminho_tabHash, "rb");

    if (tabHash == NULL)
    {
        perror("Falha ao abrir tabHash");
        exit(EXIT_FAILURE);
    }

    int valor_atual;
    const int marcador_vazio = -1;

    printf("\nTabela Hash (Estado Atual):\n");
    printf("=======================================\n");

    for (int i = 0; i < tam_tabela; i++)
    {
        // Calcula o deslocamento e lê o valor correspondente
        fseek(tabHash, i * sizeof(int), SEEK_SET);
        fread(&valor_atual, sizeof(int), 1, tabHash);

        if (valor_atual == marcador_vazio)
        {
            printf(" [%02d] -> (vazio)\n", i);
        }
        else
        {
            printf(" [%02d] -> Offset: %d\n", i, valor_atual);
        }
    }

    printf("=======================================\n\n");
    fclose(tabHash);
}

void exibir_tabela(const char *caminho_tabClientes, const char *caminho_tabHash, int tam_tabela)
{
    FILE *tabClientes = fopen(caminho_tabClientes, "rb");
    FILE *tabHash = fopen(caminho_tabHash, "rb");

    if (tabClientes == NULL || tabHash == NULL)
    {
        fprintf(stderr, "Erro ao abrir um dos arquivos: tabClientes ou tabHash.\n");
        exit(EXIT_FAILURE);
    }

    const int marcador_vazio = -1;
    printf("\nEstado Atual da Tabela Hash com Encadeamento:\n");
    printf("=============================================\n");

    for (int i = 0; i < tam_tabela; i++)
    {
        int ponteiro;
        fseek(tabHash, i * sizeof(int), SEEK_SET);
        fread(&ponteiro, sizeof(int), 1, tabHash);

        printf("Índice [%02d]:\n", i);

        if (ponteiro == marcador_vazio)
        {
            printf("  Cliente: (vazio)\n");
            printf("  Código:  (vazio)\n");
        }
        else
        {
            Cliente cliente;
            fseek(tabClientes, ponteiro, SEEK_SET);
            fread(&cliente, sizeof(Cliente), 1, tabClientes);

            printf("  Cliente: %s\n", cliente.nome);
            printf("  Código:  %d\n", cliente.cod);
        }
        printf("---------------------------------------------\n");
    }

    fclose(tabHash);
    fclose(tabClientes);
}

void exibir_tabClientes_sequencial(const char *caminho_tabClientes)
{
    FILE *tabClientes = fopen(caminho_tabClientes, "rb");
    if (tabClientes == NULL)
    {
        fprintf(stderr, "Erro ao abrir tabClientes.\n");
        exit(EXIT_FAILURE);
    }

    printf("\nTabela de Clientes (Sequencial):\n");
    printf("=================================\n");

    Cliente cliente;
    int indice = 0;

    while (fread(&cliente, sizeof(Cliente), 1, tabClientes))
    {
        printf("Cliente #%02d:\n", indice++);
        printf("  Nome: %s\n", cliente.nome);
        printf("  Código: %d\n", cliente.cod);
        printf("  Próximo Offset: %d\n", cliente.prox);
        printf("---------------------------------\n");
    }

    fclose(tabClientes);
}

void criar_tabela_hash(int tamanho_tabela)
{
    FILE *tabela = fopen("tabHash", "wb");
    if (tabela == NULL)
    {
        fprintf(stderr, "Erro ao criar tabHash.\n");
        exit(EXIT_FAILURE);
    }

    const int valor_inicial = -1; // Indica entrada vazia
    for (int i = 0; i < tamanho_tabela; i++)
    {
        fwrite(&valor_inicial, sizeof(int), 1, tabela);
    }

    printf("Tabela Hash criada com %d entradas.\n", tamanho_tabela);
    fclose(tabela);
}

int funcao_hash(int codigo, int tamanho_tabela, int L)
{
    int hash = codigo % (int)((tamanho_tabela)*pow(2, L));
    printf("Hash calculado: %d (Código: %d, Tamanho: %d, L: %d)\n", hash, codigo, tamanho_tabela, L);
    return hash;
}

int mapear_endereco(int codigo, int tam_tabela, int P, int L)
{
    int endereco = funcao_hash(codigo, tam_tabela, L);

    // Determina se o endereço precisa ser recalculado com o próximo nível
    if (endereco >= P)
    {
        endereco = funcao_hash(codigo, tam_tabela, L + 1);
    }

    return endereco;
}

void expandir_tabela(const char *arquivoClientes, const char *arquivoHash, int *tam_tabela, int *P, int *L)
{
    const int vazio = -1;

    // Abrindo os arquivos necessários
    FILE *tabHash = fopen(arquivoHash, "r+b");
    if (!tabHash)
    {
        fprintf(stderr, "Erro ao abrir tabHash.\n");
        exit(EXIT_FAILURE);
    }

    FILE *tabClientes = fopen(arquivoClientes, "r+b");
    if (!tabClientes)
    {
        fprintf(stderr, "Erro ao abrir tabClientes.\n");
        fclose(tabHash);
        exit(EXIT_FAILURE);
    }

    // Adicionando uma nova entrada vazia no final de tabHash
    fseek(tabHash, (*tam_tabela) * sizeof(int), SEEK_SET);
    fwrite(&vazio, sizeof(int), 1, tabHash);

    // Calcula o novo tamanho da tabela
    int novo_tamanho = (*tam_tabela) + *P;

    // Processa os elementos do índice P na tabela
    fseek(tabHash, (*P) * sizeof(int), SEEK_SET);
    int offset_atual;
    fread(&offset_atual, sizeof(int), 1, tabHash);

    int ultimo_offset_em_P = -1;
    int cabeca_novo_indice = -1;

    while (offset_atual != vazio)
    {
        Cliente cliente;
        fseek(tabClientes, offset_atual, SEEK_SET);
        fread(&cliente, sizeof(Cliente), 1, tabClientes);

        // Mapeia o endereço atualizado do cliente
        int novo_indice = mapear_endereco(cliente.cod, novo_tamanho, *P, *L);

        if (novo_indice == *P)
        {
            // Atualiza a lista ligada em P
            if (ultimo_offset_em_P == vazio)
            {
                fseek(tabHash, (*P) * sizeof(int), SEEK_SET);
                fwrite(&offset_atual, sizeof(int), 1, tabHash);
            }
            else
            {
                Cliente cliente_anterior;
                fseek(tabClientes, ultimo_offset_em_P, SEEK_SET);
                fread(&cliente_anterior, sizeof(Cliente), 1, tabClientes);

                cliente_anterior.prox = offset_atual;
                fseek(tabClientes, ultimo_offset_em_P, SEEK_SET);
                fwrite(&cliente_anterior, sizeof(Cliente), 1, tabClientes);
            }
            ultimo_offset_em_P = offset_atual;
        }
        else
        {
            // Atualiza a lista ligada no novo índice
            if (cabeca_novo_indice == vazio)
            {
                fseek(tabHash, (*tam_tabela) * sizeof(int), SEEK_SET);
                fwrite(&offset_atual, sizeof(int), 1, tabHash);
                cabeca_novo_indice = offset_atual;
            }
            else
            {
                Cliente ultimo_cliente;
                fseek(tabClientes, cabeca_novo_indice, SEEK_SET);
                fread(&ultimo_cliente, sizeof(Cliente), 1, tabClientes);

                ultimo_cliente.prox = offset_atual;
                fseek(tabClientes, cabeca_novo_indice, SEEK_SET);
                fwrite(&ultimo_cliente, sizeof(Cliente), 1, tabClientes);
                cabeca_novo_indice = offset_atual;
            }
        }

        // Avança para o próximo offset
        int proximo_offset = cliente.prox;
        cliente.prox = vazio;
        fseek(tabClientes, offset_atual, SEEK_SET);
        fwrite(&cliente, sizeof(Cliente), 1, tabClientes);
        offset_atual = proximo_offset;
    }

    // Finaliza a lista ligada em P
    if (ultimo_offset_em_P != vazio)
    {
        Cliente ultimo_cliente_em_P;
        fseek(tabClientes, ultimo_offset_em_P, SEEK_SET);
        fread(&ultimo_cliente_em_P, sizeof(Cliente), 1, tabClientes);

        ultimo_cliente_em_P.prox = vazio;
        fseek(tabClientes, ultimo_offset_em_P, SEEK_SET);
        fwrite(&ultimo_cliente_em_P, sizeof(Cliente), 1, tabClientes);
    }

    // Atualiza os parâmetros P, L e tamanho da tabela
    *P = (*P + 1) % (*tam_tabela);
    if (*P == 0)
    {
        *tam_tabela *= 2;
        (*L)++;
    }

    // Fecha os arquivos
    fclose(tabClientes);
    fclose(tabHash);
}

void inserir_cliente(Cliente novo_cliente, char *caminho_tabClientes, char *caminho_tabHash, int *tamanho_tabela, int *P, float fator_carga_limite, int *L)
{
    int indice_calculado = funcao_hash(novo_cliente.cod, *tamanho_tabela, *L);
    int endereco;

    FILE *arquivo_hash = fopen(caminho_tabHash, "r+b");
    if (arquivo_hash == NULL)
    {
        perror("Erro ao abrir tabela hash");
        exit(1);
    }

    int posicao_atual;
    fseek(arquivo_hash, indice_calculado * sizeof(int), SEEK_SET);
    fread(&posicao_atual, sizeof(int), 1, arquivo_hash);

    FILE *arquivo_clientes = fopen(caminho_tabClientes, "r+b");
    if (arquivo_clientes == NULL)
    {
        perror("Erro ao abrir tabela de clientes");
        fclose(arquivo_hash);
        exit(1);
    }

    int espaco_reutilizavel = 0;
    int posicao_reutilizavel;
    Cliente cliente_lido;
    rewind(arquivo_clientes);

    while (fread(&cliente_lido, sizeof(Cliente), 1, arquivo_clientes) == 1)
    {
        if (!cliente_lido.status)
        {
            posicao_reutilizavel = ftell(arquivo_clientes) - sizeof(Cliente);
            espaco_reutilizavel = 1;
            break;
        }
    }

    if (espaco_reutilizavel)
    {
        novo_cliente.prox = -1;
        novo_cliente.status = true;
        fseek(arquivo_clientes, posicao_reutilizavel, SEEK_SET);
        fwrite(&novo_cliente, sizeof(Cliente), 1, arquivo_clientes);

        if (posicao_atual == -1)
        {
            fseek(arquivo_hash, indice_calculado * sizeof(int), SEEK_SET);
            fwrite(&posicao_reutilizavel, sizeof(int), 1, arquivo_hash);
        }
        else
        {
            int ultimo_cliente = posicao_atual;
            while (ultimo_cliente != -1)
            {
                fseek(arquivo_clientes, ultimo_cliente, SEEK_SET);
                fread(&cliente_lido, sizeof(Cliente), 1, arquivo_clientes);

                if (cliente_lido.prox == -1)
                {
                    cliente_lido.prox = posicao_reutilizavel;
                    fseek(arquivo_clientes, ultimo_cliente, SEEK_SET);
                    fwrite(&cliente_lido, sizeof(Cliente), 1, arquivo_clientes);
                    break;
                }

                ultimo_cliente = cliente_lido.prox;
            }
        }
    }
    else
    {
        fseek(arquivo_clientes, 0, SEEK_END);
        endereco = ftell(arquivo_clientes);
        novo_cliente.prox = -1;
        novo_cliente.status = true;
        fwrite(&novo_cliente, sizeof(Cliente), 1, arquivo_clientes);

        if (posicao_atual == -1)
        {
            fseek(arquivo_hash, indice_calculado * sizeof(int), SEEK_SET);
            fwrite(&endereco, sizeof(int), 1, arquivo_hash);
        }
        else
        {
            int ultimo_cliente = posicao_atual;
            while (ultimo_cliente != -1)
            {
                fseek(arquivo_clientes, ultimo_cliente, SEEK_SET);
                fread(&cliente_lido, sizeof(Cliente), 1, arquivo_clientes);

                if (cliente_lido.prox == -1)
                {
                    cliente_lido.prox = endereco;
                    fseek(arquivo_clientes, ultimo_cliente, SEEK_SET);
                    fwrite(&cliente_lido, sizeof(Cliente), 1, arquivo_clientes);
                    break;
                }

                ultimo_cliente = cliente_lido.prox;
            }
        }
    }

    if (verificar_fatorCarga(arquivo_clientes, *tamanho_tabela, fator_carga_limite))
    {
        printf("\nExpansão da tabela necessária. Processando...\n");
        expandir_tabela(caminho_tabClientes, caminho_tabHash, tamanho_tabela, P, L);
    }

    fclose(arquivo_clientes);
    fclose(arquivo_hash);
}

void remover_cliente(int codigo, char *tabClientes, char *tabHash, int *table_size, int *P, int *L)
{
    int hash = funcao_hash(codigo, *table_size, *L);
    FILE *hashFile = fopen(tabHash, "r+b");
    FILE *clientesFile = fopen(tabClientes, "r+b");
    if (!hashFile || !clientesFile)
    {
        fprintf(stderr, "Erro ao abrir os arquivos.\n");
        return;
    }

    // Lendo o offset inicial do índice correspondente
    int offsetAtual, offsetAnterior = -1;
    fseek(hashFile, hash * sizeof(int), SEEK_SET);
    fread(&offsetAtual, sizeof(int), 1, hashFile);

    while (offsetAtual != -1)
    {
        // Buscar o cliente correspondente no arquivo de clientes
        fseek(clientesFile, offsetAtual, SEEK_SET);
        Cliente clienteAtual;
        fread(&clienteAtual, sizeof(Cliente), 1, clientesFile);

        if (clienteAtual.cod == codigo)
        {
            // Cliente encontrado: ajustar o encadeamento
            int proxOffset = clienteAtual.prox;

            if (offsetAnterior == -1)
            {
                // Primeiro da lista no índice hash, atualize a tabela hash
                fseek(hashFile, hash * sizeof(int), SEEK_SET);
                fwrite(&proxOffset, sizeof(int), 1, hashFile);
            }
            else
            {
                // Atualize o próximo offset do cliente anterior
                fseek(clientesFile, offsetAnterior + offsetof(Cliente, prox), SEEK_SET);
                fwrite(&proxOffset, sizeof(int), 1, clientesFile);
            }

            // Invalidar o cliente no arquivo de clientes
            clienteAtual.cod = -1;  // Código inválido para marcar como removido
            clienteAtual.prox = -1; // Limpa o encadeamento do cliente removido
            fseek(clientesFile, offsetAtual, SEEK_SET);
            fwrite(&clienteAtual, sizeof(Cliente), 1, clientesFile);

            printf("Cliente com código %d foi removido com sucesso.\n", codigo);

            fclose(hashFile);
            fclose(clientesFile);
            return;
        }

        // Atualizar ponteiros para continuar a busca
        offsetAnterior = offsetAtual;
        offsetAtual = clienteAtual.prox;
    }

    printf("Cliente com código %d não encontrado.\n", codigo);

    fclose(hashFile);
    fclose(clientesFile);
}

Cliente *encontrar_cliente(int codigo_cliente, char *caminho_tabClientes, char *caminho_tabHash, int tamanho_base, int P, int L)
{
    int indice_calculado = mapear_endereco(codigo_cliente, tamanho_base, P, L);
    int tamanho_tabela = tamanho_base + P;

    FILE *arquivo_hash = fopen(caminho_tabHash, "rb");
    if (arquivo_hash == NULL)
    {
        perror("Erro ao abrir tabela hash");
        return NULL;
    }

    // Localiza o início da lista encadeada no índice calculado
    int endereco_atual;
    fseek(arquivo_hash, indice_calculado * sizeof(int), SEEK_SET);
    fread(&endereco_atual, sizeof(int), 1, arquivo_hash);

    if (endereco_atual == -1)
    {
        fclose(arquivo_hash);
        printf("Cliente com código %d não encontrado na tabela hash.\n", codigo_cliente);
        return NULL;
    }

    FILE *arquivo_clientes = fopen(caminho_tabClientes, "rb");
    if (arquivo_clientes == NULL)
    {
        perror("Erro ao abrir tabela de clientes");
        fclose(arquivo_hash);
        return NULL;
    }

    // Percorre a lista encadeada até encontrar o cliente ou atingir o fim
    while (endereco_atual != -1)
    {
        Cliente cliente_atual;
        fseek(arquivo_clientes, endereco_atual, SEEK_SET);
        fread(&cliente_atual, sizeof(Cliente), 1, arquivo_clientes);

        if (cliente_atual.cod == codigo_cliente && cliente_atual.status)
        {
            Cliente *cliente_encontrado = malloc(sizeof(Cliente));
            if (cliente_encontrado == NULL)
            {
                perror("Erro ao alocar memória para o cliente encontrado");
                fclose(arquivo_clientes);
                fclose(arquivo_hash);
                return NULL;
            }

            *cliente_encontrado = cliente_atual;

            fclose(arquivo_clientes);
            fclose(arquivo_hash);
            printf("Cliente com código %d foi encontrado com sucesso.\n", codigo_cliente);
            return cliente_encontrado;
        }

        endereco_atual = cliente_atual.prox;
    }

    // Cliente não encontrado
    fclose(arquivo_clientes);
    fclose(arquivo_hash);
    printf("Cliente com código %d não encontrado.\n", codigo_cliente);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Uso: %s <tamanho da tabela>\n", argv[0]);
        return 1;
    }

    int L = 0;
    int P = 0;
    int table_size = atoi(argv[1]);
    float limite_fator_de_carga = 1.0;

    printf("=== Inicializando a Tabela Hash ===\n");
    criar_tabela_hash(table_size);
    char *tabClientes = "tabClientes";
    char *tabHash = "tabHash";

    // Criando arquivo inicial de clientes
    FILE *p = fopen(tabClientes, "w");
    fclose(p);

    printf("Tabela hash criada com tamanho inicial: %d\n\n", table_size);

    // Criando clientes de exemplo
    Cliente cliente1 = {5, "João"};
    Cliente cliente2 = {10, "Maria"};
    Cliente cliente3 = {15, "Caitlyn"};
    Cliente cliente4 = {20, "Ana"};
    // Cliente cliente5 = {12, "Beatriz"};

    printf("=== Adicionando Clientes ===\n");
    inserir_cliente(cliente2, tabClientes, tabHash, &table_size, &P, limite_fator_de_carga, &L);
    inserir_cliente(cliente3, tabClientes, tabHash, &table_size, &P, limite_fator_de_carga, &L);
    inserir_cliente(cliente1, tabClientes, tabHash, &table_size, &P, limite_fator_de_carga, &L);
    inserir_cliente(cliente4, tabClientes, tabHash, &table_size, &P, limite_fator_de_carga, &L);
    // inserir_cliente(cliente5, tabClientes, tabHash, &table_size, &P, limite_fator_de_carga, &L);

    printf("\nClientes adicionados com sucesso!\n\n");

    printf("=== Exibindo Tabela Hash ===\n");
    exibir_tabHash(tabHash, table_size);
    printf("\n");

    printf("=== Exibindo Tabela de Clientes ===\n");
    exibir_tabClientes_sequencial(tabClientes);
    printf("\n");

    printf("=== Buscando Cliente ===\n");
    int cod_busca = 10; // Código do cliente para busca
    Cliente *cliente_encontrado = encontrar_cliente(cod_busca, tabClientes, tabHash, table_size, P, L);
    if (cliente_encontrado)
    {
        printf("Cliente encontrado: Código = %d, Nome = %s\n", cliente_encontrado->cod, cliente_encontrado->nome);
        free(cliente_encontrado);
    }
    else
    {
        printf("Cliente com código %d não encontrado.\n", cod_busca);
    }
    printf("\n");

    printf("=== Removendo Cliente ===\n");
    int cod_remocao = 15; // Código do cliente a ser removido
    remover_cliente(cod_remocao, tabClientes, tabHash, &table_size, &P, &L);
    printf("Cliente com código %d foi excluído.\n\n", cod_remocao);

    printf("=== Exibindo Tabela Hash Após Exclusão ===\n");
    exibir_tabHash(tabHash, table_size + P);
    printf("\n");

    printf("=== Exibindo Tabela de Clientes Após Exclusão ===\n");
    exibir_tabClientes_sequencial(tabClientes);
    printf("\n");

    printf("=== Exibindo Tabela Hash Completa ===\n");
    exibir_tabela(tabClientes, tabHash, table_size + P);
    printf("\n");

    printf("=== Testes Concluídos ===\n");
    return 0;
}
