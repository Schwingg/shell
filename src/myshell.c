#include "myshell.h"

// fonctions externes
void eval(char*cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);


void handler(int sig){
    printf("dans le handler %d!\n", sig);
     
    return;
}



int main()
{
    int i=0;
    for(i=0;i<10;i++){
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
