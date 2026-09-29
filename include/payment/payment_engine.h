#pragma once
#include <payment/transaction_repository.h>

namespace payment {

class PaymentEngine {

public:
  explicit PaymentEngine(TransactionRepository& repository);
  // copy not allowed
  PaymentEngine(const PaymentEngine&) = delete;
  PaymentEngine& operator=(const PaymentEngine&) = delete;
  // move not allowed
  PaymentEngine(PaymentEngine&&) = delete;
  PaymentEngine& operator=(PaymentEngine&&) = delete;

  void process(TransactionId id);

private:
  TransactionRepository& m_repository;
};

} // namespace payment
