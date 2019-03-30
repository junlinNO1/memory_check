#include <iostream>

using namespace std;

extern void* operator new(size_t size, const char* file, unsigned int line);
extern void* operator new[](size_t size, const char* file, unsigned int line);
extern void operator delete(void* ptr);
extern void operator delete[](void* ptr);
#define new new(__FILE__,__LINE__)


void testfunc()
{
    int *p = new int;
    *p = 89;
    delete p;

    int *p1 = new int[2];
    *p1 = 89;
    *(p1 + 1) = 90;
    //delete[] p1;
}

class A
{
public:
    A()
    {
        m_int1 = new int[10];
    }
    ~A()
    {
        delete[]m_int1;
        m_int1 = NULL;
    }

    void init()
    {
        m_int1 = new int[20];
    }
    void TestFunc1()
    {

    }
private:
    int * m_int1;
};

void testfunc2()
{
    A * p = new A;
    p->init();
    p->init();
    p->init();
    delete p;
}