#include <gtest/gtest.h>

#include <nlohmann/json.hpp>
#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/interfaces/gateway/legacy_bank_contract.hpp>

#include <optional>
#include <string>

namespace {

using payment::LegacyBankRequest;
using payment::LegacyBankResponse;
using payment::PaymentType;

TEST(LegacyBankRequestTest, CreditRequestSerializesContractFields) {
  const LegacyBankRequest request{701, 8250, PaymentType::CREDIT,
                                  "credit purchase"};

  const auto json = nlohmann::json::parse(request.toJson());

  ASSERT_EQ(json.size(), 4);
  ASSERT_TRUE(json.contains("transaction_id"));
  ASSERT_TRUE(json.contains("amount"));
  ASSERT_TRUE(json.contains("description"));
  ASSERT_TRUE(json.contains("payment_type"));
  EXPECT_EQ(json.at("transaction_id"), 701);
  EXPECT_EQ(json.at("amount"), 8250);
  EXPECT_EQ(json.at("description"), "credit purchase");
  EXPECT_EQ(json.at("payment_type"), "Credit");
}

TEST(LegacyBankRequestTest, DebitPaymentTypeSerializesAsDebit) {
  const LegacyBankRequest request{702, 3900, PaymentType::DEBIT, "debit sale"};

  const auto json = nlohmann::json::parse(request.toJson());

  EXPECT_EQ(json.at("payment_type"), "Debit");
}

TEST(LegacyBankResponseTest, ParsesEveryResponseField) {
  const std::string json = R"({
    "status": "approved",
    "authorization_code": "AUTH-701",
    "message": "Payment approved",
    "bank_transaction_id": "BANK-702",
    "timestamp": "2026-10-03T12:30:00Z"
  })";

  const auto response = LegacyBankResponse::fromJson(json);

  EXPECT_EQ(response.status, "approved");
  ASSERT_TRUE(response.authorizationCode.has_value());
  EXPECT_EQ(*response.authorizationCode, "AUTH-701");
  EXPECT_EQ(response.message, "Payment approved");
  EXPECT_EQ(response.bankTransactionId, "BANK-702");
  EXPECT_EQ(response.timestamp, "2026-10-03T12:30:00Z");
}

TEST(LegacyBankResponseTest, NullAuthorizationCodeProducesEmptyOptional) {
  const std::string json = R"({
    "status": "declined",
    "authorization_code": null,
    "message": "Payment declined",
    "bank_transaction_id": "BANK-703",
    "timestamp": "2026-10-03T12:31:00Z"
  })";

  const auto response = LegacyBankResponse::fromJson(json);

  EXPECT_EQ(response.authorizationCode, std::nullopt);
}

TEST(LegacyBankResponseTest, MalformedJsonThrowsExternalResponseError) {
  EXPECT_THROW(LegacyBankResponse::fromJson("{invalid"),
               payment::ExternalResponseError);
}

TEST(LegacyBankResponseTest, MissingRequiredFieldThrowsExternalResponseError) {
  const std::string json = R"({
    "status": "approved",
    "authorization_code": "AUTH-701",
    "message": "Payment approved",
    "timestamp": "2026-10-03T12:30:00Z"
  })";

  EXPECT_THROW(LegacyBankResponse::fromJson(json),
               payment::ExternalResponseError);
}

TEST(LegacyBankResponseTest, WrongFieldTypeThrowsExternalResponseError) {
  const std::string json = R"({
    "status": 200,
    "authorization_code": "AUTH-701",
    "message": "Payment approved",
    "bank_transaction_id": "BANK-702",
    "timestamp": "2026-10-03T12:30:00Z"
  })";

  EXPECT_THROW(LegacyBankResponse::fromJson(json),
               payment::ExternalResponseError);
}

} // namespace
