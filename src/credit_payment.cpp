#include <payment/card_utils.h>
#include <payment/credit_payment.h>
#include <payment/payment_data.h>
namespace payment {

void CreditPayment::prepare(Transaction& transaction,
                            const PaymentEventCallback& callback) const {
  auto& creditData = std::get<CreditData>(transaction.getPaymentData());

  creditData.transactionData.amount = transaction.getAmount();
  creditData.transactionData.tranId = transaction.getId();
  creditData.transactionData.time = transaction.getTime();
  creditData.card.cardNumber = generateCardNumber();
  creditData.card.issuer = selectIssuer();
  creditData.installments = generateInstallments();

  transaction.setStatus(TransactionStatus::VALIDATED);
  callback(transaction, PaymentEvent::TRANSACTION_VALIDATED);
}

void CreditPayment::authorize(Transaction& transaction,
                              const PaymentEventCallback& callback) const {
  if (transaction.getAmount() > CREDIT_MAX) {
    transaction.setStatus(TransactionStatus::DENIED);
    callback(transaction, PaymentEvent::TRANSACTION_DENIED);
    return;
  }

  transaction.setStatus(TransactionStatus::APPROVED);
  callback(transaction, PaymentEvent::TRANSACTION_APPROVED);
}

void CreditPayment::complete(Transaction& transaction,
                             const PaymentEventCallback& callback) const {
  transaction.setStatus(TransactionStatus::COMPLETED);
  callback(transaction, PaymentEvent::TRANSACTION_COMPLETED);
}

void CreditPayment::cancel(Transaction& transaction,
                           const PaymentEventCallback& callback) const {
  transaction.setStatus(TransactionStatus::CANCELED);
  callback(transaction, PaymentEvent::TRANSACTION_CANCELED);
}

} // namespace payment
