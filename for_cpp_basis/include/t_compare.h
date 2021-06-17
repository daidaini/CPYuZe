#pragma once

#include "comm.h"

enum class Color : char
{
    blue = 'b',
    red  = 'r',
    white = 'w',
    yellow = 'y'
};

void test_enum_class_cmp()
{
    cout << (Color::white > Color::blue == true) << endl;
    cout << (Color::yellow < Color::red  == false) << endl;
}