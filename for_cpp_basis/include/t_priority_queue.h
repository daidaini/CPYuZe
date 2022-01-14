#pragma once

#include "comm.h"

inline namespace priority_queue_test
{
    //处理状态
    enum class ProcessState
    {
        //已处理
        Done,
        //未处理
        Undo,
        //准备好，暂不处理
        Ready
    };

    struct CancelRejectOrder
    {
        ProcessState State;
        std::string Remark;

        bool operator<(const CancelRejectOrder &rhs) const
        {
            return rhs.State == ProcessState::Undo;
        }
    };

    void test()
    {
        priority_queue<CancelRejectOrder> my_pq;
        my_pq.emplace(CancelRejectOrder{ProcessState::Done, "frist"});
        my_pq.emplace(CancelRejectOrder{ProcessState::Undo, "second"});
        my_pq.emplace(CancelRejectOrder{ProcessState::Ready, "third"});
        my_pq.emplace(CancelRejectOrder{ProcessState::Done, "four"});

        while(!my_pq.empty())
        {
            auto &data = my_pq.top();
            cout << static_cast<int>(data.State) << " : " << data.Remark << endl;
            my_pq.pop();
        }
    }
}
