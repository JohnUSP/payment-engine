#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nlohmann/json.hpp>
#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/interfaces/gateway/legacy_bank.hpp>
#include <payment/network/http_client.hpp>
#include <payment/types/transaction.hpp>

#include <algorithm>
#include <stdlib.h>
#include <string>

namespace {

using payment::HttpClient;
using payment::HttpHeaders;
using payment::HttpResponse;
using payment::LegacyBank;
using payment::PaymentType;
using payment::ProcessResult;
using payment::Transaction;

constexpr char kApiKey[] = "legacy-bank-unit-test-key";
constexpr char kAuthorizationEndpoint[] =
    "https://e47bf14d-f683-4eb3-ac2c-06c7e5d6289c.mock.pstmn.io/authorization";

class MockHttpClient : public HttpClient {
public:
  MOCK_METHOD(HttpResponse, post,
              (const std::string& url, const std::string& body,
               const HttpHeaders& headers),
              (override));
};

class LegacyBankTest : public testing::Test {
protected:
  void SetUp() override {
    ASSERT_EQ(setenv("LEGACY_BANK_API_KEY", kApiKey, 1), 0);
  }

  void TearDown() override { unsetenv("LEGACY_BANK_API_KEY"); }

  MockHttpClient client;
  LegacyBank gateway{client};

  static std::string responseBody(const std::string& status) {
    return nlohmann::json{{"status", status},
                          {"authorization_code", "AUTH-123"},
                          {"message", "Processed"},
                          {"bank_transaction_id", "BANK-456"},
                          {"timestamp", "2026-10-03T12:00:00Z"}}
        .dump();
  }

  static Transaction sampleTransaction() {
    return Transaction{501, PaymentType::CREDIT, 4675, "test payment"};
  }
};

TEST_F(LegacyBankTest, ApprovedResponseReturnsAuthorized) {
  EXPECT_CALL(client, post(testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(HttpResponse{200, responseBody("approved")}));

  const auto result = gateway.send(sampleTransaction());

  EXPECT_EQ(result, ProcessResult::Authorization::AUTHORIZED);
}

TEST_F(LegacyBankTest, DeclinedResponseReturnsDeclined) {
  EXPECT_CALL(client, post(testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(HttpResponse{200, responseBody("declined")}));

  const auto result = gateway.send(sampleTransaction());

  EXPECT_EQ(result, ProcessResult::Authorization::DECLINED);
}

TEST_F(LegacyBankTest, UnknownBankStatusReturnsError) {
  EXPECT_CALL(client, post(testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(HttpResponse{200, responseBody("processing")}));

  const auto result = gateway.send(sampleTransaction());

  EXPECT_EQ(result, ProcessResult::Authorization::ERROR);
}

TEST_F(LegacyBankTest, NonSuccessHttpStatusReturnsErrorWithoutParsingBody) {
  EXPECT_CALL(client, post(testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(HttpResponse{500, "not JSON"}));

  const auto result = gateway.send(sampleTransaction());

  EXPECT_EQ(result, ProcessResult::Authorization::ERROR);
}

TEST_F(LegacyBankTest, HttpClientErrorReturnsError) {
  EXPECT_CALL(client, post(testing::_, testing::_, testing::_))
      .WillOnce(testing::Throw(payment::HttpClientError("connection failed")));

  const auto result = gateway.send(sampleTransaction());

  EXPECT_EQ(result, ProcessResult::Authorization::ERROR);
}

TEST_F(LegacyBankTest, InvalidJsonReturnsError) {
  EXPECT_CALL(client, post(testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(HttpResponse{200, "{invalid"}));

  const auto result = gateway.send(sampleTransaction());

  EXPECT_EQ(result, ProcessResult::Authorization::ERROR);
}

TEST_F(LegacyBankTest, MissingRequiredResponseFieldReturnsError) {
  const std::string body = nlohmann::json{{"status", "approved"},
                                          {"authorization_code", "AUTH-123"},
                                          {"message", "Processed"},
                                          {"timestamp", "2026-10-03T12:00:00Z"}}
                               .dump();
  EXPECT_CALL(client, post(testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(HttpResponse{200, body}));

  const auto result = gateway.send(sampleTransaction());

  EXPECT_EQ(result, ProcessResult::Authorization::ERROR);
}

TEST_F(LegacyBankTest, SendsExpectedRequestAndHeaders) {
  const auto transaction = sampleTransaction();
  EXPECT_CALL(client, post(testing::_, testing::_, testing::_))
      .Times(1)
      .WillOnce(testing::Invoke([&transaction](const std::string& url,
                                               const std::string& body,
                                               const HttpHeaders& headers) {
        EXPECT_NE(url.find(kAuthorizationEndpoint), std::string::npos);

        const auto request = nlohmann::json::parse(body);
        EXPECT_EQ(request.at("transaction_id"), transaction.getId());
        EXPECT_EQ(request.at("amount"), transaction.getAmount());
        EXPECT_EQ(request.at("description"), transaction.getDescription());
        EXPECT_EQ(request.at("payment_type"), "Credit");

        const auto hasHeader = [&headers](const std::string& expected) {
          return std::find(headers.begin(), headers.end(), expected) !=
                 headers.end();
        };
        EXPECT_TRUE(hasHeader("Content-Type: application/json"));
        EXPECT_TRUE(hasHeader(std::string("x-api-key: ") + kApiKey));
        EXPECT_TRUE(hasHeader("x-mock-response-name: Approved"));

        return HttpResponse{200, responseBody("approved")};
      }));

  EXPECT_EQ(gateway.send(transaction),
            ProcessResult::Authorization::AUTHORIZED);
}

} // namespace
