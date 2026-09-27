#include <payment/credit_payment.h>

namespace payment {

TransactionStatus CreditPayment::process(const Transaction& transaction) const {

  if (transaction.getAmount() > CREDIT_MAX) {
    return TransactionStatus::DENIED;
  }
  return TransactionStatus::APPROVED;
}
} // namespace payment
