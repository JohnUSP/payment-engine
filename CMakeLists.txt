cmake_minimum_required(VERSION 3.20)

project(PaymentEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(payment_app
    app/main.cpp
)

add_library(payment_core
    src/transactions.cpp
)


target_include_directories(payment_core PUBLIC include)
target_link_libraries(payment_app PRIVATE payment_core)
