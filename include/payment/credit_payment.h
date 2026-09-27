#pragma once
#include <payment/payment_strategy.h>

namespace payment {

class CreditPayment : public PaymentStrategy {

public:
  TransactionStatus process(const Transaction& transaction) const override;

private:
  static constexpr Amount CREDIT_MAX = 100'000 * 100;
};

} // namespace payment
