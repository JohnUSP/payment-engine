#include <payment/external/legacy_bank_api.hpp>

namespace payment::external {

bool LegacyBankApi::sendPayment(std::int64_t amountInCents) {
  return amountInCents > 0;
}
} // namespace payment::external
