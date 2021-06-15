#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>
#include <atomic>
#include <chrono>

/*考虑在上面再包一层，这个类只是固定数量的线程池，新的类，处理任务队列已经线程池外的新增线程处理
*/
class FixedThreadPool
{
public:
    explicit FixedThreadPool(size_t thread_count)
        :_data(std::make_shared<res_info>()), _count(thread_count)
    {
        _data->_free_count.store(static_cast<int>(thread_count));
        for (size_t i = 0; i < thread_count; ++i)
        {
            std::thread([res = _data]
                {
                    std::unique_lock<std::mutex> lk(res->_mtx);
                    for (;;)
                    {
                        if (!res->_tasks.empty())
                        {
                            auto current = std::move(res->_tasks.front());
                            res->_tasks.pop();
                            lk.unlock();
                            current();
                            //std::this_thread::sleep_for(std::chrono::seconds(1));
                            res->_free_count += 1;
                            lk.lock();
                        }
                        else if (res->_is_shutdown)
                        {
                            break;
                        }
                        else {
                            res->_cond.wait(lk);
                        }
                    }
                }
            ).detach();
        }
    }

    //默认构造
    FixedThreadPool() = default;
    //默认移动构造
    FixedThreadPool(FixedThreadPool&&) = default;

    ~FixedThreadPool()
    {
        if (_data != nullptr)
        {
            std::lock_guard<std::mutex> guard(_data->_mtx);
            _data->_is_shutdown = true;
        }
        _data->_cond.notify_all();
    }

    /*
    添加任务，并唤醒一个线程（用来执行任务）
    */
    template<typename Func>
    void execute(Func&& task)
    {
        std::lock_guard<std::mutex> guard(_data->_mtx);
        _data->_tasks.emplace(std::forward<Func>(task));
        _data->_free_count -= 1;
        _data->_cond.notify_one();
    }
    
    bool all_occupied()
    {
        return _data->_free_count <= 0;
    }

    bool unoccupied()
    {
        return _data->_free_count == _count;
    }

private:
    struct res_info
    {
        std::mutex _mtx;
        std::condition_variable _cond;
        bool _is_shutdown = false;
        //线程函数队列
        std::queue<std::function<void()>> _tasks;
        std::atomic_int _free_count;
    };

    std::shared_ptr<res_info> _data;
    size_t _count;
};


/*
current()前后的锁，原因：
第一个unlock 是解的守护的 unique_lock；
执行 current() 的时候，不需要锁（对于线程池来说，只有在执行任务和等待任务的时候是游离状态，不需要锁；其他时候与线程池读写共享数据时，都必须是加锁状态）
所以，执行完current()后，继续lock
*/