#pragma once
#include <payment/payment_strategy.h>

namespace payment {

class PixPayment : public PaymentStrategy {

public:
  TransactionStatus process(const Transaction& transaction) const override;

private:
  static constexpr Amount PIX_MAX = 20'000 * 100;
};

} // namespace payment
