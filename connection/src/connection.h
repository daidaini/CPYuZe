#pragma once

#include "public.h"

class connection
{
public:
    connection();
    ~connection();

    bool connect(string ip, unsigned short port, string user, string pass, string dbname);

    //更新操作: insert、delete、update
    bool update(const string& sql);

    //查询 select
    MYSQL_RES* query(const string& sql);

    //刷新连接的其实空闲时间
    void refresh_alivetime();

    //返回存活时间
    clock_t get_alivetime();

private:
    MYSQL* _conn;   //表示和MySQL Server的一个连接
    clock_t _alivetime;


};