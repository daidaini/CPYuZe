#pragma once

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp>

#include <string>
#include <iostream>
#include <functional>
#include <utility>
#include <thread>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace std::chrono;
using namespace boost;
using namespace boost::asio;


io_service service;
class talk_to_svr
{
public:
    talk_to_svr(const string& usrname):
    sock_(service), started_(true),username_(usrname)
    {

    }

    void connect(ip::tcp::endpoint ep){
        sock_.connect(ep);
    }

    void read_answer(){
        already_read_ = 0;
        //通过read_complete 来确保能读到换行符 \n
        read(sock_, buffer(buff_), boost::bind(&talk_to_svr::read_complete, this, _1, _2));
    }

    std::string username() const { return username_; }

    void loop()
    {
        write("login "s + username_ +"\n"s);
        read_answer();
        while(started_)
        {
            write_request();
            read_answer();
            this_thread::sleep_for(milliseconds(rand() % 7000));
        }
    }

private:
    void write(const std::string& contents){
        sock_.write_some(buffer(contents));
    }

    void write_request(){
        write("ping\n");
    }

    void process_msg()
    {
        string msg(buff_, already_read_);
        if(msg.find("login ") == 0)
            on_login();
        else if(msg.find("ping") == 0)
            on_ping(msg);
        else if(msg.find("client") == 0)
            on_clients(msg);
        else
            std::cerr<<"invalid msg " << msg << endl; 
    }
   
    void on_login()
    {
        do_ask_clients();
    }

    void do_ask_clients()
    {
        write("ask_cleints\n");
        read_answer();
    }

    void on_ping(const string& msg)
    {
        std::istringstream in(msg);
        string answer;
        in >> answer >> answer;
        if(answer == "client_list_changed")
            do_ask_clients();
    }

    void on_clients(const string& msg)
    {
        string clients = msg.substr(8);
        cout << username_ << ", new client list: " <<clients;
    }


    size_t read_complete(const system::error_code& err, size_t bytes)
    {
        if(err)
            return 0;
        bool found = std::find(buff_, buff_+bytes, '\n') < buff_ + bytes;
        //一个一个读，直到读到回车符，不缓存
        return found ? 0 : 1;
    }

private:
    ip::tcp::socket sock_;
    enum{max_msg = 1024};
    size_t already_read_;
    char buff_[max_msg];
    bool started_;
    string username_;
};