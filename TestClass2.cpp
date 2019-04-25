#include "TestClass2.h"
#ifndef __MEMORY_CHECK_LIJUN__
#define __MEMORY_CHECK_LIJUN__
#endif
#include "memory_check.h"

TestClass2::TestClass2() 
{ 
    m_TestClass2 = new int[10]; 
    m_TestClassObj = new TestClass1; 
}

TestClass2::~TestClass2()
{
    delete m_TestClass2;
    //delete m_TestClassObj;
}
