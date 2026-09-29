#pragma once
#include <payment/payment_strategy.h>

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
  void printReceipt(const Transaction& transaction,
                    const PaymentEventCallback& callback) const;
};

} // namespace payment
