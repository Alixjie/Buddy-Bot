#ifndef __SYNC_QUEUE_H__
#define __SYNC_QUEUE_H__

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <utility>
#include <vector>

template <typename T>
class SyncQueue {
public:
    typedef T value_type;

private:
    int capacity_; /* The capacity of the queue, but virtual length of queue is
                   capacity_+1.*/
    int front_;    // Indicate the front of the queue.
    int back_;     // Indicate the back of the queue.
    std::vector<value_type> queue_;
    std::mutex mutex_write_;  // Prevent other threads write this sync queue.
    std::mutex mutex_read_;   // Prevent other threads read this sync queue.
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;

public:
    explicit SyncQueue(const std::size_t capacity)
    {
        capacity_ = capacity;

        queue_.reserve(capacity_ + 1);
        front_ = back_ = 0;
    }

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

    value_type waitAndPop()
    {
        std::unique_lock<std::mutex> lock_write(mutex_write_);

        notEmpty_.wait(lock_write, [this] { return readSize(); });
        std::unique_lock<std::mutex> lock_read(mutex_read_);
        value_type item = std::move(queue_[front_]);
        front_ = (front_ + 1) % (capacity_ + 1);

        notFull_.notify_one();
        return item;
    }

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

    value_type frontItem()
    {
        std::unique_lock<std::mutex> lock_write(mutex_write_);

        if (!readSize()) {
            notEmpty_.wait(lock_write, [this] { return readSize(); });
        }
        value_type item = queue_[front_];

        return item;
    }

    // bool empty();

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

    void clear()
    {
        std::unique_lock<std::mutex> lock_write(mutex_write_);
        std::unique_lock<std::mutex> lock_read(mutex_read_);

        front_ = back_ = 0;
    }

private:
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