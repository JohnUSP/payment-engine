#pragma once

#include <payment/interfaces/gateway/base/payment_gateway.hpp>

namespace payment {

class Transaction;

class SecondaryBank : public PaymentGateway {
public:
  ProcessResult::Authorization send(const Transaction&) override;
};

} // namespace payment
