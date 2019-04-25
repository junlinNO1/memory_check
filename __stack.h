#ifndef __STACK_HH__
#define __STACK_HH__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

//堆栈内存储的实际数据，可以通过typedef ElementT 来实现替换
typedef struct _element 
{
    unsigned int _id;
    const char * _file;
    const char * _func;
    unsigned int _line;
    unsigned int _size;

    _element() :_id(0), _file(NULL), _func(NULL), _line(0) {}
    _element(unsigned int id, const char * file, const char * func, unsigned int line, unsigned int size = 0)
        :_id(id), _file(file), _func(func), _line(line), _size(size)
    {}

} ElementT;

void PrintElement(const ElementT & elem);


//实现堆栈的链表节点
typedef struct _node
{
    ElementT _elem;
    _node* _next;

    _node() :_next(NULL) {}
    _node(ElementT elem):_elem(elem), _next(NULL) {}

} StackNode;

//简单的单链表实现堆栈
class SimpleLinkStack
{
public:
    //构造函数，包括一个起始的Node和元素个数
    SimpleLinkStack();
    ~SimpleLinkStack();
    bool clear();
    bool isEmpty();
    unsigned int GetLength();
    const ElementT GetTop();
    bool Push(ElementT elem);
    bool Pop(ElementT & elem);
    void printStack();

private:
    unsigned int _length;
    StackNode *_headNode;
};

#endif //__STACK_HH__
