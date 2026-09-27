#include <payment/debit_payment.h>

namespace payment {

TransactionStatus DebitPayment::process(const Transaction& transaction) const {

  if (transaction.getAmount() > DEBIT_MAX) {
    return TransactionStatus::DENIED;
  }
  return TransactionStatus::APPROVED;
}
} // namespace payment
