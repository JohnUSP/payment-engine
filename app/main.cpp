#include <charconv>
#include <iostream>
#include <payment/engine/payment_engine.hpp>
#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/factory/payment_gateway_factory.hpp>
#include <payment/interfaces/gateway/base/payment_gateway.hpp>
#include <payment/interfaces/observer/event_logger.hpp>
#include <payment/interfaces/observer/receipt_printer.hpp>
#include <payment/interfaces/persistence/in_memory_transaction_repository.hpp>
#include <payment/network/http_client.hpp>
#include <payment/types/payment_gateway_type.hpp>
#include <payment/types/transaction.hpp>
#include <string>
#include <string_view>
#include <system_error>

namespace {

struct AppConfig {
  payment::PaymentGatewayType gatewayType{
      payment::PaymentGatewayType::LEGACY_BANK};
  bool enableEventLogger{true};
  bool enableReceiptPrinter{true};
  payment::TransactionId transactionId{1001};
  payment::PaymentType transactionType{payment::PaymentType::CREDIT};
  payment::Amount transactionAmount{4000};
  std::string transactionDescription{"Credit payment transaction"};
};

struct ParsedArguments {
  AppConfig config;
  bool showHelp{false};
};

void printUsage(std::ostream& output) {
  output << "Usage: payment_app [options]\n"
         << "  --gateway legacy|secondary\n"
         << "  --observers all|logger|receipt|none\n"
         << "  --id <positive-transaction-id>\n"
         << "  --type credit|debit\n"
         << "  --amount <positive-amount-in-cents>\n"
         << "  --description <text>\n"
         << "  --help\n\n"
         << "Examples:\n"
         << "  payment_app --gateway secondary --observers logger\n"
         << "  payment_app --id 2001 --type debit --amount 7500 "
            "--description \"Custom debit transaction\"\n";
}

std::string_view requireValue(int& index, int argc, char* argv[],
                              std::string_view option) {
  if (index + 1 >= argc) {
    throw std::invalid_argument("Missing value for " + std::string(option));
  }
  return argv[++index];
}

template <typename Integer>
Integer parsePositiveInteger(std::string_view value, std::string_view option) {
  Integer parsed{};
  const auto [end, error] =
      std::from_chars(value.data(), value.data() + value.size(), parsed);
  if (error != std::errc{} || end != value.data() + value.size() ||
      parsed <= 0) {
    throw std::invalid_argument("Invalid positive integer for " +
                                std::string(option));
  }
  return parsed;
}

ParsedArguments parseArguments(int argc, char* argv[]) {
  ParsedArguments parsed;
  bool hasId = false;
  bool hasType = false;
  bool hasAmount = false;
  bool hasDescription = false;

  for (int index = 1; index < argc; ++index) {
    const std::string_view argument{argv[index]};
    if (argument == "--help") {
      parsed.showHelp = true;
      return parsed;
    }
    if (argument == "--gateway") {
      const auto value = requireValue(index, argc, argv, argument);
      if (value == "legacy") {
        parsed.config.gatewayType = payment::PaymentGatewayType::LEGACY_BANK;
      } else if (value == "secondary") {
        parsed.config.gatewayType = payment::PaymentGatewayType::SECONDARY_BANK;
      } else {
        throw std::invalid_argument("Invalid gateway: " + std::string(value));
      }
    } else if (argument == "--observers") {
      const auto value = requireValue(index, argc, argv, argument);
      if (value == "all") {
        parsed.config.enableEventLogger = true;
        parsed.config.enableReceiptPrinter = true;
      } else if (value == "logger") {
        parsed.config.enableEventLogger = true;
        parsed.config.enableReceiptPrinter = false;
      } else if (value == "receipt") {
        parsed.config.enableEventLogger = false;
        parsed.config.enableReceiptPrinter = true;
      } else if (value == "none") {
        parsed.config.enableEventLogger = false;
        parsed.config.enableReceiptPrinter = false;
      } else {
        throw std::invalid_argument("Invalid observer selection: " +
                                    std::string(value));
      }
    } else if (argument == "--id") {
      parsed.config.transactionId =
          parsePositiveInteger<payment::TransactionId>(
              requireValue(index, argc, argv, argument), argument);
      hasId = true;
    } else if (argument == "--type") {
      const auto value = requireValue(index, argc, argv, argument);
      if (value == "credit") {
        parsed.config.transactionType = payment::PaymentType::CREDIT;
      } else if (value == "debit") {
        parsed.config.transactionType = payment::PaymentType::DEBIT;
      } else {
        throw std::invalid_argument("Invalid transaction type: " +
                                    std::string(value));
      }
      hasType = true;
    } else if (argument == "--amount") {
      parsed.config.transactionAmount = parsePositiveInteger<payment::Amount>(
          requireValue(index, argc, argv, argument), argument);
      hasAmount = true;
    } else if (argument == "--description") {
      parsed.config.transactionDescription =
          requireValue(index, argc, argv, argument);
      if (parsed.config.transactionDescription.empty()) {
        throw std::invalid_argument("Description must not be empty");
      }
      hasDescription = true;
    } else {
      throw std::invalid_argument("Unknown argument: " + std::string(argument));
    }
  }

  const bool hasTransactionOption =
      hasId || hasType || hasAmount || hasDescription;
  if (hasTransactionOption &&
      !(hasId && hasType && hasAmount && hasDescription)) {
    throw std::invalid_argument("Transaction options require --id, --type, "
                                "--amount, and --description");
  }

  return parsed;
}

} // namespace

int main(int argc, char* argv[]) {
  ParsedArguments arguments;
  try {
    arguments = parseArguments(argc, argv);
  } catch (const std::exception& error) {
    std::cerr << "Error: " << error.what() << '\n';
    printUsage(std::cerr);
    return 1;
  }

  if (arguments.showHelp) {
    printUsage(std::cout);
    return 0;
  }

  const auto& config = arguments.config;
  std::cout << "Payment Engine Started\n";

  payment::InMemoryTransactionRepository repository;
  payment::CurlHttpClient httpClient;
  payment::EventLogger eventLogger;
  payment::ReceiptPrinter receiptPrinter;
  auto gateway =
      payment::PaymentGatewayFactory::create(config.gatewayType, httpClient);
  payment::PaymentEngine engine(repository, *gateway);

  if (config.enableEventLogger) {
    engine.addObserver(eventLogger);
  }
  if (config.enableReceiptPrinter) {
    engine.addObserver(receiptPrinter);
  }

  payment::Transaction transaction{config.transactionId, config.transactionType,
                                   config.transactionAmount,
                                   config.transactionDescription};

  try {
    repository.add(transaction);
    engine.process(config.transactionId);
  } catch (const std::exception& error) {
    std::cerr << "Application error: " << error.what() << '\n';
    return 1;
  }

  return 0;
}
