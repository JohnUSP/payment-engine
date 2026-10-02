cmake_minimum_required(VERSION 3.20)

project(PaymentEngine LANGUAGES CXX)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(payment_app
    app/main.cpp
)

add_library(payment_core

    src/payment/types/transaction.cpp
    src/payment/engine/payment_engine.cpp

    src/payment/factory/payment_factory.cpp
    src/payment/interfaces/process/payment_process.cpp
    src/payment/interfaces/process/credit_payment.cpp
    src/payment/interfaces/process/debit_payment.cpp
    src/payment/interfaces/persistence/in_memory_transaction_repository.cpp
    src/payment/interfaces/observer/event_logger.cpp
    src/payment/interfaces/observer/receipt_printer.cpp
    src/payment/interfaces/gateway/legacy_bank.cpp
    src/payment/interfaces/gateway/legacy_bank_contract.cpp

    src/payment/external/legacy_bank_api.cpp
    src/payment/network/http_client.cpp

    src/payment/utils/card_utils.cpp
)

target_include_directories(payment_core PUBLIC include)

target_link_libraries(payment_core PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)
target_link_libraries(payment_app PRIVATE payment_core)
