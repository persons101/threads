#include <iostream>
#include <pthread.h>


void* ProdFunction(void* threadID);
void* ConsFunction(void *threadID);

// GLOBALS
#define g_bufferSize 10
int g_buf[g_bufferSize];
#define g_busyItems 10000000  // number of loops for the busy loop
#define g_range 1000       // range for random numbers
int g_sum = 0;                  // global sum
int g_numCount = 0;             // global count of numbers produced
int g_min;                      // global min
int g_max;                      // global max
#define NUM_C_THREADS 1    // consumer thread count
#define NUM_P_THREADS 1    // producer thread count


int main(){
    
    pthread_t tidC[NUM_C_THREADS]; // array with consumer thread IDs
    pthread_t tidP[NUM_P_THREADS]; // array with producer thread IDs
    
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
    
    double avg = static_cast<double>(g_sum) / g_numCount;
    printf("RESULTS: Min=%d, Max=%d, Avg=%f\n", g_min, g_max, avg);
    
    return 0;
}

void* ProdFunction(void* threadID)
{
    pthread_t myID = pthread_self();
    srand(myID);
    for (int i=0; i < g_bufferSize; i++){
        g_buf[i] = rand() % g_range;
        g_numCount++;
    } 



    pthread_exit(0);
}

void* ConsFunction(void *threadID)
{
    //BUSY WORK
    int BUSY_SUM = 0;
    for (int i=0; i < g_busyItems; i++){
        BUSY_SUM += i;
    }
    //END BUSY WORK

    // define and init local variables to first num in buffer
    int min, max, sum;
    min = g_buf[0];
    max = g_buf[0];
    sum = g_buf[0];

    // loop through buffer
    for (int i = 1; i < g_bufferSize; i++){
        int consumedNum = g_buf[i];

        if (consumedNum < min)
            min = consumedNum;
        if (consumedNum > max)
            max = consumedNum;
        
        sum += consumedNum;
    }

    // push local vars to global
    g_sum = sum;
    g_min = min;
    g_max = max;

    pthread_exit(0);
}
