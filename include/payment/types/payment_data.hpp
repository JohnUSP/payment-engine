#pragma once
#include <payment/types/amount.hpp>
#include <payment/types/time_stamp.hpp>
#include <payment/types/transaction_id.hpp>
#include <string>
#include <variant>
namespace payment {

struct CardData {
  std::string cardNumber{};
  std::string issuer{};
};

struct TransactionData {
  Amount amount;
  TransactionId tranId{};
  TimeStamp time{};
};

struct DebitData {
  TransactionData transactionData{};
  CardData card{};
};

struct CreditData {
  TransactionData transactionData{};
  CardData card{};
  int installments{1};
};

using PaymentData = std::variant<DebitData, CreditData>;

} // namespace payment
