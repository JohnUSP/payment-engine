#include "payment/transaction.h"
#include <iostream>
#include <payment/card_utils.h>
#include <payment/pix_payment.h>
namespace payment {

void PixPayment::prepare(Transaction& transaction) const {

  auto& pixData = std::get<PixData>(transaction.getPaymentData());

  pixData.pixId = generatePixId();
  pixData.qrCode = generateQrCode();
  pixData.payerBank = selectIssuer();

  transaction.setStatus(TransactionStatus::VALIDATED);
}

void PixPayment::authorize(Transaction& transaction) const {

  if (transaction.getAmount() > PIX_MAX) {
    transaction.setStatus(TransactionStatus::DENIED);
  } else {
    transaction.setStatus(TransactionStatus::APPROVED);
  }
}

void PixPayment::complete(Transaction& transaction) const {

  transaction.setStatus(TransactionStatus::COMPLETED);
  printReceipt(transaction);
}

void PixPayment::printReceipt(const Transaction& transaction) const {
  const auto& pixData = std::get<PixData>(transaction.getPaymentData());

  Amount amount = transaction.getAmount();

  std::cout << "\n"
            << "================================\n"
            << "         PIX RECEIPT\n"
            << "================================\n"
            << std::format("Transaction ID: {}\n", transaction.getId())
            << std::format("PIX ID:         {}\n", pixData.pixId)
            << std::format("Payer bank:     {}\n", pixData.payerBank)
            << std::format("Amount:         R$ {}.{:02}\n", amount / 100,
                           amount % 100)
            << std::format("Status:         {}\n",
                           toString(transaction.getStatus()))
            << "================================\n";
}

void PixPayment::cancel(Transaction& transaction) const {

  transaction.setStatus(TransactionStatus::CANCELED);
}
} // namespace payment
