#pragma once
#include <payment/payment_observer.hpp>
#include <payment/transaction_repository.hpp>
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
