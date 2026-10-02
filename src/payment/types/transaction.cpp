#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/types/amount.hpp>
#include <payment/types/payment_data.hpp>
#include <payment/types/payment_type.hpp>
#include <payment/types/transaction.hpp>
#include <payment/types/transaction_status.hpp>
namespace payment {

Transaction::Transaction(TransactionId id, PaymentType type, Amount amount,
                         const std::string& description)
    : m_amount(validateAmount(amount)), m_type(validatePayType(type)),
      m_paymentData(makePaymentData(m_type)), m_id(id),
      m_status(TransactionStatus::PENDING), m_description(description),
      m_time(std::chrono::system_clock::now()) {}

Amount Transaction::validateAmount(Amount amount) {
  if (amount <= 0) {
    throw InvalidTransactionAmount(
        "Transaction amount must be greater than zero");
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
DebitData& Transaction::getDebitData() {
  if (m_type != PaymentType::DEBIT) {
    throw InvalidPaymentType("Transaction is not a debit payment");
  }
  return std::get<DebitData>(m_paymentData);
}

const DebitData& Transaction::getDebitData() const {
  if (m_type != PaymentType::DEBIT) {
    throw InvalidPaymentType("Transaction is not a debit payment");
  }
  return std::get<DebitData>(m_paymentData);
}

CreditData& Transaction::getCreditData() {
  if (m_type != PaymentType::CREDIT) {
    throw InvalidPaymentType("Transaction is not a credit payment");
  }
  return std::get<CreditData>(m_paymentData);
}

const CreditData& Transaction::getCreditData() const {
  if (m_type != PaymentType::CREDIT) {
    throw InvalidPaymentType("Transaction is not a credit payment");
  }
  return std::get<CreditData>(m_paymentData);
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

  throw InvalidTransactionState("Invalid transaction status");
}

PaymentType Transaction::validatePayType(PaymentType type) {

  switch (type) {
  case PaymentType::CREDIT:
  case PaymentType::DEBIT:
    return type;
  }
  throw InvalidPaymentType("Invalid payment type");
}

PaymentData Transaction::makePaymentData(PaymentType type) {
  switch (type) {
  case PaymentType::CREDIT:
    return CreditData{};
  case PaymentType::DEBIT:
    return DebitData{};
  }

  throw InvalidPaymentType("Invalid payment type");
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

  throw InvalidTransactionState(
      std::format("Invalid transaction status transition from {} to {}",
                  toString(m_status), toString(newStatus)));
}

} // namespace payment
