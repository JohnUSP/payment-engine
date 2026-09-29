#pragma once
#include <payment/payment_strategy.hpp>

namespace payment {

class CreditPayment : public PaymentStrategy {

private:
  static constexpr Amount CREDIT_MAX = 100'000 * 100;
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
