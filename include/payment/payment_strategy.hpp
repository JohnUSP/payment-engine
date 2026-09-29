#pragma once
#include <payment/payment_event.hpp>
#include <payment/transaction.hpp>

namespace payment {

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
