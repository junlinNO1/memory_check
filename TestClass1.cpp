#include "TestClass1.h"

TestClass1::TestClass1()
{ 
    m_TestClass1 = new int[10]; 
}

TestClass1::~TestClass1()
{ 
    delete[] m_TestClass1; 
}