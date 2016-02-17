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
            if (tabJobs[i]->pid == pid) {
                free(tabJobs[i]);
                tabJobs[i] = NULL;
            }
}

int getIndexJob(int pid) {
    int i = 0;
    for (i = 0; i < MAXJOBS; i++)
        if (tabJobs[i] != NULL)
            if (tabJobs[i]->pid == pid) {
                return i;
            }
    return -1;
}

char* getEtatJob(int index) {

    switch (tabJobs[index]->etat) {

        case PAUSE:
            return "Pause";
            break;

        case FG:
            return "En Cour d'execution (FG)";
            break;

        case BG:
            return "En Cour d'execution (BG)";
            break;

        default:
            return "Erreur Etat non géré";

    }


}

void afficheJob(int index) {
    if (tabJobs[index] != NULL)
        printf("[%d] \t etat: %s \t commande: %s \t pid: %d\n", index, getEtatJob(index), tabJobs[index]->commande, tabJobs[index]->pid);
}

void afficheJobs() {
    int i;
    for (i = 0; i < MAXJOBS; i++) {
        afficheJob(i);
    }
}

int checkRunningJob() {
    int i = 0;
    for (i = 0; i < MAXJOBS; i++)
        if (tabJobs[i] != NULL)
            if (tabJobs[i]->etat != PAUSE) {
                return 1;
            }
    return 0;
}


int getIndex(char* argv){
    
    int index;
    if (argv[0] == '%') {
        // c'est un jobs
        printf("c'est un jobs => %s\n", argv + sizeof (char));
        index = atoi(argv + sizeof (char));
        if(index < 0 || index >= MAXJOBS) return -1;
        if(tabJobs[index] == NULL) return -1;
        
        return index;
    } else {
        // c'est un pid
        printf("c'est un pid => %s\n", argv);
        return getIndexJob(atoi(argv));
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
            setpgid(0, 0);

            if (execvp(argv[0], argv) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }

        indexJobLibre = getIndexLibre();
        tabJobs[indexJobLibre] = malloc(sizeof (job));
        tabJobs[indexJobLibre]->pid = pid;
        strcpy(tabJobs[indexJobLibre]->commande, argv[0]);


        if (!bg) { // le pere attend fin du travail de premier plan

            tabJobs[indexJobLibre]->etat = FG;
            printf("dans !bg: commande = %s\n", tabJobs[indexJobLibre]->commande);
            printf("getIndexFG() = %d --\n", getIndexFG());


        } else { // travail d'arriere-plan, on imprime le pid
            tabJobs[indexJobLibre]->etat = BG;
            printf("%d %s", pid, cmdline);


        }

    }

    while (getIndexFG() != -1) {
        printf("verif FG = %d\n", getIndexFG());
        afficheJobs();
        sleep(1);
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
        
        int index = getIndex(argv[1]);
        
        if(index == -1){
            printf("Erreur bg: job non trouvé\n");
            return 1;
        }
        
        tabJobs[index]->etat = BG;
        kill(tabJobs[index]->pid, SIGCONT);
        return 1;
    }

    if (!strcmp(argv[0], "fg")) { // ignorer & tout seul

        printf("dans fg\n");
        
        int index = getIndex(argv[1]);
        
        if(index == -1){
            printf("Erreur fg: job non trouvé\n");
            return 1;
        }
        
        tabJobs[index]->etat = FG;
        kill(tabJobs[index]->pid, SIGCONT);
        return 1;

    }

    if (!strcmp(argv[0], "stop")) { // ignorer & tout seul

        printf("dans stop\n");
        
        int index = getIndex(argv[1]);
        
        if(index == -1){
            printf("Erreur stop: job non trouvé\n");
            return 1;
        }        
        
        tabJobs[index]->etat = PAUSE;
        kill(tabJobs[index]->pid, SIGTSTP);
        return 1;
    }

    if (!strcmp(argv[0], "wait")) { // ignorer & tout seul


        while (checkRunningJob()) {
            sleep(1);
        }

    }

    return 0; // ce n'est pas une commande integree
}
