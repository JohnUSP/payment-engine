#pragma once
#include <cstddef>
#include <payment/transaction.h>
#include <unordered_map>
#include <vector>

namespace payment {

class TransactionRepository {
public:
  TransactionRepository() = default;

  // copy not allowed
  TransactionRepository(const TransactionRepository&) = delete;
  TransactionRepository& operator=(const TransactionRepository&) = delete;
  // move not allowed
  TransactionRepository(TransactionRepository&&) = delete;
  TransactionRepository& operator=(TransactionRepository&&) = delete;

  std::size_t size() const;
  void save(Transaction&& transaction);
  void updateStatus(TransactionId id, TransactionStatus status);
  const Transaction* findById(TransactionId id) const;
  Transaction* findById(TransactionId id);
  std::vector<const Transaction*> findAll() const;
  std::vector<const Transaction*> findByStatus(TransactionStatus status) const;

private:
  std::unordered_map<TransactionId, Transaction> m_transactions;
};

} // namespace payment
