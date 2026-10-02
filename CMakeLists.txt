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
    src/payment/repositories/transaction_repository.cpp
    src/payment/strategies/credit_payment.cpp
    src/payment/strategies/debit_payment.cpp
    src/payment/factories/payment_factory.cpp
    src/payment/engine/payment_engine.cpp
    src/payment/strategies/payment_strategy.cpp
    src/payment/utils/card_utils.cpp
    src/payment/observers/receipt_printer.cpp
    src/payment/observers/event_logger.cpp
    src/payment/external/legacy_bank_api.cpp
    src/payment/gateways/legacy_bank.cpp
    src/payment/gateways/legacy_bank_contract.cpp
    src/payment/network/http_client.cpp
)

target_include_directories(payment_core PUBLIC include)

target_link_libraries(payment_core PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)
target_link_libraries(payment_app PRIVATE payment_core)
