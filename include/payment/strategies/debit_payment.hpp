#pragma once
#include <payment/interfaces/payment_strategy.hpp>
#include <payment/types/amount.hpp>
#include <payment/types/payment_event_callback.hpp>
namespace payment {
class DebitPayment : public PaymentStrategy {

private:
  static constexpr Amount DEBIT_MAX = 100'000 * 100;
  void prepare(Transaction& transaction,
               const PaymentEventCallback& callback) const override;
  void authorize(Transaction& transaction,
                 const PaymentEventCallback& callback) const override;
  void complete(Transaction& transaction,
                const PaymentEventCallback& callback) const override;
  void cancel(Transaction& transaction,
              const PaymentEventCallback& callback) const override;
};

} // namespace payment
