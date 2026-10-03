#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <payment/engine/payment_engine.hpp>
#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/interfaces/gateway/base/payment_gateway.hpp>
#include <payment/interfaces/observer/base/payment_observer.hpp>
#include <payment/interfaces/persistence/base/transaction_repository.hpp>
#include <payment/types/payment_event.hpp>
#include <payment/types/transaction.hpp>

#include <optional>
#include <vector>

namespace {

using payment::PaymentEvent;
using payment::PaymentGateway;
using payment::PaymentObserver;
using payment::ProcessResult;
using payment::Transaction;
using payment::TransactionId;
using payment::TransactionRepository;
using payment::TransactionStatus;

class MockTransactionRepository : public TransactionRepository {
public:
  MOCK_METHOD(void, add, (Transaction transaction), (override));
  MOCK_METHOD(void, update, (const Transaction& transaction), (override));
  MOCK_METHOD(std::optional<Transaction>, findById, (TransactionId id),
              (const, override));
  MOCK_METHOD(std::vector<Transaction>, findByStatus,
              (TransactionStatus status), (const, override));
};

class MockPaymentGateway : public PaymentGateway {
public:
  MOCK_METHOD(ProcessResult::Authorization, send,
              (const Transaction& transaction), (override));
};

class MockPaymentObserver : public PaymentObserver {
public:
  MOCK_METHOD(void, onPaymentEvent,
              (const Transaction& transaction, PaymentEvent event),
              (const, override));
};

Transaction makeCreditTransaction(TransactionId id = 501) {
  return Transaction{id, payment::PaymentType::CREDIT, 4200, "engine test"};
}

const std::vector<PaymentEvent> kAuthorizedEvents{
    PaymentEvent::TRANSACTION_PENDING, PaymentEvent::TRANSACTION_VALIDATED,
    PaymentEvent::TRANSACTION_APPROVED, PaymentEvent::TRANSACTION_COMPLETED};

TEST(PaymentEngineTest, MissingTransactionThrowsWithoutGatewayOrUpdate) {
  MockTransactionRepository repository;
  MockPaymentGateway gateway;
  payment::PaymentEngine engine(repository, gateway);

  EXPECT_CALL(repository, findById(808))
      .WillOnce(testing::Return(std::nullopt));
  EXPECT_CALL(gateway, send(testing::_)).Times(0);
  EXPECT_CALL(repository, update(testing::_)).Times(0);

  EXPECT_THROW(engine.process(808), payment::TransactionNotFound);
}

TEST(PaymentEngineTest, AuthorizedPaymentPersistsCompletedTransaction) {
  MockTransactionRepository repository;
  MockPaymentGateway gateway;
  const auto transaction = makeCreditTransaction();
  TransactionStatus persistedStatus = TransactionStatus::PENDING;
  payment::PaymentEngine engine(repository, gateway);

  EXPECT_CALL(repository, findById(transaction.getId()))
      .WillOnce(testing::Return(std::optional<Transaction>{transaction}));
  EXPECT_CALL(gateway, send(testing::_))
      .Times(1)
      .WillOnce(testing::Return(ProcessResult::Authorization::AUTHORIZED));
  EXPECT_CALL(repository, update(testing::_))
      .Times(1)
      .WillOnce(testing::Invoke([&persistedStatus](const Transaction& updated) {
        persistedStatus = updated.getStatus();
      }));

  engine.process(transaction.getId());

  EXPECT_EQ(persistedStatus, TransactionStatus::COMPLETED);
}

TEST(PaymentEngineTest, DeclinedPaymentPersistsDeniedTransaction) {
  MockTransactionRepository repository;
  MockPaymentGateway gateway;
  const auto transaction = makeCreditTransaction();
  TransactionStatus persistedStatus = TransactionStatus::PENDING;
  payment::PaymentEngine engine(repository, gateway);

  EXPECT_CALL(repository, findById(transaction.getId()))
      .WillOnce(testing::Return(std::optional<Transaction>{transaction}));
  EXPECT_CALL(gateway, send(testing::_))
      .Times(1)
      .WillOnce(testing::Return(ProcessResult::Authorization::DECLINED));
  EXPECT_CALL(repository, update(testing::_))
      .Times(1)
      .WillOnce(testing::Invoke([&persistedStatus](const Transaction& updated) {
        persistedStatus = updated.getStatus();
      }));

  engine.process(transaction.getId());

  EXPECT_EQ(persistedStatus, TransactionStatus::DENIED);
}

TEST(PaymentEngineTest, GatewayErrorPersistsValidatedTransaction) {
  MockTransactionRepository repository;
  MockPaymentGateway gateway;
  const auto transaction = makeCreditTransaction();
  TransactionStatus persistedStatus = TransactionStatus::PENDING;
  payment::PaymentEngine engine(repository, gateway);

  EXPECT_CALL(repository, findById(transaction.getId()))
      .WillOnce(testing::Return(std::optional<Transaction>{transaction}));
  EXPECT_CALL(gateway, send(testing::_))
      .Times(1)
      .WillOnce(testing::Return(ProcessResult::Authorization::ERROR));
  EXPECT_CALL(repository, update(testing::_))
      .Times(1)
      .WillOnce(testing::Invoke([&persistedStatus](const Transaction& updated) {
        persistedStatus = updated.getStatus();
      }));

  engine.process(transaction.getId());

  EXPECT_EQ(persistedStatus, TransactionStatus::VALIDATED);
}

TEST(PaymentEngineTest, AuthorizedFlowNotifiesObserverInOrder) {
  MockTransactionRepository repository;
  MockPaymentGateway gateway;
  MockPaymentObserver observer;
  std::vector<PaymentEvent> events;
  const auto transaction = makeCreditTransaction();
  payment::PaymentEngine engine(repository, gateway);
  engine.addObserver(observer);

  EXPECT_CALL(repository, findById(transaction.getId()))
      .WillOnce(testing::Return(std::optional<Transaction>{transaction}));
  EXPECT_CALL(gateway, send(testing::_))
      .WillOnce(testing::Return(ProcessResult::Authorization::AUTHORIZED));
  EXPECT_CALL(repository, update(testing::_)).Times(1);
  EXPECT_CALL(observer, onPaymentEvent(testing::_, testing::_))
      .Times(static_cast<int>(kAuthorizedEvents.size()))
      .WillRepeatedly(
          testing::Invoke([&events](const Transaction&, PaymentEvent event) {
            events.push_back(event);
          }));

  engine.process(transaction.getId());

  EXPECT_EQ(events, kAuthorizedEvents);
}

TEST(PaymentEngineTest, MultipleObserversEachReceiveAuthorizedEventsInOrder) {
  MockTransactionRepository repository;
  MockPaymentGateway gateway;
  MockPaymentObserver firstObserver;
  MockPaymentObserver secondObserver;
  std::vector<PaymentEvent> firstEvents;
  std::vector<PaymentEvent> secondEvents;
  const auto transaction = makeCreditTransaction();
  payment::PaymentEngine engine(repository, gateway);
  engine.addObserver(firstObserver);
  engine.addObserver(secondObserver);

  EXPECT_CALL(repository, findById(transaction.getId()))
      .WillOnce(testing::Return(std::optional<Transaction>{transaction}));
  EXPECT_CALL(gateway, send(testing::_))
      .WillOnce(testing::Return(ProcessResult::Authorization::AUTHORIZED));
  EXPECT_CALL(repository, update(testing::_)).Times(1);
  EXPECT_CALL(firstObserver, onPaymentEvent(testing::_, testing::_))
      .Times(static_cast<int>(kAuthorizedEvents.size()))
      .WillRepeatedly(testing::Invoke(
          [&firstEvents](const Transaction&, PaymentEvent event) {
            firstEvents.push_back(event);
          }));
  EXPECT_CALL(secondObserver, onPaymentEvent(testing::_, testing::_))
      .Times(static_cast<int>(kAuthorizedEvents.size()))
      .WillRepeatedly(testing::Invoke(
          [&secondEvents](const Transaction&, PaymentEvent event) {
            secondEvents.push_back(event);
          }));

  engine.process(transaction.getId());

  EXPECT_EQ(firstEvents, kAuthorizedEvents);
  EXPECT_EQ(secondEvents, kAuthorizedEvents);
}

} // namespace
