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
            DoSomething();
        }

        virtual ~Base()
        {
            cout << "~Base()\n";
            DoSomething();
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
            cout << "Derive()\n";
            DoSomething();
        }

        ~Derive()
        {
            cout << "~Derive()\n";
            DoSomething();
        }

        virtual void DoSomething()
        {
            cout << "Derive::DoSomething\n";
        }
    };

    void testing_virtual_destructor()
    {
        Base *obj = new Derive;
        obj->DoSomething();
        delete obj;
    }
}