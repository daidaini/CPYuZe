#include "connection.h"


connection::connection()
{
	// 初始化数据库连接
	_conn = mysql_init(nullptr);
}

connection::~connection()
{
    if(_conn != nullptr)
        mysql_close(_conn);
}

bool connection::connect(string ip, unsigned short port, string user, string pass, string dbname)
{
    MYSQL *p = mysql_real_connect(_conn, ip.c_str(), user.c_str(), pass.c_str(), dbname.c_str(), port, nullptr, 0);

    return p != nullptr;
}

bool connection::update(const string& sql)
{
    if(mysql_query(_conn, sql.c_str()))
    {
        LOG("更新失败:" + sql);
        return false;
    }
    return true;
}

MYSQL_RES* connection::query(const string& sql)
{
    if(mysql_query(_conn, sql.c_str()))
    {
        LOG("查询失败:"+sql);
        return nullptr;
    }
    return mysql_use_result(_conn);
}

void connection::refresh_alivetime()
{
    _alivetime = clock();
}

clock_t connection::get_alivetime()
{
    return clock() - _alivetime;
}