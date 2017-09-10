#ifndef funzioni_client_h
#define funzioni_client_h


struct Player               //da aggiungere al progetto
{
  char username[20];
  char nome;
  int X;
  int Y;
  int punteggio;
  char team;
  int indice_player;
};
struct Player P[104];               //CLIENT

void Logo();
char requestName(int sd);
int find_me(int n_players, int indice);
int check_my_punteggio(int sd , int io);
int getch();
void getpasshide(char *password);
int loginClient(int,char username[]);
void StampaObjects(char username[],char nome_p,char nome_o,int X,int Y);
void receiveObjectDestroyed(int sd, char team);
void riepilogoDati(char nome, int team, int X, int Y);
void request_receive_objects_destroyed(int sd,int richiesta,char team_char);
int Menu(char** M, int* n_players, int io, int sd , char username[]);
void Pulisci();
void addPlayer(int X,int Y,char nome,char* username,int team,int punteggio,int indice,int i);
void StampaPunteggio(int io);
void stampaPlayers(char** M, int n_players, int io);
void stampaCompagni(char** M,char team, int n_players, int io) ;
int teamSelection(int);
int requestToSpawn(int *, int *, int , int,char**);
void RequestMovementClient(int sd, char spostamento , char username[]);
void receivePlayers(int sd , char **Mappa, int *n_players);
char** allocaMappa();
char** GeneraMappa();
void stampaMappa(char **M, int n_players, int io);
int ReceiveData(char **Matrice, int *TOT_OBJECTS,int sd);
void clearBuffer();
int receive_fine_game(int sd);
#endif
