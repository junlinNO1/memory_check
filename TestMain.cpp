#include <iostream>

#ifndef __MEMORY_CHECK_LIJUN__
#define __MEMORY_CHECK_LIJUN__
#endif
#define __MEMORY_CHECK_LIJUN_C__
#include "memory_check.h"
#include "TestClass2.h"
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

	int *ppppp[10];
	for (int i = 0; i <10; ++i)
	{
	ppppp[i] = new int;
    }

	for (int i = 0; i <10; ++i)
	{
	delete ppppp[i];
    }

    TestClass2 * pobj2 = new TestClass2;
    delete pobj2;

    int * pppp = (int*)malloc(20);
    delete pppp;

    PrintMemCheckResult();
    FreeMemCheck();
    return 0;
}
