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
#include <time.h>
#include <netdb.h>
#include <signal.h>
#include <openssl/sha.h>
#include "Funzioni_Server/funzioni_server.h"
#define BACKLOG 20
#define DIM 104
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;


Player P[104];
char nomi_verdi[52];
char nomi_rossi[52];
pthread_t thread_id, thread_id2;
int n_players=0;
int n_oggetti=0;
int fine_game=0;
int tempo_scaduto=0;
int count_partite=1;
char **Matrice;
int TOT=0;
int fp[3];



//gcc server.c Funzioni_Server/funzioni_server.c -o server -lpthread && ./server
void *connection_handler(void *);
void *recv_movement_client(void *);
void *timing_thread();

int main (int argc, char **argv)
{
  srand(time(NULL));
  struct sockaddr_in serv_ind; // server indirizzo
  struct sockaddr_in client;  // client indirizzo
  socklen_t lung;
  int porta;
  int sd,val=1,accsd,check;
  char indcli[128];
  openFile(fp);
  signal(SIGPIPE, SIG_IGN);


  int n_ostacoli=0;
  int n_armi=0;
  int n_mine=0;

  if( (argc) < 2 )
  {
    fprintf(stdout, "\nInserisci la porta in cui il Server si metterà in ascolto!\n");
    exit(-1);
  }
  porta=atoi(argv[1]);

 //creazione del socket
  if((sd = socket(PF_INET,SOCK_STREAM,0)) < 0)
  {
    perror("Errore Creazione Socket");
    exit(-1);
  }

  // rende il socket riusabile
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));


  //========================CONFIG. INDIRIZZO IP=========================
  memset((void *)&serv_ind, 0, sizeof(serv_ind)); // pulisce ind
  serv_ind.sin_family = AF_INET; // ind di tipo INET
  serv_ind.sin_port = htons(porta); // scelgo porta non priv.
  serv_ind.sin_addr.s_addr = htonl(INADDR_ANY); // connessioni da ogni ip
//======================================================================


 //configurazione bind
  if (bind(sd, (struct sockaddr *)&serv_ind, sizeof(serv_ind)) < 0)
  {
    perror("Errore nella bind");
    exit(-1);
  }

//accetta fino a 20 connessioni, resta in ascolto sulla socket
  if (listen(sd, BACKLOG) < 0 )
  {
    perror("Errore nella listen");
    exit(-1);
  }

  char hello[50];

puts("\n\t\t\t\t\t\t\tSTART SERVER!");

char accsd_c;

do{


    //fprintf(stdout, "\nINIZIO PARTITA!\n");
    fine_game=0;
    riempiNomi(nomi_rossi,nomi_verdi);
    Matrice=GeneraMatrice(&n_ostacoli,&n_armi,&n_mine,&TOT);

    write_on_file(fp[0],0,"X",'X','X',0,count_partite);
    write_on_file(fp[1],0,"X",'X','X',0,count_partite);
    write_on_file(fp[2],0,"X",'X','X',0,count_partite);
    count_partite++;

  if( pthread_create( &thread_id2 , NULL ,  timing_thread , NULL) < 0)
  {
    perror("could not create thread");
    return 1;
  }

  if(fine_game==0)
  {
   while(accsd = accept(sd, (struct sockaddr *)&client, &lung))
   {
      if(fine_game>0)
       break;

      sprintf(hello,"Connection accepted from Client_socket: %d",accsd);
      puts(hello);

      accsd_c=accsd + '0';


      if( (write(accsd,&accsd_c,1)) < 0 )
      {
        perror("Error write accsd");
        close(sd);
      }

      if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &accsd) < 0)
       {
           perror("could not create thread");
           return 1;
       }

  }
}


}while(1);


  return 0;
}



/*================================THREAD====================================*/
void *connection_handler(void *socket_desc)
{

  int sd = *(int*)socket_desc;
  int check, check_login_state;
  char team,nome,username[20];
  void *status;


  do{
    //  puts("lancio login state");
      check_login_state=login_state(sd,username,P,n_players);

      if(check_login_state == 0)
      {
         char msg[30];
         sprintf(msg,"Login Rifiutato da Client_socket = [%d]",sd);
         puts(msg);
      }
      else
      {

        char msg[30];
        sprintf(msg,"Login Accettato da USER=[%s] => Client_socket = [%d]",username,sd);
        puts(msg);
        int X,Y;

        team=leggiTeam(sd);

        if(team == '<')
        {
          fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
          close(sd);
          pthread_exit(status);
        }
        else if(team == 'V')
        {
          nome=giveNameClient(sd,nomi_verdi);

            if(nome == '<')
            {
              fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
              close(sd);
              pthread_exit(status);
            }
        }
        else
        {
          nome=giveNameClient(sd,nomi_rossi);

          if(nome == '<')
          {
            fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
            close(sd);
            pthread_exit(status);
          }

        }

        do {
             check=leggiRequestSpawn(&X,&Y,sd,nome,Matrice,TOT,P);

             if(check == 2)
             {
               perror("Error Write response");
               fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
               close(sd);
               pthread_exit(status);
             }

             char response;
             if(check == 1) response = 'V';
             else response = 'F';


             if( (write(sd,&response,1)) < 0 )
            {
              perror("Error Write response");
              fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
              //freePlayer(P,sd,&n_players);
              close(sd);
              pthread_exit(status);
             }


           } while(check == 0);

          addPlayer(P,X,Y,nome,team,username,&n_players,sd);
          write_on_file(fp[0],1,username,nome,team,0,0);
        //  printf("\n\nSTAMPA PLAYERS:\n");
      //    stampaPlayers(P,n_players);

        }

 } while(check_login_state == 0);
//  pthread_mutex_unlock(&m);

if( pthread_create( &thread_id2 , NULL ,  recv_movement_client , (void*) &sd) < 0)
 {
     perror("could not create thread2");

}

pthread_join(thread_id2,NULL);

pthread_exit(status);
}





void *recv_movement_client(void *socket_desc)
{
  int sd = *(int*)socket_desc;
  int check;

  //pthread_mutex_lock(&m);

      if(sendPlayers(P,n_players,sd))
      {
        fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
        freePlayer(P,sd,&n_players,fp[1],nomi_verdi,nomi_rossi);
        close(sd);
      }

      if(SendData(Matrice,TOT,sd))
      {
        fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
        freePlayer(P,sd,&n_players,fp[1],nomi_verdi,nomi_rossi);
        close(sd);
      }


    while(1)
    {
        //fprintf(stdout, "\nN_PLAYERS=%d\n",n_players );
      if(readRequestMovementClient(sd,Matrice,P,n_players,TOT,&n_oggetti,&fine_game,fp[2]) == 2)
      {
          fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
          freePlayer(P,sd,&n_players,fp[1],nomi_verdi,nomi_rossi);
          close(sd);
          break;
      }

      if(sendPlayers(P,n_players,sd))
      {
        fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
        freePlayer(P,sd,&n_players,fp[1],nomi_verdi,nomi_rossi);
        close(sd);
        break;
      }

      if(SendData(Matrice,TOT,sd))
      {
        fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
        freePlayer(P,sd,&n_players,fp[1],nomi_verdi,nomi_rossi);
        close(sd);
        break;
      }

      if(read_Request_sendObjectDestroyed(sd,n_oggetti))
      {
        fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
        freePlayer(P,sd,&n_players,fp[1],nomi_verdi,nomi_rossi);
        close(sd);
        break;
      }

      if(send_fine_game(sd,fine_game,P,&n_players,fp[1],nomi_verdi,nomi_rossi)>0)
       {
        fprintf(stdout, "\nN_PLAYERS=%d\n", n_players);
        break;
       }

  }

}


void *timing_thread()
{
  int seconds=300;
  void *status;

  tempo_scaduto=0;

  while(seconds>0)
  {
    if(fine_game>0)
    {
  //    fprintf(stdout, "\nFINE-GAME= %d", fine_game);
      break;
    }
    seconds--;
    sleep(1);
  }

  fprintf(stdout,"\nFINE TEMPO\n");

  //fprintf(stdout, "\nFINE-GAME2= %d", fine_game);
  if(fine_game!=0)
  {
   fine_game=3;
   tempo_scaduto=1;
  }

  pthread_exit(status);
}
