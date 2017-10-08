# ProgettoLSO2

Comando per compilare il Server : gcc server.c Funzioni_Server/funzioni_server.c  -o server -lpthread -lssl -lcrypto
Comando per Compilare il Client :  gcc client.c Funzioni_Client/funzioni_client.c -o client  -lpthread

Comando per Eseguire il Server: ./server *numero porta*
Comando per Eseguire il Client : ./client *indirizzo ip* *porta*

Dipendenze con libreria: openssl
