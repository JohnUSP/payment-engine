#include <gtest/gtest.h>

#include <payment/engine/payment_engine.hpp>
#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/interfaces/gateway/base/payment_gateway.hpp>
#include <payment/interfaces/persistence/in_memory_transaction_repository.hpp>
#include <payment/types/transaction.hpp>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <latch>
#include <mutex>
#include <thread>
#include <vector>

namespace {

using payment::InMemoryTransactionRepository;
using payment::PaymentGateway;
using payment::PaymentType;
using payment::ProcessResult;
using payment::Transaction;
using payment::TransactionId;
using payment::TransactionStatus;

constexpr auto kWaitTimeout = std::chrono::seconds(3);
constexpr auto kNoSecondEntryWindow = std::chrono::milliseconds(250);

Transaction makeTransaction(TransactionId id) {
  return Transaction{id, PaymentType::CREDIT, 2500, "engine concurrency test"};
}

class BlockingPaymentGateway : public PaymentGateway {
public:
  ProcessResult::Authorization send(const Transaction& transaction) override {
    std::unique_lock lock(m_mutex);
    ++m_activeCalls;
    m_maximumConcurrentCalls =
        std::max(m_maximumConcurrentCalls, m_activeCalls);
    m_enteredTransactionIds.push_back(transaction.getId());
    m_condition.notify_all();

    m_condition.wait(lock, [this] { return m_released; });
    --m_activeCalls;
    m_condition.notify_all();
    return ProcessResult::Authorization::AUTHORIZED;
  }

  bool waitForEnteredCalls(std::size_t count,
                           std::chrono::milliseconds timeout) {
    std::unique_lock lock(m_mutex);
    return m_condition.wait_for(lock, timeout, [this, count] {
      return m_enteredTransactionIds.size() >= count;
    });
  }

  void release() {
    {
      std::lock_guard lock(m_mutex);
      m_released = true;
    }
    m_condition.notify_all();
  }

  std::size_t maximumConcurrentCalls() const {
    std::lock_guard lock(m_mutex);
    return m_maximumConcurrentCalls;
  }

  std::vector<TransactionId> enteredTransactionIds() const {
    std::lock_guard lock(m_mutex);
    return m_enteredTransactionIds;
  }

private:
  mutable std::mutex m_mutex;
  std::condition_variable m_condition;
  std::size_t m_activeCalls{0};
  std::size_t m_maximumConcurrentCalls{0};
  std::vector<TransactionId> m_enteredTransactionIds;
  bool m_released{false};
};

void joinAll(std::vector<std::thread>& threads) {
  for (auto& thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

void recordException(std::mutex& mutex,
                     std::vector<std::exception_ptr>& exceptions) {
  std::lock_guard lock(mutex);
  exceptions.push_back(std::current_exception());
}

TEST(PaymentEngineConcurrencyTest, SameTransactionIdIsSerialized) {
  InMemoryTransactionRepository repository;
  constexpr TransactionId transactionId = 9001;
  repository.add(makeTransaction(transactionId));
  BlockingPaymentGateway gateway;
  payment::PaymentEngine engine(repository, gateway);

  std::mutex exceptionMutex;
  std::vector<std::exception_ptr> exceptions;
  std::atomic<bool> secondStarted{false};
  std::vector<std::thread> threads;

  threads.emplace_back([&] {
    try {
      engine.process(transactionId);
    } catch (...) {
      recordException(exceptionMutex, exceptions);
    }
  });

  const bool firstEntered = gateway.waitForEnteredCalls(1, kWaitTimeout);
  bool secondDidNotEnterDuringFirstCall = false;
  if (firstEntered) {
    threads.emplace_back([&] {
      secondStarted.store(true, std::memory_order_release);
      try {
        engine.process(transactionId);
      } catch (const payment::InvalidTransactionState&) {
      } catch (...) {
        recordException(exceptionMutex, exceptions);
      }
    });

    while (!secondStarted.load(std::memory_order_acquire)) {
      std::this_thread::yield();
    }
    secondDidNotEnterDuringFirstCall =
        !gateway.waitForEnteredCalls(2, kNoSecondEntryWindow);
  }

  gateway.release();
  joinAll(threads);

  EXPECT_TRUE(firstEntered);
  EXPECT_TRUE(secondDidNotEnterDuringFirstCall);
  EXPECT_EQ(gateway.maximumConcurrentCalls(), 1);
  EXPECT_TRUE(exceptions.empty());
  const auto enteredIds = gateway.enteredTransactionIds();
  EXPECT_EQ(std::count(enteredIds.begin(), enteredIds.end(), transactionId), 1);

  const auto persisted = repository.findById(transactionId);
  ASSERT_TRUE(persisted.has_value());
  EXPECT_EQ(persisted->getStatus(), TransactionStatus::COMPLETED);
}

TEST(PaymentEngineConcurrencyTest, DifferentTransactionIdsRunInParallel) {
  InMemoryTransactionRepository repository;
  constexpr TransactionId firstId = 9101;
  constexpr TransactionId secondId = 9102;
  repository.add(makeTransaction(firstId));
  repository.add(makeTransaction(secondId));
  BlockingPaymentGateway gateway;
  payment::PaymentEngine engine(repository, gateway);

  std::mutex exceptionMutex;
  std::vector<std::exception_ptr> exceptions;
  std::latch ready{2};
  std::latch start{1};
  std::vector<std::thread> threads;
  threads.reserve(2);

  for (const TransactionId id : {firstId, secondId}) {
    threads.emplace_back([&, id] {
      ready.count_down();
      start.wait();
      try {
        engine.process(id);
      } catch (...) {
        recordException(exceptionMutex, exceptions);
      }
    });
  }

  ready.wait();
  start.count_down();
  const bool bothEntered = gateway.waitForEnteredCalls(2, kWaitTimeout);
  gateway.release();
  joinAll(threads);

  EXPECT_TRUE(bothEntered);
  EXPECT_GE(gateway.maximumConcurrentCalls(), 2);
  EXPECT_TRUE(exceptions.empty());
  const auto enteredIds = gateway.enteredTransactionIds();
  EXPECT_NE(std::find(enteredIds.begin(), enteredIds.end(), firstId),
            enteredIds.end());
  EXPECT_NE(std::find(enteredIds.begin(), enteredIds.end(), secondId),
            enteredIds.end());

  const auto first = repository.findById(firstId);
  ASSERT_TRUE(first.has_value());
  EXPECT_EQ(first->getStatus(), TransactionStatus::COMPLETED);

  const auto second = repository.findById(secondId);
  ASSERT_TRUE(second.has_value());
  EXPECT_EQ(second->getStatus(), TransactionStatus::COMPLETED);
}

} // namespace
