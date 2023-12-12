cmake_minimum_required(VERSION 3.16)

project(queue-emptier)

set(CMAKE_CXX_STANDARD 17)

# Include Arrow
find_package(Arrow)

# Include AWS SDK
find_package(AWSSDK REQUIRED COMPONENTS sqs)

add_executable(queue-emptier src/main.cpp)
target_link_libraries(queue-emptier PRIVATE aws-cpp-sdk-core aws-cpp-sdk-sqs)
