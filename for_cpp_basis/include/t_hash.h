#pragma once

#include "comm.h"

void find_rehash()
{
    unordered_map<string, int> my_map;
    my_map.rehash(9);
    for (int i = 0; i < 10; ++i)
    {
        my_map.emplace(to_string(i), i);
    }

    cout << my_map.load_factor() << "\n";
    cout << my_map.bucket_count() << "\n";
    cout << my_map.max_load_factor() << "\n";
    
    my_map.rehash(400);
    cout << my_map.bucket_count() << "\n";
    
}

/*
* 尝试测试下，map和unordered_map之间的性能差别
* 
*/
inline namespace t_use_hash
{
    constexpr int max_size = 1000000;
    map<int, int> my_map;
    unordered_map<int, int> my_umap;

    void Init()
    {
        my_umap.rehash(max_size);
    }

    void MapAssign()
    {
        TimerLog t("MapAssign");
        for(int i=max_size; i>0; --i)
        {
            my_map.emplace(i,max_size - i);
        }
    }
    void UmapAssign()
    {
        TimerLog t("UMapAssign");
        for(int i=max_size; i>0; --i)
        {
            my_umap.emplace(i,max_size - i);
        }
    }

    void MapUpdate()
    {
        TimerLog t("MapUpdate");
        for(int i = 0; i< max_size; ++i)
        {
            my_map[i] = i;
        }
    }

    void UmapUpdate()
    {
        TimerLog t("UmapUpdate");
        for(int i = 0; i< max_size; ++i)
        {
            my_umap[i] = i;
        }
        
    }

    void DoTest()
    {
        Init();
        MapAssign();
        UmapAssign();
        MapUpdate();
        UmapUpdate();
        

    }
}