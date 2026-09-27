#include <format>
#include <payment/transaction.h>
#include <stdexcept>
namespace payment {

Transaction::Transaction(TransactionId id, PaymentType type, Amount amount,
                         const std::string& description)
    : m_id(id), m_status(TransactionStatus::PENDING), m_type(type),
      m_amount(amount), m_description(description) {
  if (amount <= 0) {
    throw std::invalid_argument("Transaction amount must be greater than zero");
  }
  m_time = std::chrono::system_clock::now();
}

TransactionId Transaction::getId() const { return m_id; }
TransactionStatus Transaction::getStatus() const { return m_status; }
PaymentType Transaction::getType() const { return m_type; }
Amount Transaction::getAmount() const { return m_amount; }
const std::string& Transaction::getDescription() const { return m_description; }
TimeStamp Transaction::getTime() const { return m_time; }

void Transaction::setStatus(const TransactionStatus newStatus) {
  checkStatusChange(newStatus);
  m_status = newStatus;
}
void Transaction::forbidStatusChange(TransactionStatus newStatus) const {
  throw std::logic_error(
      std::format("Transaction with status {} cannot transition to {}",
                  toString(m_status), toString(newStatus)));
}
void Transaction::checkStatusChange(TransactionStatus newStatus) const {
  switch (m_status) {

  case TransactionStatus::PENDING:
    if (newStatus == TransactionStatus::VALIDATED ||
        newStatus == TransactionStatus::CANCELED) {
      return;
    }
    break;

  case TransactionStatus::VALIDATED:
    if (newStatus == TransactionStatus::APPROVED ||
        newStatus == TransactionStatus::DENIED ||
        newStatus == TransactionStatus::CANCELED) {
      return;
    }
    break;

  case TransactionStatus::APPROVED:
    if (newStatus == TransactionStatus::COMPLETED ||
        newStatus == TransactionStatus::CANCELED) {
      return;
    }
    break;

  case TransactionStatus::DENIED:
  case TransactionStatus::CANCELED:
  case TransactionStatus::COMPLETED:
    // Terminal states: no transitions allowed.
    break;
  }

  forbidStatusChange(newStatus);
}
std::string_view toString(TransactionStatus status) {
  switch (status) {
  case TransactionStatus::PENDING:
    return "Pending";
  case TransactionStatus::VALIDATED:
    return "Validated";
  case TransactionStatus::APPROVED:
    return "Approved";
  case TransactionStatus::DENIED:
    return "Denied";
  case TransactionStatus::COMPLETED:
    return "Completed";
  case TransactionStatus::CANCELED:
    return "Canceled";
  }

  throw std::logic_error("Invalid transaction status");
}
} // namespace payment
