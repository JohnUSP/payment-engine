#pragma once
#include <chrono>
#include <cstdint>
#include <string>

namespace payment {

enum class TransactionStatus { PENDING, APPROVED, DECLINED, FAILED };
enum class PaymentType { DEBIT, CREDIT, PIX };
using TransactionId = std::uint64_t;
using Amount = std::int64_t;
using TimeStamp = std::chrono::system_clock::time_point;

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
  void approve();
  void decline();
  void fail();

private:
  TransactionId m_id;
  TransactionStatus m_status;
  PaymentType m_type;
  Amount m_amount;
  std::string m_description;
  TimeStamp m_time;
  void checkStatusChange() const;
};

} // namespace payment
