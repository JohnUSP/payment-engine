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
  virtual ~HttpClient() = default;

  virtual HttpResponse post(const std::string& url, const std::string& body,
                            const HttpHeaders& headers) = 0;
};

class CurlHttpClient : public HttpClient {
public:
  HttpResponse post(const std::string& url, const std::string& body,
                    const HttpHeaders& headers) override;
};

} // namespace payment
