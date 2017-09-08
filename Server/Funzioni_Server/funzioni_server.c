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
#include <openssl/sha.h>
#include "funzioni_server.h"

#define SIZE_RIGHE 30
#define SIZE_COLONNE 80

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define COLOR_RESET   "\x1b[0m"

char ARMA = 124;                 // '|'
char MINA = '*';
char OSTACOLO = 94;             //^
char FLAG = 36;                 //$


int loginServer(int accsd,char username[], char password[])
{
  char tmpchar;
  int nbytes;


    if( (read(accsd,&tmpchar,1)) < 0 )
    {
      perror("Error reading registration on DB");
      exit(-1);
    }
      printf("\nLetto [%c]", tmpchar);

    if( (nbytes=read(accsd,username,20)) < 0 )
    {
      perror("Error Reading from Client: username");
      exit(-1);
    }
    username[nbytes]='\0';
  //  printf("\nLetto [%s]", username);


    if( (nbytes=read(accsd,password,20)) < 0 )
    {
      perror("Error reading from Client:password");
      exit(-1);
    }
    password[nbytes]='\0';
  //  printf("\nLetto [%s]", password);



    //printf("\nRichiesta di login da [%s] [%s] [%c]",username,password,tmpchar);

    if(tmpchar == 'y')
      return 1;

    else
      return 0;

}




int verify_client_registration(char username[], char password[])
{
   FILE *fp;
   char user_temp[20], pass_temp[64];

   if((fp=fopen("Accounts.txt","r"))<0)
   {
     perror("Errore apertura file : Accounts.txt");
   }

  // puts("\nverify_client_registration:");

   while(!feof(fp))
   {
      fscanf(fp,"%s %s\n",user_temp,pass_temp);
    //  fprintf(stdout,"\nUser:[%s] Pass:[%s]", user_temp,pass_temp);

        if((strcmp(user_temp,username) == 0) && (strcmp(pass_temp,password)==0))
          return 1;
   }

   fclose(fp);
   return 0;

}

int do_registration(char username[], char password[])
{
    FILE *fp;
    int fd;
    char user_temp[20], pass_temp[64],text[85];


    if((fp=fopen("Accounts.txt","r"))<0)
    {
      perror("Errore apertura file : Accounts.txt");
    }

    while(!feof(fp))
    {
       fscanf(fp,"%s %s\n",user_temp,pass_temp);

    //   printf("\nuser:%s pass:%s", user_temp,pass_temp);

         if(strcmp(user_temp,username) == 0)
           return 0;
    }

    fclose(fp);

    if((fd=open("Accounts.txt", O_WRONLY | O_APPEND ,S_IRUSR | S_IWUSR ))<0)
    {
      perror("Errore apertura file : Accounts.txt");
    }


    strcpy(text,"\n");
    strcat(text,username);
    strcat(text," ");
    strcat(text,password);

    //fprintf(stdout, "\ntext: %s", text);

    lseek(fd,0,SEEK_END);
    write(fd,text,strlen(text));

    close(fd);

    return 1;

}


int login_state(int sd, char username[], Player P[], int n_players)
{
  char password[20];
  char Hash_password[64];

  if(loginServer(sd,username,password))
  {
      Encryption_SHA256(password,Hash_password);

      if(verify_client_registration(username,Hash_password))
      {

        if(check_username(username,P,n_players))
        {
          write(sd,"Y",1);
          return 1;
        }
        else
        {
          write(sd ,"X",1);
          return 0;
        }

      }
      else
      {
    //   printf("\nLogin Failed: Utente o Password errati!\n");
        write(sd,"F",1);
        return 0;
      }

   }
   else
   {
    Encryption_SHA256(password,Hash_password);

    if(do_registration(username,Hash_password))
    {
      //printf("\nRegistrazione Effettuata!\n");
      write(sd,"Y",1);
      return 1;
    }

    else
    {
    //  printf("\nRegistrazione Fallita!\n");
      write(sd,"F",1);
      return 0;
    }
 }
}

 void convertSHA1BinaryToCharStr(const unsigned char * const hashbin, char * const hashstr) {
   for(int i = 0; i<32; ++i)
   {
     sprintf(&hashstr[i*2], "%02X", hashbin[i]);
   }
   hashstr[64]=0;
 }

 void Encryption_SHA256(char string[], char *pass_hash)
 {
   unsigned char digest[SHA256_DIGEST_LENGTH];
   char hash_string[64];
   SHA256_CTX sha256;
   SHA256_Init(&sha256);
   SHA256_Update(&sha256, string, strlen(string));
   SHA256_Final(digest, &sha256);

   convertSHA1BinaryToCharStr(digest, hash_string);

   strcpy(pass_hash,hash_string);
 }




int check_username(char username[] , Player P[] , int n_players)
{
      int i=0;

    //  fprintf(stdout, "\nSto in check_username con %s",username);


      for(i=0;i<n_players;i++)
      {
          if(strcmp(P[i].username,username)==0)
            return 0;
      }

      return 1;
}


char leggiTeam(int sd)
{
  char team;

    if( (read(sd,&team,1)) < 0 )
    {
      perror("Error Read Team from Client");
      return '<';
    }


return team;
}


char cercaNome(char nomi[])
{
  int i;
  char nome;
  for(i=0;i<52;i++)
  {
     if(nomi[i] != '@')
     {
        nome=nomi[i];
        nomi[i]='@';
        return nome;
      }
  }

  return '@'; // array vuoto!
}



void riempiNomi(char nomi_rossi[],char nomi_verdi[])
{
  int i=0;
  char lettera_start1='A';
  char lettera_start2='a';

  for(i=0;i<26;i++)
  {
    nomi_rossi[i]=lettera_start1;
    nomi_verdi[i]=lettera_start1;
    nomi_rossi[i+26]=lettera_start2;
    nomi_verdi[i+26]=lettera_start2;
    lettera_start1++;
    lettera_start2++;
  }

}



char giveNameClient(int sd, char nomi[])
{
  char nome;

    nome=cercaNome(nomi);

    if( (write(sd,&nome,1)) < 0 )
    {
      perror("Error giveNameClient()");
      return '<';
    }


  return nome;
}

int leggiRequestSpawn(int *C_X , int * C_Y, int sd,char nome,char** Mappa, int TOT_OBJECTS,Player P[])
{

  int x=0,y=0;
  int x_temp,y_temp;

      if( (recv(sd, &x_temp, 4, 0)) < 0 )
      {
        perror("Error Read Coordinata_X");
        return 2;
      }

      if( ((recv(sd, &y_temp, 4, 0)) < 0 ))
      {
        perror("Error Read Coordinata_Y");
        return 2;
      }

   x = ntohl(x_temp);
   y = ntohl(y_temp);

   *C_X=x;
   *C_Y=y;

   int check=spawnPlayer(*C_X,*C_Y,nome,Mappa,TOT_OBJECTS,P);

   return check;

}

void setTime(char *time_string)
{
  time_t current_time;

  current_time = time(NULL);
  if (current_time == ((time_t)-1))
   {
       (void) fprintf(stderr, "Failure to obtain the current time.\n");
       exit(EXIT_FAILURE);
   }

   /* Convert to local time format. */
   time_string = ctime(&current_time);

   if (time_string == NULL)
   {
       (void) fprintf(stderr, "Failure to convert the current time.\n");
       exit(EXIT_FAILURE);
   }

   /* Print to stdout. ctime() has already added a terminating newline character. */
  //  (void) printf("Current time is %s", time_string);

}

void removeFiles()
{
  remove("LoginClients.txt");
  remove("EliminationClients.txt");
  remove("Flag.txt");
}

void openFile(int fp[])
{

   removeFiles();

   int f_acc;


   if( (fp[0]=open("LoginClients.txt",O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0 )
   {
     perror("Error create or open file LoginClients.txt");
     //exit(-1);
   }

   if( (fp[1]=open("EliminationClients.txt",O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0 )
   {
     perror("Error create or open file EliminationClients.txt");
  //   exit(-1);
   }

   if( (fp[2]=open("Flag.txt",O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0 )
   {
     perror("Error create or open file Objects.txt");
  //   exit(-1);
   }

   if( (f_acc=open("Accounts.txt",O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0 )
   {
     perror("Error create or open file Objects.txt");
  //   exit(-1);
   }

   close(f_acc);



}

void write_on_file(int fp , int type, char *username,char nome, char team,int punteggio,int count_partite)
{
    char message[200];
    time_t timer;
    char buffer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    //puts(buffer);

    if(type==0)
      sprintf(message,"[PARTITA: %d]\n",count_partite);

    if(type==1) // logging
    sprintf(message, "Username: [%s] - Nome: [%c] - Team: [%c] - Time: [%s]\n",username,nome,team,buffer);

    else if(type == 2) //eliminazioni per DISCONNESSIONE
      sprintf(message, "Username: [%s] - Nome: [%c] - Team: [%c] - Time: [%s] - Punteggio: [%d] (Disconnesso)\n",username,nome,team,buffer,punteggio);

    else if(type == 3) // eliminazioni per MORTE
      sprintf(message, "Username: [%s] - Nome: [%c] - Team: [%c] - Time: [%s] - Punteggio: [%d] (Morto)\n",username,nome,team,buffer,punteggio);

    else if(type == 4) // flag
      sprintf(message, "Bandiera presa da:\nUsername: [%s] - Nome: [%c] - Team: [%c] - Time: [%s]\n",username,nome,team,buffer);

    if( (write(fp,message,strlen(message))) < 0 )
    {
      perror("Error Write on LoginClients.txt");
      exit(-1);
    }

}

void addPlayer(Player P[],int X,int Y,char nome,int team,char username[], int *n_players, int sd)
{
         strcpy(P[*n_players].username,username);
         P[*n_players].nome=nome;
         P[*n_players].team=team;
         P[*n_players].X=X;
         P[*n_players].Y=Y;
         P[*n_players].punteggio=5000;
         P[*n_players].indice_player=sd;
         (*n_players)++;

}

void liberaNome(char nome,char nomi[])
{

  if(nome>='A' && nome <= 'Z')
    nomi[nome-65]=nome;
  else
    nomi[nome-71]=nome;

}

void freePlayer(Player P[],int player_eliminato, int *n_players,int fp, char nomi_verdi[],char nomi_rossi[])
{
    int i,indice_temp,i_temp;

    for(i=0; i<*n_players;i++)
      if(P[i].indice_player == player_eliminato)
      {
          indice_temp=P[i].indice_player;
          i_temp=i;
      }

      if(P[i_temp].team == 'V')
        liberaNome(P[i_temp].nome, nomi_verdi);
      else
        liberaNome(P[i_temp].nome, nomi_rossi);

      if(P[i_temp].punteggio>0)
      {
        write_on_file(fp,2,P[i_temp].username, P[i_temp].nome,P[i_temp].team,P[i_temp].punteggio,0);
      //  fprintf(stdout, "\n* Player => P[%d] | Username [%s] : Eliminato per DISCONNESSIONE *\n",i_temp,P[i_temp].username);
      }
      else
      {
        write_on_file(fp,3,P[i_temp].username, P[i_temp].nome,P[i_temp].team,P[i_temp].punteggio,0);
    //    fprintf(stdout, "\n* Player => P[%d] | Username [%s] : Eliminato per MORTE *\n",i_temp,P[i_temp].username);
      }

  //  fprintf(stdout, "\nindice_temp=%d && indice_i = %d",indice_temp,i_temp);

    (*n_players)--;

    for(i=i_temp+1;i<*n_players+2;i++)
    {
          P[i_temp].nome=P[i].nome;
          P[i_temp].team=P[i].team;
          P[i_temp].X=P[i].X;
          P[i_temp].Y=P[i].Y;
          P[i_temp].punteggio=P[i].punteggio;
          strcpy(P[i_temp].username,P[i].username);

          if(i!=*n_players+1)
             P[i_temp].indice_player=P[i].indice_player;


            i_temp=i;
    }
}

void stampaPlayers(Player P[] , int n_players)
{
      int i;

      for(i=0;i<n_players;i++)
      {
        printf("\n[P%d]: USERNAME=[%s] | ",i+1,P[i].username);
        printf(COLOR_RESET "NOME=[%c] | ",P[i].nome);
        printf(COLOR_RESET "TEAM=[%c] | ",P[i].team);
        printf(COLOR_RESET "PUNTEGGIO= [%d] | ",P[i].punteggio);
        printf(COLOR_RESET "X=[%d] | Y=[%d] | ",P[i].X, P[i].Y);
        printf(COLOR_RESET "INDICE=[%d] |",P[i].indice_player);
     }
       printf(COLOR_RESET "\n");

}





int spawnPlayer(int C_X , int C_Y, char nome,char** Mappa,int TOT,Player P[])
{
    int i;

     char x_char=C_X+'0';
      char y_char=C_Y+'0';



      for(i=0;i<TOT;i++)
      {
              if(Mappa[i][0]==OSTACOLO)
              {
                   if(Mappa[i][1]==x_char &&Mappa[i][2]==y_char)
                       return 0;
              }
      }

  return 1;

}

char** allocaMatrice(int n)
{
    int i;
    char **M=malloc(n*sizeof(char*));

    for(i=0;i<n;i++)
      M[i]=malloc(4*sizeof(char));

//      fprintf(stdout, "\nMatrice Allocata");

    return M;
}



char** GeneraMatrice(int* n_o,int* n_a,int* n_m,int* TOT)
{
     *n_o=(rand()%31)+30;     //da 30 a 60 ostacoli
     *n_a=(rand()%31)+30;     //da 30 a 60 armi
     *n_m=(rand()%51)+50;      //da 50 a 100 mine
     *TOT=(*n_o)+(*n_a)+(*n_m)+2;
     int indice_matrice=0;
  //   fprintf(stdout, "\nPrima di Alloca");
     char** M=allocaMatrice(*TOT);


      generateObstacles(M,*n_o,&indice_matrice);
      generateWeapons(M,*n_a,&indice_matrice);
      generateMines(M,*n_m,&indice_matrice);
      generateFlags(M,*TOT);

  //    fprintf(stdout, "\nDopo i generate");
      return M;

}


void generateObstacles(char **Mappa,int n_ostacoli,int *indice)
{
  int i,C_X,C_Y;
  char C_X_c,C_Y_c;

          for(i=(*indice);i<n_ostacoli;i++)
          {

                 C_X=(rand()%27)+1;
                 C_Y=(rand()%78)+1;

                 if(C_X==11&&(C_Y==8||C_Y==9||C_Y==10))      //sposto eventuali ostacoli spawnati in cima alla barriera di sinistra, in su
                      C_X=C_X-1;

                      //sposto eventuali ostacoli spawnati a destra alla barriera di sinistra,a destra
                 if(C_Y==10&&(C_X==12||C_X==13||C_X==14||C_X==15||C_X==16||C_X==17||C_X==18))
                      C_Y=C_Y+1;

                 if(C_X==19&&(C_Y==8||C_Y==9||C_Y==10))    //sposto eventuali ostacoli spawnati in basso alla barriera di sinistra,in basso
                      C_X=C_X+1;

                      //sposto eventuali ostacoli spawnati nella colonna della bandiera di sinistra due posti a destra
                 if(C_Y==9&&(C_X==12||C_X==13||C_X==14||C_X==15||C_X==16||C_X==17||C_X==18))
                     C_Y=C_Y+2;



                 if(C_X==11&&(C_Y==69||C_Y==70||C_Y==71))      //sposto eventuali ostacoli spawnati in cima alla barriera di destra, in su
                      C_X=C_X-1;

                      //sposto eventuali ostacoli spawnati a destra alla barriera di destra,a sinistra
                 if(C_Y==69&&(C_X==12||C_X==13||C_X==14||C_X==15||C_X==16||C_X==17||C_X==18))
                      C_Y=C_Y-1;

                 if(C_X==19&&(C_Y==69||C_Y==70||C_Y==71))    //sposto eventuali ostacoli spawnati in basso alla barriera di destra,in basso
                      C_X=C_X+1;

                      //sposto eventuali ostacoli spawnati nella colonna della bandiera di destra due posti a sinistra
                  if(C_Y==70&&(C_X==12||C_X==13||C_X==14||C_X==15||C_X==16||C_X==17||C_X==18))
                       C_Y=C_Y-2;



              C_X_c='0'+C_X;
              C_Y_c='0'+C_Y;

            //  printf("\n [X] C_X=%d -> %c | C_Y=%d -> %c", C_X,C_X_c,C_Y,C_Y_c);

              Mappa[i][0]=OSTACOLO;
              Mappa[i][1]=C_X_c;
              Mappa[i][2]=C_Y_c;
              Mappa[i][3]='V';
          }


                    *indice=i;
}

void generateWeapons(char **Mappa,int n_weapons,int* indice)
{
  //printf("indice2=%d\n",*indice);
  //  printf("n_weapins=%d\n",n_weapons);
  int i,C_X,C_Y;
  char C_X_c,C_Y_c;

  for(i=(*indice);i<n_weapons+(*indice);i++)
         {
             C_X=(rand()%27)+1;
             C_Y=(rand()%78)+1;


              if(C_X==11&&(C_Y==8||C_Y==9||C_Y==10))      //sposto eventuali armi spawnati in cima alla barriera di sinistra, in su
                     C_X=C_X-1;

                     //sposto eventuali armi spawnati a destra alla barriera di sinistra,a destra
                if(C_Y==10&&(C_X==12||C_X==13||C_X==14||C_X==15||C_X==16||C_X==17||C_X==18))
                     C_Y=C_Y+1;

                if(C_X==19&&(C_Y==8||C_Y==9||C_Y==10))    //sposto eventuali armi spawnati in basso alla barriera di sinistra,in basso
                     C_X=C_X+1;



                if(C_X==11&&(C_Y==69||C_Y==70||C_Y==71))      //sposto eventuali ostacoli spawnati in cima alla barriera di destra, in su
                     C_X=C_X-1;

                     //sposto eventuali ostacoli spawnati a destra alla barriera di destra,a sinistra
                if(C_Y==69&&(C_X==12||C_X==13||C_X==14||C_X==15||C_X==16||C_X==17||C_X==18))
                     C_Y=C_Y-1;

                if(C_X==19&&(C_Y==69||C_Y==70||C_Y==71))    //sposto eventuali ostacoli spawnati in basso alla barriera di destra,in basso
                     C_X=C_X+1;

             C_X_c='0'+C_X;
             C_Y_c='0'+C_Y;

          //   printf("\n [>] C_X=%d -> %c | C_Y=%d -> %c", C_X,C_X_c,C_Y,C_Y_c);

             Mappa[i][0]=ARMA;
             Mappa[i][1]=C_X_c;
             Mappa[i][2]=C_Y_c;
             Mappa[i][3]='V';
         }

      *indice=i;
}



void generateMines(char **Mappa,int n_mines,int *indice)
{

//  printf("indice3=%d\n",*indice);
//printf("n_mine=%d\n",n_mines);
int i,C_X,C_Y;
char C_X_c,C_Y_c;

for(i=(*indice);i<n_mines+(*indice);i++)
      {
          C_X=(rand()%27)+1;
          C_Y=(rand()%78)+1;

           if(C_X==11&&(C_Y==8||C_Y==9||C_Y==10))      //sposto eventuali mine spawnati in cima alla barriera di sinistra, in su
                  C_X=C_X-1;

                  //sposto eventuali mine spawnati a destra alla barriera di sinistra,a destra
             if(C_Y==10&&(C_X==12||C_X==13||C_X==14||C_X==15||C_X==16||C_X==17||C_X==18))
                  C_Y=C_Y+1;

             if(C_X==19&&(C_Y==8||C_Y==9||C_Y==10))    //sposto eventuali mine spawnati in basso alla barriera di sinistra,in basso
                  C_X=C_X+1;


             if(C_X==11&&(C_Y==69||C_Y==70||C_Y==71))      //sposto eventuali ostacoli spawnati in cima alla barriera di destra, in su
                  C_X=C_X-1;

                  //sposto eventuali ostacoli spawnati a destra alla barriera di destra,a sinistra
             if(C_Y==69&&(C_X==12||C_X==13||C_X==14||C_X==15||C_X==16||C_X==17||C_X==18))
                  C_Y=C_Y-1;

             if(C_X==19&&(C_Y==69||C_Y==70||C_Y==71))    //sposto eventuali ostacoli spawnati in basso alla barriera di destra,in basso
                  C_X=C_X+1;

          C_X_c='0'+C_X;
          C_Y_c='0'+C_Y;
      //    printf("\n [*] C_X=%d -> %c | C_Y=%d -> %c", C_X,C_X_c,C_Y,C_Y_c);
          Mappa[i][0]=MINA;
          Mappa[i][1]=C_X_c;
          Mappa[i][2]=C_Y_c;
          Mappa[i][3]='V';
      }

   *indice=i;

}

void generateFlags(char **Mappa,int Tot)
{

  int C_X,C_Y;
   char C_X_c,C_Y_c;

    C_X=(rand()%7)+12;
    C_Y=9;                  //da 5 a 9

     C_X_c='0'+C_X;
     C_Y_c='0'+C_Y;
    // printf("\n [F] C_X=%d -> %c | C_Y=%d -> %c", C_X,C_X_c,C_Y,C_Y_c);
    Mappa[Tot-2][0]=FLAG;
    Mappa[Tot-2][1]=C_X_c;
    Mappa[Tot-2][2]=C_Y_c;
    Mappa[Tot-2][3]='V';


    C_X=(rand()%7)+12;
    C_Y=70;                 //da 70 a 74

  //  printf("\n [F] C_X=%d -> %c | C_Y=%d -> %c", C_X,C_X_c,C_Y,C_Y_c);
    C_X_c='0'+C_X;
    C_Y_c='0'+C_Y;

    Mappa[Tot-1][0]=FLAG;
    Mappa[Tot-1][1]=C_X_c;
    Mappa[Tot-1][2]=C_Y_c;
    Mappa[Tot-1][3]='V';

}

void clearBuffer(){
    char c;
    while ((c = getchar()) != '\n' && c != EOF) { };
}


int SendData(char **Matrice, int TOT_OBJECTS,int sd)
{
    int TOT_OBJECTS_conv=0;

    TOT_OBJECTS_conv=htonl(TOT_OBJECTS);

  //  fprintf(stdout, "\nINVIO %d", TOT_OBJECTS);

    if( (send(sd, (const char*)&TOT_OBJECTS_conv, 4, 0)) < 0 )
    {
      perror("Error Write TOT_OBJECTS_conv");
      return 1;
    }

    int i,j;

    for(i=0;i<TOT_OBJECTS;i++)
      for(j=0;j<4;j++)
          if( (write(sd, &Matrice[i][j], 1)) < 0 )
          {
            perror("Error Write Carattere Matrice");
            return 1;
          }

    return 0;

}



int readRequestMovementClient(int sd,char **Mappa, Player P[],int n_players, int TOT_OBJECTS, int *n_oggetti,int *fine_game,int fp)
{
    char spostamento,username[20];
    int nbytes, check = 0;

    if( (nbytes=read(sd, username, 20)) < 0)
    {
      perror("\nError username readRequestMovementClient()");
      return 2;
    }

    username[nbytes]='\0';

    if( (read(sd,&spostamento,1)) < 0 )
    {
      perror("\nError spostamento readRequestMovementClient()");
      return 2;
    }

    if(spostamento != 'm')
    {
  //  fprintf(stdout, "\nLetta Richiesta di spostamento da [%s] in [%c]",username, spostamento);

    check=check_movement_client(username,sd,Mappa,P,n_players,spostamento,TOT_OBJECTS,n_oggetti,fine_game,fp);
  //  stampaPlayers(P,n_players);
   }

  return check;


}

int find_player_by_username(Player P[], char username[], int n_players, int *X , int *Y , int *indice)
{
  int i;

    for(i=0; i<n_players; i++)
    {
           if(strcmp(P[i].username,username)==0)
           {
              *X=P[i].X;
              *Y=P[i].Y;
              *indice=i;
              return 1;
           }
    }

    return 0;
}

void find_and_destroy_object(char **Mappa, int X, int Y, int TOT_OBJECTS, char OBJECT,int *X_O)
{
  int i;

//fprintf(stdout, "\nFIND AND DESTROY (%d)(%d)(%c)",X,Y,OBJECT);
    for(i=0;i<TOT_OBJECTS;i++)
    {
      //fprintf(stdout, "\n[%c][%d][%d][%c]", Mappa[i][0], Mappa[i][1] - '0', Mappa[i][2] - '0',Mappa[i][3]);
      if(decode_char(Mappa[i][1]) == X && decode_char(Mappa[i][2]) == Y && Mappa[i][0] == OBJECT)
        *X_O=i;

    }

}

int check_plaver_vs_player(Player P[], int n_players,int indice,char spostamento)
{
  int i;
    for(i=0;i<n_players;i++)
    {
      if(spostamento == 'a')
      {
       if(P[indice].X == P[i].X && P[indice].Y - 1  == P[i].Y)
        if(P[indice].team != P[i].team)
          if(P[indice].punteggio >= P[i].punteggio)
          {
            P[i].punteggio=P[i].punteggio - 200;

            if(P[i].team == 'V')
            {
             P[i].X=15;
             P[i].Y=8;
            }
            else
            {
              P[i].X=15;
              P[i].Y=71;
            }

            return 1;
          }
          else
          {
            P[indice].punteggio=P[indice].punteggio - 200;

            if(P[indice].team == 'V')
            {
             P[indice].X=15;
             P[indice].Y=8;
            }
            else
            {
              P[indice].X=15;
              P[indice].Y=71;
            }

            return 1;
          }
        }
        else if (spostamento == 'd')
        {
          if(P[indice].X == P[i].X && P[indice].Y + 1  == P[i].Y)
           if(P[indice].team != P[i].team)
             if(P[indice].punteggio >= P[i].punteggio)
             {
               P[i].punteggio=P[i].punteggio - 200;

               if(P[i].team == 'V')
               {
                P[i].X=15;
                P[i].Y=8;
               }
               else
               {
                 P[i].X=15;
                 P[i].Y=71;
               }

               return 1;
             }
             else
             {
               P[indice].punteggio=P[indice].punteggio - 200;

               if(P[indice].team == 'V')
               {
                P[indice].X=15;
                P[indice].Y=8;
               }
               else
               {
                 P[indice].X=15;
                 P[indice].Y=71;
               }

               return 1;
             }
        }
        else if (spostamento == 'w')
        {
          if(P[indice].X - 1 == P[i].X && P[indice].Y  == P[i].Y)
           if(P[indice].team != P[i].team)
             if(P[indice].punteggio >= P[i].punteggio)
             {
               P[i].punteggio=P[i].punteggio - 200;

               if(P[i].team == 'V')
               {
                P[i].X=15;
                P[i].Y=8;
               }
               else
               {
                 P[i].X=15;
                 P[i].Y=71;
               }

               return 1;
             }
             else
             {
               P[indice].punteggio=P[indice].punteggio - 200;

               if(P[indice].team == 'V')
               {
                P[indice].X=15;
                P[indice].Y=8;
               }
               else
               {
                 P[indice].X=15;
                 P[indice].Y=71;
               }

               return 1;
             }
        }
        else
        {
          if(P[indice].X + 1 == P[i].X && P[indice].Y  == P[i].Y)
           if(P[indice].team != P[i].team)
             if(P[indice].punteggio >= P[i].punteggio)
             {
               P[i].punteggio=P[i].punteggio - 200;

               if(P[i].team == 'V')
               {
                P[i].X=15;
                P[i].Y=8;
               }
               else
               {
                 P[i].X=15;
                 P[i].Y=71;
               }

               return 1;
             }
             else
             {
               P[indice].punteggio=P[indice].punteggio - 200;

               if(P[indice].team == 'V')
               {
                P[indice].X=15;
                P[indice].Y=8;
               }
               else
               {
                 P[indice].X=15;
                 P[indice].Y=71;
               }

               return 1;
             }
        }


    }
   return 0;
}


int check_movement_client(char username[], int sd, char ** Mappa, Player P[], int n_players, char spostamento , int TOT_OBJECTS, int *n_oggetti, int *fine_game, int fp)
{
  int X,Y,indice,X_flag,Y_flag,X_O;

      //trovo le cordinate del player mediante l'username
      if(!find_player_by_username(P,username,n_players,&X,&Y,&indice))
          return 0;

    //  fprintf(stdout, "\nTrovate Cordinate di %s = in P[%d][ %d , %d ]", username, indice, X,Y);

      char object;



     if(P[indice].team == 'V')
      {
          X_flag=decode_char(Mappa[TOT_OBJECTS-2][1]);
          Y_flag=decode_char(Mappa[TOT_OBJECTS-2][2]);
     }
      else
      {
          X_flag=decode_char(Mappa[TOT_OBJECTS-1][1]);
          Y_flag=decode_char(Mappa[TOT_OBJECTS-1][2]);
       }

      // printf("\nstampa team=%c - FLAG (%d,%d)\n",P[indice].team,X_flag,Y_flag);

  switch (spostamento) {

                case 'a':

                    if((P[indice].Y-1)==10 &&(P[indice].X==11 || P[indice].X==12|| P[indice].X==13|| P[indice].X==14|| P[indice].X==15 || P[indice].X==16|| P[indice].X==17|| P[indice].X==18|| P[indice].X==19) )
                      {
                          fprintf(stdout, "\nSPOSTAMENTO NON VALIDO_1\n");
                      }
                      else if(((P[indice].Y-1)==71 &&P[indice].X==19)||((P[indice].Y-1)==71 &&P[indice].X==11)||((P[indice].Y-1)==69&&(P[indice].X==12 ||P[indice].X==13 || P[indice].X==14 || P[indice].X==15 || P[indice].X==16 ||P[indice].X==17 ||P[indice].X==18)))
                      {
                          fprintf(stdout, "\nSPOSTAMENTO NON VALIDO_2\n");
                      }
                      else if ((P[indice].Y - 1 == Y_flag) && (P[indice].X == X_flag)) {
                          fprintf(stdout, "\nSPOSTAMENTO NON VALIDO SULLA FLAG\n");
                      }
                      else if(check_plaver_vs_player(P,n_players,indice,'a'))
                      {
                  //      puts("Collisione con Player");
                        P[indice].Y--;
                        P[indice].punteggio--;
                      }
                      else
                      {
                          if(P[indice].Y-1>0 )
                          {
                              object=searchData(Mappa,X,Y-1,TOT_OBJECTS);
                              //fprintf(stdout, "\nobject [%c]\n", object);

                                if(object != 'N')
                                {
                                //  fprintf(stdout, "\nsono entrato in X\n");
                                  if(object == MINA)
                                  {
                                    P[indice].Y -= 1;
                                    find_and_destroy_object(Mappa,P[indice].X,P[indice].Y,TOT_OBJECTS,MINA,&X_O);

                                    if(Mappa[X_O][3] != '+')
                                    {
                                      Mappa[X_O][3]='+';
                                      P[indice].punteggio -= 600;
                                      addObject(n_oggetti,P[indice].nome,P[indice].team,P[indice].username,P[indice].X,P[indice].Y,MINA);
                                    }
                                    else
                                      P[indice].punteggio--;

                                    return 1;
                                  }
                                  else if (object == ARMA)
                                  {
                                    P[indice].Y -= 1;


                                    find_and_destroy_object(Mappa,P[indice].X,P[indice].Y,TOT_OBJECTS,ARMA,&X_O);

                                    if(Mappa[X_O][3] != '+')
                                    {
                                    Mappa[X_O][3]='+';
                                    P[indice].punteggio += 300;
                                    addObject(n_oggetti,P[indice].nome,P[indice].team,P[indice].username,P[indice].X,P[indice].Y,ARMA);
                                    }
                                    else
                                      P[indice].punteggio--;

                                     return 1;
                                  }
                                  else if (object == FLAG)
                                  {
                                  if(P[indice].team == 'V')
                                    *fine_game=1;
                                  else
                                     *fine_game=2;

                                     P[indice].Y -= 1;
                                     addObject(n_oggetti,P[indice].nome,P[indice].team,P[indice].username,P[indice].X,P[indice].Y,FLAG);
                                     write_on_file(fp,4,P[indice].username,P[indice].nome,P[indice].team,P[indice].punteggio,0);
                                     return 0;
                                  }
                               }
                               else if (object == 'N')
                               {
                                 //fprintf(stdout, "\nsono entrato in N\n");
                                 P[indice].Y = P[indice].Y - 1;
                                 P[indice].punteggio--;
                                 return 1;
                               }
                          }
                      }

                     break;

            case 'd':

            if(((P[indice].Y+1)==8 &&P[indice].X==19)||((P[indice].Y+1)==8 &&P[indice].X==11)||((P[indice].Y+1)==10&&(P[indice].X==12 ||P[indice].X==13 || P[indice].X==14 || P[indice].X==15 || P[indice].X==16 ||P[indice].X==17 ||P[indice].X==18)))
               {
                      fprintf(stdout, "\nSPOSTAMENTO NON VALIDO_1\n");
               }
               else if((P[indice].Y+1)==69 &&(P[indice].X==11 || P[indice].X==12|| P[indice].X==13|| P[indice].X==14|| P[indice].X==15 || P[indice].X==16|| P[indice].X==17|| P[indice].X==18 || P[indice].X==19 ) )
              {
                        fprintf(stdout, "\nSPOSTAMENTO NON VALIDO_2\n");
              }
              else if ((P[indice].Y + 1 == Y_flag) && (P[indice].X == X_flag)) {
                  fprintf(stdout, "\nSPOSTAMENTO NON VALIDO SULLA FLAG\n");
              }
              else if(check_plaver_vs_player(P,n_players,indice,'d'))
              {
                puts("Collisione con Player");
                P[indice].Y++;
                P[indice].punteggio--;
              }
              else
              {
                      if(P[indice].Y+1<79)
                      {
                          object=searchData(Mappa,X,Y+1,TOT_OBJECTS);
                        //  fprintf(stdout, "\nobject [%c]\n", object);

                        if(object != 'N')
                        {
                      //    fprintf(stdout, "\nsono entrato in X\n");
                          if(object == MINA)
                          {
                            P[indice].Y += 1;

                            find_and_destroy_object(Mappa,P[indice].X,P[indice].Y,TOT_OBJECTS,MINA,&X_O);

                            if(Mappa[X_O][3] != '+')
                            {
                              Mappa[X_O][3]='+';
                              P[indice].punteggio -= 600;
                              addObject(n_oggetti,P[indice].nome,P[indice].team,P[indice].username,P[indice].X,P[indice].Y,MINA);
                            }
                            else
                              P[indice].punteggio--;

                            return 1;
                          }
                          else if (object == ARMA)
                          {
                            P[indice].Y += 1;

                            find_and_destroy_object(Mappa,P[indice].X,P[indice].Y,TOT_OBJECTS,ARMA,&X_O);

                            if(Mappa[X_O][3] != '+')
                            {
                            Mappa[X_O][3]='+';
                            P[indice].punteggio += 300;
                            addObject(n_oggetti,P[indice].nome,P[indice].team,P[indice].username,P[indice].X,P[indice].Y,ARMA);
                            }
                            else
                              P[indice].punteggio--;

                             return 1;
                          }
                          else if (object == FLAG)
                          {
                            if(P[indice].team == 'V')
                              *fine_game=1;
                            else
                               *fine_game=2;

                              P[indice].Y += 1;
                             addObject(n_oggetti,P[indice].nome,P[indice].team,P[indice].username,P[indice].X,P[indice].Y,FLAG);
                             write_on_file(fp,4,P[indice].username,P[indice].nome,P[indice].team,P[indice].punteggio,0);
                             return 0;
                          }
                       }
                       else if (object == 'N')
                       {
                        // fprintf(stdout, "\nsono entrato in N\n");
                         P[indice].Y = P[indice].Y + 1;
                         P[indice].punteggio--;
                         return 1;
                       }
                      }
              }

               break;



                 case 'w':

            //     if(  (P[indice].X==20&&(P[indice].Y==8 ||P[indice].Y==9||P[indice].Y==10) ) || (P[indice].X==12&&(P[indice].Y==8 ||P[indice].Y==9) )   )
               if(  (P[indice].X==20&&(P[indice].Y==8 ||P[indice].Y==9||P[indice].Y==10 ||P[indice].Y==69 ||P[indice].Y==70||P[indice].Y==71 ) ) || (P[indice].X==12&&(P[indice].Y==8 ||P[indice].Y==9||P[indice].Y==70 ||P[indice].Y==71) )   )
                 {
                     fprintf(stdout, "SPOSTAMENTO NON VALIDO\n");
                 }
                 else if ((P[indice].Y == Y_flag) && (P[indice].X - 1 == X_flag)) {
                     fprintf(stdout, "\nSPOSTAMENTO NON VALIDO SULLA FLAG\n");
                 }
                 else if(check_plaver_vs_player(P,n_players,indice,'w'))
                 {
                   //puts("Collisione con Player");
                   P[indice].X--;
                   P[indice].punteggio--;
                 }
                 else{

                     if(P[indice].X-1>0)
                     {
                     object=searchData(Mappa,X-1,Y,TOT_OBJECTS);
                    // fprintf(stdout, "\nobject [%c]\n", object);

                   if(object != 'N')
                   {

                     if(object == MINA)
                     {
                       P[indice].X -= 1;

                       find_and_destroy_object(Mappa,P[indice].X,P[indice].Y,TOT_OBJECTS,MINA,&X_O);


                       if(Mappa[X_O][3] != '+')
                       {
                         Mappa[X_O][3]='+';
                         P[indice].punteggio -= 600;
                         addObject(n_oggetti,P[indice].nome,P[indice].team,P[indice].username,P[indice].X,P[indice].Y,MINA);
                       }
                       else
                         P[indice].punteggio--;

                       return 1;
                     }
                     else if (object == ARMA)
                     {
                       P[indice].X -= 1;


                       find_and_destroy_object(Mappa,P[indice].X,P[indice].Y,TOT_OBJECTS,ARMA,&X_O);

                       if(Mappa[X_O][3] != '+')
                       {
                       Mappa[X_O][3]='+';
                       P[indice].punteggio += 300;
                       addObject(n_oggetti,P[indice].nome,P[indice].team,P[indice].username,P[indice].X,P[indice].Y,ARMA);
                       }
                       else
                         P[indice].punteggio--;

                        return 1;
                     }
                     else if (object == FLAG)
                     {
                       if(P[indice].team == 'V')
                         *fine_game=1;
                       else
                          *fine_game=2;

                        P[indice].X -= 1;
                        write_on_file(fp,4,P[indice].username,P[indice].nome,P[indice].team,P[indice].punteggio,0);
                        addObject(n_oggetti,P[indice].nome,P[indice].team,P[indice].username,P[indice].X,P[indice].Y,FLAG);
                        return 0;
                     }
                  }
                  else if (object == 'N')
                  {
                  //  fprintf(stdout, "\nsono entrato in N\n");
                    P[indice].X = P[indice].X - 1;
                    P[indice].punteggio--;
                    return 1;
                  }
              }
          }

                    break;

                    case 's':
                  //  if(  (P[indice].X==10&&(P[indice].Y==8 ||P[indice].Y==9 ||P[indice].Y==10 )) || (P[indice].X==18&&(P[indice].Y==8 ||P[indice].Y==9 )))
                 if(  (P[indice].X==10&&(P[indice].Y==8 ||P[indice].Y==9 ||P[indice].Y==10|| P[indice].Y==69 ||P[indice].Y==70 ||P[indice].Y==71)) || (P[indice].X==18&&(P[indice].Y==8 ||P[indice].Y==9 ||P[indice].Y==70 ||P[indice].Y==71)))
                     {
                         printf("SPOSTAMENTO NON VALIDO\n");
                     }
                     else if ((P[indice].Y == Y_flag) && (P[indice].X + 1 == X_flag)) {
                         fprintf(stdout, "\nSPOSTAMENTO NON VALIDO SULLA FLAG\n");
                     }
                     else if(check_plaver_vs_player(P,n_players,indice,'s'))
                     {
                      // puts("Collisione con Player");
                       P[indice].X++;
                       P[indice].punteggio--;
                     }
                     else
                     {
                     if(P[indice].X+1<29)
                     {
                         object=searchData(Mappa,X+1,Y,TOT_OBJECTS);
                         //fprintf(stdout, "\nobject [%c]\n", object);

                       if(object != 'N')
                       {
                        // fprintf(stdout, "\nsono entrato in X\n");

                         if(object == MINA)
                         {
                           P[indice].X += 1;

                           find_and_destroy_object(Mappa,P[indice].X,P[indice].Y,TOT_OBJECTS,MINA,&X_O);

                           if(Mappa[X_O][3] != '+')
                           {
                             Mappa[X_O][3]='+';
                             P[indice].punteggio -= 600;
                             //write_on_file(fp,3,P[indice].username,P[indice].nome,P[indice].team,P[indice].punteggio,0);
                             addObject(n_oggetti,P[indice].nome,P[indice].team,P[indice].username,P[indice].X,P[indice].Y,MINA);
                           }
                           else
                             P[indice].punteggio--;


                           return 1;
                         }
                         else if (object == ARMA)
                         {
                           P[indice].X += 1;

                           find_and_destroy_object(Mappa,P[indice].X,P[indice].Y,TOT_OBJECTS,ARMA,&X_O);

                           if(Mappa[X_O][3] != '+')
                           {
                           Mappa[X_O][3]='+';
                           P[indice].punteggio += 300;
                           addObject(n_oggetti,P[indice].nome,P[indice].team,P[indice].username,P[indice].X,P[indice].Y,ARMA);
                           }
                           else
                             P[indice].punteggio--;

                            return 1;
                         }
                         else if (object == FLAG)
                         {
                           if(P[indice].team == 'V')
                             *fine_game=1;
                           else
                              *fine_game=2;

                            P[indice].X += 1;
                            write_on_file(fp,4,P[indice].username,P[indice].nome,P[indice].team,P[indice].punteggio,0);
                            addObject(n_oggetti,P[indice].nome,P[indice].team,P[indice].username,P[indice].X,P[indice].Y,FLAG);
                            return 0;
                         }
                      }
                      else if (object == 'N')
                      {
                      //  fprintf(stdout, "\nsono entrato in N\n");
                        P[indice].X = P[indice].X + 1;
                        P[indice].punteggio--;
                        return 1;
                    }
                }
            }


                   break;
          }

}

char searchData(char **M, int X , int Y, int TOT)
{
    int i;

      for(i=0;i<TOT;i++)
          if((decode_char(M[i][1]) == X)  && (decode_char(M[i][2]) == Y))
          {
          //  fprintf(stdout, "\nCollisione con [%c]", M[i][0] );
            return M[i][0];
          }

    return 'N';
}

void addObject(int* n_oggetti,char nome_p,char team,char username[],int X,int Y,char nome_o)
{
      strcpy(O[*n_oggetti].username,username);
      O[*n_oggetti].nome_p=nome_p;
      O[*n_oggetti].nome_o=nome_o;
      O[*n_oggetti].team=team;
      O[*n_oggetti].X=X;
      O[*n_oggetti].Y=Y;

      (*n_oggetti)++;

}

int decode_char(char c)
{
    int n=c-'0';

      return n;
}

int read_Request_sendObjectDestroyed(int sd,int n_oggetti)
{
  char ch;
  int check=0;

  if( (read(sd,&ch,1)) < 0 )
  {
    perror("Error read read_Request_sendObjectDestroyed");
    return 1;
  }

  if(ch == 'Y')
    check=sendObjectDestroyed(sd,n_oggetti);

  return check;
}

int sendObjectDestroyed(int sd, int n_oggetti)
{
    char team  , temp[4] , n_letture_c;
    int i, n_letture=0;


    if( (read(sd,&team,1)) < 0 )
    {
      perror("Error read team sendObjectDestryed()");
      return 1;
    }

    for(i=0;i<n_oggetti;i++)
      if(team==O[i].team)
          n_letture++;

    n_letture_c=n_letture +'0';

    if( (write(sd,&n_letture_c,1)) < 0 )
    {
      perror("Error send n_letture in sendObjectDestryed");
      return 1;
    }


    for(i=0;i<n_oggetti;i++)
    {
      if(team == O[i].team)
      {

          if( ((write(sd,O[i].username, 20)) < 0 ))
          {
            perror("Error write username in sendObjectDestroyed");
            return 1;

          }

          temp[0]=O[i].nome_o;
          temp[1]=O[i].X + '0';
          temp[2]=O[i].Y + '0';
          temp[3]=O[i].nome_p;

          if( ((write(sd,temp,4)) < 0 ))
          {
            perror("Error write temp in sendObjectDestroyed");
            return 1;
          }

      }
    }

    return 0;
}

int sendPlayers(Player P[], int n_players, int sd)
{
    int i;
    char  X_c, Y_c , n_players_c,sd_temp,punteggio_string[10];
    char temp[5];

    n_players_c=n_players + '0';

  //  fprintf(stdout, "\nSto inviando [%d] ==> [%c]", n_players , n_players_c );

    if( (write(sd, &n_players_c, 1)) < 0 )
    {
      perror("Error Write n_players in sendPlayers");
      return 1;
    }


    for(i=0;i<n_players;i++)
    {

         X_c= P[i].X + '0';
         Y_c= P[i].Y + '0';
         sd_temp=P[i].indice_player + '0';

            temp[0]=P[i].nome;
            temp[1]=P[i].team;
            temp[2]=X_c;
            temp[3]=Y_c;
            temp[4]=sd_temp;

      //fprintf(stdout, "\nInvio nome=[%c] | squadra=[%c] | X=[%c] Y=[%c] | INDICE = [%c]\n", temp[0] , temp[1] , temp[2] , temp[3], temp[4]);


           if( (write(sd,temp,5)) < 0 )
           {
             perror("Error Write data in sendPlayers()");
             return 1;
           }

    //       puts("STO PROVANDO A MANDARE L'USERNAME");
          if( (write(sd,P[i].username,20)) < 0 )
           {
             perror("Error Write username in sendPlayers");
             return 1;
           }
      //     puts("HO MANDATO L'USERNAME");

          sprintf(punteggio_string,"%d",P[i].punteggio);

          if( (write(sd,punteggio_string,10)) < 0 )
          {
            perror("Error Write punteggio_string in sendPlayers");
            return 1;
          }


       }

    return 0;

}

int send_fine_game(int sd,int fine_game,Player P[],int* n_players,int fp, char nomi_verdi[], char nomi_rossi[])
{
  if(fine_game==1)
  {
    fprintf(stdout, "\n\n\nFINE PARTITA!");
    if( (write(sd,"F",1)) < 0 )
    {
      fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
      perror("Error send fine_game Server.c");
    }

    freePlayer(P,sd,n_players,fp,nomi_verdi,nomi_rossi);
    close(sd);
    return 1;
  }
  else if(fine_game==2)
  {
    if( (write(sd,"B",1)) < 0 )
    {
      fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
      perror("Error send fine_game Server.c");
    }

    freePlayer(P,sd,n_players,fp,nomi_verdi,nomi_rossi);
    close(sd);
    return 1;
  }
  if(fine_game==3)
  {
    if( (write(sd,"P",1)) < 0 )
    {
      fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
      perror("Error send fine_game Server.c");
    }

    freePlayer(P,sd,n_players,fp,nomi_verdi,nomi_rossi);
    close(sd);
    return 1;
  }
  else
  {
    if( ((write(sd,"Q",1)) < 0 ))
    {
      perror("Error send fine_game Server.c");
      fprintf(stdout, "\nClient_Socket[%d] Disconnect\n", sd);
      freePlayer(P,sd,n_players,fp,nomi_verdi,nomi_rossi);
      close(sd);

    }
    return 0;
  }
}
