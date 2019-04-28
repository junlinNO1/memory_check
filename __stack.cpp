#include "__stack.h"


/*************************************************************************************
* 函数名称: PrintElement
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数: ElementT & 待打印元素
* 功     能: 打印ElementT对象
* 返 回 值: NONE
*************************************************************************************/
void PrintElement(const ElementT & elem)
{
    std::cout << elem._file << ", " << elem._func << ", " << elem._line << std::endl;
}

/*************************************************************************************
* 函数名称: SimpleLinkStack::~SimpleLinkStack
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数: void
* 功     能: 构造函数
* 返 回 值: NONE
*************************************************************************************/
SimpleLinkStack::SimpleLinkStack()
{ 
    _length = 0;
    _headNode = (StackNode *)malloc(sizeof(StackNode));
    _headNode->_next = NULL;
}

/*************************************************************************************
* 函数名称: SimpleLinkStack::~SimpleLinkStack
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数: void
* 功     能: 析构函数，资源释放
* 返 回 值: NONE
*************************************************************************************/
SimpleLinkStack::~SimpleLinkStack()
{
    ElementT elem;
    while (!isEmpty())
        Pop(elem);
    if (_headNode)
        free(_headNode);
}

/*************************************************************************************
* 函数名称: SimpleLinkStack::clear
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  清除链表中的所有元素
* 返 回 值: bool 成功与否
*************************************************************************************/
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

/*************************************************************************************
* 函数名称: SimpleLinkStack::isEmpty
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数: void
* 功     能:  判断栈是否为空
* 返 回 值: bool true-为空
*************************************************************************************/
bool SimpleLinkStack::isEmpty()
{
    if (!_length)
    {
        return true;
    }
    else
        return false;
}

/*************************************************************************************
* 函数名称: SimpleLinkStack::GetLength
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数: void
* 功     能:  获取栈的长度
* 返 回 值: ElementT 栈顶元素
*************************************************************************************/
unsigned int SimpleLinkStack::GetLength()
{
    return _length;
}

/*************************************************************************************
* 函数名称: SimpleLinkStack::GetTop
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数: void
* 功     能:  获取栈顶元素，只访问不出栈
* 返 回 值: ElementT 栈顶元素
*************************************************************************************/
const ElementT SimpleLinkStack::GetTop()
{
    if (_headNode->_next != NULL)
    {
        StackNode *top = _headNode->_next;
        return top->_elem;
    }
    return ElementT();
}

/*************************************************************************************
* 函数名称: SimpleLinkStack::Push
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数: ElementT  -待推入的元素
* 功     能:  将元素推入栈顶
* 返 回 值: bool  成功与否
*************************************************************************************/
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

/*************************************************************************************
* 函数名称: SimpleLinkStack::Pop
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数: ElementT & 输出参数，存储出栈元素
* 功     能:  将栈顶元素出栈
* 返 回 值: bool  成功与否
*************************************************************************************/
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

/*************************************************************************************
* 函数名称: SimpleLinkStack::printStack
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数: void
* 功     能:  打印当前栈中的所有元素
* 返 回 值: void
*************************************************************************************/
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
