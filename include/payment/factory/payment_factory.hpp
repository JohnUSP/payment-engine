#pragma once

#include <memory>
#include <payment/interfaces/process/base/payment_process.hpp>
#include <payment/types/payment_type.hpp>

namespace payment {

class PaymentProcessFactory {

public:
  static std::unique_ptr<PaymentProcess> create(PaymentType type);
};

} // namespace payment
