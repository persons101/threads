#include <iostream>


namespace sCode {
#include "thread.cpp"
}

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

int main(){
    Tester tester;

    tester.printArray(sCode::g_buf);

    
    sCode::main();
    
    sCode::g_buf[0] = 1;
    tester.printArray(sCode::g_buf);

    return 0;
}