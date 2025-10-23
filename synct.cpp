#include <iostream>
#include <pthread.h>

void* ProdFunction(void* tid);
void* ConsFunction(void *tid);

// GLOBALS
#define S 10
int buf[S] = {-1};
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
int bufCount = 0;                   // number of items currently in the buffer
int debugAllItemsProduced = 0;

pthread_mutex_t lock;// = PTHREAD_MUTEX_INITIALIZER; // mutex lock
pthread_cond_t empty, full;         // conditional variables


//END GLOBALS

int main(int argc, char*argv[]){
    // initializing
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&full, NULL);
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
        int* t_ptr = new int(t);
        
        int returnVal = pthread_create(&tidP[t], NULL, ProdFunction, t_ptr);
        if (returnVal){
            printf("ERROR: return code from pthread_create() is %d\n", returnVal);
            exit(-1);
        }
        
    }

    // create consumer threads
    for (int t = 0; t < NUM_C_THREADS; t++){
        int* t_ptr = new int(t);
        
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
        printf("RESULTS[m%d]: Min=%d, Max=%d, Avg=%f\n", i, min[i], max[i], avg[i]);
    }
    printf("RESULTS[*]: Min=%d, Max=%d, Avg=%f\n", totMin, totMax, totAvg);
    
    return 0;
}

void* ProdFunction(void* tid)
{
    // init
    int debugItemsProduced = 0;
    // init RNG seed
    int* myIDptr = (int*)tid;
    int myID = *myIDptr;
    srand(myID);

    // produce items
    // pthread_mutex_lock(&lock);
    for (int i=0; i < NumItems; i++){
        pthread_mutex_lock(&lock);
        //Critical Section
            while (bufCount >= S) { // buffer full
                pthread_cond_wait(&empty, &lock);
            }
        
            buf[NextIn] = rand() % range;
            NextIn = (NextIn + 1) % 10;
            bufCount++; debugItemsProduced++; debugAllItemsProduced++;
            pthread_cond_signal(&full);
        //End Critical Section
        pthread_mutex_unlock(&lock);
    } 

    //printf("DEBUG +++ Producer [%d] finished.\n",myID);

    pthread_exit(0);

    return nullptr; // THIS NEVER EXECUTES, I GOT SICK OF THE COMPILER COMPLAINING ABOUT NO RETURN STATEMENT
}

void* ConsFunction(void *tid)
{
    // init

    // init RNG seed
    int* myIDptr = (int*)tid;
    int myID = *myIDptr;
    
    // define and init local variables to first num in buffer
    int localMin, localMax, localSum, consumedNum;    

    // loop through buffer
    for (int i = 0; i < NumItems; i++){
        pthread_mutex_lock(&lock);
        // Critical Section
        while (bufCount <= 0) { // buffer empty
            pthread_cond_wait(&full, &lock);
        }
        
            // local variable 
            consumedNum = buf[NextOut];
            NextOut = (NextOut + 1) % 10;
            bufCount--;
            pthread_cond_signal(&empty);
        // End Critical Section
        pthread_mutex_unlock(&lock);
        

        if (consumedNum < localMin)
            localMin = consumedNum;
        if (consumedNum > localMax)
            localMax = consumedNum;
        
        localSum += consumedNum;
    }

    double localAvg = static_cast<double>(localSum) / NumItems; 

    // push local vars to global
    avg[myID] = localAvg;
    min[myID] = localMin;
    max[myID] = localMax;

    // print results
    printf("RESULTS[c%d]: Min=%d, Max=%d, Avg=%f\n", myID, localMin, localMax, localAvg);

    pthread_exit(0); 

    return nullptr; // THIS NEVER EXECUTES, I GOT SICK OF THE COMPILER COMPLAINING ABOUT NO RETURN STATEMENT
}
