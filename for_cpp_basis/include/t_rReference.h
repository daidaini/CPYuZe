#pragma once

#include "comm.h"

inline namespace TestMoveSemantic
{
    struct ClientUserInfo
    {
        int BranchNo = 0;
        std::string ClientName;
        char IdKind = 0;
        std::string IdNo;
        std::string Mobile;

        ClientUserInfo() = default;
        ~ClientUserInfo() = default;
        ClientUserInfo(ClientUserInfo &&rhs) noexcept
        {
            cout << "ClientUserInfo(ClientUserInfo &&rhs) noexcept" << endl;
            BranchNo = rhs.BranchNo;
            IdKind = rhs.IdKind;

            ClientName = std::move(rhs.ClientName);
            IdNo = std::move(rhs.IdNo);
            Mobile = std::move(rhs.Mobile);
        }

        ClientUserInfo &operator=(ClientUserInfo &&rhs) noexcept
        {
            cout << "ClientUserInfo &operator=(ClientUserInfo &&rhs) noexcept\n";
            BranchNo = rhs.BranchNo;
            IdKind = rhs.IdKind;

            ClientName = std::move(rhs.ClientName);
            IdNo = std::move(rhs.IdNo);
            Mobile = std::move(rhs.Mobile);
            return *this;
        }

        void Print() const
        {
            cout << ClientName << " : " << IdNo << " -- " << Mobile <<endl;
        }
    };

    ClientUserInfo GetClient()
    {
        ClientUserInfo info;
        info.BranchNo = 100;
        info.ClientName = "xiaoming"s;
        info.IdKind = 1;
        info.IdNo = "320684888899993333"s;
        info.Mobile = "17849588232"s;

        return info;
    }

    void UseClientInfo(ClientUserInfo&& client)
    {
        //ClientUserInfo cli(std::forward<ClientUserInfo>(client));
        ClientUserInfo cli(std::move(client));

        cli.Print();
    }

    void TestGetClient()
    {
        //ClientUserInfo  client = GetClient();  //RVO 返回值优化
        ClientUserInfo client;
        client = GetClient(); //移动赋值

        cout << client.ClientName << endl;

        ClientUserInfo client_2 = std::move(client); //移动构造
        //ClientUserInfo client_2{};
        //client_2 = std::move(client);  //移动赋值

        cout << client.ClientName << endl;
        cout << client_2.ClientName << endl;
    }

    void test_move_string()
    {
        string a{"first"};

        string b(std::move(a));
        string c{"123"};
        c = std::move(b);

        cout << "a  = " << a << endl;
        cout << "b  = " << b << endl;
        cout << "c  = " << c << endl;
    }

    void test_rv_param()
    {
        ClientUserInfo client = GetClient();
        cout << "begin\n";
        UseClientInfo(std::move(client));
        cout << "end\n";

        client.Print();
    }
}