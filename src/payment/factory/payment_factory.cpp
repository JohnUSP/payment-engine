#include <memory>
#include <payment/factory/payment_factory.hpp>
#include <payment/interfaces/process/credit_payment.hpp>
#include <payment/interfaces/process/debit_payment.hpp>
#include <payment/types/payment_type.hpp>
#include <stdexcept>

namespace payment {

std::unique_ptr<PaymentProcess>
PaymentProcessFactory::create(PaymentType type) {

  switch (type) {
  case PaymentType::CREDIT:
    return std::make_unique<CreditPayment>();
  case PaymentType::DEBIT:
    return std::make_unique<DebitPayment>();
  }

  throw std::invalid_argument("Unsupported payment type");
}
} // namespace payment
