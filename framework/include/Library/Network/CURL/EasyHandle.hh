#ifndef   __LIBRARY_NETWORK_CURL_EASYHANDLE_HH__
#define   __LIBRARY_NETWORK_CURL_EASYHANDLE_HH__

#include  <curl/curl.h>
#include  <string>
#include  <map>

#include  "Library/Collection/BidiMap.hpp"

namespace curlxx {
  class   EasyHandle {
  private:
    CURL*       _handle;
    curl_slist* _headers;

  public:
    EasyHandle(void);
    virtual ~EasyHandle(void);

  public:
    void init(void);
    void setOpt(CURLoption, long) const;
    void setOpt(CURLoption, const std::string&) const;
    void setOpt(CURLoption, void*) const;
    void setOpt(CURLoption, size_t) const;
    void setOpt(CURLoption, size_t (*)(void*, size_t, size_t, void*)) const;
    void perform(void) const;
    uint32_t getStatus(void) const;
    void cleanup(void);

  public:
    void setHeaders(const std::map<std::string, std::string>&);
    void setMethod(const std::string&) const;
    void setFile(const std::string&, size_t (*callback)(void*, size_t, size_t, void*)) const;
    void setBody(void*, size_t) const;
    void setPort(uint16_t) const;
    void setURL(const std::string& fullURL) const;
    void setUserAgent(const std::string&) const;
    void setResponseCallbacks(void*, size_t (*body)(void*, size_t, size_t, void*), size_t (*headers)(void*, size_t, size_t, void*)) const;

  private:
    void appendHeader(const std::string&, const std::string&);
  };
}

#endif    /* __LIBRARY_NETWORK_CURL_EASYHANDLE_HH__ */
