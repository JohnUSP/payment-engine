#include <format>
#include <iostream>
#include <payment/card_utils.h>
#include <payment/credit_payment.h>
#include <payment/payment_data.h>
namespace payment {

void CreditPayment::prepare(Transaction& transaction) const {
  auto& creditData = std::get<CreditData>(transaction.getPaymentData());

  creditData.card.cardNumber = generateCardNumber();
  creditData.card.issuer = selectIssuer();
  creditData.installments = generateInstallments();

  transaction.setStatus(TransactionStatus::VALIDATED);
}

void CreditPayment::authorize(Transaction& transaction) const {
  if (transaction.getAmount() > CREDIT_MAX) {
    transaction.setStatus(TransactionStatus::DENIED);
    return;
  }

  transaction.setStatus(TransactionStatus::APPROVED);
}

void CreditPayment::complete(Transaction& transaction) const {
  transaction.setStatus(TransactionStatus::COMPLETED);
  printReceipt(transaction);
}

void CreditPayment::cancel(Transaction& transaction) const {
  transaction.setStatus(TransactionStatus::CANCELED);
}

void CreditPayment::printReceipt(const Transaction& transaction) const {
  const auto& creditData = std::get<CreditData>(transaction.getPaymentData());

  const Amount amount = transaction.getAmount();

  std::cout << "\n"
            << "================================\n"
            << "       CREDIT RECEIPT\n"
            << "================================\n"
            << std::format("Transaction ID: {}\n", transaction.getId())
            << std::format("Issuer:         {}\n", creditData.card.issuer)
            << std::format("Card:           {}\n", creditData.card.cardNumber)
            << std::format("Installments:   {}\n", creditData.installments)
            << std::format("Amount:         R$ {}.{:02}\n", amount / 100,
                           amount % 100)
            << std::format("Status:         {}\n",
                           toString(transaction.getStatus()))
            << "================================\n";
}

} // namespace payment
