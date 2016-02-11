#include  <stdio.h>
#include  <cstring>
#include  <errno.h>
#include  <sys/stat.h>
#include  <sys/types.h>
#include  <unistd.h>

#include  "Library/Network/CURL/EasyHandle.hh"
#include  "Library/Network/CURL/Exception.hh"
#include  "Library/Network/CURL/Method.hh"
#include  "Library/ThirdParty/cppformat/format.hh"

using namespace fwk;

curlxx::EasyHandle::EasyHandle(void):
  APooled<curlxx::EasyHandle>(),
  _handle(NULL),
  _headers(NULL)
 {
}

curlxx::EasyHandle::~EasyHandle(void) {
  this->cleanup();
}

void curlxx::EasyHandle::init(void) {
  if ((this->_handle = curl_easy_init()) == NULL) {
    throw curlxx::Exception("curl_easy_init failed");
  }
}

void curlxx::EasyHandle::reinit(void) {
  this->cleanup();
}

void curlxx::EasyHandle::setOpt(CURLoption option, long parameter) const {
  CURLcode ret;

  if ((ret = curl_easy_setopt(this->_handle, option, parameter)) != CURLE_OK) {
    throw curlxx::Exception(fmt::format("curl_easy_setopt failed: {0}", curl_easy_strerror(ret)));
  }
}

void curlxx::EasyHandle::setOpt(CURLoption option, const std::string& parameter) const {
  CURLcode ret;

  if ((ret = curl_easy_setopt(this->_handle, option, parameter.c_str())) != CURLE_OK) {
    throw curlxx::Exception(fmt::format("curl_easy_setopt failed: {0}", curl_easy_strerror(ret)));
  }
}

void curlxx::EasyHandle::setOpt(CURLoption option, void* parameter) const {
  CURLcode ret;

  if ((ret = curl_easy_setopt(this->_handle, option, parameter)) != CURLE_OK) {
    throw curlxx::Exception(fmt::format("curl_easy_setopt failed: {0}", curl_easy_strerror(ret)));
  }
}

void curlxx::EasyHandle::setOpt(CURLoption option, size_t parameter) const {
  CURLcode ret;

  if ((ret = curl_easy_setopt(this->_handle, option, parameter)) != CURLE_OK) {
    throw curlxx::Exception(fmt::format("curl_easy_setopt failed: {0}", curl_easy_strerror(ret)));
  }
}

void curlxx::EasyHandle::setOpt(CURLoption option, size_t (*parameter)(void*, size_t, size_t, void*)) const {
  CURLcode ret;

  if ((ret = curl_easy_setopt(this->_handle, option, *parameter)) != CURLE_OK) {
    throw curlxx::Exception(fmt::format("curl_easy_setopt failed: {0}", curl_easy_strerror(ret)));
  }
}

void curlxx::EasyHandle::appendHeader(const std::string& header, const std::string& value) {
  curl_slist* tmp;

  if ((tmp = curl_slist_append(this->_headers, std::string(header + ": " + value).c_str())) == NULL) {
    throw curlxx::Exception("curl_slist_append failed");
  }

  this->_headers = tmp;
}

void curlxx::EasyHandle::setHeaders(const std::map<std::string, std::string>& headers) {
  for (auto& it : headers) {
    this->appendHeader(it.first, it.second);
  }

  this->setOpt(CURLOPT_HTTPHEADER, this->_headers);
}

void curlxx::EasyHandle::perform(void) const {
  CURLcode ret;

  if ((ret = curl_easy_perform(this->_handle)) != CURLE_OK) {
    throw curlxx::Exception(fmt::format("curl_easy_perform failed: {0}", curl_easy_strerror(ret)));
  }
}

uint32_t curlxx::EasyHandle::getStatus(void) const {
  long status;
  CURLcode ret;

  if ((ret = curl_easy_getinfo(this->_handle, CURLINFO_RESPONSE_CODE, &status)) != CURLE_OK) {
    throw curlxx::Exception(fmt::format("curl_easy_getinfo failed: {0}", curl_easy_strerror(ret)));
  }

  return static_cast<uint32_t>(status);
}

void curlxx::EasyHandle::cleanup(void) {
  if (this->_headers != NULL) {
    curl_slist_free_all(this->_headers);
  }

  if (this->_handle != NULL) {
    curl_easy_cleanup(this->_handle);
  }

  this->_headers = NULL;
  this->_handle = NULL;
}

void curlxx::EasyHandle::setMethod(const std::string& method) const {
  try {
    this->setOpt(curlxx::MethodToOption.key.at(method), 1L);
  } catch (const std::out_of_range&) {
    this->setOpt(CURLOPT_CUSTOMREQUEST, method.c_str());
  }
}

void curlxx::EasyHandle::setFile(const std::string& filepath, size_t (*callback)(void*, size_t, size_t, void*)) const {
  struct stat file_info;

  if (stat(filepath.c_str(), &file_info) != 0) {
    throw curlxx::Exception(fmt::format("setFile: invalid file ({0})", strerror(errno)));
  }

  FILE* file = fopen(filepath.c_str(), "rb");

  if (file == NULL) {
    throw curlxx::Exception(fmt::format("setFile: could not open file ({0})", strerror(errno)));
  }

  this->setOpt(CURLOPT_UPLOAD, 1L);
  this->setOpt(CURLOPT_READFUNCTION, callback);
  this->setOpt(CURLOPT_READDATA, file);
  this->setOpt(CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);
}

void curlxx::EasyHandle::setBody(void* ptr, size_t size) const {
  this->setOpt(CURLOPT_POSTFIELDS, ptr);
  this->setOpt(CURLOPT_POSTFIELDSIZE, size);
}

void curlxx::EasyHandle::setPort(uint16_t port) const {
  this->setOpt(CURLOPT_PORT, static_cast<long>(port));
}

void curlxx::EasyHandle::setURL(const std::string& fullURL) const {
  this->setOpt(CURLOPT_URL, fullURL.c_str());
}

void curlxx::EasyHandle::setUserAgent(const std::string& userAgent) const {
  this->setOpt(CURLOPT_USERAGENT, userAgent.c_str());
}

void curlxx::EasyHandle::setResponseCallbacks(void* responsePtr, size_t (*body)(void*, size_t, size_t, void*), size_t (*headers)(void*, size_t, size_t, void*)) const {
  this->setOpt(CURLOPT_WRITEFUNCTION, body);
  this->setOpt(CURLOPT_WRITEDATA, responsePtr);
  this->setOpt(CURLOPT_HEADERFUNCTION, headers);
  this->setOpt(CURLOPT_HEADERDATA, responsePtr);
}

CURL* curlxx::EasyHandle::getHandle(void) const {
  return this->_handle;
}
