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
void Transaction::approve() {
  checkStatusChange();
  m_status = TransactionStatus::APPROVED;
}
void Transaction::decline() {
  checkStatusChange();
  m_status = TransactionStatus::DECLINED;
}
void Transaction::fail() {
  checkStatusChange();
  m_status = TransactionStatus::FAILED;
}

void Transaction::checkStatusChange() const {
  if (m_status != TransactionStatus::PENDING) {
    throw std::logic_error("Transaction final state cannot be altered");
  }
}

} // namespace payment
