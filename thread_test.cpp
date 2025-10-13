#include <iostream>
#include <assert.h>

namespace sCode {
    #include "thread.cpp"
}
#ifndef PTHREAD_THREADS_MAX // checks if <pthread.h> is included
#include <pthread.h>
#define PTHREAD_MISSING true
#else 
#define PTHREAD_MISSING false
#endif

class Tester {
public:
    void printArray(int arr[]){
        std::cout << "g_buf[" << sCode::g_bufferSize << "] = { ";
        for (int i = 0; i < sCode::g_bufferSize - 1; i++){
            std::cout << sCode::g_buf[i] << ", ";
        }
        if (sCode::g_bufferSize > 0) 
            std::cout << sCode::g_buf[sCode::g_bufferSize - 1];
        std::cout << " }\n";
    }
};

// TESTER GLOBALS
const int g_tbufferSize = 10;    // buffer size
int g_tbuf[g_tbufferSize];        // shared int array buffer
const int g_tbusyItems = 10000000;  // number of loops for the busy loop
const int g_trange = 1000;       // range for random numbers
int g_tsum = 0;                  // global sum
int g_tnumCount = 0;             // global count of numbers produced
int g_tmin;                      // global min
int g_tmax;                      // global max
const int tNUM_C_THREADS = 1;    // consumer thread count
const int tNUM_P_THREADS = 1;    // producer thread count


int main(){
    if (PTHREAD_MISSING){
        printf("<pthread.h> is not included!");
        exit(1);
    }
    Tester tester;

    
    tester.printArray(sCode::g_buf);

    
    
    sCode::main();
    
    sCode::g_buf[0] = 1;
    tester.printArray(sCode::g_buf);

    return 0;
}