#include <payment/interfaces/gateway/secondary_bank.hpp>
#include <payment/types/transaction.hpp>

namespace payment {

ProcessResult::Authorization
SecondaryBank::send(const Transaction& transaction) {
  if (transaction.getId() == 0) {
    return ProcessResult::Authorization::ERROR;
  }

  if (transaction.getAmount() > 100'000) {
    return ProcessResult::Authorization::DECLINED;
  }

  return ProcessResult::Authorization::AUTHORIZED;
}

} // namespace payment
