#include <iostream>
#include <payment/payment_engine.h>
#include <payment/transaction.h>
#include <payment/transaction_repository.h>
#include <vector>

int main() {
  std::cout << "Payment Engine Started" << std::endl;

  std::vector<payment::Transaction> transactions{
      payment::Transaction(1001, payment::PaymentType::CREDIT, 4000,
                           "Credit payment transaction"),
      payment::Transaction(1002, payment::PaymentType::DEBIT, 5000,
                           "Debit payment transaction"),
      payment::Transaction(1003, payment::PaymentType::PIX, 6000,
                           "Pix payment transaction"),
      payment::Transaction(1004, payment::PaymentType::PIX, 2100000,
                           "Pix payment transaction above limit")};

  payment::TransactionRepository repository;
  payment::PaymentEngine engine(repository);

  for (const auto& entry : transactions) {
    repository.save(entry);
    engine.process(entry.getId());
  }

  return 0;
}
