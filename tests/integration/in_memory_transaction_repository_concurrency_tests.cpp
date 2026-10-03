#include <gtest/gtest.h>

#include <payment/interfaces/persistence/in_memory_transaction_repository.hpp>
#include <payment/types/transaction.hpp>

#include <atomic>
#include <cstddef>
#include <string>
#include <thread>
#include <vector>

namespace {

using payment::InMemoryTransactionRepository;
using payment::PaymentType;
using payment::Transaction;
using payment::TransactionId;
using payment::TransactionStatus;

constexpr std::size_t kThreadCount = 4;
constexpr std::size_t kTransactionsPerWriter = 20;
constexpr payment::Amount kAmount = 1234;
constexpr const char* kDescription = "concurrency test";

Transaction makeTransaction(TransactionId id) {
  return Transaction{id, PaymentType::CREDIT, kAmount, kDescription};
}

bool isExpectedTransaction(const Transaction& transaction, TransactionId id) {
  return transaction.getId() == id &&
         transaction.getType() == PaymentType::CREDIT &&
         transaction.getAmount() == kAmount &&
         transaction.getDescription() == kDescription &&
         transaction.getStatus() == TransactionStatus::PENDING;
}

void joinAll(std::vector<std::thread>& threads) {
  for (auto& thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

TEST(InMemoryTransactionRepositoryConcurrencyTest, ConcurrentReaders) {
  InMemoryTransactionRepository repository;
  constexpr TransactionId firstId = 1000;
  constexpr std::size_t initialCount = 8;
  for (std::size_t index = 0; index < initialCount; ++index) {
    repository.add(makeTransaction(firstId + index));
  }

  std::atomic<bool> failed{false};
  std::vector<std::thread> threads;
  for (std::size_t threadIndex = 0; threadIndex < kThreadCount; ++threadIndex) {
    threads.emplace_back([&repository, &failed, threadIndex] {
      try {
        for (std::size_t iteration = 0; iteration < 100; ++iteration) {
          const TransactionId id =
              firstId + (threadIndex + iteration) % initialCount;
          const auto found = repository.findById(id);
          if (!found || !isExpectedTransaction(*found, id)) {
            failed.store(true, std::memory_order_relaxed);
          }

          const auto pending =
              repository.findByStatus(TransactionStatus::PENDING);
          if (pending.size() != initialCount) {
            failed.store(true, std::memory_order_relaxed);
          }
          for (const auto& transaction : pending) {
            if (transaction.getId() < firstId ||
                transaction.getId() >= firstId + initialCount ||
                !isExpectedTransaction(transaction, transaction.getId())) {
              failed.store(true, std::memory_order_relaxed);
            }
          }
        }
      } catch (...) {
        failed.store(true, std::memory_order_relaxed);
      }
    });
  }

  joinAll(threads);
  EXPECT_FALSE(failed.load());
}

TEST(InMemoryTransactionRepositoryConcurrencyTest,
     ConcurrentIndependentWriters) {
  InMemoryTransactionRepository repository;
  constexpr TransactionId firstId = 10'000;
  std::atomic<bool> failed{false};
  std::vector<std::thread> threads;
  for (std::size_t threadIndex = 0; threadIndex < kThreadCount; ++threadIndex) {
    threads.emplace_back([&repository, &failed, threadIndex] {
      try {
        for (std::size_t index = 0; index < kTransactionsPerWriter; ++index) {
          const TransactionId id =
              firstId + threadIndex * kTransactionsPerWriter + index;
          repository.add(makeTransaction(id));
        }
      } catch (...) {
        failed.store(true, std::memory_order_relaxed);
      }
    });
  }

  joinAll(threads);
  ASSERT_FALSE(failed.load());

  for (std::size_t threadIndex = 0; threadIndex < kThreadCount; ++threadIndex) {
    for (std::size_t index = 0; index < kTransactionsPerWriter; ++index) {
      const TransactionId id =
          firstId + threadIndex * kTransactionsPerWriter + index;
      const auto found = repository.findById(id);
      ASSERT_TRUE(found.has_value());
      EXPECT_TRUE(isExpectedTransaction(*found, id));
    }
  }
}

TEST(InMemoryTransactionRepositoryConcurrencyTest,
     ConcurrentReadersAndWriters) {
  InMemoryTransactionRepository repository;
  constexpr TransactionId firstId = 20'000;
  constexpr std::size_t readerCount = 4;
  constexpr std::size_t writerCount = 3;
  constexpr std::size_t transactionsPerWriter = 16;
  constexpr TransactionId totalTransactions =
      writerCount * transactionsPerWriter;

  std::atomic<bool> start{false};
  std::atomic<bool> failed{false};
  std::vector<std::thread> threads;
  threads.reserve(readerCount + writerCount);

  for (std::size_t writerIndex = 0; writerIndex < writerCount; ++writerIndex) {
    threads.emplace_back([&repository, &start, &failed, writerIndex] {
      while (!start.load(std::memory_order_acquire)) {
        std::this_thread::yield();
      }
      try {
        for (std::size_t index = 0; index < transactionsPerWriter; ++index) {
          const TransactionId id =
              firstId + writerIndex * transactionsPerWriter + index;
          repository.add(makeTransaction(id));
        }
      } catch (...) {
        failed.store(true, std::memory_order_relaxed);
      }
    });
  }

  for (std::size_t readerIndex = 0; readerIndex < readerCount; ++readerIndex) {
    threads.emplace_back([&repository, &start, &failed, readerIndex] {
      while (!start.load(std::memory_order_acquire)) {
        std::this_thread::yield();
      }
      try {
        for (std::size_t iteration = 0; iteration < 100; ++iteration) {
          const TransactionId id =
              firstId + (readerIndex + iteration) % totalTransactions;
          const auto found = repository.findById(id);
          if (found && !isExpectedTransaction(*found, id)) {
            failed.store(true, std::memory_order_relaxed);
          }

          const auto pending =
              repository.findByStatus(TransactionStatus::PENDING);
          for (const auto& transaction : pending) {
            const auto observedId = transaction.getId();
            if (observedId < firstId ||
                observedId >= firstId + totalTransactions ||
                !isExpectedTransaction(transaction, observedId)) {
              failed.store(true, std::memory_order_relaxed);
            }
          }
        }
      } catch (...) {
        failed.store(true, std::memory_order_relaxed);
      }
    });
  }

  start.store(true, std::memory_order_release);
  joinAll(threads);
  ASSERT_FALSE(failed.load());

  const auto pending = repository.findByStatus(TransactionStatus::PENDING);
  ASSERT_EQ(pending.size(), totalTransactions);
  for (std::size_t index = 0; index < totalTransactions; ++index) {
    const TransactionId id = firstId + index;
    const auto found = repository.findById(id);
    ASSERT_TRUE(found.has_value());
    EXPECT_TRUE(isExpectedTransaction(*found, id));
  }
}

} // namespace
