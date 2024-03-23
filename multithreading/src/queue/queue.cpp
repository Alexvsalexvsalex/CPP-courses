#include <iostream>
#include <stdexcept>
#include <vector>

#include "queue.h"

namespace mt {
    QueueWithSpinlock::QueueWithSpinlock(
            size_t capacity)
            : capacity(capacity)
            , size(0)
            , offset(0)
            , values(std::vector<int>(capacity))
            , closed(false)
            , spinlock() { }

    void QueueWithSpinlock::push(const int& value) {
        spinlock.lock();
        while (size == capacity && !closed) {
            spinlock.unlock();
            spinlock.lock();
        }
        if (!closed) {
            values[(offset + size) % capacity] = value;
            size += 1;
        }
        spinlock.unlock();
    }

    std::optional<int> QueueWithSpinlock::pop() {
        spinlock.lock();
        while (size == 0 && !closed) {
            spinlock.unlock();
            spinlock.lock();
        }
        std::optional<int> result;
        if (!closed) {
            result = values[offset % capacity];
            size -= 1;
            offset += 1;
        }
        spinlock.unlock();
        return result;
    }

    void QueueWithSpinlock::close() {
        spinlock.lock();
        closed = true;
        spinlock.unlock();
    }

    QueueWithCondVar::QueueWithCondVar(
            size_t capacity)
            : capacity(capacity)
            , size(0)
            , offset(0)
            , values(std::vector<int>(capacity))
            , closed(false)
            , values_mutex()
            , values_empty_cv()
            , values_full_cv() { }

    void QueueWithCondVar::push(const int& value) {
        std::unique_lock<std::mutex> guard(values_mutex);
        values_full_cv.wait(guard, [&]() { return !(size == capacity && !closed); } );
        if (!closed) {
            values[(offset + size) % capacity] = value;
            size += 1;
            guard.unlock();  // Why is values_mutex.unlock() not working here? (It was leading assertion)
            values_empty_cv.notify_one();
        }
    }

    std::optional<int> QueueWithCondVar::pop() {
        std::unique_lock<std::mutex> guard(values_mutex);
        values_empty_cv.wait(guard, [&]() { return !(size == 0 && !closed); });
        std::optional<int> result;
        if (!closed) {
            result = values[offset % capacity];
            size -= 1;
            offset += 1;
            guard.unlock();
            values_full_cv.notify_one();
        }
        return result;
    }

    void QueueWithCondVar::close() {
        std::unique_lock<std::mutex> guard(values_mutex);
        closed = true;
        guard.unlock();
        values_full_cv.notify_all();
        values_empty_cv.notify_all();
    }
}