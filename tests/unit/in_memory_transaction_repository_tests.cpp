#include <gtest/gtest.h>

#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/interfaces/persistence/in_memory_transaction_repository.hpp>
#include <payment/types/transaction.hpp>

#include <optional>
#include <string>
#include <vector>

namespace {

using payment::InMemoryTransactionRepository;
using payment::PaymentType;
using payment::Transaction;
using payment::TransactionStatus;

Transaction
makeTransaction(payment::TransactionId id,
                PaymentType type = PaymentType::CREDIT,
                payment::Amount amount = 2400,
                const std::string& description = "repository test",
                TransactionStatus status = TransactionStatus::PENDING) {
  Transaction transaction{id, type, amount, description};
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

TEST(InMemoryTransactionRepositoryTest, AddAndFindByIdPreservesTransaction) {
  InMemoryTransactionRepository repository;
  const auto transaction =
      makeTransaction(101, PaymentType::DEBIT, 3750, "saved debit",
                      TransactionStatus::VALIDATED);

  repository.add(transaction);
  const auto found = repository.findById(101);

  ASSERT_TRUE(found.has_value());
  EXPECT_EQ(found->getId(), transaction.getId());
  EXPECT_EQ(found->getType(), transaction.getType());
  EXPECT_EQ(found->getAmount(), transaction.getAmount());
  EXPECT_EQ(found->getDescription(), transaction.getDescription());
  EXPECT_EQ(found->getStatus(), transaction.getStatus());
}

TEST(InMemoryTransactionRepositoryTest, FindByIdReturnsNulloptWhenMissing) {
  const InMemoryTransactionRepository repository;

  EXPECT_EQ(repository.findById(999), std::nullopt);
}

TEST(InMemoryTransactionRepositoryTest, AddRejectsDuplicateId) {
  InMemoryTransactionRepository repository;
  repository.add(makeTransaction(101));

  EXPECT_THROW(repository.add(makeTransaction(101, PaymentType::DEBIT)),
               payment::InvalidTransactionOperation);
}

TEST(InMemoryTransactionRepositoryTest, UpdatePersistsUpdatedTransaction) {
  InMemoryTransactionRepository repository;
  repository.add(makeTransaction(101));
  auto updated = *repository.findById(101);
  updated.setStatus(TransactionStatus::VALIDATED);

  repository.update(updated);

  const auto found = repository.findById(101);
  ASSERT_TRUE(found.has_value());
  EXPECT_EQ(found->getStatus(), TransactionStatus::VALIDATED);
}

TEST(InMemoryTransactionRepositoryTest, UpdateRejectsMissingTransaction) {
  InMemoryTransactionRepository repository;
  const auto transaction = makeTransaction(404);

  EXPECT_THROW(repository.update(transaction), payment::TransactionNotFound);
}

TEST(InMemoryTransactionRepositoryTest, FindByStatusReturnsOnlyMatches) {
  InMemoryTransactionRepository repository;
  repository.add(makeTransaction(101, PaymentType::CREDIT, 2400, "first",
                                 TransactionStatus::VALIDATED));
  repository.add(makeTransaction(102, PaymentType::DEBIT, 1800, "second",
                                 TransactionStatus::VALIDATED));
  repository.add(makeTransaction(103, PaymentType::CREDIT, 900, "pending"));
  repository.add(makeTransaction(104, PaymentType::DEBIT, 500, "denied",
                                 TransactionStatus::DENIED));

  const auto validated = repository.findByStatus(TransactionStatus::VALIDATED);
  ASSERT_EQ(validated.size(), 2);
  for (const auto& transaction : validated) {
    EXPECT_EQ(transaction.getStatus(), TransactionStatus::VALIDATED);
  }

  EXPECT_TRUE(repository.findByStatus(TransactionStatus::APPROVED).empty());
}

TEST(InMemoryTransactionRepositoryTest, RetrievedCopyRequiresUpdateToPersist) {
  InMemoryTransactionRepository repository;
  repository.add(makeTransaction(101));

  auto retrieved = *repository.findById(101);
  retrieved.setStatus(TransactionStatus::VALIDATED);

  const auto unchanged = repository.findById(101);
  ASSERT_TRUE(unchanged.has_value());
  EXPECT_EQ(unchanged->getStatus(), TransactionStatus::PENDING);

  repository.update(retrieved);

  const auto updated = repository.findById(101);
  ASSERT_TRUE(updated.has_value());
  EXPECT_EQ(updated->getStatus(), TransactionStatus::VALIDATED);
}

} // namespace
