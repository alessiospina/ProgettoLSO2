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
#include "funzioni_client.h"

char ARMA = 124;                 // '|'
char MINA = '*';
char OSTACOLO = 94;             //^
char FLAG = 36;                 //$


#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define COLOR_RESET   "\x1b[0m"


#define SIZE_RIGHE 30
#define SIZE_COLONNE 80


void Logo()
{
     char* logo[8];
     system("@cls||clear");

     logo[0]="\t\t\t\t\t\t\t\t\t\t BENVENUTO IN";
     logo[1]=GREEN"\t\t\t\t\t    ██████╗  █████╗ ████████╗████████╗██╗     ███████╗" RED "██████╗  █████╗ ███████╗██╗  ██╗";
     logo[2]=GREEN"\t\t\t\t\t    ██╔══██╗██╔══██╗╚══██╔══╝╚══██╔══╝██║     ██╔════╝" RED "██╔══██╗██╔══██╗██╔════╝██║  ██║";
     logo[3]=GREEN"\t\t\t\t\t    ██████╔╝███████║   ██║      ██║   ██║     █████╗" RED "  ██████╔╝███████║███████╗███████║";
     logo[4]=GREEN"\t\t\t\t\t    ██╔══██╗██╔══██║   ██║      ██║   ██║     ██╔══╝ " RED " ██╔══██╗██╔══██║╚════██║██╔══██║";
     logo[5]=GREEN"\t\t\t\t\t    ██████╔╝██║  ██║   ██║      ██║   ███████╗███████╗" RED "██████╔╝██║  ██║███████║██║  ██║";
     logo[6]=GREEN"\t\t\t\t\t    ╚═════╝ ╚═╝  ╚═╝   ╚═╝      ╚═╝   ╚══════╝╚══════╝" RED "╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝";


     printf("%s",logo[0]);
     printf("\n\n");
     printf("%s",logo[1]);
     printf("\n");
      printf("%s",logo[2]);
      printf("\n");
     printf("%s",logo[3]);
      printf("\n");
      printf("%s",logo[4]);
       printf("\n");
     printf("%s",logo[5]);
      printf("\n");
      printf("%s",logo[6]);
       printf("\n");
     printf(COLOR_RESET"");
     printf("\n\n");

}


int loginClient(int sd,char username[])
{
      char password[20];
      char tmpchar,go[20];
      int n;
      char risposta_Server;

  do {
    //  clearBuffer();
      printf("\nIl tuo account è gia registrato? (y/n): ");

          do
         {
            scanf(" %c",&tmpchar);
         }while(  (tmpchar!='y'&&tmpchar!='n') && printf("tasto non valido, Riscrivi : ")  );



      if( (write(sd,&tmpchar,1)) < 0 )
      {
        perror("Error Write Require Registration on DB");
        exit(-1);
      }


    do
    {
      printf("\n[USERNAME(max 9 caratteri)]: ");
      clearBuffer();
      fgets(username,20,stdin);
      username[strlen(username)-1]='\0';

    } while((strlen(username) > 9  && fprintf(stdout, "Lunghezza non valida!")));


      if( (write(sd,username,strlen(username))) < 0 )
      {
        perror("Error Write username to Server");
        exit(-1);
      }

    do
    {
      printf("\n[PASSWORD(max 9 caratteri)]: ");
      fgets(password,20,stdin);
      password[strlen(password)-1]='\0';
    }while((strlen(password)>9  &&  fprintf(stdout, "Lunghezza non valida!")));

    fprintf(stdout, "Inseriti Username=[%s] Password=[%s]", username, password);


      if( (write(sd,password, strlen(password))) < 0 )
      {
        perror("Error Write Password to Server");
        exit(-1);
      }


        if( (read(sd,&risposta_Server,1)) < 0 )
        {
          perror("Error read risposta_Server");
        }

       fprintf(stdout, "\nrisposta_Server=[%c]", risposta_Server );


      if(risposta_Server == 'F')
        printf("\nLogin Errato\n");


      else if(risposta_Server == 'X')
        printf("\nUTENTE GIA' LOGGATO CON QUESTO USERNAME\n");

      else
        printf("\nLogin EFFETTUATO!\n");

    //  clearBuffer();
    } while(risposta_Server != 'Y');



}



int Menu(char** M, int *n_players,int io, int sd, char username[])             //da aggiungere al progetto
{
    int richiesta=-1;
    char tasto; //   SPOSTAMENTO!!!!!!!


    printf("[a-w-d-s] per muoverti, m per aprire il menu\ninput : ");
     do
     {
        while((tasto=getchar()!='\n')&& (tasto!=EOF));
        scanf("%c",&tasto);

     }while(  (tasto!='a'&&tasto!='w'&&tasto!='d'&&tasto!='s'&&tasto!='m') && printf("tasto non valido, Riscrivi : ")  );

       RequestMovementClient(sd, tasto, username);

                      // system("@cls||clear");
                  if(tasto == 'm')
                  {
                       printf("x------------------------------------------------------------x");
                       printf("\n| 0) Per disconnetterti\t\t\t\t\t     |\n| 1) Per vedere la lista degli utenti collegati\t\t     |\n| 2) Per vedere i tuoi compagni\t\t\t\t     |\n| ");
                       printf("3) Per vedere la posizione degli oggetti incontrati \t     |\n");
                       printf("x------------------------------------------------------------x");
                       printf("\n  SCELTA = ");
                       scanf("%d",&richiesta);

                        switch(richiesta)
                        {
                            case 0:
                                 close(sd);
                                 exit(1);
                            break;

                            case 1:

                                stampaPlayers(M,*n_players,io);
                                //richiesta=-1;
                                Pulisci();
                            break;

                            case 2:

                                  stampaCompagni(M,P[io].team,*n_players,io);
                              //    richiesta=-1;
                                  Pulisci();
                            break;

                            case 3:
                                 //richiesta=-1;
                                // Pulisci();
                            break;

                            default:
                                printf("\nScelta non valida");
                            //    richiesta=-1;
                                Pulisci();
                            break;
                        }
                    }


return richiesta;


}

int check_my_punteggio(int sd , int io)
{
    if(P[io].punteggio <= 0)
    {
      fprintf(stdout, "\n\nSEI STATO ELIMINATO!\n");
      close(sd);
      return 1;
    }
return 0;
}

void request_receive_objects_destroyed(int sd,int richiesta,char team_char)
{
   if(richiesta == 3)
   {

    if( (write(sd, "Y", 1)) < 0 )
    {
      perror("Error write request_receive_objects_destroyed ");
      //exit(-1);
    }

    receiveObjectDestroyed(sd,team_char);
    Pulisci();
  }
    else
     if( (write(sd, "N", 1)) < 0 )
     {
       perror("Error write request_receive_objects_destroyed");
       //exit(-1);
     }
}

void addPlayer(int X,int Y,char nome,char* username,int team,int punteggio,int indice,int i)      //da aggiungere al progetto
{
         P[i].X=X;
         P[i].Y=Y;
         P[i].nome=nome;
         strcpy(P[i].username,username);
         P[i].team=team;
         P[i].punteggio=punteggio;
        // n_players++;
         P[i].indice_player=indice;
}


void StampaPunteggio(int io)       //da aggiungere al progetto
{
    printf("PUNTEGGIO : [%d]\n",P[io].punteggio);
}

int find_me(int n_players, int indice)
{
  int i;

   for(i=0;i<n_players;i++)
      if(P[i].indice_player == indice)
        return i;

//return 0;
}


void stampaPlayers(char** M, int n_players, int io)                   //da aggiungere al progetto
{
      int i;
        system("@cls||clear");
          stampaMappa(M,n_players,io);
            printf("\n\tUTENTI COLLEGATI\n\n");

        for(i=0;i<n_players;i++)
        {
            if(i==io)
            {
                if(P[i].team=='V')
                {
                     printf("P%d* : username=" GREEN "%s   ",i+1,P[i].username);
                     printf(COLOR_RESET "team=" GREEN "%c   ",P[i].team);
                }
                else
                {
                      printf("P%d* : username=" RED "%s   ",i+1,P[i].username);
                      printf(COLOR_RESET "team=" RED "%c   ",P[i].team);
                }
            }
            else
            {
                if(P[i].team=='V')
                {
                     printf("P%d  : username=" GREEN "%s   ",i+1,P[i].username);
                     printf(COLOR_RESET "team=" GREEN "%c   ",P[i].team);
                }
                else
                {
                      printf("P%d  : username=" RED "%s   ",i+1,P[i].username);
                      printf(COLOR_RESET "team=" RED "%c   ",P[i].team);
                }

            }
              printf(COLOR_RESET "\n");
        }
        printf(COLOR_RESET "\n");
}

void stampaCompagni(char** M,char team,int n_players, int io)         //da aggiungere al progetto
{
    int i;
     system("@cls||clear");
       for(i=0;i<n_players;i++)
       {
           if(P[i].team==team)
               M[P[i].X][P[i].Y]=P[i].nome;
       }

       stampaMappa(M,n_players, io);

       for(i=0;i<n_players;i++)
       {
           if(P[i].team==team)
               M[P[i].X][P[i].Y]=' ';
       }


       printf("\n\tCOMPAGNI DI SQUADRA\n\n");

     for(i=0;i<n_players;i++)
     {
            if(P[i].team==team)
            {
                if(team=='V')
                {
                   printf("nome=" GREEN "%c   ",P[i].nome);
                    printf(COLOR_RESET "Posizione(%d,",P[i].X);
                   printf("%d)   ",P[i].Y);
                   printf("punteggio=%d",P[i].punteggio);
                }
                else
                {
                   printf("nome=" RED "%c   ",P[i].nome);
                   printf(COLOR_RESET "Posizione(%d,",P[i].X);
                   printf("%d)   ",P[i].Y);
                   printf("punteggio=%d",P[i].punteggio);
                }

                printf("\n");
            }
     }


}

void Pulisci()                 //da aggiungere al progetto
{
   printf("\n\nPremi Enter per continuare... ");
     char prev=0;

        while(1)
        {
            char c = getchar();

            if(c == '\n' && prev == c)
            {

                system("@cls||clear");
                break;
            }

            prev = c;
        }
}


int teamSelection(int sd)
{
  char squadra;
  int team;

  printf("\nScegli il team di appartenenza (0=verde,1=rosso)\n");
       while( scanf("%d",&team)!=1||team<0||team>1)
       {
                             printf("Team non valido : ");
                             while (getchar() != '\n');
       }

       if(team == 0) squadra = 'V' ;
       else squadra = 'R';

       if( (write(sd,&squadra, 1)) < 0 )
       {
         perror("Error Write Team to Server");
         exit(-1);
       }

  return team;
}

char requestName(int sd)
{
  char nome;

    if( (read(sd,&nome,1)) < 0 )
    {
      perror("Error request Name");
      exit(-1);
    }

    //fprintf(stdout, "letto %c", nome);

  return nome;
}


int requestToSpawn(int * x, int * y, int team,int sd, char **Mappa)
{

do{

  printf("\nScrivi la tua posizione (riga,colonna)\nriga(1-28): ");

     while( scanf("%d",x)!=1||*x<1||*x>28)
     {
            printf("Coordinata riga non valida, Reinserisci : ");
            while (getchar() != '\n');
     }

     if(team==0)
     {

          printf("colonna(1-39) : ");
          while( scanf("%d",y)!=1||*y<1||*y>39)
          {
                    printf("Coordinata colonna non valida, Reinserisci : ");
                    while (getchar() != '\n');
          }



     }
     else
     {

          printf("colonna(40-78) : ");
          while( scanf("%d",y)!=1||*y<1||*y<40||*y>78)
          {
                    printf("Coordinata colonna non valida, Reinserisci : ");
                    while (getchar() != '\n');
          }
     }

   } while(Mappa[*x][*y]=='#' && fprintf(stdout, "\nSpawn Non Valido su una Barriera"));

    int conv_x=htonl(*x);
    int conv_y=htonl(*y);
    char response;




     if( (send(sd, (const char*)&conv_x, 4, 0)) < 0 )
     {
       perror("Error Write Coordinata_X");
       exit(-1);
     }

     if( (send(sd, (const char*)&conv_y, 4, 0)) < 0 )
     {
       perror("Error Write Coordinata_Y");
       exit(-1);
     }


     if( (read(sd,&response,1)) < 0 )
     {
       perror("Error Read Response from server");
       exit(-1);
     }


     if(response == 'V') return 1;
     else return 0;

}

void riepilogoDati(char nome, int team, int X, int Y)
{
  system("clear");
  Logo();
  fprintf(stdout,             "\n+--------------------Riepilogo Dati-------------------+");
    if(team==0)
      fprintf(stdout,"\n| NOME=["GREEN"%c"COLOR_RESET"] | TEAM=["GREEN"VERDI"COLOR_RESET"] | Cordinate di Spawn=[%d,%d] |",nome,X,Y);
    else
    fprintf(stdout,"\n| NOME=["RED"%c"COLOR_RESET"] | TEAM=["RED"ROSSI"COLOR_RESET"] | Cordinate di Spawn=[%d,%d] |",nome,X,Y);
  fprintf(stdout, COLOR_RESET "\n+-----------------------------------------------------+");

  getchar();
  fprintf(stdout, "\nPremi un tasto per continuare...");
  while( getchar() != '\n' );

}

char** allocaMappa()
{
    int i;
    char **M=malloc(SIZE_RIGHE*sizeof(char *));

    for(i=0;i<SIZE_RIGHE;i++)
      M[i]=malloc(SIZE_COLONNE*sizeof(char));

    return M;
}

char** GeneraMappa()
{
    int i,j;
    char **M=allocaMappa();

    for(i=0;i<SIZE_RIGHE;i++)
    {
      for(j=0;j<SIZE_COLONNE;j++)
      {
        if(i == 0) //riga in alto
          M[i][j]='#';

        else if(j==0 && i!=0) // riga a sx
          M[i][j]='#';

        else if(j==(SIZE_COLONNE-1) && i != 0) // riga a dx
          M[i][j]='#';

        else if(i==(SIZE_RIGHE-1)) // riga sotto
         M[i][j]='#';

        else if ((i==11 || i==12 || i==13 || i == 14 || i==15 || i == 16 || i == 17 || i == 18 || i == 19) && j == 10)
          M[i][j]='#';

        else if ((i==11 && j == 9) || (i == 19 && j == 9) || (i == 11 && j == 8 ) || (i == 19 && j == 8 ))
          M[i][j]='#';

        else if (((i==11 || i==12 || i==13 || i == 14 || i==15 || i == 16 || i == 17 || i == 18 || i == 19) && j == 69))
          M[i][j]='#';

        else if ((i==11 && j == 70) || (i == 19 && j == 70) || (i == 11 && j == 71 ) || (i == 19 && j == 71 ))
          M[i][j]='#';
        else if(M[i][j]!=ARMA && M[i][j]!=MINA && M[i][j]!=OSTACOLO)
           M[i][j]=' ';

      }
    }


     //generateFlags(M);                  //Tutte le cose dovranno essere nascoste finchè sarà un giocatore ad andarci sopra e a scoprirle


    return M;
}


void stampaMappa(char **M, int n_players, int io)
{
  int i,j,c=0,r=0,k;
  char team;

   for(i=0;i<n_players;i++)
     M[P[i].X][P[i].Y]=P[i].nome;



printf("\n ");
    StampaPunteggio(io);                                         //da aggiungere al progetto

printf("\n ");


       for(i=0;i<80;i++)
       {
              printf("%d",c);
              if(c==9)
                  c=-1;
             c++;
      }


         printf("\n");
 for(i=0;i<SIZE_RIGHE;i++)
 {
      printf(COLOR_RESET "%d",r);
      if(r==9)
         r=-1;
      r++;


     for(j=0;j<SIZE_COLONNE;j++)
     {
          if(j<40 && (M[i][j]=='#'  || M[i][j]==FLAG ))
          {
               printf(GREEN   "%c",M[i][j]);
          }
          else if(j>=40 && (M[i][j]=='#'  || M[i][j]==FLAG))
          {
                printf(RED    "%c",M[i][j]);
          }
          else if( (M[i][j]>='A'&&M[i][j]<='Z')  || (M[i][j]>='a'&&M[i][j]<='z'))
          {
               if(P!=NULL)
               {
                          for(k=0;k<n_players;k++)
                          {

                              if(M[i][j]==P[k].nome && i==P[k].X && j==P[k].Y)
                              {
                                   team=P[k].team;
                                   break;
                              }
                          }
               }

              if(team=='V')
                  printf(GREEN   "%c",M[i][j]);
              else
                 printf(RED   "%c",M[i][j]);
          }
          else
             printf(COLOR_RESET "%c",M[i][j]);
     }
   printf("\n");
 }

  printf(COLOR_RESET "\n");
}





void clearBuffer(){
    char c;
    while ((c = getchar()) != '\n' && c != EOF) { };
}

int ReceiveData(char **Mappa, int *TOT_OBJECTS,int sd)
{
  int TOT_OBJECTS_temp=0;

      if( (recv(sd, &TOT_OBJECTS_temp, 4, 0)) < 0 )
      {
        perror("Error Read Coordinata_X");
        exit(-1);
      }

      *TOT_OBJECTS = ntohl(TOT_OBJECTS_temp);

      //fprintf(stdout, "\nHo ricevuto %d", *TOT_OBJECTS);

      int i,j,k;
      char temp[4];

      for(i=0;i<*TOT_OBJECTS;i++)
      {
        for(j=0;j<4;j++)
            if( (read(sd, &temp[j], 1)) < 0 )
            {
              perror("Error Read carattere Matrice");
              exit(-1);
            }

            char X_c=temp[1];
            int X_i=X_c-'0';
            char Y_c=temp[2];
            int Y_i=Y_c-'0';

          //  printf("\n %d %d", X_i , Y_i);

            if(temp[3] == 'V')
             Mappa[X_i][Y_i]=temp[0];
            else
              Mappa[X_i][Y_i]=' ';
      }

}

void RequestMovementClient(int sd, char spostamento , char username[])
{
    //fprintf(stdout , "\nInvio Richiesta di Spostamento [%s %c] al Server\n", username , spostamento);

      if( (write(sd,username,strlen(username))) < 0 )
      {
        perror("Error Write Username - RequestMovementClient() ");
        //exit(-1);
      }

      if( (write(sd,&spostamento,1) < 0 ))
      {
        perror("Error Write spostamento - RequestMovementClient() ");
      //  exit(-1);
      }

}

void receiveObjectDestroyed(int sd, char team)
{
    int i,n,X,Y, n_letture, nbytes;
    char username[20],temp[4], n_letture_c;

    if( (write(sd,&team,1)) < 0 )
    {
      perror("Error write team in receiveObjectDestroyed");
      //exit(-1);
    }

    if( (read(sd,&n_letture_c,1)) < 0 )
    {
      perror("Error receive n_letture receiveObjectDestroyed");
    //  exit(-1);
    }

    n_letture=n_letture_c - '0';

    for(i=0; i<n_letture; i++) // DA AGGIUSTARE! DEVO SAPERE IL NUMERO DI ITERAZIONI DA FARE
    {
        if( (nbytes=read(sd,username,20)) < 0 )
        {
          perror("Error receive username receiveObjectDestroyed");
        //  exit(-1);
        }
          username[nbytes]='0';

        if( (read(sd,temp,4)) < 0 )
        {
          perror("Error temp read receiveObjectDestroyed");
          //exit(-1);
        }

        X=temp[1] - '0';
        Y=temp[2] - '0';

        StampaObjects(username,temp[3],temp[0],X,Y);

    }

}

void StampaObjects(char username[],char nome_p,char nome_o,int X,int Y)
{
        printf("USERNAME=%s NOME=%c : ",username,nome_p);
        printf("[%c] -> (%d,%d)\n",nome_o,X,Y);
}

void receivePlayers(int sd , char **Mappa, int *n_players)
{
        int punteggio, X , Y , i,indice, old_X , old_Y;
        char username[20],punteggio_string[10];
        char n_players_c, punteggio_c , X_c , Y_c , nome , team;
        char temp[5];

        if( (read(sd, &n_players_c, 1)) < 0 )
        {
          perror("Error Read n_players in receivePlayers");
          exit(-1);
        }

        *n_players=n_players_c - '0';

         fprintf(stdout, "\nRicevuto n_players = %d\n", *n_players);

         for(i=0;i<*n_players; i++)
         {

           if( (read(sd,temp,5)) < 0 )
           {
             perror("Error Read team in receivePlayers");
             exit(-1);
           }

        //  puts("ATTENDO L'USERNAME");
           if( (read(sd,username,20)) < 0 )
           {
             perror("Error Read username in receivePlayers");
             exit(-1);
           }
          // puts("USERNAME RICEVUTO");

          if( (read(sd,punteggio_string,10)) < 0 )
          {
            perror("Error Read punteggio_string in receivePlayers");
            exit(-1);
          }


            X=temp[2] - '0';
            Y=temp[3]  - '0';
            indice=temp[4] - '0';
            punteggio=atoi(punteggio_string);

          //  puts("Prima addPlayer");
        fprintf(stdout, "\nRicevuto username=[%s] nome=[%c] | squadra=[%c] | Punteggio=[%d] | X=[%d] Y=[%d] | indice =[%d]",username, temp[0] , temp[1] ,punteggio, X , Y, indice);

        old_X=P[i].X;
        old_Y=P[i].Y;
        Mappa[old_X][old_Y]=' ';

        fprintf(stdout, "\nP[%d] old_X=%d , old_Y=%d",i,old_X,old_Y);

        addPlayer(X,Y,temp[0],username,temp[1],punteggio,indice,i);

        }
      //   puts("Esco dal For");

}

int receive_fine_game(int sd)
{
    char temp=' ';

    if( (read(sd,&temp,1)) < 0 )
    {
      //perror("Error read receive_fine_game");
      fprintf(stdout, "PAREGGIO");
    }

    //fprintf(stdout, "\nRicevuto fine_temp=%c",temp);

    if(temp == 'F')
    {
      fprintf(stdout, "\n\nHanno vinto i VERDI!\nFINE PARTITA.\n");
      return 1;
    }
    else if (temp == 'B')
    {
      fprintf(stdout, "\n\nHanno vinto i ROSSI!\nFINE PARTITA.\n");
      return 1;
    }
    else if (temp == 'P')
    {
      fprintf(stdout, "\n\nPareggio\nFINE PARTITA.\n");
      return 1;
    }

  return 0;
}
