#include <gtest/gtest.h>

#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/network/http_client.hpp>

#include <nlohmann/json.hpp>

#include <string>

namespace {

// Network-dependent checks are intentionally opt-in through
// ENABLE_HTTP_INTEGRATION_TESTS.
constexpr char kEchoEndpoint[] = "https://httpbin.org/post";

TEST(CurlHttpClientIntegrationTest, SuccessfulPostReturnsHttpResponse) {
  payment::CurlHttpClient client;
  const std::string body = R"({"message":"integration check"})";
  const payment::HttpHeaders headers{"Content-Type: application/json"};

  const auto response = client.post(kEchoEndpoint, body, headers);

  EXPECT_GE(response.statusCode, 200);
  EXPECT_LT(response.statusCode, 300);
  EXPECT_FALSE(response.body.empty());
}

TEST(CurlHttpClientIntegrationTest, CapturesEchoedResponseBody) {
  payment::CurlHttpClient client;
  const std::string body = R"({"echo_marker":"response-body-check"})";
  const payment::HttpHeaders headers{"Content-Type: application/json"};

  const auto response = client.post(kEchoEndpoint, body, headers);

  ASSERT_GE(response.statusCode, 200);
  ASSERT_LT(response.statusCode, 300);
  EXPECT_NE(response.body.find("response-body-check"), std::string::npos);
}

TEST(CurlHttpClientIntegrationTest, SendsBodyAndCustomHeader) {
  payment::CurlHttpClient client;
  const std::string body = R"({"request_marker":"body-was-received"})";
  const payment::HttpHeaders headers{"Content-Type: application/json",
                                     "X-Integration-Test: header-was-received"};

  const auto response = client.post(kEchoEndpoint, body, headers);

  ASSERT_GE(response.statusCode, 200);
  ASSERT_LT(response.statusCode, 300);
  const auto echoed = nlohmann::json::parse(response.body);
  EXPECT_EQ(echoed.at("json").at("request_marker"), "body-was-received");
  EXPECT_EQ(echoed.at("headers").at("X-Integration-Test"),
            "header-was-received");
}

TEST(CurlHttpClientIntegrationTest, UnreachableLocalEndpointThrows) {
  payment::CurlHttpClient client;
  const payment::HttpHeaders headers{"Content-Type: application/json"};

  EXPECT_THROW(client.post("http://127.0.0.1:1", "{}", headers),
               payment::HttpClientError);
}

} // namespace
