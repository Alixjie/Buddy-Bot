#include "semaphore.h"

Semaphore::Semaphore(bool state) : state_(state) { }
Semaphore::Semaphore() : state_(false) { }

void Semaphore::signal() {
    std::unique_lock<std::mutex> lock(mutex_);
    state_ = true;
    condition_.notify_all();
}

void Semaphore::wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this] { return state_; });
    state_ = false;
}