#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>
#include <atomic>
#include <chrono>

/*�����������ٰ�һ�㣬�����ֻ�ǹ̶��������̳߳أ��µ��࣬������������Ѿ��̳߳���������̴߳���
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

    //Ĭ�Ϲ���
    FixedThreadPool() = default;
    //Ĭ���ƶ�����
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
    ������񣬲�����һ���̣߳�����ִ������
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
        //�̺߳�������
        std::queue<std::function<void()>> _tasks;
        std::atomic_int _free_count;
    };

    std::shared_ptr<res_info> _data;
    size_t _count;
};


/*
current()ǰ�������ԭ��
��һ��unlock �ǽ���ػ��� unique_lock��
ִ�� current() ��ʱ�򣬲���Ҫ���������̳߳���˵��ֻ����ִ������͵ȴ������ʱ��������״̬������Ҫ��������ʱ�����̳߳ض�д��������ʱ���������Ǽ���״̬��
���ԣ�ִ����current()�󣬼���lock
*/