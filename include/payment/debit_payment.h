#pragma once
#include <payment/payment_strategy.h>

namespace payment {

class DebitPayment : public PaymentStrategy {

public:
  TransactionStatus process(const Transaction& transaction) const override;

private:
  static constexpr Amount DEBIT_MAX = 100'000 * 100;
};

} // namespace payment
