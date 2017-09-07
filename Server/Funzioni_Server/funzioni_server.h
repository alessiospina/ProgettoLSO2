#ifndef funzioni_server_h
#define funzioni_server_h

struct TPlayer
{
  char username[20];
  char nome;
  int X;
  int Y;
  char team;
  int punteggio;
  int indice_player;
};

typedef struct TPlayer Player;

struct Objects
{
 char username[20];
 char nome_p;
 char nome_o;
 char team;
 int X;
 int Y;
};
 struct Objects O[230];



int verify_client_registration(char *username, char* password);
int do_registration(char *username, char* password);
int login_state(int sd, char username[], Player P[], int n_players);
int loginServer(int,char username[], char password[]);
void openFile(int fp[]);
void riempiNomi(char nomi_rossi[],char nomi_verdi[]);
void clearBuffer();
char** allocaMatrice(int n);
char** GeneraMatrice(int* n_o,int* n_a,int* n_m,int* TOT);
void write_on_file(int fp , int type, char *username,char nome, char team,int punteggio,int count_partite);
char giveNameClient(int sd, char nomi[]);
void generateObstacles(char **Mappa,int n_ostacoli,int *indice);
void generateWeapons(char **Mappa, int n_armi, int* indice);
void generateMines(char **Mappa, int n_mine, int *indice);
void generateFlags(char **Mappa, int tot);
int check_username( char username[] , Player P[] , int n_players);
void addObject(int* n_oggetti,char nome_p,char team,char username[],int X,int Y,char nome_o);
int sendObjectDestroyed(int sd, int n_oggetti);
int read_Request_sendObjectDestroyed(int sd,int n_oggetti);
char leggiTeam(int);
int leggiRequestSpawn(int *, int *, int ,char nome,char **Mappa, int TOT_OBJECTS, Player P[]);
int spawnPlayer(int C_X , int C_Y, char nome,char** Mappa,int TOT,Player P[]);
void addPlayer(Player P[],int X,int Y,char nome,int team,char username[], int* n_players, int sd);
void freePlayer(Player P[],int player_eliminato, int *n_players,int fp, char nomi_verdi[], char nomi_rossi[] );
void stampaPlayers(Player P[] , int n_players);
int SendData(char **Matrice, int TOT_OBJECTS,int sd);
int sendPlayers(Player P[], int n_players, int sd);
int readRequestMovementClient(int sd,char **Mappa, Player P[],int n_players, int TOT_OBJECTS, int *n_oggetti, int* fine_game,int fp);
int find_player_by_username(Player P[], char username[], int n_players, int *X , int *Y , int *indice);
int check_movement_client(char username[], int sd, char ** Mappa, Player P[], int n_players, char spostamento,int TOT_OBJECTS,int *n_oggetti,int *fine_game,int fp);
char searchData(char **M, int X , int Y, int TOT);
int decode_char(char c);
int send_fine_game(int sd,int fine_game,Player P[], int *n_players,int fp, char nomi_verdi[], char nomi_rossi[]);
void find_and_destroy_object(char **Mappa, int X, int Y, int TOT_OBJECTS, char OBJECT,int *X_O);

#endif
