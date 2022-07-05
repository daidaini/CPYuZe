#pragma once

#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>

#define MAX_EVENT 20
#define READ_BUF_LEN 256


bool DoPrepare();

int SetNonblocking(int sock);

char rot13_char(char c);

void HandleEpollUsing();

void OnEpAccept();
