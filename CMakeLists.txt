cmake_minimum_required(VERSION 3.20)

project(PaymentEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(payment_app
    app/main.cpp
)

add_library(payment_core
    src/payment/types/transaction.cpp
    src/payment/repositories/transaction_repository.cpp
    src/payment/strategies/pix_payment.cpp
    src/payment/strategies/credit_payment.cpp
    src/payment/strategies/debit_payment.cpp
    src/payment/factories/payment_factory.cpp
    src/payment/engine/payment_engine.cpp
    src/payment/interfaces/payment_strategy.cpp
    src/payment/utils/card_utils.cpp
    src/payment/observers/receipt_printer.cpp
    src/payment/observers/event_logger.cpp
)


target_include_directories(payment_core PUBLIC include)
target_link_libraries(payment_app PRIVATE payment_core)
