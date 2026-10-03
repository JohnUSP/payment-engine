cmake_minimum_required(VERSION 3.20)

project(PaymentEngine LANGUAGES CXX)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

option(ENABLE_HTTP_INTEGRATION_TESTS
    "Enable HTTP integration tests that require network access"
    OFF
)

option(ENABLE_COVERAGE
    "Enable code coverage instrumentation"
    OFF
)

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

target_compile_options(payment_core PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(payment_app PRIVATE -Wall -Wextra -Wpedantic)

if(BUILD_TESTING)
    find_package(GTest REQUIRED)
    find_package(Threads REQUIRED)

    if(NOT TARGET GTest::gmock)
        find_path(GMOCK_SOURCE_DIR
            NAMES src/gmock-all.cc
            PATHS /usr/src/googletest/googlemock
            NO_DEFAULT_PATH
        )

        if(GMOCK_SOURCE_DIR)
            add_library(payment_test_gmock STATIC
                ${GMOCK_SOURCE_DIR}/src/gmock-all.cc
            )
            target_include_directories(payment_test_gmock
                PUBLIC ${GMOCK_SOURCE_DIR}/include
                PRIVATE ${GMOCK_SOURCE_DIR}
            )
            target_link_libraries(payment_test_gmock PUBLIC GTest::gtest)
            add_library(GTest::gmock ALIAS payment_test_gmock)
        else()
            message(FATAL_ERROR "GoogleMock is required to build payment_tests")
        endif()
    endif()

    add_executable(payment_tests
        tests/smoke_test.cpp
        tests/unit/transaction_tests.cpp
        tests/unit/payment_process_tests.cpp
        tests/unit/payment_factory_tests.cpp
        tests/unit/in_memory_transaction_repository_tests.cpp
        tests/unit/legacy_bank_tests.cpp
        tests/unit/legacy_bank_contract_tests.cpp
        tests/unit/payment_engine_tests.cpp
        tests/unit/payment_observer_tests.cpp
        tests/integration/in_memory_transaction_repository_concurrency_tests.cpp
        tests/integration/payment_engine_concurrency_tests.cpp
    )

    target_link_libraries(payment_tests
        PRIVATE
            payment_core
            GTest::gmock
            GTest::gtest_main
            Threads::Threads
            nlohmann_json::nlohmann_json
    )

    target_compile_options(payment_tests PRIVATE -Wall -Wextra -Wpedantic)

    include(GoogleTest)
    gtest_discover_tests(payment_tests)
endif()

if(ENABLE_HTTP_INTEGRATION_TESTS)
    find_package(GTest REQUIRED)
    include(GoogleTest)

    add_executable(payment_http_integration_tests
        tests/integration/curl_http_client_integration_tests.cpp
    )

    target_link_libraries(payment_http_integration_tests
        PRIVATE
            payment_core
            GTest::gtest_main
    )

    target_compile_options(payment_http_integration_tests PRIVATE -Wall -Wextra -Wpedantic)

    gtest_discover_tests(payment_http_integration_tests)
endif()

if(ENABLE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(FATAL_ERROR
            "ENABLE_COVERAGE currently requires GCC (GNU C++ compiler); "
            "found '${CMAKE_CXX_COMPILER_ID}'."
        )
    endif()

    if(NOT BUILD_TESTING)
        message(FATAL_ERROR "ENABLE_COVERAGE requires BUILD_TESTING=ON")
    endif()

    find_program(LCOV_EXECUTABLE lcov REQUIRED)
    find_program(GENHTML_EXECUTABLE genhtml REQUIRED)

    set(COVERAGE_FLAGS
        -fprofile-arcs
        -ftest-coverage
        --coverage
        -fprofile-update=atomic
    )

    target_compile_options(payment_core PRIVATE ${COVERAGE_FLAGS})
    target_link_options(payment_core PUBLIC --coverage)
    target_compile_options(payment_tests PRIVATE ${COVERAGE_FLAGS})
    target_link_options(payment_tests PRIVATE ${COVERAGE_FLAGS})

    set(COVERAGE_INFO "${CMAKE_BINARY_DIR}/coverage.info")
    set(FILTERED_COVERAGE_INFO "${CMAKE_BINARY_DIR}/coverage.filtered.info")
    set(COVERAGE_REPORT_DIR "${CMAKE_BINARY_DIR}/coverage")

    add_custom_target(coverage
        # Clean previous coverage traces and report directories
        COMMAND ${CMAKE_COMMAND} -E rm -f "${COVERAGE_INFO}" "${FILTERED_COVERAGE_INFO}"
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${COVERAGE_REPORT_DIR}"

        # Reset counters
        COMMAND ${LCOV_EXECUTABLE}
            --zerocounters
            --directory "${CMAKE_BINARY_DIR}"
            --quiet
            --ignore-errors inconsistent,inconsistent
            --ignore-errors unused,unused

        # Execute test runner binary
        COMMAND $<TARGET_FILE:payment_tests>

        # Capture raw coverage data
        COMMAND ${LCOV_EXECUTABLE}
            --capture
            --directory "${CMAKE_BINARY_DIR}"
            --output-file "${COVERAGE_INFO}"
            --quiet
            --keep-going
            --ignore-errors inconsistent,inconsistent
            --ignore-errors unused,unused

        # Filter out system paths, test directories, build artifacts, and vendor files
        COMMAND ${LCOV_EXECUTABLE}
            --remove "${COVERAGE_INFO}"
            "/usr/*"
            "*/tests/*"
            "*/build/*"
            "*/_deps/*"
            "*/googletest/*"
            "*/googlemock/*"
            "*/gtest/*"
            "*/gmock/*"
            --output-file "${FILTERED_COVERAGE_INFO}"
            --quiet
            --ignore-errors inconsistent,inconsistent
            --ignore-errors unused,unused

        # Generate single HTML report
        COMMAND ${GENHTML_EXECUTABLE}
            "${FILTERED_COVERAGE_INFO}"
            --output-directory "${COVERAGE_REPORT_DIR}"
            --quiet
            --legend

        DEPENDS payment_tests
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        USES_TERMINAL
        COMMENT "Running payment_tests and generating clean HTML coverage report..."
    )
endif()
