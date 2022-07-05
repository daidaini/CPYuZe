#pragma once

#include "proxy_protocol.h"

//#include <boost/asio/io_service.hpp>
//#include <boost/asio/write.hpp>
//#include <boost/asio/buffer.hpp>
//#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

#include <string>
#include <iostream>
#include <functional>
#include <utility>
#include <thread>
#include <chrono>
#include <algorithm>
#include <memory>

#include <time.h>
using namespace std;
using namespace boost;
using namespace boost::asio;

using boost::asio::ip::tcp;

#define USE_BIND 1 // use std::bind or lambda
enum
{
    BUF_SIZE = 1024
};

io_service service;
class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket) : socket_(std::move(socket))
    {
    }

    void Start()
    {
        DoRead();
    }

    void DoRead()
    {
        socket_.async_read_some(asio::buffer(buffer_),
                                std::bind(
                                    &Session::OnRead, shared_from_this(),
                                    std::placeholders::_1, std::placeholders::_2));
    }

    void DoWrite(size_t length)
    {
        asio::async_write(
            socket_,
            asio::buffer(buffer_, length),
            std::bind(&Session::OnWrite, shared_from_this(),
                      std::placeholders::_1,
                      std::placeholders::_2));
    }

    void OnRead(system::error_code ec, size_t length)
    {
        cout << "OnRead\n";
        if (!ec)
        {
            if(client_ip_.empty())
            {
                ParseClientIp((uint8_t*)buffer_, strlen(buffer_));
                if(client_ip_.empty())
                    client_ip_ = "some ip";
            }
            else
                cout << "Recv data : " << buffer_ << endl;
            DoWrite(length);
        }
        else
        {
            if (ec == asio::error::eof)
            {
                std::cerr << "Socket read EOF: " << ec.message() << std::endl;
            }
            else if (ec == asio::error::operation_aborted)
            {
                std::cerr << "Socket operation aborted: " << ec.message() << endl;
            }
            else
            {
                std::cerr << "Socket read error: " << ec.message() << endl;
            }
        }
    }

    void OnWrite(system::error_code ec, size_t length)
    {
        cout << "OnWrite\n";
        boost::ignore_unused(length);
        if (!ec)
        {
            DoRead();
        }
    }

private:
    void ParseClientIp(uint8_t *buffer, uint32_t buffer_length)
    {
        if (!client_ip_.empty())
            return;

        pp_info_t pp_info_out;
        int32_t rc = pp_parse_hdr(buffer, buffer_length, &pp_info_out);
        if (!rc)
        {
            printf("Not a PROXY protocol header\n");
        }
        else if (rc < 0)
        {
            fprintf(stderr, "pp_parse_hdr() failed: %s", pp_strerror(rc));
            //printf("pp_parse_hdr() failed: %s", pp_strerror(rc));
        }
        else
        {
            printf("%d bytes PROXY protocol header: %s %s %hu %hu\n",
                   rc,
                   pp_info_out.src_addr, pp_info_out.dst_addr,
                   pp_info_out.src_port, pp_info_out.dst_port);
            client_ip_ = pp_info_out.src_addr;
        }
        /* ALWAYS clear the pp_info after a call to pp_parse_hdr() */
        pp_info_clear(&pp_info_out);
    }

private:
    ip::tcp::socket socket_;
    //std::array<char, BUF_SIZE> buffer_;
    char buffer_[BUF_SIZE]{};

    std::string client_ip_;
};

class Server
{
public:
    Server(boost::asio::io_service &service, std::uint16_t port)
        : acceptor_(service, ip::tcp::endpoint(tcp::v4(), port))
    {
        DoAccept();
    }

private:
    void DoAccept()
    {
        acceptor_.async_accept(
            [this](system::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<Session>(std::move(socket))
                        ->Start();
                }
                DoAccept();
            });
    }

    tcp::acceptor acceptor_;
};