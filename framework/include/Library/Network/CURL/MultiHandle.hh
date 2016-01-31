#ifndef   __LIBRARY_NETWORK_CURL_MULTIHANDLE_HH__
#define   __LIBRARY_NETWORK_CURL_MULTIHANDLE_HH__

#include  <vector>
#include  <curl/curl.h>

#include  "Library/Network/CURL/EasyHandle.hh"

namespace curlxx {
  class MultiHandle {
  public:
    static const size_t PIPELINED_REQUESTS_MAX_NB; /*!< max number of requests sent at once (arbitrary, should be small (~10) not to ddos the remote server by mistake). */

  private:
    CURLM* _multiHandle;
    std::vector<curlxx::EasyHandle*> _easyHandles;

  public:
    MultiHandle(void);
    virtual ~MultiHandle(void);

  public:
    EasyHandle* findHandle(CURL*);
    bool hasHandles(void) const;

  public:
    void init(void);
    void addHandle(EasyHandle*);
    void removeHandle(EasyHandle*);
    bool perform(void) const;
    void timeout(struct timeval&) const;
    void fdset(fd_set*, fd_set*, fd_set*, int&) const;
    CURLMsg* infoRead(void) const;
    void cleanup(void);
  };
}

#endif    /* __LIBRARY_NETWORK_CURL_MULTIHANDLE_HH__ */
