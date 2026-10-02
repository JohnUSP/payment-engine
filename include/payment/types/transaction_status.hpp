#pragma once

#include <string_view>

namespace payment {

enum class TransactionStatus {
  PENDING,
  VALIDATED,
  APPROVED,
  DENIED,
  CANCELED,
  COMPLETED
};

inline std::string_view toString(TransactionStatus status) {

  switch (status) {
  case TransactionStatus::PENDING:
    return "Pending";
  case TransactionStatus::VALIDATED:
    return "Validated";
  case TransactionStatus::APPROVED:
    return "Approved";
  case TransactionStatus::DENIED:
    return "Denied";
  case TransactionStatus::COMPLETED:
    return "Completed";
  case TransactionStatus::CANCELED:
    return "Canceled";
  }

  return "Unknown Transaction Status";
}
} // namespace payment
