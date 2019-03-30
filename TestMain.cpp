#include <iostream>

#ifndef MEMORY_CHECK
#define MEMORY_CHECK
#endif
#include "memory_check.h"

//---------------------------------------------------------------
// ´ý²âÊÔ´úÂë 
//---------------------------------------------------------------
extern void bad_code() {
    int *p = new int;
    char *q = new char[5];
    delete[]q;
}

void good_code() {
    int *p = new int;
    char *q = new char[5];
    delete p;
    delete p;
    //delete[]q;
}

int main()
{
    good_code();
    bad_code();

    int *t1 = new int[10];
    delete t1;
    delete t1;

    int hh = 1;
    int * t2 = &hh;
    //delete t2;

    PrintMemCheckResult();
    FreeMemCheck();
    return 0;
}
