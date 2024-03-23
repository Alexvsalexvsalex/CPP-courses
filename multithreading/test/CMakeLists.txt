add_executable(spinlock_test spinlock/test.cpp)
target_link_libraries(spinlock_test PUBLIC CONAN_PKG::boost spinlock)
add_test(spinlock_test spinlock_test)

add_executable(queue_test queue/test.cpp)
target_link_libraries(queue_test PUBLIC CONAN_PKG::boost queue)
add_test(queue_test queue_test)
