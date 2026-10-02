#pragma once

#include <functional>
#include <payment/types/payment_event.hpp>

namespace payment {

class Transaction;

using PaymentEventCallback =
    std::function<void(const Transaction& transaction, PaymentEvent event)>;

} // namespace payment
