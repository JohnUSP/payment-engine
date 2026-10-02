#pragma once

#include <payment/types/payment_event_callback.hpp>
#include <payment/types/process_result.hpp>
namespace payment {

class Transaction;
class PaymentGateway;
class PaymentStrategy {

public:
  virtual ~PaymentStrategy() = default;
  void process(Transaction& transaction, PaymentGateway& gateway,
               const PaymentEventCallback& callback) const;

protected:
  virtual ProcessResult::PreAuthorization
  prepare(Transaction& transaction,
          const PaymentEventCallback& callback) const = 0;
  virtual ProcessResult::Authorization
  send(Transaction& transaction, PaymentGateway& gateway,
       const PaymentEventCallback& callback) const = 0;
  virtual ProcessResult::Finalization
  complete(Transaction& transaction,
           const PaymentEventCallback& callback) const = 0;
  virtual void cancel(Transaction& transaction,
                      const PaymentEventCallback& callback) const = 0;
};

} // namespace payment
