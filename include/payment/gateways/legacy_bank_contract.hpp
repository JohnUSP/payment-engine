#pragma once
#include <optional>
#include <payment/types/amount.hpp>
#include <payment/types/payment_type.hpp>
#include <payment/types/transaction_id.hpp>
#include <string>

namespace payment {

struct LegacyBankRequest {
  LegacyBankRequest(TransactionId id, Amount amount, PaymentType type,
                    std::string description)
      : transactionId(id), amount(amount), paymentType(type),
        description(std::move(description)) {}

  TransactionId transactionId;
  Amount amount;
  PaymentType paymentType;
  std::string description;

  std::string toJson() const;
};

struct LegacyBankResponse {
  std::string status;
  std::optional<std::string> authorizationCode;
  std::string message;
  std::string bankTransactionId;
  std::string timestamp;

  static LegacyBankResponse fromJson(const std::string& json);
};



} // namespace payment
