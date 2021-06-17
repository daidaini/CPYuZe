#pragma once

#include "comm.h"

/*
* char* , const char* , char* const , const char* const
*/

void test_const()
{
    char *one = new char[12];
    strcpy(one, "1234");
    char *one_ohter = new char[12];
    one = one_ohter;

    const char *two = one;
    two = one_ohter; //ok
    //strcpy(two, "2345"); //error
    /*
    * const char* 是指针值可变，但是指向的内容不可变
    */

    char *const three = one;
    //three = one_ohter;  //error
    strcpy(three, "2345"); //OK
    /*
    * char* const 是指向内容可变，但是指针值不能变
    */

    const char* const four  = one;
    //four = one_ohter; //error
    //strcpy(four, "2345"); //error

}
