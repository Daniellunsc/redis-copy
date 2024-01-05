cmake_minimum_required(VERSION 3.13)
project(redis-starter-cpp)
set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard

set(SOURCE_FILES src/Server.cpp)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
file(GLOB_RECURSE SOURCE_FILES "src/*.h" "src/*.cpp")
add_executable(server ${SOURCE_FILES})
target_link_libraries(server Threads::Threads)
