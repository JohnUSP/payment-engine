// debit_payment.cpp

#include <format>
#include <iostream>
#include <payment/card_utils.h>
#include <payment/debit_payment.h>
#include <payment/payment_data.h>

namespace payment {

void DebitPayment::prepare(Transaction& transaction) const {
  auto& debitData = std::get<DebitData>(transaction.getPaymentData());

  debitData.card.cardNumber = generateCardNumber();
  debitData.card.issuer = selectIssuer();

  transaction.setStatus(TransactionStatus::VALIDATED);
}

void DebitPayment::authorize(Transaction& transaction) const {
  if (transaction.getAmount() > DEBIT_MAX) {
    transaction.setStatus(TransactionStatus::DENIED);
    return;
  }

  transaction.setStatus(TransactionStatus::APPROVED);
}

void DebitPayment::complete(Transaction& transaction) const {
  transaction.setStatus(TransactionStatus::COMPLETED);
  printReceipt(transaction);
}

void DebitPayment::cancel(Transaction& transaction) const {
  transaction.setStatus(TransactionStatus::CANCELED);
}

void DebitPayment::printReceipt(const Transaction& transaction) const {
  const auto& debitData = std::get<DebitData>(transaction.getPaymentData());

  const Amount amount = transaction.getAmount();

  std::cout << "\n"
            << "================================\n"
            << "        DEBIT RECEIPT\n"
            << "================================\n"
            << std::format("Transaction ID: {}\n", transaction.getId())
            << std::format("Issuer:         {}\n", debitData.card.issuer)
            << std::format("Card:           {}\n", debitData.card.cardNumber)
            << std::format("Amount:         R$ {}.{:02}\n", amount / 100,
                           amount % 100)
            << std::format("Status:         {}\n",
                           toString(transaction.getStatus()))
            << "================================\n";
}

} // namespace payment
