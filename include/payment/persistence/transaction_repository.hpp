
#pragma once

#include <optional>
#include <payment/types/transaction.hpp>
#include <vector>

namespace payment {

class TransactionRepository {
public:
  virtual ~TransactionRepository() = default;

  virtual void add(Transaction transaction) = 0;

  virtual void update(const Transaction& transaction) = 0;

  virtual std::optional<Transaction> findById(TransactionId id) const = 0;

  virtual std::vector<Transaction>
  findByStatus(TransactionStatus status) const = 0;
};

} // namespace payment
