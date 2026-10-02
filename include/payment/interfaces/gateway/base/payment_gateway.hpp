#pragma once

#include <payment/types/process_result.hpp>

namespace payment {

class Transaction;

class PaymentGateway {

public:
  virtual ~PaymentGateway() = default;
  virtual ProcessResult::Authorization send(const Transaction&) = 0;
};

} // namespace payment
