#pragma once
#include <payment/payment_strategy.h>
namespace payment {

class PixPayment : public PaymentStrategy {

private:
  static constexpr Amount PIX_MAX = 20'000 * 100;
  void prepare(Transaction& transaction) const override;
  void authorize(Transaction& transaction) const override;
  void complete(Transaction& transaction) const override;
  void cancel(Transaction& transaction) const override;
  void printReceipt(const Transaction& transaction) const;
};

} // namespace payment
