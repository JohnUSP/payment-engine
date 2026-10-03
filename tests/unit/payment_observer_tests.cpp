#include <gtest/gtest.h>

#include <payment/interfaces/observer/event_logger.hpp>
#include <payment/interfaces/observer/receipt_printer.hpp>
#include <payment/types/payment_event.hpp>
#include <payment/types/transaction.hpp>

#include <iostream>
#include <sstream>
#include <string>

namespace {

using payment::PaymentEvent;
using payment::PaymentType;
using payment::ReceiptPrinter;
using payment::Transaction;
using payment::TransactionStatus;

class CoutCapture {
public:
  CoutCapture() : m_previous(std::cout.rdbuf(m_output.rdbuf())) {}
  ~CoutCapture() { std::cout.rdbuf(m_previous); }

  std::string str() const { return m_output.str(); }

private:
  std::ostringstream m_output;
  std::streambuf* m_previous;
};

void complete(Transaction& transaction) {
  transaction.setStatus(TransactionStatus::VALIDATED);
  transaction.setStatus(TransactionStatus::APPROVED);
  transaction.setStatus(TransactionStatus::COMPLETED);
}

bool hasFieldValue(const std::string& output, const std::string& label,
                   const std::string& value) {
  const auto labelPosition = output.find(label);
  if (labelPosition == std::string::npos) {
    return false;
  }

  const auto valuePosition =
      output.find_first_not_of(" \t", labelPosition + label.size());
  return valuePosition != std::string::npos &&
         output.compare(valuePosition, value.size(), value) == 0 &&
         (valuePosition + value.size() == output.size() ||
          output[valuePosition + value.size()] == '\n');
}

TEST(EventLoggerTest, LogsTransactionIdAndCompletedEvent) {
  Transaction transaction{701, PaymentType::CREDIT, 4250, "observer test"};
  complete(transaction);
  payment::EventLogger logger;
  CoutCapture output;

  logger.onPaymentEvent(transaction, PaymentEvent::TRANSACTION_COMPLETED);

  const auto log = output.str();
  EXPECT_NE(log.find("701"), std::string::npos);
  EXPECT_NE(log.find(payment::toString(PaymentEvent::TRANSACTION_COMPLETED)),
            std::string::npos);
}

TEST(ReceiptPrinterTest, IgnoresNonCompletedEvents) {
  const Transaction transaction{702, PaymentType::CREDIT, 1000, "not complete"};
  ReceiptPrinter printer;
  CoutCapture output;

  printer.onPaymentEvent(transaction, PaymentEvent::TRANSACTION_APPROVED);

  EXPECT_TRUE(output.str().empty());
}

TEST(ReceiptPrinterTest, PrintsCompletedCreditReceiptDetails) {
  Transaction transaction{703, PaymentType::CREDIT, 4250, "credit receipt"};
  auto& creditData = transaction.getCreditData();
  creditData.transactionData.amount = transaction.getAmount();
  creditData.transactionData.tranId = transaction.getId();
  creditData.transactionData.time = transaction.getTime();
  creditData.card.issuer = "Example Credit";
  creditData.card.cardNumber = "4111111111111111";
  creditData.installments = 3;
  complete(transaction);
  ReceiptPrinter printer;
  CoutCapture output;

  printer.onPaymentEvent(transaction, PaymentEvent::TRANSACTION_COMPLETED);

  const auto receipt = output.str();
  EXPECT_NE(receipt.find("703"), std::string::npos);
  EXPECT_NE(receipt.find("R$ 42.50"), std::string::npos);
  EXPECT_NE(receipt.find("Example Credit"), std::string::npos);
  EXPECT_NE(receipt.find("4111111111111111"), std::string::npos);
  EXPECT_TRUE(hasFieldValue(receipt, "Installments:", "3"));
}

TEST(ReceiptPrinterTest, PrintsCompletedDebitReceiptDetails) {
  Transaction transaction{704, PaymentType::DEBIT, 1875, "debit receipt"};
  auto& debitData = transaction.getDebitData();
  debitData.transactionData.amount = transaction.getAmount();
  debitData.transactionData.tranId = transaction.getId();
  debitData.transactionData.time = transaction.getTime();
  debitData.card.issuer = "Example Debit";
  debitData.card.cardNumber = "5555444433331111";
  complete(transaction);
  ReceiptPrinter printer;
  CoutCapture output;

  printer.onPaymentEvent(transaction, PaymentEvent::TRANSACTION_COMPLETED);

  const auto receipt = output.str();
  EXPECT_NE(receipt.find("704"), std::string::npos);
  EXPECT_NE(receipt.find("R$ 18.75"), std::string::npos);
  EXPECT_NE(receipt.find("Example Debit"), std::string::npos);
  EXPECT_NE(receipt.find("5555444433331111"), std::string::npos);
}

} // namespace
