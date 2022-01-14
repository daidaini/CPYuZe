#include "server.h"


constexpr std::uint16_t port = 8090;
int main()
{
    asio::io_service service;

    cout << "server start... \n";
    Server server{service , port};

    service.run();

    getchar();
}