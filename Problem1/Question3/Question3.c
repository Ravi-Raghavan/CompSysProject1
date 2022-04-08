#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <signal.h>
#include <time.h>      

typedef struct tree_node {
    int pid;
    int START_INDEX;
    int END_INDEX;
    struct tree_node ** children;
    int numChildren;
} tree_node;

typedef struct arrayStatistics{
    int max;
    double average;
    int sum;
    int count;
    int * hidden;
    int hiddenIdentified;
    int * alreadyIdentified;
} arrayStatistics;

tree_node ** HEAP;
int HEAP_LENGTH = 1;
int L, H, MAX_PROCESSES, X, IDENTIFY_HIDDEN;
int * ARRAY;

int maximum(int a , int b){
    if (a > b){
        return a;
    }
    return b;
}

static void __print_tree(tree_node *root, int level){
    int i;
    for (i = 0; i < level; i ++){
        printf("\t");
    }
    printf("PID: %d, START INDEX: %d, END INDEX: %d, numChildren: %d\n", root->pid, root -> START_INDEX, root -> END_INDEX, root -> numChildren);
    tree_node ** pointer = root -> children;
    for (i = 0; i < root -> numChildren; i ++){
        tree_node * childPointer = *(pointer + i);
        __print_tree(childPointer, level + 1);
    }
}

void print_tree(tree_node *root){
    __print_tree(root, 0);
}

int minimum(int a, int b){
    if(a < b){
        return a;
    }
    return b;
}
int generateID(int times, int n, int i){
    return X * (n - times) + i + 1;
}

void generateXChildrenRecursive(tree_node *node, int times, int n, int left){
    if(times > 0){
        node ->numChildren = X;
        node -> children = malloc(X * sizeof(tree_node*));
        for(int i = 0; i < X; i ++){
            *(node -> children + i) = malloc(sizeof(tree_node));
            (*(node -> children + i)) -> pid = generateID(times, n, i);
            (*(node -> children + i)) -> START_INDEX = 0;
            (*(node -> children + i)) -> END_INDEX = -1;
            (*(node -> children + i)) -> numChildren = 0;
        }
        if(left == 1){
            generateXChildrenRecursive(*(node -> children + 0), times - 1, n, left);
        }
        else{
            generateXChildrenRecursive(*(node -> children + X - 1), times - 1, n, left);
        }
    }
}

void generateXChildren(tree_node * node, int left){
    int times = (MAX_PROCESSES - 1) / X;
    generateXChildrenRecursive(node, times, times, left);
}

void setIndexTree(tree_node *node, int start, int end){
    int startIndex = 0;
    tree_node * currentNode = node;
    currentNode -> START_INDEX = start;
    currentNode -> END_INDEX = end;
    if(currentNode -> numChildren == 0){return;}
    int length = currentNode -> END_INDEX - currentNode -> START_INDEX + 1;
    int assignChildren = minimum(length, X);
    if(assignChildren == 0){return;}
    int lengthPerChild = (length) / (assignChildren);
    tree_node ** children = currentNode -> children;
    for(int j = 0; j < assignChildren; j ++){
        tree_node * childNode = *(children + j);
        if(j == 0){
            childNode -> START_INDEX = currentNode -> START_INDEX;
            childNode -> END_INDEX = childNode -> START_INDEX + lengthPerChild - 1;
        }
        else if(j == assignChildren - 1){
            childNode -> START_INDEX = (*(children + j - 1)) -> END_INDEX + 1;
            childNode -> END_INDEX = currentNode -> END_INDEX;
        }
        else{
            childNode -> START_INDEX = (*(children + j - 1)) -> END_INDEX + 1;
            childNode -> END_INDEX = childNode -> START_INDEX + lengthPerChild - 1;
        }
    }

    for(int i = 0; i < currentNode -> numChildren; i ++){
        setIndexTree(*(currentNode ->children + i), (*(currentNode ->children + i)) -> START_INDEX, (*(currentNode ->children + i)) -> END_INDEX );
    }
    return;
}

void generateHeap(){
    int mod = MAX_PROCESSES % X;
    
    if(MAX_PROCESSES < X + 1){
        HEAP = malloc(sizeof(tree_node*));
        tree_node * ptr = malloc(sizeof(tree_node));
        *(HEAP + 0) = ptr;
        return;
    }
    int numProcesses;
    if(MAX_PROCESSES % X == 0){
        numProcesses = MAX_PROCESSES - (X - 1); 
    }
    else{
        numProcesses = MAX_PROCESSES - (mod - 1);
    }
    HEAP = malloc(numProcesses * sizeof(tree_node*));
    for(int i = 0; i < numProcesses; i ++){
        tree_node * ptr = malloc(sizeof(tree_node));
        ptr -> pid = i;
        ptr -> children = malloc(X * sizeof(tree_node*));
        ptr -> START_INDEX = 0;
        ptr -> END_INDEX = -1;
        for(int k = 0; k < X; k ++){
            *(ptr -> children + k) = malloc(sizeof(tree_node));
        }
        ptr ->numChildren = 0;
        *(HEAP + i) = ptr;

    }
    for(int i = 0; i < numProcesses; i ++){
        tree_node *ptr = *(HEAP + i);
        if((ptr -> pid) * X + X < numProcesses){
            ptr -> numChildren = X;
            for(int j = 1; j <= X; j++){
                int index = (X * ptr -> pid) + j;
                *(ptr -> children + j - 1) = *(HEAP + index);
            }
        }
    }
    HEAP_LENGTH = numProcesses;
    
}

void displayHeap(){
    for(int i = 0; i < HEAP_LENGTH; i ++){
        tree_node * ptr = *(HEAP + i);
        printf("PID OF Tree Node IS %d AND ITS START INDEX IS %d AND ITS END INDEX IS %d\n", ptr -> pid, ptr -> START_INDEX, ptr -> END_INDEX);
        printf("I will now print the children of the Process %d\n", ptr -> pid);
        for(int j = 0 ; j < ptr -> numChildren; j ++){
            printf("PID OF CHILD NODE IS: %d\n", (*(ptr -> children + j)) -> pid);
        }
    }
}

void setIndexHeap(){
    int startIndex = 0;
    tree_node * currentNode = *(HEAP + startIndex);
    currentNode -> START_INDEX = 0;
    currentNode -> END_INDEX = (L + H - 1);
    for(int i = 0; i < HEAP_LENGTH; i ++){
        currentNode = *(HEAP + i);
        if(currentNode -> numChildren == 0){continue;}
        int length = currentNode -> END_INDEX - currentNode -> START_INDEX + 1;
        int assignChildren = minimum(length, X);
        int lengthPerChild = (length) / (assignChildren);
        tree_node ** children = currentNode -> children;
        for(int j = 0; j < assignChildren; j ++){
            tree_node * childNode = *(children + j);
            if(j == 0){
                childNode -> START_INDEX = currentNode -> START_INDEX;
                childNode -> END_INDEX = childNode -> START_INDEX + lengthPerChild - 1;
            }
            else if(j == assignChildren - 1){
                childNode -> START_INDEX = (*(children + j - 1)) -> END_INDEX + 1;
                childNode -> END_INDEX = currentNode -> END_INDEX;
            }
            else{
                childNode -> START_INDEX = (*(children + j - 1)) -> END_INDEX + 1;
                childNode -> END_INDEX = childNode -> START_INDEX + lengthPerChild - 1;
            }
        }
    }
}

int find(int * array, int length, int target){
    for(int i = 0; i < length; i ++){
        if(array[i] == target){
            return 1;
        }
    }
    return 0;
}


arrayStatistics * executeTreeNodeHiddenIdentify(tree_node * node, int ** readPipe, int * writePipe, arrayStatistics * calculations){
    if(node -> numChildren == 0){
        if(readPipe != NULL){
            for(int i = 0; i < X; i ++){
                    close(*(*(readPipe + i) + 1));
            }
        }
        int start = node -> START_INDEX; 
        int end = node -> END_INDEX;
        int * hidden = malloc(sizeof(int) * H);
        int hiddenIdentified = 0;
        int remainingHidden = IDENTIFY_HIDDEN - calculations -> hiddenIdentified;
        for(int i = start; i <= end; i ++){
            if(ARRAY[i] == -1){
                if(hiddenIdentified < remainingHidden 
                        && find(calculations -> alreadyIdentified, calculations -> hiddenIdentified + hiddenIdentified, i) == 0){
                    *(hidden + hiddenIdentified) = i;
                    *(calculations -> alreadyIdentified + hiddenIdentified) = i;
                    hiddenIdentified ++;
                    printf("Hi I am Process %d and I found the hidden key in Position A[%d]\n", node -> pid, i);
                }
                else{
                    continue;
                }
            }
        }
        calculations -> hidden = hidden;
        calculations -> hiddenIdentified += hiddenIdentified;
        if(writePipe != NULL){
            if(write(*(writePipe + 1), calculations, sizeof(arrayStatistics)) != sizeof(arrayStatistics)){
                perror("WRITE FAILED!\n");
            }
            close(*(writePipe + 0));
        }
        return calculations;
    }
    else{
        tree_node ** children = node -> children;
        arrayStatistics ** childCalculations = malloc(X * sizeof(arrayStatistics*));
        int ** newReadPipe = malloc(X * sizeof(int*));
        for(int i = 0; i < X; i ++){
            *(newReadPipe + i) = malloc(2 * sizeof(int));
            if(pipe(*(newReadPipe + i)) < 0){
                perror("Pipe Creation Error!\n");
            }
        }
        for(int i = 0; i < X; i ++){*(childCalculations + i) = malloc(sizeof(arrayStatistics)); (*(childCalculations + i)) -> alreadyIdentified = (int*)(calloc(L + H, sizeof(int)));}
        for(int i = 0; i < X; i ++){
            tree_node * childNode = *(children + i);
            if(i == 0){
                (*(childCalculations + i)) -> hiddenIdentified = calculations -> hiddenIdentified;
                (*(childCalculations + i)) -> alreadyIdentified = calculations -> alreadyIdentified;
            }
            if(i > 0){
                (*(childCalculations + i)) -> hiddenIdentified = (*(childCalculations + i - 1)) -> hiddenIdentified;
                (*(childCalculations + i)) -> alreadyIdentified = (*(childCalculations + i - 1)) -> alreadyIdentified;
            }
            if((*(childCalculations + i)) -> hiddenIdentified >= IDENTIFY_HIDDEN){
                break;
            }
            
            pid_t pid = fork();
            if(pid < 0){
                perror("Fork Failed! \n");
            }
            else if(pid == 0){
                executeTreeNodeHiddenIdentify(childNode, newReadPipe, *(readPipe + i), *(childCalculations + i)) ;
                exit(0);
            }
            else if(pid > 0){
                if(calculations -> hiddenIdentified <= IDENTIFY_HIDDEN){
                    waitpid(pid, NULL, WUNTRACED | WCONTINUED);
                    if(read(*(*(readPipe + i) + 0), *(childCalculations + i), sizeof(arrayStatistics)) != sizeof(arrayStatistics)){
                        perror("Read failed! \n");
                    }
                }
                calculations -> hiddenIdentified = (*(childCalculations + i)) -> hiddenIdentified;
                calculations -> alreadyIdentified = (*(childCalculations + i)) -> alreadyIdentified;
                close(*(*(readPipe + i) + 1));
            }
        }
        if(writePipe != NULL){
            if(write(*(writePipe + 1), calculations, sizeof(arrayStatistics)) != sizeof(arrayStatistics)){
                perror("WRITE FAILED!\n");
            }
            close(*(writePipe + 0));
        }
        return calculations;
    }
}

arrayStatistics * executeTreeNodeCalculations(tree_node * node, int ** readPipe, int * writePipe, arrayStatistics * calculations){
    printf("Hello I am Process %d and My Parent is Process %d \n", (int)(getpid()), (int)(getppid()));
    if(node -> numChildren == 0){
        if(readPipe != NULL){
            for(int i = 0; i < X; i ++){
                close(*(*(readPipe + i) + 1));
            }
        }
        int start = node -> START_INDEX; 
        int end = node -> END_INDEX;
        int sum = 0;
        int max = -1;
        int count = 0;
        for(int i = start; i <= end; i ++){
            if(ARRAY[i] == -1){
                continue;
            }
            else{
                sum += ARRAY[i];
                count ++;
                max = maximum(max, ARRAY[i]);
            }
        }

        calculations -> sum = sum;
        calculations -> max = max;
        calculations -> count = count;
        calculations -> average = ((double) sum) / ((double) count);
        if(writePipe != NULL){
            if(write(*(writePipe + 1), calculations, sizeof(arrayStatistics)) != sizeof(arrayStatistics)){
                perror("WRITE FAILED!\n");
            }
            close(*(writePipe + 0));
        }
        return calculations;
    }
    else{
        tree_node ** children = node -> children;
        arrayStatistics ** childCalculations = malloc(X * sizeof(arrayStatistics*));
        pid_t * pidArray = (pid_t *)(malloc(X * sizeof(pid_t)));
        int ** newReadPipe = malloc(X * sizeof(int*));
        for(int i = 0; i < X; i ++){
            *(newReadPipe + i) = malloc(2 * sizeof(int));
            if(pipe(*(newReadPipe + i)) < 0){
                perror("Pipe Creation Error!\n");
            }
        }
        for(int i = 0; i < X; i ++){*(childCalculations + i) = malloc(sizeof(arrayStatistics));}
        for(int i = 0; i < X; i ++){
            tree_node * childNode = *(children + i);
            pid_t pid = fork();
            if(pid < 0){
                perror("Fork Failed! \n");
            }
            else if(pid == 0){
                executeTreeNodeCalculations(childNode, newReadPipe, *(readPipe + i), *(childCalculations + i)) ;
                exit(0);
            }
            else if(pid > 0){
                pidArray[i] = pid;
                close(*(*(readPipe + i) + 1));
                continue;
            }
        }
        int max = -1;
        int sum = 0;
        int count = 0;
        for(int i = 0; i < X; i ++){
            if(read(*(*(readPipe + i) + 0), *(childCalculations + i), sizeof(arrayStatistics)) != sizeof(arrayStatistics)){
                    perror("Read failed! \n");
            }
            waitpid(pidArray[i], NULL, 0);
        }
        for(int i = 0; i < X; i ++){
            max = maximum(max, (*(childCalculations + i)) -> max);
            sum += (*(childCalculations + i)) -> sum;
            count += (*(childCalculations + i)) -> count;
        }
        double average = ((double) sum) / ((double) count);
        calculations -> max = max;
        calculations -> sum = sum;
        calculations -> count = count;
        calculations -> average = average;
        if(writePipe != NULL){
            if(write(*(writePipe + 1), calculations, sizeof(arrayStatistics)) != sizeof(arrayStatistics)){
                perror("WRITE FAILED!\n");
            }
            close(*(writePipe + 0));
        }
        return calculations;
    }
}

void executeProgramHeap(){
    int ** readPipe = malloc(X * sizeof(int*));
    for(int i = 0; i < X; i ++){
        *(readPipe + i) = malloc(2 * sizeof(int));
        if(pipe(*(readPipe + i)) < 0){
            perror("Pipe Creation Failed!\n");
        }
    }
    int * writePipe = malloc(2 * sizeof(int));
    if(pipe(writePipe) < 0){
        perror("Pipe Creation Failed\n");
    }
        
    arrayStatistics * calculations = malloc(sizeof(arrayStatistics));
    generateHeap();
    setIndexHeap();
    calculations = executeTreeNodeCalculations(*(HEAP + 0), readPipe, writePipe, calculations);
    printf("MAXIMUM: %d\n", calculations -> max);
    printf("AVERAGE: %f\n", (double)(calculations -> average));
    printf("SUM: %d\n", calculations -> sum);
    int ** readPipeTwo = malloc(X * sizeof(int*));
    for(int i = 0; i < X; i ++){
        *(readPipeTwo + i) = malloc(2 * sizeof(int));
        if(pipe(*(readPipeTwo + i)) < 0){
            perror("Pipe Creation Failed!\n");
        }
    }
    int * writePipeTwo = malloc(2 * sizeof(int));
    if(pipe(writePipeTwo) < 0){
        perror("Pipe Creation Failed\n");
    }

    arrayStatistics * newCalculations = malloc(sizeof(arrayStatistics));
    newCalculations -> alreadyIdentified = (int*) (malloc(H * sizeof(int)));
    newCalculations = executeTreeNodeHiddenIdentify(*(HEAP + 0), readPipeTwo, writePipeTwo, newCalculations);

}
void executeLeftTree(){
    int ** readPipe = malloc(X * sizeof(int*));
    for(int i = 0; i < X; i ++){
        *(readPipe + i) = malloc(2 * sizeof(int));
        if(pipe(*(readPipe + i)) < 0){
            perror("Pipe Creation Failed!\n");
        }
    }
    int * writePipe = malloc(2 * sizeof(int));
    if(pipe(writePipe) < 0){
        perror("Pipe Creation Failed\n");
    }
        
    arrayStatistics * calculations = malloc(sizeof(arrayStatistics));
    tree_node * root = malloc(sizeof(tree_node));
    root -> numChildren = 0;
    root -> pid = 0;
    root -> START_INDEX = 0;
    root -> END_INDEX = -1;
    generateXChildren(root, 1);
    setIndexTree(root, 0, L + H - 1);
    calculations = executeTreeNodeCalculations(root, readPipe, writePipe, calculations);
    printf("MAXIMUM: %d\n", calculations -> max);
    printf("AVERAGE: %f\n", (double)(calculations -> average)); 
    printf("SUM: %d\n", calculations -> sum);
    int ** readPipeTwo = malloc(X * sizeof(int*));
    for(int i = 0; i < X; i ++){
        *(readPipeTwo + i) = malloc(2 * sizeof(int));
        if(pipe(*(readPipeTwo + i)) < 0){
            perror("Pipe Creation Failed!\n");
        }
    }
    int * writePipeTwo = malloc(2 * sizeof(int));
    if(pipe(writePipeTwo) < 0){
        perror("Pipe Creation Failed\n");
    }

    arrayStatistics * newCalculations = malloc(sizeof(arrayStatistics));
    newCalculations -> alreadyIdentified = (int*) (calloc(L + H, sizeof(int)));
    newCalculations = executeTreeNodeHiddenIdentify(root, readPipeTwo, writePipeTwo, newCalculations);
}
void executeRightTree(){
    int ** readPipe = malloc(X * sizeof(int*));
    for(int i = 0; i < X; i ++){
        *(readPipe + i) = malloc(2 * sizeof(int));
        if(pipe(*(readPipe + i)) < 0){
            perror("Pipe Creation Failed!\n");
        }
    }
    int * writePipe = malloc(2 * sizeof(int));
    if(pipe(writePipe) < 0){
        perror("Pipe Creation Failed\n");
    }
        
    arrayStatistics * calculations = malloc(sizeof(arrayStatistics));
    tree_node * root = malloc(sizeof(tree_node));
    root -> numChildren = 0;
    root -> pid = 0;
    root -> START_INDEX = 0;
    root -> END_INDEX = -1;
    generateXChildren(root, 0);
    setIndexTree(root, 0, L + H - 1);
    calculations = executeTreeNodeCalculations(root, readPipe, writePipe, calculations);
    printf("MAXIMUM: %d\n", calculations -> max);
    printf("AVERAGE: %f\n", (double)(calculations -> average)); 
    printf("SUM: %d\n", calculations -> sum);

    int ** readPipeTwo = malloc(X * sizeof(int*));
    for(int i = 0; i < X; i ++){
        *(readPipeTwo + i) = malloc(2 * sizeof(int));
        if(pipe(*(readPipeTwo + i)) < 0){
            perror("Pipe Creation Failed!\n");
        }
    }
    int * writePipeTwo = malloc(2 * sizeof(int));
    if(pipe(writePipeTwo) < 0){
        perror("Pipe Creation Failed\n");
    }

    arrayStatistics * newCalculations = malloc(sizeof(arrayStatistics));
    newCalculations -> alreadyIdentified = (int*) (calloc(L + H, sizeof(int)));
    newCalculations = executeTreeNodeHiddenIdentify(root, readPipeTwo, writePipeTwo, newCalculations);
}
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
        printf("Error with file pointer\n");
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
        int set = 0;
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
    printf("SUM: %d\n", sum);

}
int main(int argc, char * argv[]){
    double time_spent = 0.0;
    clock_t begin = clock();
    L = atoi(argv[1]);
    H = atoi(argv[2]);
    MAX_PROCESSES = atoi(argv[3]) + 1;
    X = atoi(argv[4]);
    IDENTIFY_HIDDEN = atoi(argv[5]);
    ARRAY = malloc((L + H) * sizeof(int));
    
    generateTestFile();
    populateArray();
    
    //To execute the Heap Tree, run the following function
    //printf("--------Program Output from Heap Execution --------\n");
    //executeProgramHeap();
    //printf("--------Verified Answers --------\n");
    //verifyAnswers();

    //To execute the Left Sided Tree, run the following function 
    //printf("--------Program Output from Left Skewed Tree Execution --------\n");
    //executeLeftTree();

    //To execute the Right Sided Tree, run the following function 
    printf("--------Program Output from Right Skewed Tree Execution --------\n");
    executeRightTree();

    clock_t end = clock();
 
    // calculate elapsed time by finding difference (end - begin) and
    // dividing the difference by CLOCKS_PER_SEC to convert to seconds
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
        
    printf("The elapsed time is %f seconds\n", time_spent);

    
    
}
