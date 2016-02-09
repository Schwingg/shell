#include "csapp.h"
#define MAXARGS 128


typedef struct job{
    // 0: arreté
    // 1: foreground
    // 2: background
    int etat; 
    char* commande;
    int pid;

}job;

job* tabJobs[10];



