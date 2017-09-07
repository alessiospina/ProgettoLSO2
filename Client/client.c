#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h> // Threads
#include <sys/socket.h> // Socket
#include <sys/un.h> // Connection
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <signal.h>
#include <assert.h>
#include "Funzioni_Client/funzioni_client.h"
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t v;

#define BACKLOG 20
#define RIGHE 30
#define COLONNE 80

int n_players=0;
char **Mappa;
char username[20];
char spostamento;
int indice;
int TOT;



//gcc client.c Funzioni_Client/funzioni_client.c -o client  -lpthread && ./client
void INThandler(int);


int main (int argc, char **argv)
{
  int sd; // id del socket
  int richiesta;
  struct sockaddr_in serv_ind;
  struct hostent *ipserv;
  int porta;
  pthread_t thread_id1,thread_id2;
  int Coordinata_X,Coordinata_Y;
  Mappa=GeneraMappa();
  char temp_sd,team_char;
  int ok,io=0;


  if( ( argc ) < 3 )
  {
    fprintf(stdout, "\nArgomento non valido, inserisci l'indizzo IP del Server e la Porta\n Esempio: ./server 95.13.14.156 8080\n");
    exit(-1);
  }

  porta=atoi(argv[2]);
  fprintf(stdout, "\nConnessione ==> [%s]:[%d]\n", argv[1],porta);

  //==================CONF. INDIZZO IP===============================
  memset((void *)&serv_ind, 0, sizeof(serv_ind)); // pulizia ind
  serv_ind.sin_family = AF_INET; // ind di tipo INET
  serv_ind.sin_port = htons(porta); // porta a cui collegarsi
  inet_aton(argv[1],&serv_ind.sin_addr);
  //====================================================================

  if ( (sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
     perror("Errore in creazione socket");
     exit(-1);
  }

  if (connect(sd, (struct sockaddr *) &serv_ind, sizeof(serv_ind)) == -1) {
  perror("Errore nella connessione");
  exit(-1);
  }


  if( (read(sd,&temp_sd,1)) < 0 )
  {
    perror("Error read indice");
    exit(-1);
  }

  indice=temp_sd - '0';
  Logo();
  loginClient(sd,username);
  int team=teamSelection(sd);

  if(team==0) team_char='V';
  else team_char='R';

  char nome=requestName(sd);

   do
   {
    ok=requestToSpawn(&Coordinata_X,&Coordinata_Y,team,sd,Mappa);
    fprintf(stdout, "Ho restituito ok=%d", ok );
    if(ok==0)
      fprintf(stdout,"\nPosizione Errata!\n");
   }
   while(ok==0);

   riepilogoDati(nome,team,Coordinata_X,Coordinata_Y);

   receivePlayers(sd,Mappa,&n_players);
   ReceiveData(Mappa,&TOT,sd);
   io=find_me(n_players,indice);
   stampaMappa(Mappa,n_players,io);

  while(1)
  {

      //receiveObjectDestroyed(sd,team_char);
      richiesta=Menu(Mappa,&n_players,io,sd,username);
      receivePlayers(sd,Mappa,&n_players);
      ReceiveData(Mappa,&TOT,sd);
      io=find_me(n_players,indice);

      if(check_my_punteggio(sd,io))
        break;

      system("clear");
      stampaMappa(Mappa,n_players,io);
      request_receive_objects_destroyed(sd,richiesta,team_char);

      if(richiesta>=0)
      {
        io=find_me(n_players,indice);
        stampaMappa(Mappa,n_players,io);
      }

      if(receive_fine_game(sd))
      {
        stampaMappa(Mappa,n_players,io);
        break;
      }

  }


 return 0;
}
