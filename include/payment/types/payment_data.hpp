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

struct PixData {
  TransactionData transactionData{};
  std::string pixId{};
  std::string payerBank{};
  std::string qrCode{};
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

using PaymentData = std::variant<PixData, DebitData, CreditData>;

} // namespace payment
