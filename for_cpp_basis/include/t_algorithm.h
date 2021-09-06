#pragma once

#include "comm.h"

inline namespace test_algorithm
{

    void use_sort()
    {
        vector<string> src {"100", "99", "11" ,"6011", "33" };

        std::sort(src.begin(), src.end(), [](const string& lhs, const string& rhs){return stoi(lhs) < stoi(rhs);});

        for(const auto& item: src)
        {
            cout << item << " , ";
        }
        cout << endl;
    }
}