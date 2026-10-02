#include <payment/gateways/payment_gateway.hpp>
#include <payment/strategies/debit_payment.hpp>
#include <payment/types/payment_data.hpp>
#include <payment/types/payment_event.hpp>
#include <payment/types/payment_event_callback.hpp>
#include <payment/types/transaction.hpp>
#include <payment/utils/card_utils.hpp>

namespace payment {

namespace {
constexpr Amount DEBIT_MAX = 100'000 * 100;
}

bool DebitPayment::preAuthorize(Transaction& transaction) const {
  auto& debitData = transaction.getDebitData();

  debitData.transactionData.amount = transaction.getAmount();
  debitData.transactionData.tranId = transaction.getId();
  debitData.transactionData.time = transaction.getTime();
  debitData.card.cardNumber = generateCardNumber();
  debitData.card.issuer = selectIssuer();

  return transaction.getAmount() <= DEBIT_MAX;
}

bool DebitPayment::confirm(Transaction& transaction) const {

  auto& debitData = transaction.getDebitData();
  return true;
}

ProcessResult::PreAuthorization
DebitPayment::prepare(Transaction& transaction,
                      const PaymentEventCallback& callback) const {

  callback(transaction, PaymentEvent::TRANSACTION_PENDING);

  if (!preAuthorize(transaction)) {
    return ProcessResult::PreAuthorization::ERROR;
  }

  transaction.setStatus(TransactionStatus::VALIDATED);
  callback(transaction, PaymentEvent::TRANSACTION_VALIDATED);

  return ProcessResult::PreAuthorization::SUCCESS;
}

ProcessResult::Authorization
DebitPayment::send(Transaction& transaction, PaymentGateway& gateway,
                   const PaymentEventCallback& callback) const {

  const ProcessResult::Authorization authorizationResult =
      gateway.send(transaction);

  switch (authorizationResult) {
  case ProcessResult::Authorization::DECLINED:
    transaction.setStatus(TransactionStatus::DENIED);
    callback(transaction, PaymentEvent::TRANSACTION_DENIED);
    break;
  case ProcessResult::Authorization::AUTHORIZED:
    transaction.setStatus(TransactionStatus::APPROVED);
    callback(transaction, PaymentEvent::TRANSACTION_APPROVED);
    break;
  case ProcessResult::Authorization::ERROR:
    break;
  }

  return authorizationResult;
}

ProcessResult::Finalization
DebitPayment::complete(Transaction& transaction,
                       const PaymentEventCallback& callback) const {

  if (!confirm(transaction)) {
    return ProcessResult::Finalization::ERROR;
  }
  transaction.setStatus(TransactionStatus::COMPLETED);
  callback(transaction, PaymentEvent::TRANSACTION_COMPLETED);

  return ProcessResult::Finalization::SUCCESS;
}

void DebitPayment::cancel(Transaction& transaction,
                          const PaymentEventCallback& callback) const {
  transaction.setStatus(TransactionStatus::CANCELED);
  callback(transaction, PaymentEvent::TRANSACTION_CANCELED);
}

} // namespace payment
