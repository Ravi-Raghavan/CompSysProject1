#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <signal.h>
#include <time.h>       

int L;
int H;
int MAX_PROCESSES;
int IDENTIFY_HIDDEN;
int * ARRAY;
int maximum(int a , int b){
    if (a > b){
        return a;
    }
    return b;
}

typedef struct tree_node {
    unsigned numProcesses;
    int pid;
    int ppid;
    struct tree_node * firstChild;
    int START_INDEX;
    int END_INDEX;
} tree_node;

typedef struct arrayStatistics{
    int max;
    double average;
    int sum;
    int count;
    double * hidden;
    int hiddenIdentified;
} arrayStatistics;

arrayStatistics * hiddenIdentify(tree_node *node, arrayStatistics * calculations){
    int start = node -> START_INDEX;
    int end = node -> END_INDEX;
    double * hidden = malloc(sizeof(double) * H);
    int hiddenIdentified = 0;
    int remainingHidden = IDENTIFY_HIDDEN - calculations -> hiddenIdentified;
    for(int i = start; i <= end; i ++){
        if(ARRAY[i] == -1){
            if(hiddenIdentified < remainingHidden){
                printf("Hidden Number has been Identified by Process %d at Location %d in the Array\n", (int)(getpid()), i);
                *(hidden + hiddenIdentified) = i;
                hiddenIdentified ++;
            }
        }
    }
    calculations -> hidden = hidden;
    calculations -> hiddenIdentified += hiddenIdentified;
    return calculations;
}
arrayStatistics * calculate(tree_node *node, arrayStatistics * calculations){
    int start = node -> START_INDEX;
    int end = node -> END_INDEX;
    int sum = 0;
    int max = -1;
    int count = 0;
    for(int i = start; i <= end; i ++){
        if(ARRAY[i] != -1){
            count ++;
            sum += ARRAY[i];
            if(ARRAY[i] > max){max = ARRAY[i];}
        }
    }
    
    calculations -> max = max;
    calculations -> average = ((double) sum) / ((double)count);
    calculations -> sum = sum;
    calculations -> count = count;
    return calculations;
}

void executeTreeNodeCalculate(tree_node * node , int readPipe[2], int writePipe[2], arrayStatistics * calculations){
    pid_t pid;
    printf("Hi I am process %d and my parent is %d\n", (int)(getpid()), (int)(getppid()));
    if(node -> numProcesses >= MAX_PROCESSES){
        calculate(node, calculations);
        if(writePipe != NULL){
            if(write(writePipe[1], calculations, sizeof(arrayStatistics)) != sizeof(arrayStatistics)){
                perror("Write to pipe! \n");
                exit(1);
            }
        }
        if(node->pid > 1){
            exit(0);
        }
    }
    else{
        // Each process spawns at most one process
        tree_node* childNode = malloc(sizeof(tree_node));
        node -> firstChild = childNode;
        childNode -> START_INDEX = 0;
        childNode -> END_INDEX = node -> END_INDEX - ((L + H) / MAX_PROCESSES);
        node -> START_INDEX = childNode -> END_INDEX + 1;

        childNode -> pid = node -> pid + 1;
        childNode -> ppid = node -> pid;
        childNode -> numProcesses = node -> numProcesses + 1;
        pid = fork();
        if(pid < 0){
            printf("Fork Error! \n");
        }
        else if(pid == 0){
            arrayStatistics * childCalculations = malloc(sizeof(arrayStatistics));
            int newPipe[2];
            if(pipe(newPipe) < 0){
                perror("pipe\n");
                exit(1);
            }
            executeTreeNodeCalculate(childNode, newPipe, readPipe, childCalculations);

        }
        else if(pid > 0){
            calculate(node, calculations);
            arrayStatistics * childCalculations = malloc(sizeof(arrayStatistics));
            if(readPipe != NULL){
                if(read(readPipe[0], childCalculations, sizeof(arrayStatistics)) !=sizeof(arrayStatistics)){
                    perror("Read from pipe \n");
                    exit(1);
                }
            }

            kill(pid, SIGKILL);
            waitpid(pid, NULL, WUNTRACED | WCONTINUED);
            free(childNode);

            calculations -> max = maximum(calculations -> max, childCalculations -> max);
            calculations -> sum = calculations -> sum + childCalculations -> sum;
            calculations -> count = calculations -> count + childCalculations -> count;
            calculations -> average = ((double)(calculations -> sum)) / ((double)(calculations -> count));
            if(writePipe != NULL){
                if(write(writePipe[1], calculations, sizeof(arrayStatistics)) != sizeof(arrayStatistics)){
                    perror("Write to pipe! \n");
                    exit(1);
                }
                if(node->pid > 1){
                    exit(0);
                }
            }
            else{
                printf("Max=%d, Avg=%f\n", calculations -> max, calculations -> average);
                if(node->pid > 1){
                    exit(0);
                }
            }
        }
    }
}

void executeTreeNodeHiddenIdentify(tree_node *node, int readPipe[2], int writePipe[2], arrayStatistics * calculations){
    pid_t pid;
    if(node -> numProcesses >= MAX_PROCESSES){
        hiddenIdentify(node, calculations);
        if(writePipe != NULL){
            if(write(writePipe[1], calculations, sizeof(arrayStatistics)) != sizeof(arrayStatistics)){
                perror("Write to pipe! \n");
                exit(1);
            }
        }
        if(node->pid > 1){
            exit(0);
        }
    }
    else{
        // Each process spawns at most one process
        tree_node* childNode = malloc(sizeof(tree_node));
        node -> firstChild = childNode;
        childNode -> START_INDEX = 0;
        childNode -> END_INDEX = node -> END_INDEX - ((L + H) / MAX_PROCESSES);
        node -> START_INDEX = childNode -> END_INDEX + 1;

        childNode -> pid = node -> pid + 1;
        childNode -> ppid = node -> pid;
        childNode -> numProcesses = node -> numProcesses + 1;
        pid = fork();
        if(pid < 0){
            printf("Fork Error! \n");
        }
        else if(pid == 0){
            arrayStatistics * childCalculations = malloc(sizeof(arrayStatistics));
            int newPipe[2];
            if(pipe(newPipe) < 0){
                perror("pipe\n");
                exit(1);
            }
            executeTreeNodeHiddenIdentify(childNode, newPipe, readPipe, childCalculations);

        }
        else if(pid > 0){
            
            arrayStatistics * childCalculations = malloc(sizeof(arrayStatistics));
            if(readPipe != NULL){
                if(read(readPipe[0], childCalculations, sizeof(arrayStatistics)) !=sizeof(arrayStatistics)){
                    perror("Read from pipe \n");
                    exit(1);
                }
            }

            kill(pid, SIGKILL);
            waitpid(pid, NULL, WUNTRACED | WCONTINUED);
            free(childNode);
            calculations -> hiddenIdentified = childCalculations -> hiddenIdentified;
            calculations -> hidden = childCalculations -> hidden;
            if(calculations -> hiddenIdentified < IDENTIFY_HIDDEN){
                hiddenIdentify(node, calculations);
            }
            if(writePipe != NULL){
                if(write(writePipe[1], calculations, sizeof(arrayStatistics)) != sizeof(arrayStatistics)){
                    perror("Write to pipe! \n");
                    exit(1);
                }
                if(node->pid > 1){
                    exit(0);
                }
            }
            else{
                if(node->pid > 1){
                    exit(0);
                }
            }
        }
    }
}

void displayArray(int * array, size_t n){
    for(int i = 0; i < n; i ++){
        printf("%d ", array[i]);
    }

    printf("\n");
};
void shuffleArray(int *inputArray, size_t arraySize)
{
    if (arraySize > 1) 
    {
        for (int i = 0; i < arraySize - 1; i++) 
        {
          int j = i + rand() / (RAND_MAX / (arraySize - i) + 1);
          int temporaryValue = inputArray[j];
          inputArray[j] = inputArray[i];
          inputArray[i] = temporaryValue;
        }
    }
};

void populateArray(){
    char * filename = "numbers.txt";
    FILE *fptr = fopen(filename, "r");
    if(fptr == NULL){
        printf("Error\n");
        exit(1);
    }
    for(int i = 0; i < L + H; i ++){
        fscanf(fptr, "%d", &ARRAY[i]);
    }
    fclose(fptr);
}

void generateTestFile(){
    srand ( time(NULL) );
    int MAX = (RAND_MAX) / L;
     FILE *fptr;
    int array[L + H];
    int hiddenLocations[H];
    int li[L + H];
    for (int i = 0; i < L + H; i ++){
        li[i] = i;

    }
    shuffleArray(li, L + H);
    for(int i = 0; i < H; i ++){
        hiddenLocations[i] = li[i];
    }
    for(int i = 0; i < (L + H); i ++){
        int set = 0;;
        for(int j = 0; j < H; j ++){
            if(hiddenLocations[j] == i){
                array[i] = -1;
                set = 1;
                break;
            }
        }
        if(set == 1){continue;}
        array[i] = rand() % MAX;
    }
    fptr = fopen("numbers.txt", "w");
    if(fptr == NULL){
        printf("Error\n");
        exit(1);
    }
    for(int i = 0; i < L + H; i ++){
        fprintf(fptr, "%d\n", array[i]);
    }
    fclose(fptr);
}
void verifyAnswers(){
    int max = -1;
    double avg = 0.0;
    int sum = 0;
    int count = 0;
    int hidden = 0;
    for(int i = 0 ; i < L + H; i ++){
        if(ARRAY[i] != -1){
            max = maximum(max, ARRAY[i]);
            count ++;
            sum += ARRAY[i];
        }
        else{
            if(hidden < IDENTIFY_HIDDEN){
                printf("Found Hidden Key at Index %d\n", i);
                hidden ++;
            }
        }
    }
    avg = ((double) sum) / ((double) count);
    printf("MAXIMUM: %d\n", max);
    printf("AVERAGE: %f\n", avg);
    //printf("SUM: %d\n", sum);

}

int main(int argc, char * argv[]){
    double time_spent = 0.0;
 
    clock_t begin = clock();

    pid_t pid;
    
    L = atoi(argv[1]);
    H = atoi(argv[2]);
    MAX_PROCESSES = atoi(argv[3]);
    IDENTIFY_HIDDEN = atoi(argv[4]);

    ARRAY = malloc((L + H) * sizeof(int));
    generateTestFile();
    populateArray();

    tree_node* nodeA = malloc(sizeof(tree_node));
    arrayStatistics * calculationsA = malloc(sizeof(arrayStatistics));
    nodeA -> START_INDEX = 0;
    nodeA -> END_INDEX = L + H - 1;
    nodeA -> pid = 1;
    nodeA -> ppid = 0;
    nodeA -> numProcesses = 1;

    int pfd[2];
    if(pipe(pfd) < 0){
        perror("pipe\n");
        exit(1);
    }

    executeTreeNodeCalculate(nodeA, pfd, NULL, calculationsA);

    int newReadPipe[2];
    if(pipe(newReadPipe) < 0){
        perror("pipe\n");
        exit(1);
    }

    tree_node* nodeB = malloc(sizeof(tree_node));
    arrayStatistics * calculationsB = malloc(sizeof(arrayStatistics));
    nodeB -> START_INDEX = 0;
    nodeB -> END_INDEX = L + H - 1;
    nodeB -> pid = 1;
    nodeB -> ppid = 0;
    nodeB -> numProcesses = 1;
    
    executeTreeNodeHiddenIdentify(nodeB, newReadPipe, NULL, calculationsB);

    //printf("--------Verified Answers --------\n");
    //verifyAnswers();
    
    printf("Program Complete! \n");

    clock_t end = clock();
 
    // calculate elapsed time by finding difference (end - begin) and
    // dividing the difference by CLOCKS_PER_SEC to convert to seconds
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
        
    printf("The elapsed time is %f seconds\n", time_spent);
}
