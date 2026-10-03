# Payment Processing Engine

A C++20 demo application that simulates processing bank transactions, including CREDIT and DEBIT payments, through configurable payment gateways. This training and integration project is not a production payment system.

## Requirements

- C++20 compiler
- CMake >= 3.20
- libcurl
- nlohmann_json
- GoogleTest / GoogleMock when tests are enabled

## Build

Normal build:

```sh
cmake -S . -B build
cmake --build build
```

Tests are enabled by default through CTest's `BUILD_TESTING` option. To build without tests:

```sh
cmake -S . -B build -DBUILD_TESTING=OFF
cmake --build build
```

## CMake Options

- `BUILD_TESTING`: enables or disables tests.
- `ENABLE_HTTP_INTEGRATION_TESTS`: enables optional tests that perform real HTTP integration.
- `ENABLE_COVERAGE`: enables GCC/lcov code coverage.
- `ENABLE_SANITIZERS`: enables AddressSanitizer and UndefinedBehaviorSanitizer.
- `ENABLE_THREAD_SANITIZER`: enables ThreadSanitizer.
- `ENABLE_STRICT_WARNINGS`: enables additional compiler warnings.

Coverage cannot be combined with either sanitizer mode. AddressSanitizer/UndefinedBehaviorSanitizer and ThreadSanitizer cannot be enabled together.

Tests enabled:

```sh
cmake -S . -B build -DBUILD_TESTING=ON
```

No tests:

```sh
cmake -S . -B build -DBUILD_TESTING=OFF
```

Coverage:

```sh
cmake -S . -B build \
  -DBUILD_TESTING=ON \
  -DENABLE_COVERAGE=ON
```

ASan + UBSan:

```sh
cmake -S . -B build \
  -DBUILD_TESTING=ON \
  -DENABLE_SANITIZERS=ON
```

ThreadSanitizer:

```sh
cmake -S . -B build \
  -DBUILD_TESTING=ON \
  -DENABLE_THREAD_SANITIZER=ON
```

Strict warnings:

```sh
cmake -S . -B build \
  -DENABLE_STRICT_WARNINGS=ON
```

## Running Tests

```sh
ctest --test-dir build --output-on-failure
```

HTTP integration tests are built separately when `ENABLE_HTTP_INTEGRATION_TESTS=ON`.

## Running the Application

```sh
./build/payment_app
```

Supported options:

```text
--gateway legacy|secondary
--observers all|logger|receipt|none
--id <positive-id>
--type credit|debit
--amount <positive-amount-in-cents>
--description "<text>"
--help
```

By default, the application uses the legacy gateway, enables both observers, and processes the default demo CREDIT transaction. Example:

```sh
./build/payment_app \
  --gateway secondary \
  --observers logger \
  --id 2001 \
  --type debit \
  --amount 7500 \
  --description "Demo payment"
```

If any transaction option is provided, all four transaction fields (`--id`, `--type`, `--amount`, and `--description`) are required.

## Configuration

LegacyBank requires `LEGACY_BANK_API_KEY`, configured through an environment variable:

```sh
export LEGACY_BANK_API_KEY="..."
```

Do not pass the API key through command-line arguments.

## Scope

This is a demo/training payment processor using in-memory transaction storage and mock/demo gateway integrations. It is not intended for production payment processing.
