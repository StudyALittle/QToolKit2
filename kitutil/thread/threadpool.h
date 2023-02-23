#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <list>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "kitutil_global.h"

namespace wkit {

class ThreadPool;

/**
 * @brief The TTask class: 线程池任务基类
 */
class KITUTIL_EXPORT TTask
{
public:
    using Ptr = std::shared_ptr<TTask>;

    friend class ThreadPool;

    TTask(){ m_bRun = nullptr; }
    TTask(const TTask& task) = delete;
    TTask(TTask&& task) = delete;

    virtual ~TTask(){}

protected:
    /**
     * @brief setBRunAddr: 设置 m_bRun 的地址，如果线程退出，可以通过访问m_bRun判断是否需要退出线程，由线程池设置
     * @param bRun
     */
    void setBRunAddr(bool *bRun) { m_bRun = bRun; }

    /**
     * @brief run: 实现耗时的操作
     */
    virtual void run() = 0;

protected:
    bool *m_bRun;
};

///
/// \brief The ThreadPool class: 线程池
///
class KITUTIL_EXPORT ThreadPool
{
public:
    using Ptr = std::shared_ptr<ThreadPool>;

    /**
     * @brief ThreadPool
     */
    ThreadPool();
    ~ThreadPool();

    /**
     * @brief instance: 获取线程池单例
     * @return
     */
    static ThreadPool &instance();

    /**
     * @brief start: 开启线程池
     * @param maxThreadNum: 线程数量
     * @param maxTaskSize: 最大任务数量，超过的任务丢弃（0：不限制任务数量）
     */
    void start(int threadNum = 10, int maxTaskSize = 0);

    /**
     * @brief pushTask: 添加执行任务
     * @param task
     */
    void pushTask(TTask::Ptr task);

    /**
     * @brief setSaveTaskMaxSize: 设置队列存储任务最大数量
     * @param maxTaskSize
     */
    void setSaveTaskMaxSize(int maxTaskSize);

    /**
     * @brief stop: 关闭线程池
     */
    void stop();

    /**
     * @brief getLeftoverTaskNum: 获取剩余任务数量(不准确、线程不安全)
     * @return
     */
    unsigned long long getLeftoverTaskNum();

protected:
    /**
     * @brief taskExec: 执行任务线程
     * @param threadPool
     */
    static void taskExec(ThreadPool *threadPool);

    /**
     * @brief getTask: 获取一个任务
     * @return
     */
    TTask::Ptr getTask();

private:
    /**
     * @brief clearTasks: 清空任务
     */
    void clearTasks();

private:
    // 任务（队列）最大存储量（默认无限制）
    int m_maxTaskSize;
    // 线程是否运行
    bool m_bRun;
    // 最大线程数量
    int m_maxThreadNum;
    // 待执行任务列表
    std::queue<TTask::Ptr > m_tasks;
    // 锁
    std::mutex m_mutex;
    // 条件变量
    std::condition_variable m_condition;
    // 线程退出数量
    int m_threadQuitNum;
    // 线程退出锁
    std::mutex m_mutexQuit;
    // 保存运行的线程对象
    std::list<std::shared_ptr<std::thread> > m_threads;
};

} // end namespace

#endif // THREADPOOL_H
