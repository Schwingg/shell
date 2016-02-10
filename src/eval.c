/* eval : interprete une ligne de commande passee en parametre       */
#include "myshell.h"

// fonctions externes
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

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

void afficheJob(int index) {
    if (tabJobs[index] != NULL)
        printf("[%d] \t etat: %s \t commande: %s \t pid: %d\n", index, tabJobs[index]->etat == PAUSE ? "Pause" : "En cours d'execution", tabJobs[index]->commande, tabJobs[index]->pid);
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


            if (execve(argv[0], argv, environ) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }

        indexJobLibre = getIndexLibre();
        tabJobs[indexJobLibre] = malloc(sizeof (job));
        tabJobs[indexJobLibre]->pid = pid;
        tabJobs[indexJobLibre]->commande = argv[0];


        if (!bg) { // le pere attend fin du travail de premier plan

            tabJobs[indexJobLibre]->etat = FG;
            printf("dans !bg\n");
            printf("getIndexFG() = %d --\n", getIndexFG());
            int status;
            if (waitpid(pid, &status, WUNTRACED) < 0)
                unix_error("waitfg: waitpid error");
            printf("fin attente pid\n");

            /*
            while (getIndexFG() != -1) {
                printf("verif FG = %d\n", getIndexFG());
                sleep(1);

            }
            */

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
        int i;
        for (i = 0; i < MAXJOBS; i++) {
            afficheJob(i);
        }


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

    if (!strcmp(argv[0], "stp")) { // ignorer & tout seul

        printf("dans stp\n");

        return 1;
    }


    return 0; // ce n'est pas une commande integree
}
