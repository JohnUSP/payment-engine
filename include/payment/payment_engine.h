#pragma once
#include <payment/payment_observer.h>
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
  void addObserver(const PaymentObserver& observer);

private:
  TransactionRepository& m_repository;
  std::vector<const PaymentObserver*> m_observers;
  void notifyObservers(const Transaction& transaction,
                       PaymentEvent event) const;
};

} // namespace payment
