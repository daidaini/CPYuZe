#pragma once

#include <memory>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <functional>

#include "connection.h"

class connection_pool
{
public:
    static connection_pool& getinstance();

    //获取连接
    shared_ptr<connection> getconnection();

private:
    connection_pool();

    ~connection_pool() = default;

    //从配置文件中加载配置项
    bool load_config_file();

    //单独运行一个线程，用于生产连接
    void produce_connection_task();

    //单独运行一个线程，用于删除扫描队列中超时的连接
    void scan_connection_task();

private:
    //mysql 登录信息
    string _ip = "101.226.207.176";
    unsigned short _port = 3306;
    string _user = "moni";
    string _password = "Moni@1234";
    string _dbname = "moni";

    //连接池 连接的初始量
    int _init_size = 10;
    //连接池 最大连接量
    int _max_size = 1024;

    //连接池最大空闲时间 s
    int _max_idle_time = 60;
    //最大超时时间 ms
    int _connettion_timeout = 100;

    queue<connection*> _connection_que;

    mutex _queue_mutex;

    //记录创建的connection连接的总数
    atomic_int _connection_cnt;  

    condition_variable _cv;
};