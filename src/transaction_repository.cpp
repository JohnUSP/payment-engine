
#include <format>
#include <payment/transaction_repository.h>
#include <stdexcept>

namespace payment {

void TransactionRepository::save(const Transaction& transaction) {

  auto [it, inserted] =
      m_transactions.insert({transaction.getId(), transaction});

  if (!inserted) {
    throw std::logic_error(std::format(
        "Cannot save transaction: ID {} already exists", transaction.getId()));
  }
}

const Transaction* TransactionRepository::findById(TransactionId id) const {

  auto it = m_transactions.find(id);

  if (it != m_transactions.end()) {
    return &it->second;
  }
  return nullptr;
}

std::size_t TransactionRepository::size() const {

  return m_transactions.size();
}

std::vector<const Transaction*> TransactionRepository::findAll() const {

  std::vector<const Transaction*> output;
  output.reserve(m_transactions.size());
  for (const auto& [id, tx] : m_transactions) {
    output.push_back(&tx);
  }

  return output;
}

std::vector<const Transaction*>
TransactionRepository::findByStatus(TransactionStatus status) const {

  std::vector<const Transaction*> output;
  for (const auto& [id, tx] : m_transactions) {
    if (tx.getStatus() == status) {
      output.push_back(&tx);
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
