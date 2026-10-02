#pragma once
#include <payment/types/payment_event.hpp>
#include <payment/types/transaction_id.hpp>
#include <vector>
namespace payment {

class Transaction;
class PaymentObserver;
class TransactionRepository;
class PaymentGateway;
class PaymentEngine {

public:
  explicit PaymentEngine(TransactionRepository& repository,
                         PaymentGateway& gateway);
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
  PaymentGateway& m_gateway;
  std::vector<const PaymentObserver*> m_observers;
  void notifyObservers(const Transaction& transaction,
                       PaymentEvent event) const;
};

} // namespace payment
