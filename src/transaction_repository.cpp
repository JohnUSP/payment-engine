
#include <format>
#include <payment/transaction_repository.h>
#include <stdexcept>

namespace payment {

void TransactionRepository::save(Transaction&& transaction) {

  const TransactionId id = transaction.getId();
  auto [it, inserted] = m_transactions.emplace(id, std::move(transaction));

  if (!inserted) {
    throw std::logic_error(
        std::format("Cannot save transaction: ID {} already exists", id));
  }
}

const Transaction* TransactionRepository::findById(TransactionId id) const {

  auto it = m_transactions.find(id);

  if (it != m_transactions.end()) {
    return &it->second;
  }
  return nullptr;
}

Transaction* TransactionRepository::findById(TransactionId id) {

  const auto& const_this = static_cast<const TransactionRepository&>(*this);

  return const_cast<Transaction*>(const_this.findById(id));
}

std::size_t TransactionRepository::size() const {

  return m_transactions.size();
}

std::vector<const Transaction*> TransactionRepository::findAll() const {

  std::vector<const Transaction*> output;
  output.reserve(m_transactions.size());
  for (const auto& [id, transaction] : m_transactions) {
    output.push_back(&transaction);
  }

  return output;
}

std::vector<const Transaction*>
TransactionRepository::findByStatus(TransactionStatus status) const {

  std::vector<const Transaction*> output;
  for (const auto& [id, transaction] : m_transactions) {
    if (transaction.getStatus() == status) {
      output.push_back(&transaction);
    }
  }

  return output;
}

void TransactionRepository::updateStatus(TransactionId id,
                                         TransactionStatus status) {

  auto it = m_transactions.find(id);

  if (it == m_transactions.end()) {
    throw std::out_of_range(
        std::format("Transaction with ID {} not found", id));
  }

  it->second.setStatus(status);
}
} // namespace payment
