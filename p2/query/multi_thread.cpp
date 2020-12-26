//
// Created by hzd on 2020/11/2.
//

#include "multi_thread.h"
static unsigned int threadNumber=autodetect();

unsigned int autodetect()
{
    threadNumber=std::thread::hardware_concurrency();
    return threadNumber;
}

unsigned int getthreadNumber()
{
    return threadNumber;
}
