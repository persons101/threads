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

#ifndef BUF_SIZE
#error Variable "BUF_SIZE" not defined with #define.  
#elif BUF_SIZE < 1
#error Variable "BUF_SIZE" cannot be 0 or negative 
#endif

#ifdef NUM_C_THREADS // TODO Change this to check for Lab 3 or Lab 4
    #ifndef NUM_C_THREADS
    #error Variable "NUM_C_THREADS" not defined with #define.
    #elif NUM_C_THREADS < 1
    #error Variable "NUM_C_THREADS" must be positive
    #endif

    #ifndef NUM_P_THREADS
    #error Variable "NUM_P_THREADS" not defined with #define.
    #elif NUM_P_THREADS < 1
    #error Variable "NUM_P_THREADS" must be positive
    #endif
#endif

class Tester {
private:
    int* array; //
public:
    bool setArray(int arr[]) { array = arr; }
    int* getArray() const { return array; }

    void printArray(){
        std::cout << "buf[" << BUF_SIZE << "] = { ";
        for (int i = 0; i < BUF_SIZE - 1; i++){
            std::cout << array << ", ";
        }
        if (BUF_SIZE > 0) 
            std::cout << array[BUF_SIZE - 1];
        std::cout << " }\n";
    }
};

// TESTER GLOBALS
#define tBUF_SIZE 10    // buffer size
int tbuf[tBUF_SIZE];        // shared int array buffer
int tsum = 0;                  // global sum
int tnumCount = 0;             // global count of numbers produced
int tmin;                      // global min
int tmax;                      // global max
#define tNUM_C_THREADS 1    // consumer thread count
#define tNUM_P_THREADS 1    // producer thread count


int main(){
    if (PTHREAD_MISSING){
        printf("<pthread.h> is not included!");
        exit(1);
    }
    Tester tester;
    tester.setArray(sCode::buf);
    
    tester.printArray();

    
    
    sCode::main();
    
    sCode::buf[0] = 1;
    tester.printArray();

    return 0;
}