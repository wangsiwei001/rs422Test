/**
 * @file SC513_task.h
 * @author sunxzh (@513)
 * @brief 任务控制类接口函数
 * @version 1.0
 * @date 2023-10-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __SC513_TASK_H_
#define __SC513_TASK_H_

#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>

typedef struct THREAD_NODE_LIST
{
    char name[128];
    unsigned int id;
    pthread_t tid;  
    pthread_attr_t attr;
	struct sched_param param; 
    struct THREAD_NODE_LIST *next;
}pthreadNodeList;

/**
 * @brief 创建新的任务
 * 
 * @param task_id 创建任务的 ID 地址，创建成功后由系统分配 ID
 * @param task_name 创建的线程名字
 * @param function_pointer 任务的入口点
 * @param stack_size 任务分配的堆栈大小，以字节为单位
 * @param priority 任务分配的优先级
 * @return signed int 
 * 1) 0：函数调用成功；
 * 2) -1：其他原因函数调用失败；
 * 3) -2：任意指针为空；
 * 4) -3：不能再创建新的任务；
 * 5) -4：任务名称太长；
 * 6) -5：指定的任务名称已经被使用；
 * 7) -6：优先级无效
 */
extern signed int TaskCreate(unsigned int * task_id, const char * task_name, const void *function_pointer, unsigned int stack_size, unsigned int priority);

/**
 * @brief 销毁特定 ID 的任务
 * 
 * @param task_id 待销毁的任务 ID
 * @return signed int 
 * 1) 0：函数调用成功；
 * 2) -1：其他原因函数调用失败；
 * 3) -2：任务 ID 无效
 */
extern signed int TaskDelete(unsigned int task_id);

/**
 * @brief 任务延时设置
 * 
 * @param millisecond 需要延时的时间计数，单位为毫秒
 * @return signed int 
 * 1) 0：函数调用成功；
 * 2) -1：函数调用失败
 */
extern signed int TaskDelay(unsigned int millisecond);

#endif