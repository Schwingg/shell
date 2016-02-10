#include "csapp.h"
#define MAXARGS 128
#define MAXJOBS 10

enum etat {PAUSE, FG, BG};

int getIndexLibre();
int getIndexFG();

void afficheJob(int index);

typedef struct job{

    enum etat etat; 
    char* commande;
    int pid;

}job;

job* tabJobs[MAXJOBS];



