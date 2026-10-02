#pragma once
#include <payment/strategies/payment_strategy.hpp>
#include <payment/types/amount.hpp>
#include <payment/types/payment_event_callback.hpp>

namespace payment {

class Transaction;
class PaymentGateway;
class CreditPayment : public PaymentStrategy {

private:
  static constexpr Amount CREDIT_MAX = 100'000 * 100;
  bool preAuthorize(Transaction& transaction) const;
  bool confirm(Transaction& transaction) const;

  ProcessResult::PreAuthorization
  prepare(Transaction& transaction,
          const PaymentEventCallback& callback) const override;
  ProcessResult::Authorization
  send(Transaction& transaction, PaymentGateway& gateway,
       const PaymentEventCallback& callback) const override;
  ProcessResult::Finalization
  complete(Transaction& transaction,
           const PaymentEventCallback& callback) const override;
  void cancel(Transaction& transaction,
              const PaymentEventCallback& callback) const override;
};

} // namespace payment
