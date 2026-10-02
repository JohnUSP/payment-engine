#include <payment/gateways/payment_gateway.hpp>
#include <payment/strategies/credit_payment.hpp>
#include <payment/types/payment_data.hpp>
#include <payment/types/payment_event.hpp>
#include <payment/types/payment_event_callback.hpp>
#include <payment/types/transaction.hpp>
#include <payment/utils/card_utils.hpp>

namespace payment {

namespace {
constexpr Amount CREDIT_MAX = 100'000 * 100;
}
bool CreditPayment::preAuthorize(Transaction& transaction) const {

  auto& creditData = transaction.getCreditData();

  creditData.transactionData.amount = transaction.getAmount();
  creditData.transactionData.tranId = transaction.getId();
  creditData.transactionData.time = transaction.getTime();
  creditData.card.cardNumber = generateCardNumber();
  creditData.card.issuer = selectIssuer();
  creditData.installments = generateInstallments();
  return transaction.getAmount() <= CREDIT_MAX;
}

bool CreditPayment::confirm(Transaction& transaction) const {
  auto& creditData = transaction.getCreditData();
  return true;
}

ProcessResult::PreAuthorization
CreditPayment::prepare(Transaction& transaction,
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
CreditPayment::send(Transaction& transaction, PaymentGateway& gateway,
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
CreditPayment::complete(Transaction& transaction,
                        const PaymentEventCallback& callback) const {
  transaction.setStatus(TransactionStatus::COMPLETED);

  if (!confirm(transaction)) {
    return ProcessResult::Finalization::ERROR;
  }
  callback(transaction, PaymentEvent::TRANSACTION_COMPLETED);
  return ProcessResult::Finalization::SUCCESS;
}

void CreditPayment::cancel(Transaction& transaction,
                           const PaymentEventCallback& callback) const {
  transaction.setStatus(TransactionStatus::CANCELED);
  callback(transaction, PaymentEvent::TRANSACTION_CANCELED);
}

} // namespace payment
