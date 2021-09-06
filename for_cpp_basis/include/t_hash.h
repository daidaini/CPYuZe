#pragma once

#include "comm.h"

void find_rehash()
{
    unordered_map<string, int> my_map;
    my_map.rehash(1000);
    for (int i = 0; i < 10; ++i)
    {
        my_map.emplace(to_string(i), i);
    }

    cout << my_map.load_factor() << "\n";
    cout << my_map.bucket_count() << "\n";
    
    my_map.rehash(400);
    cout << my_map.bucket_count() << "\n";
}

inline namespace t_use_hash
{

}