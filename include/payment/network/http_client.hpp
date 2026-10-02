#pragma once

#include <string>
#include <vector>

namespace payment {

using HttpHeaders = std::vector<std::string>;
struct HttpResponse {
  int statusCode;
  std::string body;
};

class HttpClient {
public:
  HttpResponse post(const std::string& url, const std::string& body,
                    const HttpHeaders& headers);
};

} // namespace payment
