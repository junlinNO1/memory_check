#include <iostream>

#ifndef MEMORY_CHECK
#define MEMORY_CHECK
#endif
#include "memory_check.h"
#include "__stack.h"
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

class B
{
public:
    B()
    {
       int *pb = new int;
       pB1 = new int[10];
    }
    ~B()
    {
       //printf("~B()  BBBBB.\n");
       delete pB1;
    }
   int * pB1;
};

class A{
public:
    A()
    {
       pA1 = new B; 
    }
    ~A()
    {
       //printf("~A()  AAAA.\n");
       delete pA1; 
    }

    B * pA1;
};

int main()
{
    //good_code();
    //bad_code();

    int *t1 = new int[10];
    delete t1;
    delete t1;

    int hh = 1;
    int * t2 = &hh;
    delete t2;

    A * pA2 = new A;
    delete pA2;

    PrintMemCheckResult();
    FreeMemCheck();
    return 0;
}
