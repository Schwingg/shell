/* eval : interprete une ligne de commande passee en parametre       */
#include "myshell.h"

// fonctions externes
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

void handlerCHLD(int sig);


int getIndexLibre() {
    int i = 0;
    for (i = 0; i < MAXJOBS; i++)
        if (tabJobs[i] == NULL)
            return i;
    return -1;
}

int getIndexFG() {
    int i = 0;
    for (i = 0; i < MAXJOBS; i++)
        if (tabJobs[i] != NULL)
            if (tabJobs[i]->etat == FG)
                return i;
    return -1;
}

void rmJob(int pid) {
    int i = 0;
    for (i = 0; i < MAXJOBS; i++)
        if (tabJobs[i] != NULL)
            if (tabJobs[i]->pid == pid){
                free(tabJobs[i]);
                tabJobs[i] = NULL;
            }
}


int getIndexJob(int pid) {
    int i = 0;
    for (i = 0; i < MAXJOBS; i++)
        if (tabJobs[i] != NULL)
            if (tabJobs[i]->pid == pid){
                return i;
            }
    return -1;
}


void afficheJob(int index) {
    if (tabJobs[index] != NULL)
        printf("[%d] \t etat: %s \t commande: %s \t pid: %d\n", index, tabJobs[index]->etat == PAUSE ? "Pause" : "En cours d'execution", tabJobs[index]->commande, tabJobs[index]->pid);
}
void afficheJobs() {
        int i;
        for (i = 0; i < MAXJOBS; i++) {
            afficheJob(i);
        }
}


void eval(char *cmdline) {
    char *argv[MAXARGS]; // argv pour execve()
    char buf[MAXLINE]; // contient ligne commande modifiee
    int bg; // arriere-plan ou premier plan ?
    pid_t pid; // process id

    int indexJobLibre;

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL)
        return; // ignorer lignes vides

    if (!builtin_command(argv)) { // commande integree ?
        // si oui, executee directement
        if ((pid = Fork()) == 0) { // si non, executee par un fils


            if (execvp(argv[0], argv) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }

        indexJobLibre = getIndexLibre();
        tabJobs[indexJobLibre] = malloc(sizeof (job));
        tabJobs[indexJobLibre]->pid = pid;
        strcpy(tabJobs[indexJobLibre]->commande,argv[0]);
        

        if (!bg) { // le pere attend fin du travail de premier plan

            tabJobs[indexJobLibre]->etat = FG;
            printf("dans !bg: commande = %s\n", tabJobs[indexJobLibre]->commande );
            printf("getIndexFG() = %d --\n", getIndexFG());
           

            
            while (getIndexFG() != -1) {
                printf("verif FG = %d\n", getIndexFG());
                afficheJobs();
                sleep(1);
            }
            
            

        } else { // travail d'arriere-plan, on imprime le pid
            tabJobs[indexJobLibre]->etat = BG;
            printf("%d %s", pid, cmdline);


        }
    }
    return;
}

// si le premier parametre est une commande integree,
// l'executer et renvoyer "vrai"

int builtin_command(char **argv) {
    if (!strcmp(argv[0], "quit")) // commande "quitter"
        exit(0);
    if (!strcmp(argv[0], "&")) // ignorer & tout seul
        return 1;

    if (!strcmp(argv[0], "jobs")) { // ignorer & tout seul

        printf("jobs\n");
        afficheJobs();


        return 1;
    }

    if (!strcmp(argv[0], "bg")) { // ignorer & tout seul

        printf("dans bg\n");

        return 1;
    }

    if (!strcmp(argv[0], "fg")) { // ignorer & tout seul

        printf("dans fg\n");

        return 1;
    }

    if (!strcmp(argv[0], "stop")) { // ignorer & tout seul

        printf("dans stop\n");

        return 1;
    }


    return 0; // ce n'est pas une commande integree
}
