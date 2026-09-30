#include <payment/types/amount.hpp>
#include <payment/types/payment_data.hpp>
#include <payment/types/payment_type.hpp>
#include <payment/types/transaction.hpp>
#include <payment/types/transaction_status.hpp>
#include <stdexcept>
namespace payment {

Transaction::Transaction(TransactionId id, PaymentType type, Amount amount,
                         const std::string& description)
    : m_amount(validateAmount(amount)), m_type(validatePayType(type)),
      m_paymentData(makePaymentData(m_type)), m_id(id),
      m_status(TransactionStatus::PENDING), m_description(description),
      m_time(std::chrono::system_clock::now()) {}

Amount Transaction::validateAmount(Amount amount) {
  if (amount <= 0) {
    throw std::invalid_argument("Transaction amount must be greater than zero");
  }
  return amount;
}
TransactionId Transaction::getId() const { return m_id; }
TransactionStatus Transaction::getStatus() const { return m_status; }
PaymentType Transaction::getType() const { return m_type; }
Amount Transaction::getAmount() const { return m_amount; }
const std::string& Transaction::getDescription() const { return m_description; }
TimeStamp Transaction::getTime() const { return m_time; }
const PaymentData& Transaction::getPaymentData() const { return m_paymentData; }
PaymentData& Transaction::getPaymentData() {

  const auto& const_this = static_cast<const Transaction&>(*this);
  return const_cast<PaymentData&>(const_this.getPaymentData());
}

void Transaction::setStatus(const TransactionStatus newStatus) {

  checkStatusChange(validateStatus(newStatus));
  m_status = newStatus;
}

TransactionStatus Transaction::validateStatus(TransactionStatus status) {

  switch (status) {
  case TransactionStatus::PENDING:
  case TransactionStatus::VALIDATED:
  case TransactionStatus::APPROVED:
  case TransactionStatus::DENIED:
  case TransactionStatus::COMPLETED:
  case TransactionStatus::CANCELED:
    return status;
  }

  throw std::logic_error("Invalid transaction status");
}

PaymentType Transaction::validatePayType(PaymentType type) {

  switch (type) {
  case PaymentType::PIX:
  case PaymentType::CREDIT:
  case PaymentType::DEBIT:
    return type;
  }
  throw std::invalid_argument("Invalid payment type");
}

PaymentData Transaction::makePaymentData(PaymentType type) {
  switch (type) {
  case PaymentType::PIX:
    return PixData{};
  case PaymentType::CREDIT:
    return CreditData{};
  case PaymentType::DEBIT:
    return DebitData{};
  }

  throw std::invalid_argument("Invalid payment type");
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

} // namespace payment
