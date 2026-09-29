#include <payment/card_utils.hpp>
#include <payment/debit_payment.hpp>
#include <payment/payment_data.hpp>

namespace payment {

void DebitPayment::prepare(Transaction& transaction,
                           const PaymentEventCallback& callback) const {

  auto& debitData = std::get<DebitData>(transaction.getPaymentData());

  debitData.transactionData.amount = transaction.getAmount();
  debitData.transactionData.tranId = transaction.getId();
  debitData.transactionData.time = transaction.getTime();
  debitData.card.cardNumber = generateCardNumber();
  debitData.card.issuer = selectIssuer();

  transaction.setStatus(TransactionStatus::VALIDATED);
  callback(transaction, PaymentEvent::TRANSACTION_VALIDATED);
}

void DebitPayment::authorize(Transaction& transaction,
                             const PaymentEventCallback& callback) const {
  if (transaction.getAmount() > DEBIT_MAX) {
    transaction.setStatus(TransactionStatus::DENIED);
    callback(transaction, PaymentEvent::TRANSACTION_DENIED);
    return;
  }

  transaction.setStatus(TransactionStatus::APPROVED);
  callback(transaction, PaymentEvent::TRANSACTION_APPROVED);
}

void DebitPayment::complete(Transaction& transaction,
                            const PaymentEventCallback& callback) const {
  transaction.setStatus(TransactionStatus::COMPLETED);
  callback(transaction, PaymentEvent::TRANSACTION_COMPLETED);
}

void DebitPayment::cancel(Transaction& transaction,
                          const PaymentEventCallback& callback) const {
  transaction.setStatus(TransactionStatus::CANCELED);
  callback(transaction, PaymentEvent::TRANSACTION_CANCELED);
}

} // namespace payment
