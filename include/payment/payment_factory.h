#pragma once
#include <memory>
#include <payment/payment_strategy.h>

namespace payment {

class PaymentFactory {

public:
  static std::unique_ptr<PaymentStrategy> create(PaymentType type);
};
} // namespace payment
