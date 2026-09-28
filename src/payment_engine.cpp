#include <format>
#include <memory>
#include <payment/payment_engine.h>
#include <payment/payment_factory.h>
#include <stdexcept>

namespace payment {

PaymentEngine::PaymentEngine(TransactionRepository& repository)
    : m_repository(repository) {}

void PaymentEngine::process(TransactionId id) {

  Transaction* transaction = m_repository.findById(id);
  if (!transaction) {
    throw std::out_of_range(
        std::format("Transaction with ID {} not found", id));
  }

  auto payment = PaymentFactory::create(transaction->getType());

  payment->process(*transaction);
}

} // namespace payment
