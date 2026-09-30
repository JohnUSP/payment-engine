#pragma once
#include <string_view>

namespace payment {

enum class PaymentType { DEBIT, CREDIT, PIX };

inline std::string_view toString(PaymentType type) {

  switch (type) {
  case PaymentType::DEBIT:
    return "Debit";
    break;
  case PaymentType::CREDIT:
    return "Credit";
    break;
  case PaymentType::PIX:
    return "Pix";
    break;
  }

  return "Unknown Payment Type";
}

} // namespace payment
