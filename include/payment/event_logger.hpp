#pragma once
#include <payment/payment_observer.hpp>
#include <payment/transaction.hpp>

namespace payment {

struct LogInfo {
  TransactionId id{};
  std::string_view event;
};

class EventLogger : public PaymentObserver {

public:
  void onPaymentEvent(const Transaction& transaction,
                      PaymentEvent event) const override;

private:
  void logEvent(const LogInfo& info) const;
};

} // namespace payment
