#include <iostream>
#include <payment/transaction.h>
#include <payment/transaction_repository.h>
int main() {
  std::cout << "Payment Engine Started" << std::endl;

  payment::Transaction tx1(1367, payment::PaymentType::CREDIT, 6000,
                           "Credit payment transaction");
  payment::Transaction tx2(9875, payment::PaymentType::PIX, 5000,
                           "Pix payment transaction");

  payment::TransactionRepository repository;
  repository.save(tx1);
  repository.save(tx2);

  const payment::Transaction* tx3 = repository.findById(1367);

  if (tx3) {
    std::cout << tx3->getDescription() << " ID: " << tx3->getId()
              << " Amount: " << tx3->getAmount() << std::endl;
  }

  const payment::Transaction* missing = repository.findById(999999);
  if (!missing) {
    std::cout << std::format("Transaction not found: ID {}", 999999)
              << std::endl;
  }

  return 0;
}
