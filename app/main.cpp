#include <iostream>
#include <payment/engine/payment_engine.hpp>
#include <payment/gateways/legacy_bank.hpp>
#include <payment/network/http_client.hpp>
#include <payment/observers/event_logger.hpp>
#include <payment/observers/receipt_printer.hpp>
#include <payment/repositories/transaction_repository.hpp>
#include <payment/types/transaction.hpp>
#include <vector>

int main() {
  std::cout << "Payment Engine Started" << std::endl;

  std::vector<payment::Transaction> transactions;

  transactions.reserve(4);

  transactions.emplace_back(1001, payment::PaymentType::CREDIT, 4000,
                            "Credit payment transaction");

  transactions.emplace_back(1002, payment::PaymentType::DEBIT, 5000,
                            "Debit payment transaction");

  payment::TransactionRepository repository;
  payment::HttpClient httpClient;
  payment::LegacyBank gateway{httpClient};

  payment::PaymentEngine engine(repository, gateway);

  payment::EventLogger eventLogger;
  engine.addObserver(eventLogger);
  payment::ReceiptPrinter receiptPrinter;
  engine.addObserver(receiptPrinter);

  for (auto& entry : transactions) {
    const auto id = entry.getId();
    repository.save(std::move(entry));
    engine.process(id);
  }

  return 0;
}
