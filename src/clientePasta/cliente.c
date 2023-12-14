#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <host_do_servidor> <porta_servidor> <nome_arquivo> <tam_buffer>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *host_servidor = argv[1];
    int porta_servidor = atoi(argv[2]);
    char *nome_arquivo = argv[3];
    int tam_buffer = atoi(argv[4]);

    struct sockaddr_in endereco_servidor;
    int cliente_socket;

    // Criação do socket
    cliente_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (cliente_socket == -1) {
        perror("Erro ao criar socket");
        return EXIT_FAILURE;
    }

    memset(&endereco_servidor, 0, sizeof(endereco_servidor));
    endereco_servidor.sin_family = AF_INET;
    endereco_servidor.sin_port = htons(porta_servidor);

    // Converte o endereço IP de string para forma binária
    if (inet_pton(AF_INET, host_servidor, &endereco_servidor.sin_addr) <= 0) {
        perror("Endereço inválido");
        return EXIT_FAILURE;
    }

    // Estabelece conexão com o servidor
    if (connect(cliente_socket, (struct sockaddr *)&endereco_servidor, sizeof(endereco_servidor)) < 0) {
        perror("Erro ao conectar-se ao servidor");
        return EXIT_FAILURE;
    }
    printf("Conectado ao servidor\n");

    // Envia o nome do arquivo para o servidor
    send(cliente_socket, nome_arquivo, strlen(nome_arquivo), 0);

    // Abre o arquivo para escrita
    FILE *arquivo = fopen(nome_arquivo, "w+");
    if (arquivo == NULL) {
        perror("Erro ao criar o arquivo");
        close(cliente_socket);
        return EXIT_FAILURE;
    }

    char buffer[MAX_BUFFER_SIZE];
    int bytes_recebidos_total = 0;
    int bytes_recebidos;

    // Inicia o cronômetro
    struct timeval inicio, fim;
    gettimeofday(&inicio, NULL);

    // Recebe dados do servidor e escreve no arquivo
    while ((bytes_recebidos = recv(cliente_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_recebidos, arquivo);
        bytes_recebidos_total += bytes_recebidos;
    }

    gettimeofday(&fim, NULL);

    // Fecha o arquivo e a conexão
    fclose(arquivo);
    close(cliente_socket);

    // Calcula tempo gasto
    double tempo_gasto = (double)(fim.tv_sec - inicio.tv_sec) + (double)(fim.tv_usec - inicio.tv_usec) / 1000000;

    // Calcula a taxa de transferência
    double taxa_transferencia = (bytes_recebidos_total * 8) / (tempo_gasto * 1000); // Em kbps

    // Imprime informações
    //printf("Buffer = %5d byte(s), %10.2f kbps (%d bytes em %.6f s)\n", tam_buffer, taxa_transferencia, bytes_recebidos_total, tempo_gasto);
    printf("%d,%.4f,%d,%.6f\n", tam_buffer, taxa_transferencia, bytes_recebidos_total, tempo_gasto);
    return EXIT_SUCCESS;
}
