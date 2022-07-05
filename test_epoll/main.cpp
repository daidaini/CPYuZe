#include "include/base.h"

#include <iostream>
#include <thread>

using namespace std;


int main(int argc, char* argv[])
{

    if(!DoPrepare())
        return 0;

    printf("Start listening\n");

    std::thread t1(HandleEpollUsing);
    std::thread t2(HandleEpollUsing);
    
    t1.join();
    t2.join();
    
    return 0;
}