#pragma once

#include <payment/interfaces/gateway/base/payment_gateway.hpp>

namespace payment {

class Transaction;
class HttpClient;

class LegacyBank : public PaymentGateway {
public:
  explicit LegacyBank(HttpClient&);
  ProcessResult::Authorization send(const Transaction&) override;

private:
  HttpClient& m_client;
};

} // namespace payment
