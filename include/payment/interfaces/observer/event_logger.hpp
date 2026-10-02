#pragma once

#include <payment/interfaces/observer/base/payment_observer.hpp>
#include <payment/types/transaction_id.hpp>
#include <string_view>

namespace payment {

class Transaction;
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
