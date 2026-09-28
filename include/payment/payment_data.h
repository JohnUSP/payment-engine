#pragma once
#include <string>
#include <variant>

namespace payment {

struct PixData {
  std::string pixId{};
  std::string payerBank{};
  std::string qrCode{};
};

struct CardData {
  std::string cardNumber{};
  std::string issuer{};
};

struct DebitData {
  CardData card{};
};

struct CreditData {
  CardData card{};
  int installments{1};
};

using PaymentData = std::variant<PixData, DebitData, CreditData>;

} // namespace payment
