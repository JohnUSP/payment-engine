#include <curl/curl.h>
#include <memory>
#include <payment/exceptions/payment_exceptions.hpp>
#include <payment/network/http_client.hpp>
#include <stdexcept>
#include <string>
#include <utility>

namespace payment {

namespace {

constexpr long TIMEOUT_MS = 5000L;

size_t writeCallback(char* data, size_t size, size_t nmemb, void* userData) {
  auto* response = static_cast<std::string*>(userData);
  response->append(data, size * nmemb);

  return size * nmemb;
}

struct CurlDeleter {
  void operator()(CURL* curl) const {
    if (curl) {
      curl_easy_cleanup(curl);
    }
  }
};

struct CurlSlistDeleter {
  void operator()(curl_slist* list) const {
    if (list) {
      curl_slist_free_all(list);
    }
  }
};

} // namespace

HttpResponse CurlHttpClient::post(const std::string& url,
                                  const std::string& body,
                                  const HttpHeaders& headers) {
  std::unique_ptr<CURL, CurlDeleter> curl{curl_easy_init()};

  if (!curl) {
    throw std::runtime_error("Failed to initialize CURL");
  }

  curl_slist* rawHeaders = nullptr;

  for (const auto& header : headers) {
    curl_slist* updatedHeaders = curl_slist_append(rawHeaders, header.c_str());

    if (!updatedHeaders) {
      curl_slist_free_all(rawHeaders);
      throw std::runtime_error("Failed to create CURL headers");
    }

    rawHeaders = updatedHeaders;
  }

  std::unique_ptr<curl_slist, CurlSlistDeleter> curlHeaders{rawHeaders};

  curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, curlHeaders.get());
  curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl.get(), CURLOPT_POST, 1L);
  curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, body.c_str());
  curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDSIZE,
                   static_cast<long>(body.size()));
  curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT_MS, TIMEOUT_MS);

  std::string responseBody;

  curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &responseBody);

  CURLcode result = curl_easy_perform(curl.get());

  if (result != CURLE_OK) {
    throw HttpClientError(curl_easy_strerror(result));
  }

  long statusCode = 0;

  curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &statusCode);

  return HttpResponse{static_cast<int>(statusCode), std::move(responseBody)};
}

} // namespace payment
