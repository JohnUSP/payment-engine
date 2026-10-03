#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/interfaces/gateway/base/payment_gateway.hpp>
#include <payment/interfaces/process/base/payment_process.hpp>
#include <payment/interfaces/process/credit_payment.hpp>
#include <payment/interfaces/process/debit_payment.hpp>
#include <payment/types/payment_event.hpp>
#include <payment/types/payment_event_callback.hpp>
#include <payment/types/process_result.hpp>
#include <payment/types/transaction.hpp>

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace {

using payment::PaymentEvent;
using payment::PaymentType;
using payment::ProcessResult;
using payment::Transaction;
using payment::TransactionStatus;

class MockPaymentGateway : public payment::PaymentGateway {
public:
  MOCK_METHOD(ProcessResult::Authorization, send, (const Transaction&),
              (override));
};

std::unique_ptr<payment::PaymentProcess> makeProcess(PaymentType type) {
  if (type == PaymentType::CREDIT) {
    return std::make_unique<payment::CreditPayment>();
  }
  return std::make_unique<payment::DebitPayment>();
}

Transaction makeTransaction(PaymentType type, payment::Amount amount = 2500) {
  return Transaction{1001, type, amount, "process test"};
}

Transaction makeTransactionWithStatus(PaymentType type,
                                      TransactionStatus status) {
  auto transaction = makeTransaction(type);

  switch (status) {
  case TransactionStatus::PENDING:
    break;
  case TransactionStatus::VALIDATED:
    transaction.setStatus(TransactionStatus::VALIDATED);
    break;
  case TransactionStatus::APPROVED:
    transaction.setStatus(TransactionStatus::VALIDATED);
    transaction.setStatus(TransactionStatus::APPROVED);
    break;
  case TransactionStatus::DENIED:
    transaction.setStatus(TransactionStatus::VALIDATED);
    transaction.setStatus(TransactionStatus::DENIED);
    break;
  case TransactionStatus::CANCELED:
    transaction.setStatus(TransactionStatus::CANCELED);
    break;
  case TransactionStatus::COMPLETED:
    transaction.setStatus(TransactionStatus::VALIDATED);
    transaction.setStatus(TransactionStatus::APPROVED);
    transaction.setStatus(TransactionStatus::COMPLETED);
    break;
  }

  return transaction;
}

std::string paymentTypeName(const testing::TestParamInfo<PaymentType>& info) {
  return info.param == PaymentType::CREDIT ? "Credit" : "Debit";
}

class PaymentProcessTest : public testing::TestWithParam<PaymentType> {};

TEST_P(PaymentProcessTest, AuthorizedFlowCompletesAndEmitsEventsInOrder) {
  const PaymentType type = GetParam();
  auto process = makeProcess(type);
  auto transaction = makeTransaction(type);
  MockPaymentGateway gateway;
  std::vector<PaymentEvent> events;
  const payment::PaymentEventCallback callback = [&events](const Transaction&,
                                                           PaymentEvent event) {
    events.push_back(event);
  };

  EXPECT_CALL(gateway, send(testing::_))
      .Times(1)
      .WillOnce(testing::Return(ProcessResult::Authorization::AUTHORIZED));

  process->process(transaction, gateway, callback);

  EXPECT_EQ(transaction.getStatus(), TransactionStatus::COMPLETED);
  EXPECT_EQ(events,
            (std::vector<PaymentEvent>{PaymentEvent::TRANSACTION_PENDING,
                                       PaymentEvent::TRANSACTION_VALIDATED,
                                       PaymentEvent::TRANSACTION_APPROVED,
                                       PaymentEvent::TRANSACTION_COMPLETED}));
}

TEST_P(PaymentProcessTest, DeclinedFlowEndsDeniedWithoutCompletion) {
  const PaymentType type = GetParam();
  auto process = makeProcess(type);
  auto transaction = makeTransaction(type);
  MockPaymentGateway gateway;
  std::vector<PaymentEvent> events;
  const payment::PaymentEventCallback callback = [&events](const Transaction&,
                                                           PaymentEvent event) {
    events.push_back(event);
  };

  EXPECT_CALL(gateway, send(testing::_))
      .Times(1)
      .WillOnce(testing::Return(ProcessResult::Authorization::DECLINED));

  process->process(transaction, gateway, callback);

  EXPECT_EQ(transaction.getStatus(), TransactionStatus::DENIED);
  EXPECT_EQ(events,
            (std::vector<PaymentEvent>{PaymentEvent::TRANSACTION_PENDING,
                                       PaymentEvent::TRANSACTION_VALIDATED,
                                       PaymentEvent::TRANSACTION_DENIED}));
}

TEST_P(PaymentProcessTest, GatewayErrorLeavesTransactionValidated) {
  const PaymentType type = GetParam();
  auto process = makeProcess(type);
  auto transaction = makeTransaction(type);
  MockPaymentGateway gateway;
  std::vector<PaymentEvent> events;
  const payment::PaymentEventCallback callback = [&events](const Transaction&,
                                                           PaymentEvent event) {
    events.push_back(event);
  };

  EXPECT_CALL(gateway, send(testing::_))
      .Times(1)
      .WillOnce(testing::Return(ProcessResult::Authorization::ERROR));

  process->process(transaction, gateway, callback);

  EXPECT_EQ(transaction.getStatus(), TransactionStatus::VALIDATED);
  EXPECT_EQ(events,
            (std::vector<PaymentEvent>{PaymentEvent::TRANSACTION_PENDING,
                                       PaymentEvent::TRANSACTION_VALIDATED}));
}

TEST_P(PaymentProcessTest, AmountAtLimitReachesGateway) {
  const PaymentType type = GetParam();
  auto process = makeProcess(type);
  auto transaction = makeTransaction(type, 100'000 * 100);
  MockPaymentGateway gateway;
  std::vector<PaymentEvent> events;
  const payment::PaymentEventCallback callback = [&events](const Transaction&,
                                                           PaymentEvent event) {
    events.push_back(event);
  };

  EXPECT_CALL(gateway, send(testing::_))
      .Times(1)
      .WillOnce(testing::Return(ProcessResult::Authorization::AUTHORIZED));

  process->process(transaction, gateway, callback);

  EXPECT_EQ(transaction.getStatus(), TransactionStatus::COMPLETED);
}

TEST_P(PaymentProcessTest, AmountOverLimitStopsBeforeGateway) {
  const PaymentType type = GetParam();
  auto process = makeProcess(type);
  auto transaction = makeTransaction(type, 100'000 * 100 + 1);
  MockPaymentGateway gateway;
  std::vector<PaymentEvent> events;
  const payment::PaymentEventCallback callback = [&events](const Transaction&,
                                                           PaymentEvent event) {
    events.push_back(event);
  };

  EXPECT_CALL(gateway, send(testing::_)).Times(0);

  process->process(transaction, gateway, callback);

  EXPECT_EQ(transaction.getStatus(), TransactionStatus::PENDING);
  EXPECT_EQ(events,
            (std::vector<PaymentEvent>{PaymentEvent::TRANSACTION_PENDING}));
}

TEST_P(PaymentProcessTest, ValidatedTransactionSkipsPreparation) {
  const PaymentType type = GetParam();
  auto process = makeProcess(type);
  auto transaction =
      makeTransactionWithStatus(type, TransactionStatus::VALIDATED);
  MockPaymentGateway gateway;
  std::vector<PaymentEvent> events;
  const payment::PaymentEventCallback callback = [&events](const Transaction&,
                                                           PaymentEvent event) {
    events.push_back(event);
  };

  EXPECT_CALL(gateway, send(testing::_))
      .Times(1)
      .WillOnce(testing::Return(ProcessResult::Authorization::AUTHORIZED));

  process->process(transaction, gateway, callback);

  EXPECT_EQ(transaction.getStatus(), TransactionStatus::COMPLETED);
  EXPECT_EQ(events,
            (std::vector<PaymentEvent>{PaymentEvent::TRANSACTION_APPROVED,
                                       PaymentEvent::TRANSACTION_COMPLETED}));
}

INSTANTIATE_TEST_SUITE_P(BothStrategies, PaymentProcessTest,
                         testing::Values(PaymentType::CREDIT,
                                         PaymentType::DEBIT),
                         paymentTypeName);

using InvalidInitialState = std::tuple<PaymentType, TransactionStatus>;

std::string invalidInitialStateName(
    const testing::TestParamInfo<InvalidInitialState>& info) {
  return std::to_string(info.index);
}

class InvalidInitialStateTest
    : public testing::TestWithParam<InvalidInitialState> {};

TEST_P(InvalidInitialStateTest, ThrowsWithoutCallingGateway) {
  const auto [type, status] = GetParam();
  auto process = makeProcess(type);
  auto transaction = makeTransactionWithStatus(type, status);
  MockPaymentGateway gateway;
  std::vector<PaymentEvent> events;
  const payment::PaymentEventCallback callback = [&events](const Transaction&,
                                                           PaymentEvent event) {
    events.push_back(event);
  };

  EXPECT_CALL(gateway, send(testing::_)).Times(0);

  EXPECT_THROW(process->process(transaction, gateway, callback),
               payment::InvalidTransactionState);
  EXPECT_TRUE(events.empty());
}

INSTANTIATE_TEST_SUITE_P(
    UnsupportedInitialStates, InvalidInitialStateTest,
    testing::Combine(testing::Values(PaymentType::CREDIT, PaymentType::DEBIT),
                     testing::Values(TransactionStatus::APPROVED,
                                     TransactionStatus::DENIED,
                                     TransactionStatus::CANCELED,
                                     TransactionStatus::COMPLETED)),
    invalidInitialStateName);

} // namespace
