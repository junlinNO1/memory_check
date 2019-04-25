#ifndef __TestClass2__HH__
#define __TestClass2__HH__
#include "TestClass1.h"

class TestClass1;

class TestClass2
{
public:
    TestClass2();
    ~TestClass2();

private:
    int * m_TestClass2;
    TestClass1 * m_TestClassObj;
};

#endif