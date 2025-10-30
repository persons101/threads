/*
*       Purpose:    Demonstrate critical sections and semaphores by creating multiple producer and consumer threads.
*       
*       Author:     William Mason
*       Class:      CS3080
*       Term:       Fall 2025
*/

// INCLUDES
#include <iostream>
#include <pthread.h>

// GLOBALS
#define S 10                        // Buffer Size
#define NUM_C_THREADS 6             // consumer thread count
#define NUM_P_THREADS 6             // producer thread count

int buf[S];
int min_array[NUM_C_THREADS];             // global min array
int max_array[NUM_C_THREADS];             // global max array
double avg_array[NUM_C_THREADS];          // global avg array
int NextIn = 0, NextOut = 0;        // Current buffer access
int NumItems;                       // number of items for each producer to produce
int range = 1000;                   // range for random numbers
int bufCount = 0;                   // number of items currently in the buffer

pthread_mutex_t lock;                // mutex lock
pthread_cond_t empty, full;         // conditional variables
//END GLOBALS

void *ProdFunction(void *tid);
void *ConsFunction(void *tid);

int main(int argc, char*argv[]){
    // initialize conditions and variables
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&full, NULL);
    int totMax, totMin;
    double totAvg;

    // declare pthread ID arrays (IDs)
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
        int returnVal = pthread_create(&tidP[t], NULL, ProdFunction, (void*)(size_t)t);
        if (returnVal){
            printf("ERROR: return code from pthread_create() is %d\n", returnVal);
            exit(-1);
        }
    }

    // create consumer threads
    for (int t = 0; t < NUM_C_THREADS; t++){
        int returnVal = pthread_create(&tidC[t], NULL, ConsFunction, (void*)(size_t)t);
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
        totAvg += (avg_array[i] * NumItems); // get sum from avg
    }
    totAvg /= (NUM_C_THREADS * NumItems); 

    // min/max
    totMin = min_array[0];
    totMax = max_array[0];

    for (int i = 1; i < NUM_C_THREADS; i++){
        if (min_array[i] < totMin) 
            totMin = min_array[i];
        if (max_array[i] > totMax)
            totMax = max_array[i];
    }

    printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
    
    // NOT A PART OF THE LAB SPEC
    // Prints all consumers from main, confirms that numbers are the same
    //  
    // for (int i = 0; i < NUM_C_THREADS; i++){
    //     printf("RESULTS[m%d]: Min=%d, Max=%d, Avg=%f\n", i, min_array[i], max_array[i], avg_array[i]);
    // }
    // 

    printf("RESULTS[m*]: Min=%d, Max=%d, Avg=%f\n", totMin, totMax, totAvg);
    
    return 0;
}

void *ProdFunction(void *tid)
{
    // init RNG seed
    int myID = *(int*)tid;
    srand(myID);

    // produce items
    for (int i=0; i < NumItems; i++){
        pthread_mutex_lock(&lock);
        //Critical Section
            // wait for space in buffer
            while (bufCount >= S) { // buffer full
                pthread_cond_wait(&empty, &lock);
            }
        
            // add number to buffer at next available spot
            buf[NextIn] = rand() % range;
            NextIn = (NextIn + 1) % 10;
            bufCount++; 
            pthread_cond_signal(&full);
        //End Critical Section
        pthread_mutex_unlock(&lock);
    } 

    //printf("DEBUG +++ Producer [%d] finished.\n",myID);

    pthread_exit(0);
}

void *ConsFunction(void *tid)
{
    // init myID
    int myID = *(int*)tid;
    
    // define local variables
    int localMin, localMax, localSum, consumedNum;    

    // loop through buffer
    for (int i = 0; i < NumItems; i++){
        pthread_mutex_lock(&lock);
        // Critical Section
            // wait for buffer to not be empty
            while (bufCount <= 0) { // buffer empty
                pthread_cond_wait(&full, &lock);
            }
        
            // copy from buffer to local variable 
            consumedNum = buf[NextOut];
            NextOut = (NextOut + 1) % 10;
            bufCount--;
            pthread_cond_signal(&empty);
        // End Critical Section
        pthread_mutex_unlock(&lock);
        
        if (i == 0){ // initialize min and max with first item consumed
            localMin = consumedNum;
            localMax = consumedNum;
        }

        // update localMin, localMax, localAvg
        if (consumedNum < localMin)
            localMin = consumedNum;
        if (consumedNum > localMax)
            localMax = consumedNum;
        
        localSum += consumedNum;
    }

    double localAvg = static_cast<double>(localSum) / NumItems; 

    // push local vars to global
    avg_array[myID] = localAvg;
    min_array[myID] = localMin;
    max_array[myID] = localMax;

    // print results
    printf("RESULTS[c%d]: Min=%d, Max=%d, Avg=%f\n", myID, localMin, localMax, localAvg);

    pthread_exit(0); 
}
