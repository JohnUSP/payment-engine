#pragma once
#include <cstddef>
#include <payment/transaction.h>
#include <unordered_map>
#include <vector>

namespace payment {

class TransactionRepository {
public:
  std::size_t size() const;
  void save(const Transaction& transaction);
  void updateStatus(TransactionId id, TransactionStatus status);
  const Transaction* findById(TransactionId id) const;
  std::vector<const Transaction*> findAll() const;
  std::vector<const Transaction*> findByStatus(TransactionStatus status) const;

private:
  std::unordered_map<TransactionId, Transaction> m_transactions;
};

} // namespace payment
