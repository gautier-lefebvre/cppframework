#include  "Core/Network/Http/HttpRequest.hpp"

using namespace fwk;

HttpRequest::HttpRequest(void):
    AHttpMessage(),
    APooled<HttpRequest>(),
    method(""),
    url(""),
    queryString(),
    success(nullptr),
    error(nullptr),
    clean(nullptr),
    file({false, ""}),
    asynchronous()
{
    this->asynchronous.isAsynchronous = true;
    this->asynchronous.isValid = false;
    this->asynchronous.response = nullptr;
}

HttpRequest::~HttpRequest(void) {}

void  HttpRequest::reinit(void) {
    this->AHttpMessage::reinit();
    this->method = "";
    this->url = "";
    this->queryString.clear();
    this->success = nullptr;
    this->error = nullptr;
    this->clean = nullptr;
    this->file.isFile = false;
    this->file.filepath = "";
    this->asynchronous.isAsynchronous = true;
    this->asynchronous.isValid = false;
    this->cleanResponse();
}

void  HttpRequest::cleanResponse(void) {
    if (this->asynchronous.response != nullptr) {
        HttpResponse::returnToPool(this->asynchronous.response);
    }
    this->asynchronous.response = nullptr;
}

bool  HttpRequest::wait(void) {
    SCOPELOCK(&(this->asynchronous.lock));
    this->asynchronous.lock.wait([this] (void) -> bool { return !this->asynchronous.isValid || this->asynchronous.response; });

    return this->asynchronous.isValid || this->asynchronous.response;
}

void HttpRequest::wake(HttpResponse *response) {
    SCOPELOCK(&(this->asynchronous.lock));
    this->asynchronous.response = response;
    this->asynchronous.isValid = (response != nullptr);
    this->asynchronous.lock.notify();
}

void HttpRequest::appendToQueryString(const std::string& key, const std::string &value) {
    this->queryString.emplace_back(key, value);
}
