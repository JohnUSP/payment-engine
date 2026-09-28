#include <payment/payment_strategy.h>

namespace payment {

void PaymentStrategy::process(Transaction& transaction) const {

  bool keepProcessing = true;

  while (keepProcessing) {
    switch (transaction.getStatus()) {
    case TransactionStatus::PENDING:
      prepare(transaction);
      break;
    case TransactionStatus::VALIDATED:
      authorize(transaction);
      break;
    case TransactionStatus::APPROVED:
      complete(transaction);
      break;
    case TransactionStatus::DENIED:
    case TransactionStatus::CANCELED:
    case TransactionStatus::COMPLETED:
      keepProcessing = false;
      break;
    }
  }
}

} // namespace payment
