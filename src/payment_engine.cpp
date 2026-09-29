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

  PaymentEventCallback callback = [this](const Transaction& transaction,
                                         PaymentEvent event) {
    notifyObservers(transaction, event);
  };
  payment->process(*transaction, callback);
}

void PaymentEngine::addObserver(const PaymentObserver& observer) {

  m_observers.push_back(&observer);
}

void PaymentEngine::notifyObservers(const Transaction& transaction,
                                    PaymentEvent event) const {

  for (const auto* observer : m_observers) {
    observer->onPaymentEvent(transaction, event);
  }
}

} // namespace payment
