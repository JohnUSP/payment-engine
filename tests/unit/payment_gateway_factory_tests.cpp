#include <gtest/gtest.h>

#include <payment/factory/payment_gateway_factory.hpp>
#include <payment/interfaces/gateway/legacy_bank.hpp>
#include <payment/interfaces/gateway/secondary_bank.hpp>
#include <payment/network/http_client.hpp>

#include <stdexcept>
#include <string>

namespace {

class StubHttpClient : public payment::HttpClient {
public:
  payment::HttpResponse post(const std::string&, const std::string&,
                             const payment::HttpHeaders&) override {
    return {};
  }
};

} // namespace

TEST(PaymentGatewayFactoryTest, CreatesLegacyBank) {
  StubHttpClient httpClient;

  auto gateway = payment::PaymentGatewayFactory::create(
      payment::PaymentGatewayType::LEGACY_BANK, httpClient);

  ASSERT_NE(gateway, nullptr);
  EXPECT_NE(dynamic_cast<payment::LegacyBank*>(gateway.get()), nullptr);
}

TEST(PaymentGatewayFactoryTest, CreatesSecondaryBank) {
  StubHttpClient httpClient;

  auto gateway = payment::PaymentGatewayFactory::create(
      payment::PaymentGatewayType::SECONDARY_BANK, httpClient);

  ASSERT_NE(gateway, nullptr);
  EXPECT_NE(dynamic_cast<payment::SecondaryBank*>(gateway.get()), nullptr);
}

TEST(PaymentGatewayFactoryTest, RejectsInvalidGatewayType) {
  StubHttpClient httpClient;
  const auto invalidType = static_cast<payment::PaymentGatewayType>(999);

  EXPECT_THROW(payment::PaymentGatewayFactory::create(invalidType, httpClient),
               std::invalid_argument);
}
