#pragma once
#include <payment/types/amount.hpp>
#include <payment/types/payment_data.hpp>
#include <payment/types/payment_type.hpp>
#include <payment/types/transaction_status.hpp>
#include <string>

namespace payment {

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
  DebitData& getDebitData();
  const DebitData& getDebitData() const;
  CreditData& getCreditData();
  const CreditData& getCreditData() const;
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
};

} // namespace payment
