#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>

char identifier;
char cmd[200];
int res;
typedef struct tree_node {
    unsigned nr_children;
    char name;
    struct tree_node ** children;
    int exitCode;
} tree_node;

void explain_wait_status(pid_t pid, int status){
    if(WIFEXITED(status)){
        fprintf(stderr, "Child with PID = %ld terminated normally, exited status = %d\n", (long)pid, WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status)){
        fprintf(stderr, "Child with PID = %ld was terminated by a signal, signo = %d\n", (long)pid, WTERMSIG(status));
    }
    else if (WIFSTOPPED(status)){
        fprintf(stderr, "Child with PID = %ld was stopped by a signal, signo = %d\n", (long)pid, WSTOPSIG(status));
    }
    else{
        fprintf(stderr, "%s: Internal error: Unhandled Case, PID = %ld, status = %d\n", __func__, (long)pid, status);
    }
}

static void __print_tree(tree_node *root, int level){
    int i;
    for (i = 0; i < level; i ++){
        printf("\t");
    }
    printf("%c\n", root->name);
    tree_node ** pointer = root -> children;
    for (i = 0; i < root -> nr_children; i ++){
        tree_node * childPointer = *(pointer + i);
        __print_tree(childPointer, level + 1);
    }


}

void print_tree(tree_node *root){
    __print_tree(root, 0);
}

void add_child(tree_node *parent, tree_node *child){
    unsigned numChildren = parent -> nr_children;
    tree_node ** pointer = parent -> children;

    *(pointer + numChildren) = child;
    parent -> nr_children = numChildren + 1;
}

tree_node* generate_process_tree(){
    tree_node *nodeA, *nodeB, *nodeC, *nodeD;

    nodeA = malloc(sizeof(tree_node));
    nodeB = malloc(sizeof(tree_node));
    nodeC = malloc(sizeof(tree_node));
    nodeD = malloc(sizeof(tree_node));

    nodeA -> children = malloc(2 * sizeof(tree_node*));
    nodeB -> children = malloc(1 * sizeof(tree_node*));

    nodeA -> name = 'A';
    nodeB -> name = 'B';
    nodeC -> name = 'C';
    nodeD -> name = 'D';

    nodeA -> exitCode = 2;
    nodeB -> exitCode = 4;
    nodeC -> exitCode = 6;
    nodeD -> exitCode = 10;

    add_child(nodeA, nodeB);
    add_child(nodeA, nodeC);
    add_child(nodeB, nodeD);

    return nodeA;

}

void DFS(tree_node *node){
    pid_t pid, cpid;
    sleep(3);
    printf("Process %c is currently executing and the pid of this process is %d \n", node-> name, (int)(getpid()));
    //sleep(4);
    identifier = node -> name;
    unsigned numChildren = node -> nr_children;
    tree_node ** pointer = node -> children;
    int status;
    for(int i = 0; i < numChildren; i ++){
        tree_node * childPointer = *(pointer + i);
        pid = fork();
        if(pid < 0){
            printf("Fork Failed! \n");
        }
        else if(pid == 0){
            DFS(childPointer);
        }
        else{
            printf("Process %c is now going to wait for %c \n", node-> name, childPointer -> name);
            sleep(3);
            waitpid(pid, &status, WUNTRACED | WCONTINUED);
            sleep(4);
            explain_wait_status(pid, status);
            sleep(2);
        }
    }
    sleep(4);
    printf("Process %c is now going to exit! \n", node-> name);
    sleep(5);
    identifier = node -> name;
    exit(node -> exitCode);
}

int main(void){
    int status;
    tree_node* nodeA = generate_process_tree();
    pid_t pid = fork();
    if(pid < 0){
        printf("Fork Error! \n");
    }
    else if(pid == 0){
        DFS(nodeA);
    }
    else if(pid > 0){
        sleep(7);
        printf("We are now in Process %d(i.e. the main process) and We will now print the Process Tree(i.e. pstree) Starting from its Parent \n", (int)(getpid()));
        sprintf(cmd,"pstree -np -C age %d",(int)(getppid()));
        res = system(cmd);

        printf("We are now in Process %d(i.e. the main process) and We will now print the Process Tree(i.e. pstree) Starting from Itself \n", (int)(getpid()));
        sprintf(cmd,"pstree -np -C age %d",(int)(getpid()));
        res = system(cmd);

        printf("We are now in Process %d(i.e. the main process) and We will now print the Process Tree(i.e. pstree) Starting from its Child \n",(int)(getpid()));
        sprintf(cmd,"pstree -np -C age %d",(int)(pid));
        res = system(cmd);
        sleep(35);

        printf("We are now in Process %d(i.e. the main process) and We will now print the Process Tree(i.e. pstree) Starting from its Parent \n", (int)(getpid()));
        sprintf(cmd,"pstree -np -C age %d",(int)(getppid()));
        res = system(cmd);

        printf("We are now in Process %d(i.e. the main process) and We will now print the Process Tree(i.e. pstree) Starting from Itself \n", (int)(getpid()));
        sprintf(cmd,"pstree -np -C age %d",(int)(getpid()));
        res = system(cmd);

        printf("We are now in Process %d(i.e. the main process) and We will now print the Process Tree(i.e. pstree) Starting from its Child \n",(int)(getpid()));
        sprintf(cmd,"pstree -np -C age %d",(int)(pid));
        res = system(cmd);

        waitpid(pid, &status, WUNTRACED | WCONTINUED);
        explain_wait_status(pid, status);
        //sleep(2);
        printf("The Process Tree will now be printed in its entirety \n");
        print_tree(nodeA);
    }
    
}
