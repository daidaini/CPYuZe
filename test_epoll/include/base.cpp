#include "base.h"
#include <string.h>
#include <iostream>
#include <fmt/format.h>
#include <thread>


int SetNonblocking(int sock)
{
    int opts = fcntl(sock, F_GETFL);
    if (-1 == opts)
    {
        perror("fcntl(sock,GETFL) failed");
        return -1;
    }

    opts = opts | O_NONBLOCK;
    if (-1 == fcntl(sock, F_SETFL, opts))
    {
        perror("fcntl(sock,SETFL,opts) failed");
        return -1;
    }

    return 0;
}

char rot13_char(char c)
{
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}

#define SERVER_PORT 8090
const char *const local_addr = "192.168.6.25";
sockaddr_in server_addr{};
int listenfd = 0;
//IP地址缓存
char host_buf[NI_MAXHOST]{};
char port_buf[NI_MAXSERV]{};

bool DoPrepare()
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listenfd)
    {
        perror("Open listen socket");
        return false;
    }

    // enable address reuse
    int on = 1;
    int result = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (-1 == result)
    {
        perror("Set socket");
        return false;
    }

    server_addr.sin_family = AF_INET;
    //inet_aton(local_addr, &(server_addr.sin_addr));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    result = bind(listenfd, (const sockaddr*)&server_addr, sizeof(server_addr));
    if (-1 == result)
    {
        perror("Bind port\n");
        return false;
    }

    result = SetNonblocking(listenfd);
    if (-1 == result)
    {
        return false;
    }

    result = listen(listenfd, 200);
    if (-1 == result)
    {
        perror("start listen");
        return false;
    }

    return true;
}

void OnEpAccept(int epfd)
{
    while (true)
    {
        sockaddr in_addr = {0};
        socklen_t in_addr_len = sizeof(in_addr);

        int accp_fd = accept(listenfd, &in_addr, &in_addr_len);
        if (-1 == accp_fd)
        {
            perror("Accept");
            break;
        }

        int __result = getnameinfo(&in_addr, sizeof(in_addr),
                                   host_buf, sizeof(host_buf),
                                   port_buf, sizeof(port_buf),
                                   NI_NUMERICHOST | NI_NUMERICSERV);

        if (0 == __result)
        {
            //printf("New connection : host = %s, port = %s\n", host_buf, port_buf);
            fmt::print("New connection : host = {}, port = {}\n", host_buf, port_buf);
        }

        __result = SetNonblocking(accp_fd);
        if (-1 == __result)
        {
            return;
        }

        epoll_event ev{};
        ev.data.fd = accp_fd;
        ev.events = EPOLLIN | EPOLLET;
        //为新的socket 设置 epoll事件
        __result = epoll_ctl(epfd, EPOLL_CTL_ADD, accp_fd, &ev);
        if (-1 == __result)
        {
            perror("epoll_ctl");
            return;
        }
    }
}


void HandleEpollUsing()
{
    int epfd = epoll_create1(0);
    if (1 == epfd)
    {
        perror("Create epoll instance");
        return;
    }

    epoll_event ev{};
    ev.data.fd = listenfd;
    ev.events = EPOLLIN | EPOLLET; //边缘触发选项

    //设置epoll的事件
    int result = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
    if (-1 == result)
    {
        perror("Set epoll_ctl");
        return;
    }

    epoll_event ep_events[MAX_EVENT]{};
    for (;;)
    {
        //等待事件
        int wait_count = epoll_wait(epfd, ep_events, MAX_EVENT, -1);
        for (int i = 0; i < wait_count; ++i)
        {
            uint32_t events = ep_events[i].events;

            //判断epoll是否发生错误
            if (events & EPOLLERR || events & EPOLLHUP || (!events & EPOLLIN))
            {
                printf("Epoll has error[%X (%d)]\n", events, events);
                close(ep_events[i].data.fd);
                continue;
            }
            else if (listenfd == ep_events[i].data.fd)
            {
                //listen 的 文件描述符事件触发，为accept事件
                OnEpAccept(epfd);
                continue;
            }
            else
            {
                //其余事件为 file descirber 可以读取
                bool done = false;
                //因为采用边缘触发 ，所以这个需要使用循环。
                //如果不循环，程序无法完全读取到缓存去里的数据内容

                for (;;)
                {
                    char buf[READ_BUF_LEN]{};
                    ssize_t result_len = read(ep_events[i].data.fd, buf, sizeof(buf));
                    if(-1 == result_len)
                    {
                        done = true;
                        if(EAGAIN != errno)
                        {
                            perror("Read data");
                        }
                        //printf("-1 == result_len break\n");
                        break;
                    }
                    else if(0 == result_len)
                    {
                        done = true;
                        printf("(0 == result_len; break\n");
                        break;
                    }
                    std::cout << '[' << std::this_thread::get_id() << ']';
                    fmt::print(buf);
                    //write(STDOUT_FILENO, buf, result_len);
                    char delim[] = "--------------------------\n";
                    write(STDOUT_FILENO, delim, strlen(delim));
                }
                if(!done)
                {
                    printf("Close connection\n");
                    close(ep_events[i].data.fd);
                }
            }
        }
    }
    close(epfd);
}