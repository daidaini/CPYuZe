#include "connection_pool.h"

connection_pool &connection_pool::getinstance()
{
    static connection_pool instance;
    return instance;
}

bool connection_pool::load_config_file()
{
    /*
        ...
    */
   return true;
}

connection_pool::connection_pool()
{
    if(!load_config_file())
        return;

    for (int i = 0; i < _init_size; i++)
    {
        connection * p = new connection();

        if(!p->connect(_ip, _port, _user, _password, _dbname))
        {
            LOG("mysql connect failed");
            continue;
        }

        _connection_que.push(p);

        p->refresh_alivetime();

        ++_connection_cnt;
    }
    //启动 生产连接的线程
    thread(std::bind(&connection_pool::produce_connection_task, this)).detach();
    //启动 扫描超时的空闲连接 的线程
    thread(std::bind(&connection_pool::scan_connection_task, this)).detach();
}

shared_ptr<connection> connection_pool::getconnection()
{
    unique_lock<mutex> lk(_queue_mutex);

    while (_connection_que.empty())
    {
        if(cv_status::timeout == _cv.wait_for(lk, chrono::milliseconds(_connettion_timeout)))
        {
            if(_connection_que.empty())
            {
                LOG("获取空闲连接超时了... 获取连接失败");
                return nullptr;
            }
        }
    }

    /*对于shared_ptr而言，出作用域之后就会自动析构，析构时释放连接。
    * 我们需要的是回收连接，而不是释放，所以需要自定义删除器
    */
    shared_ptr<connection> sp(_connection_que.front(), 
        [&](connection* pcon){
            unique_lock<mutex> lk(_queue_mutex);
            pcon->refresh_alivetime();
            _connection_que.push(pcon);
        }
    );    

    _connection_que.pop();
    _cv.notify_all();
    return sp;
}

void connection_pool::produce_connection_task()
{
    do
    {
        unique_lock<mutex> lk(_queue_mutex);

        //线程队列非空，不需要生产，直接阻塞
        while(!_connection_que.empty())
        {
            _cv.wait(lk);
        }

        if(_connection_cnt < _max_size)
        {
            connection * conn = new connection();
            bool ret = conn->connect(_ip, _port, _user, _password, _dbname);

            if(ret)
            {
                conn->refresh_alivetime();
                _connection_que.push(conn);
                _connection_cnt++;
            }
            
        }
        //通知消费者可以获取连接
        _cv.notify_all();

    } while (true);
    
}


void connection_pool::scan_connection_task()
{
    do
    {
        /* 简单地通过sleep来模拟定时处理 */
        this_thread::sleep_for(chrono::seconds(_max_idle_time));

        unique_lock<mutex> lk(_queue_mutex);
        while(_connection_cnt > _init_size)
        {
            connection * p = _connection_que.front();

            if(p->get_alivetime() > (1000 * _max_idle_time))
            {
                _connection_que.pop();
                _connection_cnt--;
                delete p;
            }
            else{
                break;
            }
        }

    } while (true);
    
}