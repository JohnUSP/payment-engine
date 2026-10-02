#pragma once

#include <payment/interfaces/observer/base/payment_observer.hpp>
#include <payment/types/payment_data.hpp>
#include <payment/types/payment_event.hpp>

namespace payment {

class Transaction;

class ReceiptPrinter : public PaymentObserver {

public:
  void onPaymentEvent(const Transaction& transaction,
                      PaymentEvent event) const override;

private:
  void printReceipt(const Transaction& transaction) const;
  void printDebitReceipt(const DebitData& data) const;
  void printCreditReceipt(const CreditData& data) const;
};
} // namespace payment
