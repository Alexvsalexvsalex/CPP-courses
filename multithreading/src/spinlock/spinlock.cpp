#include <stdexcept>
#include <cassert>

#include "spinlock.h"

namespace mt {
    Spinlock::Spinlock(): flag(false) {}

    void Spinlock::set_flag(bool expected, bool desired) {
        bool exp = expected;
        while (!flag.compare_exchange_strong(exp, desired)) {
            exp = expected;
        }
    }

    void Spinlock::lock() {
        set_flag(false, true);
    }
    
    void Spinlock::unlock() {
        set_flag(true, false);
    }
}