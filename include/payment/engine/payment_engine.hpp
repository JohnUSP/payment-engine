#pragma once

#include <memory>
#include <mutex>
#include <payment/types/payment_event.hpp>
#include <payment/types/transaction_id.hpp>
#include <unordered_map>
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
  mutable std::mutex m_observersMutex;
  std::mutex m_transactionLocksMutex;
  std::unordered_map<TransactionId, std::shared_ptr<std::mutex>>
      m_transactionLocks;
  void notifyObservers(const Transaction& transaction,
                       PaymentEvent event) const;
};

} // namespace payment
