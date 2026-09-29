#pragma once
#include <memory>
#include <payment/payment_strategy.hpp>

namespace payment {

class PaymentFactory {

public:
  static std::unique_ptr<PaymentStrategy> create(PaymentType type);
};
} // namespace payment
