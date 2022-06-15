#include "threadpool.h"

#include <iostream>

using namespace wkit;

ThreadPool::ThreadPool()
{
    m_maxThreadNum = 0;
    m_maxTaskSize = 0;
    m_threadQuitNum = 0;
    m_bRun = false;
}

ThreadPool::~ThreadPool(){
    stop();
}

//开启线程池
void ThreadPool::start(int maxThreadNum, int maxTaskSize)
{
    if(m_bRun) // 只能调用一次
        return;

    m_maxTaskSize = maxTaskSize;
    m_maxThreadNum = maxThreadNum;
    m_bRun = true;
    //创建执行任务线程
    for (int i = 0; i < maxThreadNum; i ++) {
        std::shared_ptr<std::thread> _thread = std::make_shared<std::thread>(taskExec, this);
        // _thread.detach();
        m_threads.push_back(_thread);
    }
    m_threadQuitNum = 0;
}

//获取线程池单例
ThreadPool &ThreadPool::instance()
{
    static ThreadPool s_threadPool;
    return s_threadPool;
}

//设置队列存储任务最大数量
void ThreadPool::setSaveTaskMaxSize(int maxTaskSize){
    this->m_maxTaskSize = maxTaskSize;
}

//添加任务
void ThreadPool::pushTask(TTask::Ptr task){
    if(this->m_maxTaskSize == 0){ //任务大小不做限制
        {
            std::unique_lock<std::mutex> lck(this->m_mutex);
            m_tasks.push(task);
        }
        this->m_condition.notify_one();
    }else {
        std::unique_lock<std::mutex> lck(this->m_mutex);
        if(static_cast<int>(m_tasks.size()) < m_maxTaskSize){
            m_tasks.push(task);
            lck.unlock();
            this->m_condition.notify_one();
        }else {
            //超出任务最大长度
            //throw "The task queue is too long";
        }
    }
}

//关闭线程池
void ThreadPool::stop(){
    //清空任务列表
    clearTasks();
    //唤醒
    {
        {
            std::unique_lock <std::mutex> lck(this->m_mutex);
            this->m_bRun = false;
        }
        //唤醒等待的线程
        this->m_condition.notify_all();
    }
    //回收线程资源
    for(auto itList = m_threads.begin(); itList != m_threads.end(); itList ++){
        std::shared_ptr<std::thread> t = *itList;
        if(t->joinable()){
            t->join();
        }
    }
    m_threads.clear();
}

//清空任务
void ThreadPool::clearTasks(){
    std::unique_lock<std::mutex> lck(this->m_mutex);
    while (!this->m_tasks.empty()) {
        std::shared_ptr<TTask> task = this->m_tasks.front();
        this->m_tasks.pop();
    }
}

//获取剩余任务数量(不准确、线程不安全)
unsigned long long ThreadPool::getLeftoverTaskNum(){
    return this->m_tasks.size();
}

//执行任务线程
void ThreadPool::taskExec(ThreadPool *threadPool){
    while (threadPool->m_bRun) {
        { //多加一个括号（作用域），保证每次task智能指针计数都释放
            std::shared_ptr<TTask> task = threadPool->getTask();
            if(!threadPool->m_bRun){ //停止线程
                break;
            }
            if(!task){
                continue;
            }
            try {
                task->run(); //执行线程任务
            } catch (...) {
                continue;
            }
        }
    }
    //每一个线程退出就加1，便于判断是不是全部线程都全部执行完成
    std::unique_lock <std::mutex> lck(threadPool->m_mutexQuit);
    threadPool->m_threadQuitNum += 1;
}

//获取一个任务
TTask::Ptr ThreadPool::getTask(){
    TTask::Ptr task = nullptr;
    if(this->m_bRun){
        //执行函数
        try {
            std::unique_lock<std::mutex> lck(this->m_mutex);
            if(!this->m_tasks.empty()){
                task = this->m_tasks.front();
                this->m_tasks.pop();
            }else { //任务队列没数据，等待被唤醒
                this->m_condition.wait(lck);
            }
        } catch (...) {
            return nullptr;
        }
    }
    return task;
}
