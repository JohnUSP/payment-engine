#include <array>
#include <payment/card_utils.h>
#include <random>

namespace payment {

std::string generateCardNumber() {
  static std::mt19937 generator(std::random_device{}());
  static std::uniform_int_distribution<int> digitDistribution(0, 9);

  std::string cardNumber;
  cardNumber.reserve(16);

  for (int i = 0; i < 16; ++i) {
    cardNumber += static_cast<char>('0' + digitDistribution(generator));
  }

  return cardNumber;
}

std::string_view selectIssuer() {
  static constexpr std::array<std::string_view, 5> issuers{
      "Nubank", "Itau", "Bradesco", "Santander", "Banco do Brasil"};

  static std::mt19937 generator(std::random_device{}());
  static std::uniform_int_distribution<std::size_t> distribution(
      0, issuers.size() - 1);

  return issuers[distribution(generator)];
}

int generateInstallments() {
  static std::mt19937 generator(std::random_device{}());
  static std::uniform_int_distribution<int> distribution(1, 12);

  return distribution(generator);
}

std::string generatePixId() {
  static constexpr char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "abcdefghijklmnopqrstuvwxyz"
                                  "0123456789";

  static std::mt19937 generator(std::random_device{}());
  static std::uniform_int_distribution<std::size_t> distribution(
      0, sizeof(chars) - 2);

  std::string id;
  id.reserve(19);

  for (int block = 0; block < 4; ++block) {
    if (block > 0) {
      id += '-';
    }

    for (int i = 0; i < 4; ++i) {
      id += chars[distribution(generator)];
    }
  }

  return id;
}
std::string generateQrCode() {
  static constexpr char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "abcdefghijklmnopqrstuvwxyz"
                                  "0123456789";

  static std::mt19937 generator(std::random_device{}());
  static std::uniform_int_distribution<std::size_t> distribution(
      0, sizeof(chars) - 2);

  std::string qrCode = "PIX:";

  for (int i = 0; i < 24; ++i) {
    qrCode += chars[distribution(generator)];
  }
  return qrCode;
}

} // namespace payment
