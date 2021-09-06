#pragma once

#include "comm.h"

inline namespace test_virtual
{
    /*验证虚析构函数的作用及调用顺序
    */
    class Base
    {
    public:
        Base()
        {
            cout << "Base()\n";
        }

        virtual ~Base()
        {
            cout << "~Base()\n";
        }

        virtual void DoSomething()
        {
            cout << "Base::DoSomething\n";
        }
    };

    class Derive : public Base
    {
    public:
        Derive()
        {
            m_Data.push_back(10);
            m_Data.push_back(20);
            cout << "Derive()\n";
        }

        ~Derive()
        {
            cout << "~Derive()\n";
        }

        void DoSomething()
        {
            cout << "Derive::DoSomething\n";
        }

        vector<int> m_Data;
    };

    void testing_virtual_destructor()
    {
        Base *obj = new Derive;
        obj->DoSomething();
        delete obj;
    }
}