#pragma once
#include <payment/transaction.h>

namespace payment {

class PaymentStrategy {

public:
  virtual ~PaymentStrategy() = default;
  virtual TransactionStatus process(const Transaction& transaction) const = 0;
};

} // namespace payment
