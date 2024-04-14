#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <condition_variable>
#include <mutex>

class Semaphore {
public:
    Semaphore(const int count = 0);
    Semaphore();
    void signal();
    void wait();

private:
    int count_;
    std::mutex mutex_;
    std::condition_variable condition_;
};

#endif