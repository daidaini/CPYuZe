
#include "client.h"

ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8080);
void run_client(const string& client_name){
    talk_to_svr client(client_name);

    try{
        client.connect(ep);
        client.loop();
    }
    catch(system::system_error& err)
    {
        cout << "client terminated\n";
    }
}

int main()
{
    run_client("devinbo");


    getchar();
}
