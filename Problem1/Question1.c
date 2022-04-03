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

void displayArray(int * array, size_t n)
{
    printf("The Array is as follows: ");
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

void populateArray()
{

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
        array[i] = rand() % 20;
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

void makeCalculations(int * ARRAY){
    int MAXIMUM = 0;
    double AVERAGE = 0;
    int SUM = 0;
    int COUNT = 0;
    for(int i = 0; i < (L + H); i ++){
        int value = *(ARRAY + i);
        if(value == -1){
            continue;
        }
        SUM += value;
        COUNT ++;
        AVERAGE = ((double)SUM) / ((double)COUNT);
        if(value > MAXIMUM){MAXIMUM = value;}

    }
    printf("MAXIMUM: %d\n", MAXIMUM);
    printf("AVERAGE: %f\n", AVERAGE);
}

void identifyHidden(int * ARRAY){
    int hiddenIdentified = 0;
    for(int i = 0; i < (L + H); i ++){
        int value = *(ARRAY + i);
        if(value == -1 && hiddenIdentified < IDENTIFY_HIDDEN){
            printf("Hidden Key Identified at Index %d\n", i);
            hiddenIdentified ++;
        }
    }
}


int main(int argc, char * argv[]){
    double time_spent = 0.0;
    clock_t begin = clock();

    L = atoi(argv[1]);
    H = atoi(argv[2]);
    IDENTIFY_HIDDEN = atoi(argv[3]);

    ARRAY = malloc((L + H) * sizeof(int));
    pid_t pid = fork();
    generateTestFile();
    populateArray();

    makeCalculations(ARRAY);
    identifyHidden(ARRAY);
    
    clock_t end = clock();
 
    // calculate elapsed time by finding difference (end - begin) and
    // dividing the difference by CLOCKS_PER_SEC to convert to seconds
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
        
    printf("The elapsed time is %f seconds\n", time_spent);
}
