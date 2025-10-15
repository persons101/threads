#include <iostream>
#include <pthread.h>

void* ProdFunction(void* threadID);
void* ConsFunction(void *threadID);

// GLOBALS
#define S 10
int buf[S];
#define busyItems 10000000  // number of loops for the busy loop
#define range 1000                  // range for random numbers
#define NUM_C_THREADS 6             // consumer thread count
#define NUM_P_THREADS 6             // producer thread count
int min[NUM_C_THREADS];             // global min array
int max[NUM_C_THREADS];             // global max array
double avg[NUM_C_THREADS];          // global avg array
int NextIn = 0, NextOut = 0;        // Current buffer access
int totMax, totMin;
double totAvg;
int NumItems;                       // number of items for each producer to produce
//END GLOBALS

int main(){
    
    pthread_t tidC[NUM_C_THREADS]; // array with consumer thread IDs
    pthread_t tidP[NUM_P_THREADS]; // array with producer thread IDs
    
    // create producer threads
    for (int t = 0; t < NUM_P_THREADS; t++){
        int* t_ptr = &t;
        
        int returnVal = pthread_create(&tidP[t], NULL, ProdFunction, (void*)t);
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

    printf("RESULTS: Min=%d, Max=%d, Avg=%f\n", totMin, totMax, totAvg);
    
    return 0;
}

void* ProdFunction(void* threadID)
{
    pthread_t myID = pthread_self();
    srand(myID);
    for (int i=0; i < S; i++){
        buf[i] = rand() % range;
        NumItems++;
    } 



    pthread_exit(0);
}

void* ConsFunction(void *param)
{
    int* myIDptr = (int*)param;
    int myID = *myIDptr;
    // define and init local variables to first num in buffer
    int my_min, my_max, my_sum;
    my_min = buf[0];
    my_max = buf[0];
    my_sum = buf[0];

    // loop through buffer
    for (int i = 0; i < NumItems; i++){
        int consumedNum = buf[i];

        if (consumedNum < my_min)
            my_min = consumedNum;
        if (consumedNum > my_max)
            my_max = consumedNum;
        
        my_sum += consumedNum;
    }

    double my_avg = static_cast<double>(my_sum) / NumItems; 

    // push local vars to global
    avg[myID] = my_avg;
    min[myID] = my_min;
    max[myID] = my_max;

    pthread_exit(0);
}
