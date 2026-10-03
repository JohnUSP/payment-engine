#include <gtest/gtest.h>

#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/types/transaction.hpp>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using payment::PaymentType;
using payment::Transaction;
using payment::TransactionStatus;
using StateTransition = std::pair<TransactionStatus, TransactionStatus>;

Transaction transactionWithStatus(TransactionStatus status) {
  Transaction transaction{1001, PaymentType::DEBIT, 2500, "unit test"};

  switch (status) {
  case TransactionStatus::PENDING:
    return transaction;
  case TransactionStatus::VALIDATED:
    transaction.setStatus(TransactionStatus::VALIDATED);
    return transaction;
  case TransactionStatus::APPROVED:
    transaction.setStatus(TransactionStatus::VALIDATED);
    transaction.setStatus(TransactionStatus::APPROVED);
    return transaction;
  case TransactionStatus::DENIED:
    transaction.setStatus(TransactionStatus::VALIDATED);
    transaction.setStatus(TransactionStatus::DENIED);
    return transaction;
  case TransactionStatus::CANCELED:
    transaction.setStatus(TransactionStatus::CANCELED);
    return transaction;
  case TransactionStatus::COMPLETED:
    transaction.setStatus(TransactionStatus::VALIDATED);
    transaction.setStatus(TransactionStatus::APPROVED);
    transaction.setStatus(TransactionStatus::COMPLETED);
    return transaction;
  }

  throw std::logic_error("Unknown transaction status");
}

std::string
transitionName(const testing::TestParamInfo<StateTransition>& info) {
  return std::to_string(info.index);
}

class ValidTransitionTest : public testing::TestWithParam<StateTransition> {};

TEST_P(ValidTransitionTest, ChangesToTargetStatus) {
  const auto [from, to] = GetParam();
  auto transaction = transactionWithStatus(from);

  EXPECT_NO_THROW(transaction.setStatus(to));
  EXPECT_EQ(transaction.getStatus(), to);
}

INSTANTIATE_TEST_SUITE_P(
    ValidTransitions, ValidTransitionTest,
    testing::Values(StateTransition{TransactionStatus::PENDING,
                                    TransactionStatus::VALIDATED},
                    StateTransition{TransactionStatus::PENDING,
                                    TransactionStatus::CANCELED},
                    StateTransition{TransactionStatus::VALIDATED,
                                    TransactionStatus::APPROVED},
                    StateTransition{TransactionStatus::VALIDATED,
                                    TransactionStatus::DENIED},
                    StateTransition{TransactionStatus::VALIDATED,
                                    TransactionStatus::CANCELED},
                    StateTransition{TransactionStatus::APPROVED,
                                    TransactionStatus::COMPLETED},
                    StateTransition{TransactionStatus::APPROVED,
                                    TransactionStatus::CANCELED}),
    transitionName);

class InvalidTransitionTest : public testing::TestWithParam<StateTransition> {};

TEST_P(InvalidTransitionTest, ThrowsInvalidTransactionOperation) {
  const auto [from, to] = GetParam();
  auto transaction = transactionWithStatus(from);

  EXPECT_THROW(transaction.setStatus(to), payment::InvalidTransactionOperation);
}

INSTANTIATE_TEST_SUITE_P(
    InvalidTransitions, InvalidTransitionTest,
    testing::Values(
        StateTransition{TransactionStatus::PENDING, TransactionStatus::PENDING},
        StateTransition{TransactionStatus::PENDING,
                        TransactionStatus::APPROVED},
        StateTransition{TransactionStatus::PENDING, TransactionStatus::DENIED},
        StateTransition{TransactionStatus::PENDING,
                        TransactionStatus::COMPLETED},
        StateTransition{TransactionStatus::VALIDATED,
                        TransactionStatus::PENDING},
        StateTransition{TransactionStatus::VALIDATED,
                        TransactionStatus::VALIDATED},
        StateTransition{TransactionStatus::VALIDATED,
                        TransactionStatus::COMPLETED},
        StateTransition{TransactionStatus::APPROVED,
                        TransactionStatus::PENDING},
        StateTransition{TransactionStatus::APPROVED,
                        TransactionStatus::VALIDATED},
        StateTransition{TransactionStatus::APPROVED,
                        TransactionStatus::APPROVED},
        StateTransition{TransactionStatus::APPROVED,
                        TransactionStatus::DENIED}),
    transitionName);

std::vector<StateTransition> terminalTransitions() {
  const std::vector<TransactionStatus> terminalStates{
      TransactionStatus::DENIED, TransactionStatus::CANCELED,
      TransactionStatus::COMPLETED};
  const std::vector<TransactionStatus> allStates{
      TransactionStatus::PENDING,  TransactionStatus::VALIDATED,
      TransactionStatus::APPROVED, TransactionStatus::DENIED,
      TransactionStatus::CANCELED, TransactionStatus::COMPLETED};

  std::vector<StateTransition> transitions;
  for (const auto from : terminalStates) {
    for (const auto to : allStates) {
      transitions.emplace_back(from, to);
    }
  }
  return transitions;
}

class TerminalStateTest : public testing::TestWithParam<StateTransition> {};

TEST_P(TerminalStateTest, RejectsFurtherStateChanges) {
  const auto [from, to] = GetParam();
  auto transaction = transactionWithStatus(from);

  EXPECT_THROW(transaction.setStatus(to), payment::InvalidTransactionOperation);
}

INSTANTIATE_TEST_SUITE_P(TerminalStates, TerminalStateTest,
                         testing::ValuesIn(terminalTransitions()),
                         transitionName);

TEST(TransactionConstructionTest, StoresProvidedValuesAndStartsPending) {
  const Transaction transaction{42, PaymentType::CREDIT, 1299, "purchase"};

  EXPECT_EQ(transaction.getId(), 42);
  EXPECT_EQ(transaction.getType(), PaymentType::CREDIT);
  EXPECT_EQ(transaction.getAmount(), 1299);
  EXPECT_EQ(transaction.getDescription(), "purchase");
  EXPECT_EQ(transaction.getStatus(), TransactionStatus::PENDING);
}

TEST(TransactionConstructionTest, RejectsZeroAmount) {
  EXPECT_THROW((Transaction{42, PaymentType::DEBIT, 0, "invalid"}),
               payment::InvalidTransactionAmount);
}

TEST(TransactionConstructionTest, RejectsNegativeAmount) {
  EXPECT_THROW((Transaction{42, PaymentType::DEBIT, -1, "invalid"}),
               payment::InvalidTransactionAmount);
}

TEST(TransactionPaymentDataTest, DebitTransactionAllowsDebitData) {
  Transaction transaction{42, PaymentType::DEBIT, 1299, "debit"};

  EXPECT_NO_THROW((void)transaction.getDebitData());
}

TEST(TransactionPaymentDataTest, CreditTransactionAllowsCreditData) {
  Transaction transaction{42, PaymentType::CREDIT, 1299, "credit"};

  EXPECT_NO_THROW((void)transaction.getCreditData());
}

TEST(TransactionPaymentDataTest, DebitTransactionRejectsCreditData) {
  Transaction transaction{42, PaymentType::DEBIT, 1299, "debit"};

  EXPECT_THROW((void)transaction.getCreditData(), payment::InvalidPaymentType);
}

TEST(TransactionPaymentDataTest, CreditTransactionRejectsDebitData) {
  Transaction transaction{42, PaymentType::CREDIT, 1299, "credit"};

  EXPECT_THROW((void)transaction.getDebitData(), payment::InvalidPaymentType);
}

} // namespace
