#pragma once

#include <memory>
#include <payment/types/payment_gateway_type.hpp>

namespace payment {

class HttpClient;
class PaymentGateway;

class PaymentGatewayFactory {
public:
  static std::unique_ptr<PaymentGateway> create(PaymentGatewayType type,
                                                HttpClient& httpClient);
};

} // namespace payment
