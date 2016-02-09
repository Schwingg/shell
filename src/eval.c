/* eval : interprete une ligne de commande passee en parametre       */
#include "myshell.h"

// fonctions externes
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

int getIndexLibre(){
    int i=0;
    for(i=0;i<10;i++)
        if(tabJobs[i] == NULL)
            return i;
    return -1;
}


void eval(char *cmdline)
{
    char *argv[MAXARGS]; // argv pour execve()
    char buf[MAXLINE];   // contient ligne commande modifiee
    int bg;              // arriere-plan ou premier plan ?
    pid_t pid;           // process id
    
    int indexJobLibre;
    
    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL)
        return;                      // ignorer lignes vides

    if (!builtin_command(argv)) {    // commande integree ?
        // si oui, executee directement
        if ((pid = Fork()) == 0) {      // si non, executee par un fils
            
            indexJobLibre = getIndexLibre();
                    //malloc(sizeof(job));
            printf("indexmachin = %d\n",indexJobLibre);
            
            if (execve(argv[0], argv, environ) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }

        if (!bg) { // le pere attend fin du travail de premier plan
            int status;
            printf("dans !bg\n");
            if (waitpid(pid, &status, 0) < 0)
                unix_error("waitfg: waitpid error");
            printf("fin attente pid\n");
            
        }
        else       // travail d'arriere-plan, on imprime le pid
            printf("%d %s", pid, cmdline);
    }
    return;
}

// si le premier parametre est une commande integree,
// l'executer et renvoyer "vrai"
int builtin_command(char **argv)
{
    if (!strcmp(argv[0], "quit")) // commande "quitter"
        exit(0);
    if (!strcmp(argv[0], "&"))    // ignorer & tout seul
        return 1;
    
    if (!strcmp(argv[0], "jobs")){    // ignorer & tout seul
        
        printf("dans jobs\n");
        
        return 1;
    }
    
    if (!strcmp(argv[0], "bg")){    // ignorer & tout seul
        
        printf("dans bg\n");
        
        return 1;
    }
    
    if (!strcmp(argv[0], "fg")){    // ignorer & tout seul
        
        printf("dans fg\n");
        
        return 1;
    }
    
    if (!strcmp(argv[0], "stp")){    // ignorer & tout seul
        
        printf("dans stp\n");
        
        return 1;
    }
    
    
    return 0;                     // ce n'est pas une commande integree
}
