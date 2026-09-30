#include <payment/strategies/pix_payment.hpp>
#include <payment/types/payment_data.hpp>
#include <payment/types/payment_event.hpp>
#include <payment/types/payment_event_callback.hpp>
#include <payment/types/transaction.hpp>
#include <payment/types/transaction_status.hpp>
#include <payment/utils/card_utils.hpp>

namespace payment {

void PixPayment::prepare(Transaction& transaction,
                         const PaymentEventCallback& callback) const {

  auto& pixData = std::get<PixData>(transaction.getPaymentData());

  pixData.transactionData.amount = transaction.getAmount();
  pixData.transactionData.tranId = transaction.getId();
  pixData.transactionData.time = transaction.getTime();
  pixData.pixId = generatePixId();
  pixData.qrCode = generateQrCode();
  pixData.payerBank = selectIssuer();

  transaction.setStatus(TransactionStatus::VALIDATED);
  callback(transaction, PaymentEvent::TRANSACTION_VALIDATED);
}

void PixPayment::authorize(Transaction& transaction,
                           const PaymentEventCallback& callback) const {

  if (transaction.getAmount() > PIX_MAX) {
    transaction.setStatus(TransactionStatus::DENIED);
    callback(transaction, PaymentEvent::TRANSACTION_DENIED);
  } else {
    transaction.setStatus(TransactionStatus::APPROVED);
    callback(transaction, PaymentEvent::TRANSACTION_APPROVED);
  }
}

void PixPayment::complete(Transaction& transaction,
                          const PaymentEventCallback& callback) const {

  transaction.setStatus(TransactionStatus::COMPLETED);
  callback(transaction, PaymentEvent::TRANSACTION_COMPLETED);
}

void PixPayment::cancel(Transaction& transaction,
                        const PaymentEventCallback& callback) const {

  transaction.setStatus(TransactionStatus::CANCELED);
  callback(transaction, PaymentEvent::TRANSACTION_CANCELED);
}
} // namespace payment
