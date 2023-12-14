import subprocess
import threading
import time

arquivos = ['carrancasClassification.png', 'palavras.txt', 'audio.mp3', 'matematica_discreta.pdf']
buff_sizes = [128, 256, 512, 1024]

arquivoResultados = open("resultados.csv", "w+")
arquivoResultados.write("tam_buffer,taxa_transferencia(s),bytes_recebidos_total,tempo_gasto\n")

def runServer(porta, buf):
    subprocess.run(["./servidor", str(porta), str(buf)])

def runClient(arquivo, porta, buf):
    process = subprocess.Popen(["cd ../clientePasta && ./cliente", str(porta), arquivo, str(buf)], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    stdout, _ = process.communicate()
    print(stdout)  # Aqui você pode analisar a saída do cliente para obter resultados

porta = 5022
for arquivo in arquivos:
    for buf in buff_sizes:
        server_thread = threading.Thread(target=runServer, args=(porta, buf))
        server_thread.start()

        time.sleep(1)  # Apenas para garantir que o servidor esteja ativo antes do cliente

        client_thread = threading.Thread(target=runClient, args=(arquivo, porta, buf,))
        client_thread.start()

        print("Pronto!")
        porta += 1

arquivoResultados.close()
