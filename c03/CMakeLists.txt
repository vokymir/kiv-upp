CMAKE_MINIMUM_REQUIRED(VERSION 4.0)

PROJECT(upp_03)

FILE(GLOB_RECURSE 01_mutex_src 01_mutex/*.cpp 01_mutex/*.h)
FILE(GLOB_RECURSE 02_deadlock_src 02_deadlock/*.cpp 02_deadlock/*.h)
FILE(GLOB_RECURSE 03_condvar_src 03_condvar/*.cpp 03_condvar/*.h)
FILE(GLOB_RECURSE 04_semaphore_src 04_semaphore/*.cpp 04_semaphore/*.h)

ADD_EXECUTABLE(01_mutex ${01_mutex_src})
ADD_EXECUTABLE(02_deadlock ${02_deadlock_src})
ADD_EXECUTABLE(03_condvar ${03_condvar_src})
ADD_EXECUTABLE(04_semaphore ${04_semaphore_src})
