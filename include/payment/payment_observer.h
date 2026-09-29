#pragma once
#include <payment/payment_event.h>

namespace payment {

class Transaction;

class PaymentObserver {

public:
  virtual ~PaymentObserver() = default;
  virtual void onPaymentEvent(const Transaction& transaction,
                              PaymentEvent event) const = 0;
};

} // namespace payment
