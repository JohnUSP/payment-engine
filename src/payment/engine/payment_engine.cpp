#include <format>
#include <payment/engine/payment_engine.hpp>
#include <payment/factories/payment_factory.hpp>
#include <payment/gateways/payment_gateway.hpp>
#include <payment/observers/payment_observer.hpp>
#include <payment/repositories/transaction_repository.hpp>
#include <payment/types/payment_event.hpp>
#include <payment/types/transaction.hpp>
#include <payment/types/transaction_id.hpp>
#include <stdexcept>

namespace payment {

PaymentEngine::PaymentEngine(TransactionRepository& repository,
                             PaymentGateway& gateway)
    : m_repository(repository), m_gateway(gateway) {}

void PaymentEngine::process(TransactionId id) {

  auto* transaction = m_repository.findById(id);
  if (!transaction) {
    throw std::out_of_range(
        std::format("Transaction with ID {} not found", id));
  }

  auto payment = PaymentFactory::create(transaction->getType());

  PaymentEventCallback callback = [this](const Transaction& transaction,
                                         PaymentEvent event) {
    notifyObservers(transaction, event);
  };
  payment->process(*transaction, m_gateway, callback);
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
