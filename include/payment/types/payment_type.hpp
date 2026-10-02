#pragma once
#include <string_view>

namespace payment {

enum class PaymentType { DEBIT, CREDIT };

inline std::string_view toString(PaymentType type) {

  switch (type) {
  case PaymentType::DEBIT:
    return "Debit";
  case PaymentType::CREDIT:
    return "Credit";
  }

  return "Unknown Payment Type";
}

} // namespace payment
