#pragma once
#include <payment/types/payment_event_callback.hpp>

namespace payment {

class Transaction;

class PaymentStrategy {

public:
  virtual ~PaymentStrategy() = default;
  void process(Transaction& transaction,
               const PaymentEventCallback& callback) const;

protected:
  virtual void prepare(Transaction& transaction,
                       const PaymentEventCallback& callback) const = 0;
  virtual void authorize(Transaction& transaction,
                         const PaymentEventCallback& callback) const = 0;
  virtual void complete(Transaction& transaction,
                        const PaymentEventCallback& callback) const = 0;
  virtual void cancel(Transaction& transaction,
                      const PaymentEventCallback& callback) const = 0;
};

} // namespace payment
