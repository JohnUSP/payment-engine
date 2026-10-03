#include <gtest/gtest.h>

#include <payment/factory/payment_factory.hpp>
#include <payment/interfaces/process/credit_payment.hpp>
#include <payment/interfaces/process/debit_payment.hpp>
#include <payment/types/payment_type.hpp>

#include <stdexcept>

TEST(PaymentProcessFactoryTest, CreatesCreditPayment) {
  auto process =
      payment::PaymentProcessFactory::create(payment::PaymentType::CREDIT);

  ASSERT_NE(process, nullptr);
  EXPECT_NE(dynamic_cast<payment::CreditPayment*>(process.get()), nullptr);
}

TEST(PaymentProcessFactoryTest, CreatesDebitPayment) {
  auto process =
      payment::PaymentProcessFactory::create(payment::PaymentType::DEBIT);

  ASSERT_NE(process, nullptr);
  EXPECT_NE(dynamic_cast<payment::DebitPayment*>(process.get()), nullptr);
}

TEST(PaymentProcessFactoryTest, RejectsInvalidPaymentType) {
  const auto invalidType = static_cast<payment::PaymentType>(999);

  EXPECT_THROW(payment::PaymentProcessFactory::create(invalidType),
               std::invalid_argument);
}
