#pragma once
#include <stddef.h>
#include <optional>

#include "spinlock.h"
#include <mutex>
#include <condition_variable>

namespace mt
{
    class IQueue {
    public:
        virtual ~IQueue() = default;
        virtual void push(const int &value) = 0;
        virtual std::optional<int> pop() = 0;
        virtual void close() = 0;
    };

    // Многопоточная очередь, для синхронизации надо использовать Spinlock
    class QueueWithSpinlock: public IQueue
    {
    public:
        ~QueueWithSpinlock() = default;
        explicit QueueWithSpinlock(size_t capacity);
        void push(const int& value) override;
        std::optional<int> pop() override;
        void close() override;

    private:
        const size_t capacity;
        size_t size;
        size_t offset;
        std::vector<int> values;
        bool closed;
        mt::Spinlock spinlock;
    };

    // Многопоточная очередь, для синхронизации надо std::mutex и std::condition_variable
    class QueueWithCondVar: public IQueue
    {
    public:
        ~QueueWithCondVar() = default;
        explicit QueueWithCondVar(size_t capacity);
        void push(const int& value) override;
        std::optional<int> pop() override;
        void close() override;

    private:
        const size_t capacity;
        size_t size;
        size_t offset;
        std::vector<int> values;
        bool closed;
        std::mutex values_mutex;
        std::condition_variable values_empty_cv;
        std::condition_variable values_full_cv;
    };
}