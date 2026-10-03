#include <exception>
#include <format>
#include <iostream>
#include <memory>
#include <payment/engine/payment_engine.hpp>
#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/factory/payment_factory.hpp>
#include <payment/interfaces/observer/base/payment_observer.hpp>
#include <payment/interfaces/persistence/base/transaction_repository.hpp>
#include <syncstream>

namespace payment {

PaymentEngine::PaymentEngine(TransactionRepository& repository,
                             PaymentGateway& gateway)
    : m_repository(repository), m_gateway(gateway) {}

void PaymentEngine::process(TransactionId id) {

  std::shared_ptr<std::mutex> transactionMutex;
  {
    std::lock_guard lock(m_transactionLocksMutex);
    const auto [it, inserted] =
        m_transactionLocks.emplace(id, std::make_shared<std::mutex>());
    transactionMutex = it->second;
  }

  std::lock_guard lock(*transactionMutex);
  auto transaction = m_repository.findById(id);
  if (!transaction) {
    throw TransactionNotFound(
        std::format("Transaction with ID {} not found", id));
  }

  auto payment = PaymentProcessFactory::create(transaction->getType());

  PaymentEventCallback callback =
      [this](const Transaction& processedTransaction, PaymentEvent event) {
        notifyObservers(processedTransaction, event);
      };
  payment->process(*transaction, m_gateway, callback);
  m_repository.update(*transaction);
}

void PaymentEngine::addObserver(const PaymentObserver& observer) {
  std::lock_guard lock(m_observersMutex);
  m_observers.push_back(&observer);
}

void PaymentEngine::notifyObservers(const Transaction& transaction,
                                    PaymentEvent event) const {
  std::vector<const PaymentObserver*> observers;
  {
    std::lock_guard lock(m_observersMutex);
    observers = m_observers;
  }

  for (const auto* observer : observers) {
    try {
      observer->onPaymentEvent(transaction, event);
    } catch (const std::exception& exception) {
      std::osyncstream(std::cerr)
          << "Payment observer failed: " << exception.what() << '\n';
    } catch (...) {
      std::osyncstream(std::cerr)
          << "Payment observer failed: unknown exception\n";
    }
  }
}

} // namespace payment
