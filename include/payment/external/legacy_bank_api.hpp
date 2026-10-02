#pragma once

#include <cstdint>

namespace payment::external {

class LegacyBankApi {
public:
  bool sendPayment(std::int64_t amountInCents);
};

} // namespace payment::external
