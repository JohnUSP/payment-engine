
#include <format>
#include <mutex>
#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/interfaces/persistence/in_memory_transaction_repository.hpp>
#include <payment/types/transaction.hpp>

namespace payment {

void InMemoryTransactionRepository::add(Transaction transaction) {

  std::unique_lock lock(m_mutex);

  const auto id = transaction.getId();

  auto [it, inserted] = m_transactions.emplace(id, std::move(transaction));
  if (!inserted) {
    throw InvalidTransactionOperation(
        std::format("Cannot save transaction: ID {} already exists", id));
  }
}

void InMemoryTransactionRepository::update(const Transaction& transaction) {

  std::unique_lock lock(m_mutex);

  const auto id = transaction.getId();

  auto it = m_transactions.find(id);
  if (it == m_transactions.end()) {
    throw TransactionNotFound(
        std::format("Transaction with ID {} not found", id));
  }

  it->second = transaction;
}

std::optional<Transaction>
InMemoryTransactionRepository::findById(TransactionId id) const {

  std::shared_lock lock(m_mutex);
  const auto it = m_transactions.find(id);

  if (it != m_transactions.end()) {
    return it->second;
  }

  return std::nullopt;
}

std::vector<Transaction>
InMemoryTransactionRepository::findByStatus(TransactionStatus status) const {

  std::shared_lock lock(m_mutex);
  std::vector<Transaction> output;

  for (const auto& [id, transaction] : m_transactions) {
    if (transaction.getStatus() == status) {
      output.push_back(transaction);
    }
  }

  return output;
}

} // namespace payment
