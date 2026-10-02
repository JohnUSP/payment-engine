// include/payment/exceptions/exceptions.hpp

#pragma once

#include <stdexcept>

namespace payment {

class HttpClientError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class ExternalResponseError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class DomainError : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

class InvalidTransactionState : public DomainError {
public:
  using DomainError::DomainError;
};

class InvalidTransactionAmount : public DomainError {
public:
  using DomainError::DomainError;
};

class InvalidPaymentType : public DomainError {
public:
  using DomainError::DomainError;
};

} // namespace payment
