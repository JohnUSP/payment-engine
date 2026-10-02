#include <format>
#include <iostream>
#include <payment/interfaces/observer/event_logger.hpp>
#include <payment/types/transaction.hpp>
#include <syncstream>

namespace payment {

void EventLogger::onPaymentEvent(const Transaction& transaction,
                                 PaymentEvent event) const {
  LogInfo info;
  info.id = transaction.getId();
  info.event = toString(event);
  logEvent(info);
}

void EventLogger::logEvent(const LogInfo& info) const {

  std::osyncstream{std::cout}
      << std::format("[ EVENT ] Transaction {} {}", info.id, info.event)
      << std::endl;
}

} // namespace payment
