#ifndef __STACK_HH__
#define __STACK_HH__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

//堆栈内存储的实际数据，可以通过typedef ElementT 来实现替换
typedef struct _element 
{
    const char * _file;
    const char * _func;
    unsigned int _line;

    _element() :_file(NULL), _func(NULL), _line(0) {}
    _element(const char * file, const char * func, unsigned int line)
        :_file(file), _func(func), _line(line)
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
    SimpleLinkStack() { _length = 0; _headNode = (StackNode *)malloc(sizeof(StackNode)); };
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
