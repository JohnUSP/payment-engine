#include <payment/factory/payment_gateway_factory.hpp>
#include <payment/interfaces/gateway/legacy_bank.hpp>
#include <payment/interfaces/gateway/secondary_bank.hpp>
#include <stdexcept>

namespace payment {

std::unique_ptr<PaymentGateway>
PaymentGatewayFactory::create(PaymentGatewayType type, HttpClient& httpClient) {
  switch (type) {
  case PaymentGatewayType::LEGACY_BANK:
    return std::make_unique<LegacyBank>(httpClient);
  case PaymentGatewayType::SECONDARY_BANK:
    return std::make_unique<SecondaryBank>();
  }

  throw std::invalid_argument("Unsupported payment gateway type");
}

} // namespace payment
