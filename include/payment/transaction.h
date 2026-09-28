#pragma once
#include <chrono>
#include <cstdint>
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
using TransactionId = std::uint64_t;
using Amount = std::int64_t;
using TimeStamp = std::chrono::system_clock::time_point;
std::string_view toString(TransactionStatus status);
class Transaction {

public:
  Transaction(TransactionId id, PaymentType type, Amount amount,
              const std::string& description);
  TransactionId getId() const;
  TransactionStatus getStatus() const;
  PaymentType getType() const;
  Amount getAmount() const;
  const std::string& getDescription() const;
  TimeStamp getTime() const;
  void setStatus(TransactionStatus status);
  const PaymentData& getPaymentData() const;
  PaymentData& getPaymentData();

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
