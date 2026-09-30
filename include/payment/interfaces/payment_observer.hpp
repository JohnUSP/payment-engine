#pragma once
#include <payment/types/payment_event.hpp>

namespace payment {

class Transaction;

class PaymentObserver {

public:
  virtual ~PaymentObserver() = default;
  virtual void onPaymentEvent(const Transaction& transaction,
                              PaymentEvent event) const = 0;
};

} // namespace payment
