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

int main(int argc, void**argv){
    
    pthread_t tidC[NUM_C_THREADS]; // array with consumer thread IDs
    pthread_t tidP[NUM_P_THREADS]; // array with producer thread IDs
    
    // command line arguments
    if (argc != 2) {
        printf(">>> Error, expected 4 arguments.\n");
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
    pthread_t myID = pthread_self();
    srand(myID);
    for (int i=0; i < NumItems; i++){
        
        buf[i] = rand() % range;
        NextIn = (NextIn + 1) % 10;
    } 



    pthread_exit(0);
}

void* ConsFunction(void *param)
{
    int* myIDptr = (int*)param;
    int myID = *myIDptr;
    // define and init local variables to first num in buffer
    int myMin, myMax, mySum;
    myMin = buf[0];
    myMax = buf[0];
    mySum = buf[0];

    // loop through buffer
    for (int i = 1; i < NumItems; i++){
        NextOut = (NextOut + 1) % 10;

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
