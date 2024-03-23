#pragma once

#include <atomic>

namespace mt {
    // spinlock
    // Примитив синхронизации, обладающий интерфейсом как у std::mutex
    // Отличие в том, что может быть реализован в user space, если использовать атомарные переменные
    // Главный минус -- активное ожидание
    class Spinlock {
        public:
            Spinlock();
            void lock();
            void unlock();
        private:
            std::atomic_bool flag;
            void set_flag(bool expected, bool desired);
    };
}