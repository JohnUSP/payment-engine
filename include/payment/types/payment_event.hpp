#pragma once

#include <string_view>

namespace payment {

class Transaction;

enum class PaymentEvent {
  TRANSACTION_PENDING,
  TRANSACTION_VALIDATED,
  TRANSACTION_APPROVED,
  TRANSACTION_DENIED,
  TRANSACTION_CANCELED,
  TRANSACTION_COMPLETED
};

inline std::string_view toString(PaymentEvent event) {
  switch (event) {
  case PaymentEvent::TRANSACTION_PENDING:
    return "PENDING";

  case PaymentEvent::TRANSACTION_VALIDATED:
    return "VALIDATED";

  case PaymentEvent::TRANSACTION_APPROVED:
    return "APPROVED";

  case PaymentEvent::TRANSACTION_DENIED:
    return "DENIED";

  case PaymentEvent::TRANSACTION_CANCELED:
    return "CANCELED";

  case PaymentEvent::TRANSACTION_COMPLETED:
    return "COMPLETED";
  }

  return "UNKNOWN";
}

} // namespace payment
