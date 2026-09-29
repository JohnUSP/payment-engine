#pragma once
#include <chrono>
#include <cstdint>
#include <string>
#include <variant>
namespace payment {

using TransactionId = std::uint64_t;
using Amount = std::int64_t;
using TimeStamp = std::chrono::system_clock::time_point;
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
