#include <format>
#include <nlohmann/json.hpp>
#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/gateways/legacy_bank_contract.hpp>

namespace payment {

std::string LegacyBankRequest::toJson() const {
  nlohmann::json json;

  json["transaction_id"] = transactionId;
  json["amount"] = amount;
  json["description"] = description;
  json["payment_type"] = toString(paymentType);

  return json.dump();
}

LegacyBankResponse LegacyBankResponse::fromJson(const std::string& json) {
  try {
    const auto parsed = nlohmann::json::parse(json);

    LegacyBankResponse response;

    response.status = parsed.at("status").get<std::string>();
    response.message = parsed.at("message").get<std::string>();

    if (!parsed.at("authorization_code").is_null()) {
      response.authorizationCode =
          parsed.at("authorization_code").get<std::string>();
    }

    response.bankTransactionId =
        parsed.at("bank_transaction_id").get<std::string>();

    response.timestamp = parsed.at("timestamp").get<std::string>();

    return response;

  } catch (const nlohmann::json::parse_error& e) {
    throw ExternalResponseError(
        std::format("Invalid JSON response from LegacyBank: {}", e.what()));

  } catch (const nlohmann::json::out_of_range& e) {
    throw ExternalResponseError(
        std::format("Missing field in LegacyBank response: {}", e.what()));

  } catch (const nlohmann::json::type_error& e) {
    throw ExternalResponseError(
        std::format("Invalid field type in LegacyBank response: {}", e.what()));
  }

}

} // namespace payment
