#pragma once
#include <payment/transaction.h>

namespace payment {

class PaymentStrategy {

public:
  virtual ~PaymentStrategy() = default;
  void process(Transaction& transaction) const;

protected:
  virtual void prepare(Transaction& transaction) const = 0;
  virtual void authorize(Transaction& transaction) const = 0;
  virtual void complete(Transaction& transaction) const = 0;
  virtual void cancel(Transaction& transaction) const = 0;
};

} // namespace payment
