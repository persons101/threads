#include <iostream>
#include <pthread.h>

void* ProdFunction(void* threadID);
void* ConsFunction(void *threadID);

// GLOBALS
#define S 10
int buf[S];
#define NUM_C_THREADS 6             // consumer thread count
#define NUM_P_THREADS 6             // producer thread count
int min[NUM_C_THREADS];             // global min array
int max[NUM_C_THREADS];             // global max array
double avg[NUM_C_THREADS];          // global avg array
int NextIn = 0, NextOut = 0;        // Current buffer access
int totMax, totMin;
double totAvg;
int NumItems;                       // number of items for each producer to produce
int range = 1000;                   // range for random numbers
pthread_mutex_t lock;               // mutex lock
pthread_cond_t empty, full;
//END GLOBALS

int main(int argc, char*argv[]){
    
    pthread_t tidC[NUM_C_THREADS]; // array with consumer thread IDs
    pthread_t tidP[NUM_P_THREADS]; // array with producer thread IDs
    
    // command line arguments
    if (argc != 2+1 ) {
        printf(">>> Error, expected 2+1 arguments, got %d\n", argc);
        printf(">>> Terminating.\n");
        exit(0);
    }

    NumItems = atoi(argv[1]);
    range = atoi(argv[2]);
    if (NumItems < 1) {
        printf(">>> Error, number of items must be positive.\n");
        printf(">>> Terminating.\n");
        exit(0);
    }

    // create producer threads
    for (int t = 0; t < NUM_P_THREADS; t++){
        int* t_ptr = &t;
        
        int returnVal = pthread_create(&tidP[t], NULL, ProdFunction, (void*)t_ptr);
        if (returnVal){
            printf("ERROR: return code from pthread_create() is %d\n", returnVal);
            exit(-1);
        }
        
    }

    // create consumer threads
    for (int t = 0; t < NUM_C_THREADS; t++){
        int* t_ptr = &t;
        
        int returnVal = pthread_create(&tidC[t], NULL, ConsFunction, t_ptr);
        if (returnVal){
            printf("ERROR: return code from pthread_create() is %d\n", returnVal);
            exit(-1);
        }
    }
    


    // delete/join consumer threads
    for (int t = 0; t < NUM_C_THREADS; t++){
        pthread_join(tidC[t], NULL);
    }
    
    // delete/join producer threads
    for (int t = 0; t < NUM_P_THREADS; t++){
        pthread_join(tidP[t], NULL);
    }
    
    // calculate global/total min,max,avg
    // avg
    totAvg = 0.0;
    for (int i = 0; i < NUM_C_THREADS; i++){
        totAvg += avg[i] * 1000; // get sum from avg
    }
    totAvg /= NUM_C_THREADS; 
    // min/max
    totMin = min[0];
    totMax = max[0];

    for (int i = 1; i < NUM_C_THREADS; i++){
        if (min[i] < totMin) 
            totMin = min[i];
        if (max[i] > totMax)
            totMax = max[i];
    }

    for (int i = 0; i < NUM_C_THREADS; i++){
        printf("RESULTS[%d]: Min=%d, Max=%d, Avg=%f\n", i, min[i], max[i], avg[i]);
    }
    printf("RESULTS[*]: Min=%d, Max=%d, Avg=%f\n", totMin, totMax, totAvg);
    
    return 0;
}

void* ProdFunction(void* threadID)
{
    int* myIDptr = (int*)threadID;
    int myID = *myIDptr;
    srand(myID);

    // produce items
    pthread_mutex_lock(&lock);
    for (int i=0; i < NumItems; i++){
        // while () { // buffer full
        //     pthread_cond_signal(&full);
        //     pthread_cond_wait(&empty, &lock);
        // }
        pthread_cond_wait(&empty, &lock);
        
        pthread_mutex_lock(&lock);
        buf[NextIn] = rand() % range;
        NextIn = (NextIn + 1) % 10;

        pthread_mutex_unlock(&lock);
    } 

    printf("DEBUG +++ Producer [%d] finished.\n",myID);

    pthread_exit(0);
}

void* ConsFunction(void *param)
{
    int* myIDptr = (int*)param;
    int myID = *myIDptr;
    // define and init local variables to first num in buffer
    int myMin, myMax, mySum;


    pthread_mutex_lock(&lock);
    while (NextIn == NextOut) { // buffer empty
        pthread_cond_signal(&empty);
        pthread_cond_wait(&full, &lock);
    }
    myMin = buf[0];
    myMax = buf[0];
    mySum = buf[0];

    // loop through buffer
    for (int i = 1; i < NumItems; i++){
        while (NextIn == NextOut) { // buffer empty
            pthread_cond_signal(&empty);
            pthread_cond_wait(&full, &lock);
        }
        NextOut = (NextOut + 1) % 10;

        pthread_mutex_unlock(&lock);

        // local variable 
        int consumedNum = buf[i];

        if (consumedNum < myMin)
            myMin = consumedNum;
        if (consumedNum > myMax)
            myMax = consumedNum;
        
        mySum += consumedNum;
    }

    double myAvg = static_cast<double>(mySum) / NumItems; 

    // push local vars to global
    avg[myID] = myAvg;
    min[myID] = myMin;
    max[myID] = myMax;

    // print results
    printf("RESULTS{%d}: Min=%d, Max=%d, Avg=%f\n", myID, myMin, myMax, myAvg);

    pthread_exit(0);
}
