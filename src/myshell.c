#include "myshell.h"

// fonctions externes
void eval(char*cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);


void handler(int sig){
    printf("dans le handler %d!\n", sig);
    int pid = getPidFG();
    if(pid != -1){
        kill(SIGTSTP, pid );
        printf("processus mis en bg");
    }
    else
        printf("Erreur: pas de processus en foreground");

    return;
}



int main()
{
    int i=0;
    for(i=0;i<MAXJOBS;i++){
        tabJobs[i] = NULL;
    }
    
    
    char cmdline[MAXLINE];                  // ligne de commande
    Signal(SIGTSTP, handler);
    while (1) {                            // boucle d'interpretation
        printf("<my_shell> ");             // message d'invite
        Fgets(cmdline, MAXLINE, stdin);    // lire commande
        if (feof(stdin))                   // fin (control-D)
            exit(0);
        eval(cmdline);                     // interpreter commande
    }
}
