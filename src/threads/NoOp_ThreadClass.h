#ifndef __NOOP_THREADCLASS_H__
#define __NOOP_THREADCLASS_H__

#include "Abstract_ThreadClass.h"

/**
 * A thread that does nothing, useful when mocking app
 */
class NoOp_ThreadClass
{
private:
    /* data */
public:
    NoOp_ThreadClass(/* args */);
    ~NoOp_ThreadClass();
};

NoOp_ThreadClass::NoOp_ThreadClass(/* args */)
{
}

NoOp_ThreadClass::~NoOp_ThreadClass()
{
}


#endif // __NOOP_THREADCLASS_H__