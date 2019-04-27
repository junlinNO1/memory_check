#include "__stack.h"

//打印ElementT对象
void PrintElement(const ElementT & elem)
{
    std::cout << elem._file << ", " << elem._func << ", " << elem._line << std::endl;
}

SimpleLinkStack::SimpleLinkStack()
{ 
    _length = 0;
    _headNode = (StackNode *)malloc(sizeof(StackNode));
    _headNode->_next = NULL;
}

SimpleLinkStack::~SimpleLinkStack()
{
    ElementT elem;
    while (!isEmpty())
        Pop(elem);
    if (_headNode)
        free(_headNode);
}

//清除链表中的所有元素
bool SimpleLinkStack::clear()
{
    while (_headNode->_next)
    {
        ElementT elem;
        if (!Pop(elem))
        {
            return false;
        }
    }
    return true;
}

//判断栈是否为空
bool SimpleLinkStack::isEmpty()
{
    if (!_length)
    {
        return true;
    }
    else
        return false;
}

//获取栈的长度
unsigned int SimpleLinkStack::GetLength()
{
    return _length;
}

//获取栈顶元素，只访问不出栈
const ElementT SimpleLinkStack::GetTop()
{
    if (_headNode->_next != NULL)
    {
        StackNode *top = _headNode->_next;
        return top->_elem;
    }
    return ElementT();
}

//将元素推入栈顶
bool SimpleLinkStack::Push(const ElementT elem)
{
    StackNode* pNode = (StackNode*)malloc(sizeof(StackNode));
    pNode->_elem = elem;
    pNode->_next = NULL;

    //采用头插法，即pNode代替栈顶的，所以pNode的下一个应为head的下一个；
    pNode->_next = _headNode->_next;
    _headNode->_next = pNode;
    _length++;
    return true;
}

//将栈顶元素出栈
bool SimpleLinkStack::Pop(ElementT & elem)
{
    if (_headNode->_next != NULL)
    {
        StackNode* top = _headNode->_next;
        _headNode->_next = top->_next;
        elem = top->_elem;
        _length--;
        free(top);
        return true;
    }
    else
        return false;
}

//打印当前栈中的所有元素
void SimpleLinkStack::printStack()
{
    if (_headNode->_next != NULL)
    {
        StackNode* pNode = _headNode->_next;
        ElementT & elem = pNode->_elem;
        PrintElement(pNode->_elem);
        while (pNode->_next)
        {
            PrintElement(pNode->_next->_elem);
            pNode = pNode->_next;
        }
    }
}
