#include <memory>
#include <payment/credit_payment.h>
#include <payment/debit_payment.h>
#include <payment/payment_factory.h>
#include <payment/pix_payment.h>
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
