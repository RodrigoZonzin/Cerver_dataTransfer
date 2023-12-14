PORTA ?= 5020
BUF_SIZE ?= 128

all:
	gcc ./clientePasta/cliente.c -o ./clientePasta/cliente
	gcc servidor.c -o servidor

runServer: 
	./servidor $(PORTA) $(BUF_SIZE)
