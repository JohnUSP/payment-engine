#include <payment/pix_payment.h>

namespace payment {

TransactionStatus PixPayment::process(const Transaction& transaction) const {

  if (transaction.getAmount() > PIX_MAX) {
    return TransactionStatus::DENIED;
  }
  return TransactionStatus::APPROVED;
}

} // namespace payment
