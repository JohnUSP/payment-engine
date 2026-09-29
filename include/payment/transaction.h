#pragma once
#include <payment/payment_data.h>
#include <string>
#include <string_view>

namespace payment {

enum class TransactionStatus {
  PENDING,
  VALIDATED,
  APPROVED,
  DENIED,
  CANCELED,
  COMPLETED
};
enum class PaymentType { DEBIT, CREDIT, PIX };
std::string_view toString(TransactionStatus status);
class Transaction {

public:
  // copy not allowed
  Transaction(const Transaction&) = delete;
  Transaction& operator=(const Transaction&) = delete;
  // move allowed
  Transaction(Transaction&&) noexcept = default;
  Transaction& operator=(Transaction&&) noexcept = default;
  // constructor
  Transaction(TransactionId id, PaymentType type, Amount amount,
              const std::string& description);
  // getters
  TransactionId getId() const;
  TransactionStatus getStatus() const;
  PaymentType getType() const;
  Amount getAmount() const;
  const std::string& getDescription() const;
  TimeStamp getTime() const;
  const PaymentData& getPaymentData() const;
  PaymentData& getPaymentData();
  // setters
  void setStatus(TransactionStatus status);

private:
  Amount m_amount;
  PaymentType m_type; // always befor paymentData;
  PaymentData m_paymentData;
  TransactionId m_id;
  TransactionStatus m_status;
  std::string m_description;
  TimeStamp m_time;
  static Amount validateAmount(Amount amount);
  static TransactionStatus validateStatus(TransactionStatus status);
  static PaymentType validatePayType(PaymentType type);
  static PaymentData makePaymentData(PaymentType type);
  void checkStatusChange(TransactionStatus newStatus) const;
  void forbidStatusChange(TransactionStatus newStatus) const;
};

} // namespace payment
