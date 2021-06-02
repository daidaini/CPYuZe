#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>  //
#include <functional>

class fixed_thread_pool
{
public:
    explicit fixed_thread_pool(size_t thread_count)
    :data_(std::make_shared<res_info>())
    {
        for(size_t i=0; i<thread_count; ++i)
        {
            std::thread([res= data_]
            {
                std::unique_lock<std::mutex> lk(res->mtx_);
                for(;;)
                {
                    if(!res->tasks_.empty())
                    {
                        auto current = std::move(res->tasks_.front());
                        res->tasks_.pop();  
                        lk.unlock();
                        current();
                        lk.lock();
                    }
                    else if(res->is_shutdown)
                    {
                        break;
                    }
                    else{
                        res->cond_.wait(lk);
                    }
                }
            }
            ).detach();
        }
    }

    //默认构造
    fixed_thread_pool() = default;
    //默认移动构造
    fixed_thread_pool(fixed_thread_pool&&) = default;

    ~fixed_thread_pool()
    {
        if(data_ != nullptr)
        {
            std::lock_guard<std::mutex> guard(data_->mtx_);
            data_->is_shutdown = true;
        }
        data_->cond_.notify_all();
    }

    /*
    添加任务，并唤醒一个线程（用来执行任务）
    */
    template<typename Func>
    void execute(Func&& task)
    {
        {
            std::lock_guard<std::mutex> guard(data_->mtx_);
            data_->tasks_.emplace(std::forward<Func>(task));
        }
        data_->cond_.notify_one();
    }

private:
    struct res_info
    {
        std::mutex mtx_;
        std::condition_variable cond_;
        bool is_shutdown = false;
        //线程函数队列
        std::queue<std::function<void()>> tasks_;
    }; 

    std::shared_ptr<res_info> data_;
};


/*
current()前后的锁，原因：
第一个unlock 是解的守护的 unique_lock；
执行 current() 的时候，不需要锁（对于线程池来说，只有在执行任务和等待任务的时候是游离状态，不需要锁；其他时候与线程池读写共享数据时，都必须是加锁状态）
所以，执行完current()后，继续lock
*/