#pragma once
#include <payment/transaction_repository.h>

namespace payment {

class PaymentEngine {

public:
  explicit PaymentEngine(TransactionRepository& repository);
  void process(TransactionId id);

private:
  TransactionRepository& m_repository;
};

} // namespace payment
