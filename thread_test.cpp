#include <iostream>
#include <assert.h>
#include <exception>

namespace sCode {
    #include "thread.cpp"
}
#ifndef PTHREAD_THREADS_MAX // checks if <pthread.h> is included
#include <pthread.h>
#define PTHREAD_MISSING true
#else 
#define PTHREAD_MISSING false
#endif

#ifndef S
#error Variable "S" (buffer size) not defined with #define.  
#elif S < 1
#error Variable "S" (buffer size) cannot be 0 or negative 
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
        std::cout << "buf[" << S << "] = { ";
        for (int i = 0; i < S - 1; i++){
            std::cout << array << ", ";
        }
        if (S > 0) 
            std::cout << array[S - 1];
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
        printf(">>> ERROR: <pthread.h> is not included in student file!");
    }
    Tester tester;
    tester.setArray(sCode::buf);
    
    tester.printArray();

    try {
        
        sCode::main();

    }
    catch {
        
    }
    
    sCode::buf[0] = 1;
    tester.printArray();

    return 0;
}