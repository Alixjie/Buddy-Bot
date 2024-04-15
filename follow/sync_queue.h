#ifndef __SYNC_QUEUE_H__
#define __SYNC_QUEUE_H__

#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <utility>
#include <vector>

#include "semaphore.h"

template <typename T>
class SyncQueue {
public:
    typedef T value_type;

private:
    int capacity_;  // The capacity of the queue, but virtual length of queue is
                    // capacity_+1.
    int front_;     // Indicate the front of the queue.
    int back_;      // Indicate the back of the queue.
    std::vector<value_type> queue_;

    std::mutex mutex_write_;  // Prevent other threads write this sync queue.
    std::mutex mutex_read_;   // Prevent other threads read this sync queue.
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;

public:
    /**
     * Initialize queue.
     * \param capacity is the carrying capacity of queue.
     **/
    explicit SyncQueue(const std::size_t capacity)
    {
        capacity_ = capacity;

        queue_.reserve(capacity_ + 1);
        front_ = back_ = 0;
    }

    /**
     * Push an item into queue, if queue is full, wait for not full.
     * \param item the item waited for pushing.
     **/
    void push(const value_type &item)
    {
        // Prevent others from writing queue.
        std::unique_lock<std::mutex> lock_write(mutex_write_);
        // Wait for queue not full.
        notFull_.wait(lock_write, [this] { return readSize() < capacity_; });

        // Start writing, so prevent others read and write.
        std::unique_lock<std::mutex> lock_read(mutex_read_);
        queue_[back_] = std::move(item);
        back_ = (back_ + 1) % (capacity_ + 1);

        notEmpty_.notify_one();
    }

    bool push(const value_type &item, uint delay)
    {
        std::unique_lock<std::mutex> lock_write(mutex_write_);

        struct timespec begin, end;
        clock_gettime(CLOCK_REALTIME, &begin);
        notFull_.wait_for(lock_write, std::chrono::milliseconds(delay),
                          [this] { return readSize() < capacity_; });

        clock_gettime(CLOCK_REALTIME, &end);

        long seconds = end.tv_sec - begin.tv_sec;
        long nanoseconds = end.tv_nsec - begin.tv_nsec;
        long elapsed = seconds * 1000 + nanoseconds / 1000;

        if (elapsed>delay){
            return false;
        }

        // Start writing, so prevent others read and write.
        std::unique_lock<std::mutex> lock_read(mutex_read_);
        queue_[back_] = std::move(item);
        back_ = (back_ + 1) % (capacity_ + 1);

        notEmpty_.notify_one();

        return true;
    }

    /**
     * Wait for queue not empty and pop an item.
     * \return the item popped.
     **/
    value_type waitAndPop()
    {
        std::unique_lock<std::mutex> lock_write(mutex_write_);

        // Ensure not empty.
        notEmpty_.wait(lock_write, [this] { return readSize(); });

        std::unique_lock<std::mutex> lock_read(mutex_read_);
        value_type item = std::move(queue_[front_]);
        front_ = (front_ + 1) % (capacity_ + 1);

        notFull_.notify_one();
        return item;
    }

    void waitAndPop(uint l, value_type *items)
    {
        std::unique_lock<std::mutex> lock_write(mutex_write_);

        // Ensure not empty.
        notEmpty_.wait(lock_write, [&] { return readSize() >= l; });

        std::unique_lock<std::mutex> lock_read(mutex_read_);
        for (int i = 0; i < l; ++i) {
            items[i] = std::move(queue_[front_]);
            front_ = (front_ + 1) % (capacity_ + 1);
        }

        notFull_.notify_one();
    }

    /**
     * Try to pop an item, if can, pop it else return false immediately.
     * \param item the reference for possible popped item.
     * \return the status of this operate.
     **/
    bool tryPop(value_type &item)
    {
        std::unique_lock<std::mutex> lock_write(mutex_write_);

        if (readSize() == 0) {
            return false;
        }

        std::unique_lock<std::mutex> lock_read(mutex_read_);
        item = std::move(queue_[front_]);
        front_ = (front_ + 1) % (capacity_ + 1);

        notFull_.notify_one();
        return true;
    }

    /**
     * Get front item but not pop it.
     * \return the front itme.
     **/
    value_type frontItem()
    {
        std::unique_lock<std::mutex> lock_write(mutex_write_);

        notEmpty_.wait(lock_write, [this] { return readSize(); });
        value_type item = queue_[front_];

        return item;
    }

    // bool empty();

    /**
     * Parapre for external users, so it is up to the function to lock the
     * write itself.
     * \return current size of queue.
     **/
    std::size_t size()
    {
        std::unique_lock<std::mutex> lock_write(mutex_write_);
        std::size_t temp_size;

        if (front_ <= back_) {
            temp_size = back_ - front_;
        } else {
            temp_size = ((back_ + (capacity_ + 1)) - front_) % (capacity_ + 1);
        }

        return temp_size;
    }

    /**
     * Clear queue.
     **/
    void clear()
    {
        std::unique_lock<std::mutex> lock_write(mutex_write_);
        std::unique_lock<std::mutex> lock_read(mutex_read_);

        front_ = back_ = 0;
    }

private:
    /**
     * Parapre for inside functions, require others to lock write.
     * \return current size of queue.
     **/
    std::size_t readSize()
    {
        std::size_t temp_size;

        if (front_ <= back_) {
            temp_size = back_ - front_;
        } else {
            temp_size = ((back_ + (capacity_ + 1)) - front_) % (capacity_ + 1);
        }

        return temp_size;
    }
};

#endif
