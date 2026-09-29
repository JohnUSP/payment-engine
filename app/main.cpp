#include <iostream>
#include <payment/event_logger.hpp>
#include <payment/payment_engine.hpp>
#include <payment/receipt_printer.hpp>
#include <payment/transaction.hpp>
#include <payment/transaction_repository.hpp>
#include <vector>

int main() {
  std::cout << "Payment Engine Started" << std::endl;

  std::vector<payment::Transaction> transactions;

  transactions.reserve(4);

  transactions.emplace_back(1001, payment::PaymentType::CREDIT, 4000,
                            "Credit payment transaction");

  transactions.emplace_back(1002, payment::PaymentType::DEBIT, 5000,
                            "Debit payment transaction");

  transactions.emplace_back(1003, payment::PaymentType::PIX, 6000,
                            "Pix payment transaction");

  transactions.emplace_back(1004, payment::PaymentType::PIX, 2'100'000,
                            "Pix payment transaction above limit");

  payment::TransactionRepository repository;
  payment::PaymentEngine engine(repository);

  payment::ReceiptPrinter receiptPrinter;
  engine.addObserver(receiptPrinter);

  payment::EventLogger eventLogger;
  engine.addObserver(eventLogger);

  for (auto& entry : transactions) {
    const auto id = entry.getId();
    repository.save(std::move(entry));
    engine.process(id);
  }

  return 0;
}
