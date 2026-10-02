#pragma once

namespace payment {

struct ProcessResult {

  enum class PreAuthorization { SUCCESS, ERROR };

  enum class Authorization { AUTHORIZED, DECLINED, ERROR };

  enum class Finalization { SUCCESS, ERROR };
};

} // namespace payment
