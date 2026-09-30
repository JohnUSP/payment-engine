#include <memory>
#include <payment/factories/payment_factory.hpp>
#include <payment/strategies/credit_payment.hpp>
#include <payment/strategies/debit_payment.hpp>
#include <payment/strategies/pix_payment.hpp>
#include <payment/types/payment_type.hpp>
#include <stdexcept>

namespace payment {

std::unique_ptr<PaymentStrategy> PaymentFactory::create(PaymentType type) {

  switch (type) {
  case PaymentType::PIX:
    return std::make_unique<PixPayment>();
  case PaymentType::CREDIT:
    return std::make_unique<CreditPayment>();
  case PaymentType::DEBIT:
    return std::make_unique<DebitPayment>();
  }

  throw std::invalid_argument("Unsupported payment type");
}
} // namespace payment
