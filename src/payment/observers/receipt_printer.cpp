#include <iostream>
#include <payment/observers/receipt_printer.hpp>
#include <payment/types/transaction.hpp>
#include <type_traits>
#include <variant>

namespace payment {

void ReceiptPrinter::onPaymentEvent(const Transaction& transaction,
                                    PaymentEvent event) const {

  if (event == PaymentEvent::TRANSACTION_COMPLETED) {
    printReceipt(transaction);
  }
}

void ReceiptPrinter::printReceipt(const Transaction& transaction) const {

  const auto& paymentData = transaction.getPaymentData();

  std::visit(
      [this](const auto& data) {
        using T = std::decay_t<decltype(data)>;

        if constexpr (std::is_same_v<T, PixData>) {
          printPixReceipt(data);

        } else if constexpr (std::is_same_v<T, DebitData>) {
          printDebitReceipt(data);

        } else if constexpr (std::is_same_v<T, CreditData>) {
          printCreditReceipt(data);
        }
      },
      paymentData);
}

void ReceiptPrinter::printPixReceipt(const PixData& data) const {

  const auto amount = data.transactionData.amount;
  const auto id = data.transactionData.tranId;
  const auto time = data.transactionData.time;
  const auto& pixId = data.pixId;
  const auto& bank = data.payerBank;

  std::cout << "\n"
            << "================================\n"
            << "         PIX RECEIPT\n"
            << "================================\n"
            << std::format("Transaction ID: {}\n", id)
            << std::format("PIX ID:         {}\n", pixId)
            << std::format("Payer bank:     {}\n", bank)
            << std::format("Amount:         R$ {}.{:02}\n", amount / 100,
                           amount % 100)
            << std::format("Time: {:%Y-%m-%d %H:%M}\n", time)
            << "================================\n";
}

void ReceiptPrinter::printDebitReceipt(const DebitData& data) const {

  const auto amount = data.transactionData.amount;
  const auto id = data.transactionData.tranId;
  const auto time = data.transactionData.time;
  const auto& issuer = data.card.issuer;
  const auto& cardNumber = data.card.cardNumber;

  std::cout << "\n"
            << "================================\n"
            << "        DEBIT RECEIPT\n"
            << "================================\n"
            << std::format("Transaction ID: {}\n", id)
            << std::format("Issuer:         {}\n", issuer)
            << std::format("Card:           {}\n", cardNumber)
            << std::format("Amount:         R$ {}.{:02}\n", amount / 100,
                           amount % 100)
            << std::format("Time: {:%Y-%m-%d %H:%M}\n", time)
            << "================================\n";
}

void ReceiptPrinter::printCreditReceipt(const CreditData& data) const {

  const auto amount = data.transactionData.amount;
  const auto id = data.transactionData.tranId;
  const auto time = data.transactionData.time;
  const auto installments = data.installments;
  const auto& issuer = data.card.issuer;
  const auto& cardNumber = data.card.cardNumber;

  std::cout << "\n"
            << "================================\n"
            << "        CREDIT RECEIPT\n"
            << "================================\n"
            << std::format("Transaction ID: {}\n", id)
            << std::format("Issuer:         {}\n", issuer)
            << std::format("Card:           {}\n", cardNumber)
            << std::format("Amount:         R$ {}.{:02}\n", amount / 100,
                           amount % 100)
            << std::format("Installments:   {}\n", installments)
            << std::format("Time: {:%Y-%m-%d %H:%M}\n", time)
            << "================================\n";
}

} // namespace payment
