#include "thread_pool.h"

#include <string>
#include <thread>
#include <iostream>

#include <unistd.h>

int main()
{
    using namespace std;

    fixed_thread_pool my_pool(3);

    string src = "线程测试开始";
    
    my_pool.execute([]()
        {
            for (int i = 0; i < 1000; ++i)
            {
                printf("a,");
                usleep(1);
            }
        }
    );

    my_pool.execute([src]()
        {
            cout << src << endl;
            for (int i = 0; i < 1000; ++i)
            {
                printf("b,");
                usleep(2);
            }
        }
    );

    my_pool.execute([src]()
        {
            cout << src << endl;
            for (int i = 0; i < 1000; ++i)
            {
                printf("c,");
                usleep(3);
            }
        }
    );
    
    sleep(5);
    cout << "exit\n";
}