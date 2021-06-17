#pragma once

#include "comm.h"


void th_func(std::future<int> & fut)
{
    int x = fut.get(); //阻塞

    cout << "value: " << x <<endl;
}

void test_use_promise()
{
    std::promise<int> prom;
    
    std::future<int> fut = prom.get_future();

    std::thread t(th_func, std::ref(fut));

    prom.set_value(144);

    t.join();
}

int th_func_2(int in)
{
    this_thread::sleep_for(chrono::seconds(2));
    cout << "in thread \n";
    return in * 2;
}

void test_use_packaged_task()
{
    std::packaged_task<int(int)> task(th_func_2);

    std::future<int> fut = task.get_future();

    std::thread(std::move(task), 5).detach();

    int result = fut.get();

    cout << "result = " << result <<endl;
}



void test_use_async()
{
    std::future<int> fut = std::async(launch::async, std::bind(th_func_2, 21));

    //int result = fut.get();

    //cout << "result = " << result <<endl;

    cout << "in use async\n";
}