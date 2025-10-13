#include <iostream>
#include <pthread.h>


void* ProdFunction(void* threadID);
void* ConsFunction(void *threadID);

// GLOBALS
#define BUF_SIZE 10
int buf[BUF_SIZE];
#define busyItems 10000000  // number of loops for the busy loop
#define range 1000       // range for random numbers
int sum = 0;                  // global sum
int numCount = 0;             // global count of numbers produced
int min;                      // global min
int max;                      // global max
#define NUM_C_THREADS 1    // consumer thread count
#define NUM_P_THREADS 1    // producer thread count
int NextIn = 0, NextOut = 0;


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
    
    double avg = static_cast<double>(sum) / numCount;
    printf("RESULTS: Min=%d, Max=%d, Avg=%f\n", min, max, avg);
    
    return 0;
}

void* ProdFunction(void* threadID)
{
    pthread_t myID = pthread_self();
    srand(myID);
    for (int i=0; i < BUF_SIZE; i++){
        buf[i] = rand() % range;
        numCount++;
    } 



    pthread_exit(0);
}

void* ConsFunction(void *threadID)
{
    //BUSY WORK
    int BUSY_SUM = 0;
    for (int i=0; i < busyItems; i++){
        BUSY_SUM += i;
    }
    //END BUSY WORK

    // define and init local variables to first num in buffer
    int my_min, my_max, my_sum;
    my_min = buf[0];
    my_max = buf[0];
    my_sum = buf[0];

    // loop through buffer
    for (int i = 1; i < BUF_SIZE; i++){
        int consumedNum = buf[i];

        if (consumedNum < my_min)
            my_min = consumedNum;
        if (consumedNum > my_max)
            my_max = consumedNum;
        
        my_sum += consumedNum;
    }

    // push local vars to global
    sum = my_sum;
    min = my_min;
    max = my_max;

    pthread_exit(0);
}
