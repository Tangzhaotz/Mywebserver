#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>

//线程池就是一个生产者与消费者模型，当有任务发生的时候就往任务队列添加任务，主线程类似于生产者，子线程类似于消费者
class ThreadPool {
public:
    //构造函数，进行初始化
    explicit ThreadPool(size_t threadCount = 8): pool_(std::make_shared<Pool>()) {  //explict修饰i构造函数时，可以防止隐式转换和复制初始化
            assert(threadCount > 0);

            // 创建threadCount个子线程
            for(size_t i = 0; i < threadCount; i++) {
                std::thread([pool = pool_] {  //创建一个线程
                    std::unique_lock<std::mutex> locker(pool->mtx);  //将进行加锁
                    while(true) {
                        if(!pool->tasks.empty()) {  //当任务不为空的时候
                            // 从任务队列中取第一个任务
                            auto task = std::move(pool->tasks.front());
                            // 移除掉队列中第一个元素
                            pool->tasks.pop();
                            locker.unlock();  //解锁
                            task();  //任务执行的代码
                            locker.lock();
                        } 
                        //任务的队列不为空的时候
                        else if(pool->isClosed) break;
                        else pool->cond.wait(locker);   // 如果队列为空，等待，阻塞在这里，当往队列添加任务时就可以被唤醒继续执行
                    }
                }).detach();// 线程分离
            }
    }

    ThreadPool() = default;  //无参构造函数

    ThreadPool(ThreadPool&&) = default;  //拷贝构造函数
    
    ~ThreadPool() {  //析构函数
        if(static_cast<bool>(pool_)) {
            {
                std::lock_guard<std::mutex> locker(pool_->mtx);
                pool_->isClosed = true;
            }
            pool_->cond.notify_all();  //条件变量通知
        }
    }

    template<class F>
    void AddTask(F&& task) {  //往队列中添加新的任务
        {
            std::lock_guard<std::mutex> locker(pool_->mtx);  //先上锁
            pool_->tasks.emplace(std::forward<F>(task));  //添加任务
        }
        pool_->cond.notify_one();   // 唤醒一个等待的线程
    }

private:
    // 结构体，池子
    struct Pool {
        std::mutex mtx;     // 互斥锁
        std::condition_variable cond;   // 条件变量
        bool isClosed;          // 是否关闭
        std::queue<std::function<void()>> tasks;    // 队列（保存的是任务）
    };
    std::shared_ptr<Pool> pool_;  //  池子
};


#endif //THREADPOOL_H