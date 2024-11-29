
    printf("arquivo:\n");
    fread(read, sizeof(temp), 24, fl);
    // Ler os dados do arquivo e imprimir
    for (int i = 0; i < 24; i++) {
        printf("Numero: %d, Nome: %s, Data de Nascimento: %s\n", read[i].num, read[i].nome, read[i].Data_nasc);
    }
