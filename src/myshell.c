#include "myshell.h"

// fonctions externes
void eval(char*cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

void handlerTSTP(int sig) {
    printf("\n\ndans le handlerTSTP %d pid: %d!\n", sig, getpid());
    int index = getIndexFG();
    printf("index trouve: %d\n", index);
    if (index != -1) {
        kill(tabJobs[index]->pid, SIGSTOP);
        printf("processus %d mis en bg\n", tabJobs[index]->pid);



        int i;
        for (i = 0; i < MAXJOBS; i++) {
            afficheJob(i);
        }

    } else
        printf("Erreur: pas de processus en foreground");

    return;
}

void handlerCHLD(int sig) {
    printf("\n\ndans le handlerCHLD %d!\n", sig);

    int status;
    int i;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WUNTRACED | WNOHANG)) > 0) {
        i = getIndexJob(pid);

        if (i == -1) {
            printf("Dans IF processus non trouve: %d\n", tabJobs[i]->pid);
            continue;
        }
        if (WIFSIGNALED(status)) {
            printf("Dans ELSE Terminé a cause d'un signal: %d\n", tabJobs[i]->pid);
            rmJob(tabJobs[i]->pid);
        }

        if (WIFEXITED(status)) { // terminé normalement
            printf("Dans ELSE Terminé normalement: %d\n", tabJobs[i]->pid);
            rmJob(tabJobs[i]->pid);
        }

        if (WIFSTOPPED(status)) { // mis en pause
            printf("Dans ELSE  Mis en pause: %d\n", tabJobs[i]->pid);
            tabJobs[i]->etat = PAUSE;
        }

    }


    printf("fin handlerCHLD\n");
}

void handlerINT(int sig) {

    printf("debut handlerINT\n");
    int i = getIndexFG();
    if (i != -1) {
        kill(-tabJobs[i]->pid, SIGKILL);
    }
    printf("fin handlerINT\n");
}

int main() {
    int i = 0;
    for (i = 0; i < MAXJOBS; i++) {
        tabJobs[i] = NULL;
    }


    char cmdline[MAXLINE]; // ligne de commande
    Signal(SIGTSTP, handlerTSTP);
    Signal(SIGCHLD, handlerCHLD);
    Signal(SIGINT, handlerINT);

    while (1) { // boucle d'interpretation
        printf("<my_shell> "); // message d'invite
        Fgets(cmdline, MAXLINE, stdin); // lire commande
        if (feof(stdin)) // fin (control-D)
            exit(0);
        eval(cmdline); // interpreter commande
    }
}
