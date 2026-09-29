#include <iostream>
#include <payment/event_logger.h>
namespace payment {

void EventLogger::onPaymentEvent(const Transaction& transaction,
                                 PaymentEvent event) const {
  LogInfo info;
  info.id = transaction.getId();
  info.event = toString(event);
  logEvent(info);
}

void EventLogger::logEvent(const LogInfo& info) const {

  std::cout << std::format("[ EVENT ] Transaction {} {}", info.id, info.event)
            << std::endl;
}

} // namespace payment
