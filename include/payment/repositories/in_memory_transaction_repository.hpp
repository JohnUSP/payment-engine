#pragma once
#include <payment/persistence/transaction_repository.hpp>
#include <unordered_map>

namespace payment {

class InMemoryTransactionRepository : public TransactionRepository {

public:
  void add(Transaction transaction) override;
  void update(const Transaction& transaction) override;
  std::optional<Transaction> findById(TransactionId id) const override;
  std::vector<Transaction>
  findByStatus(TransactionStatus status) const override;

private:
  std::unordered_map<TransactionId, Transaction> m_transactions;
};

} // namespace payment
