#include <payment/payment_strategy.h>

namespace payment {

void PaymentStrategy::process(Transaction& transaction,
                              const PaymentEventCallback& callback) const {

  bool keepProcessing = true;

  while (keepProcessing) {
    switch (transaction.getStatus()) {
    case TransactionStatus::PENDING:
      prepare(transaction, callback);
      break;
    case TransactionStatus::VALIDATED:
      authorize(transaction, callback);
      break;
    case TransactionStatus::APPROVED:
      complete(transaction, callback);
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
