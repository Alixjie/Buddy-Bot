#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <condition_variable>
#include <mutex>

class Semaphore {
public:
    Semaphore(bool state = false);
    Semaphore();
    void signal();
    void wait();

private:
    bool state_;
    std::mutex mutex_;
    std::condition_variable condition_;
};

#endif
