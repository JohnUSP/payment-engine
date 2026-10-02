#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/interfaces/gateway/legacy_bank.hpp>
#include <payment/interfaces/gateway/legacy_bank_contract.hpp>
#include <payment/network/http_client.hpp>
#include <payment/types/transaction.hpp>
#include <string>

namespace payment {

LegacyBank::LegacyBank(HttpClient& client) : m_client(client) {}

namespace {

constexpr std::string_view LEGACY_BANK_URL_AUTHORIZATION =
    "https://e47bf14d-f683-4eb3-ac2c-06c7e5d6289c.mock.pstmn.io/authorization";

} // namespace

ProcessResult::Authorization LegacyBank::send(const Transaction& transaction) {
  LegacyBankRequest request(transaction.getId(), transaction.getAmount(),
                            transaction.getType(),
                            transaction.getDescription());

  const std::string body = request.toJson();

  const char* apiKey = std::getenv("LEGACY_BANK_API_KEY");

  if (!apiKey) {
    throw std::runtime_error("LEGACY_BANK_API_KEY is not set");
  }

  const HttpHeaders headers{"Content-Type: application/json",
                            std::string("x-api-key: ") + apiKey,
                            "x-mock-response-name: Approved"};

  try {
    const HttpResponse response = m_client.post(
        std::string{LEGACY_BANK_URL_AUTHORIZATION}, body, headers);

    if (response.statusCode < 200 || response.statusCode >= 300) {
      return ProcessResult::Authorization::ERROR;
    }

    const LegacyBankResponse bankResponse =
        LegacyBankResponse::fromJson(response.body);

    if (bankResponse.status == "approved") {
      return ProcessResult::Authorization::AUTHORIZED;
    }

    if (bankResponse.status == "declined") {
      return ProcessResult::Authorization::DECLINED;
    }

    return ProcessResult::Authorization::ERROR;

  } catch (const HttpClientError&) {
    return ProcessResult::Authorization::ERROR;

  } catch (const ExternalResponseError&) {
    return ProcessResult::Authorization::ERROR;
  }
}

} // namespace payment
