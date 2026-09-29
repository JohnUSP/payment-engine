#pragma once
#include <payment/payment_observer.hpp>
#include <payment/transaction.hpp>

namespace payment {

class ReceiptPrinter : public PaymentObserver {
public:
  void onPaymentEvent(const Transaction& transaction,
                      PaymentEvent event) const override;

private:
  void printReceipt(const Transaction& transaction) const;
  void printPixReceipt(const PixData& data) const;
  void printDebitReceipt(const DebitData& data) const;
  void printCreditReceipt(const CreditData& data) const;
};
} // namespace payment
