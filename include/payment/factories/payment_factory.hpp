#pragma once
#include <memory>
#include <payment/strategies/payment_strategy.hpp>
#include <payment/types/payment_type.hpp>
namespace payment {
class PaymentFactory {

public:
  static std::unique_ptr<PaymentStrategy> create(PaymentType type);
};

} // namespace payment
