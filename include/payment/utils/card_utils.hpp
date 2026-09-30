#pragma once

#include <string>
#include <string_view>

namespace payment {

std::string generateCardNumber();
std::string_view selectIssuer();
int generateInstallments();
std::string generatePixId();
std::string generateQrCode();

} // namespace payment
