#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <porta_servidor> <tam_buffer>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int porta_servidor = atoi(argv[1]);
    int tam_buffer = atoi(argv[2]);

    struct sockaddr_in endereco_servidor, endereco_cliente;
    int servidor_socket, cliente_socket;
    int endereco_len = sizeof(endereco_cliente);

    // Criação do socket
    servidor_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_socket == -1) {
        perror("Erro ao criar socket");
        return EXIT_FAILURE;
    }

    memset(&endereco_servidor, 0, sizeof(endereco_servidor));
    endereco_servidor.sin_family = AF_INET;
    endereco_servidor.sin_addr.s_addr = INADDR_ANY;
    //inet_pton(AF_INET, "179.108.194.149", &endereco_servidor.sin_addr);
    endereco_servidor.sin_port = htons(porta_servidor);

    //para reutilizar a mesma porta apos a execucao 
    int opcao_reutilizar = 1;
    if (setsockopt(servidor_socket, SOL_SOCKET, SO_REUSEADDR, &opcao_reutilizar, sizeof(opcao_reutilizar)) < 0) {
        perror("Erro ao configurar SO_REUSEADDR");
        return EXIT_FAILURE;
    }

    // Faz a ligação do socket a uma porta
    if (bind(servidor_socket, (struct sockaddr *)&endereco_servidor, sizeof(endereco_servidor)) < 0) {
        perror("Erro ao fazer bind");
        return EXIT_FAILURE;
    }

    // Espera por conexões
    listen(servidor_socket, 1);
    printf("Aguardando conexão...\n");

    // Aceita a conexão
    cliente_socket = accept(servidor_socket, (struct sockaddr *)&endereco_cliente, (socklen_t*)&endereco_len);
    if (cliente_socket < 0) {
        perror("Erro ao aceitar conexão");
        return EXIT_FAILURE;
    }
    printf("Conexão estabelecida\n");

    // Recebe o nome do arquivo
    char nome_arquivo[MAX_BUFFER_SIZE];
    int bytes_recebidos = recv(cliente_socket, nome_arquivo, sizeof(nome_arquivo), 0);
    if (bytes_recebidos <= 0) {
        perror("Erro ao receber o nome do arquivo");
        close(cliente_socket);
        close(servidor_socket);
        return EXIT_FAILURE;
    }
    nome_arquivo[bytes_recebidos] = '\0';

    // Abre o arquivo
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        close(cliente_socket);
        close(servidor_socket);
        return EXIT_FAILURE;
    }

    // Envia os dados do arquivo
    char buffer[MAX_BUFFER_SIZE];
    int bytes_enviados_total = 0;
    int bytes_lidos;
    struct timeval inicio, fim;

    gettimeofday(&inicio, NULL);

    while ((bytes_lidos = fread(buffer, 1, tam_buffer, arquivo)) > 0) {
        int bytes_enviados = send(cliente_socket, buffer, bytes_lidos, 0);
        if (bytes_enviados < 0) {
            perror("Erro ao enviar dados");
            break;
        }
        bytes_enviados_total += bytes_enviados;
    }

    gettimeofday(&fim, NULL);

    // Fecha conexão e arquivo
    fclose(arquivo);
    close(cliente_socket);
    close(servidor_socket);

    // Calcula tempo gasto
    double tempo_gasto = (double)(fim.tv_sec - inicio.tv_sec) + (double)(fim.tv_usec - inicio.tv_usec) / 1000000;

    // Imprime informações
    printf("Arquivo: %s\n", nome_arquivo);
    printf("Bytes enviados: %d\n", bytes_enviados_total);
    printf("Tempo gasto: %.6f segundos\n", tempo_gasto);

    return EXIT_SUCCESS;
}
