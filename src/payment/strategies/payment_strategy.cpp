#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/gateways/payment_gateway.hpp>
#include <payment/strategies/payment_strategy.hpp>
#include <payment/types/process_result.hpp>
#include <payment/types/transaction.hpp>

namespace payment {

void PaymentStrategy::process(Transaction& transaction, PaymentGateway& gateway,
                              const PaymentEventCallback& callback) const {

  const auto status = transaction.getStatus();
  if (status == TransactionStatus::PENDING) {

    const ProcessResult::PreAuthorization preAuthResult =
        prepare(transaction, callback);

    if (preAuthResult != ProcessResult::PreAuthorization::SUCCESS) {
      return;
    }
  } else if (status != TransactionStatus::VALIDATED) {
    throw InvalidTransactionState(std::format(
        "Transaction in state {} cannot be processed", toString(status)));
  }

  const ProcessResult::Authorization authResult =
      send(transaction, gateway, callback);

  if (authResult != ProcessResult::Authorization::AUTHORIZED) {
    return;
  }

  const ProcessResult::Finalization finalResult =
      complete(transaction, callback);

  if (finalResult != ProcessResult::Finalization::SUCCESS) {
    return;
  }
}

} // namespace payment
