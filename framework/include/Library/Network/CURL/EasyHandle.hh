#ifndef   __LIBRARY_NETWORK_CURL_EASYHANDLE_HH__
#define   __LIBRARY_NETWORK_CURL_EASYHANDLE_HH__

#include  <curl/curl.h>
#include  <string>
#include  <map>

#include  "Library/Collection/BidiMap.hpp"
#include  "Library/Factory/APooled.hpp"

namespace fwk {
  /**
   *  \namespace fwk::curlxx
   *  \brief CURL library encapsulation
   */
  namespace curlxx {
    /**
     *  \class EasyHandle Library/Network/CURL/EasyHandle.hh
     *  \brief Encapsulation of CURL easy handle functions.
     */
    class EasyHandle :public APooled<curlxx::EasyHandle> {
    private:
      CURL*       _handle; /*!< the curl easy handle object. */
      curl_slist* _headers; /*!< the headers of the current request. */

    public:
      /**
       *  \brief Constructor of EasyHandle.
       */
      EasyHandle(void);

      /**
       *  \brief Destructor of EasyHandle.
       */
      virtual ~EasyHandle(void);

    public:
      /**
       *  \brief Creates the easy handle and header list.
       *  \throw curlx::Exception if the underlying function failed.
       */
      void  init(void);

      /**
       *  \brief Cleans up the handle.
       */
      virtual void  reinit(void);

    public:
      /**
       *  \brief Encapsulation of the curl_easy_setopt function.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param option the CURL option (see curl man).
       *  \param parameter the parameter associated to this option (see curl man);
       */
      void  setOpt(CURLoption option, long parameter) const;

      /**
       *  \brief Encapsulation of the curl_easy_setopt function.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param option the CURL option (see curl man).
       *  \param parameter the parameter associated to this option (see curl man);
       */
      void  setOpt(CURLoption option, const std::string& parameter) const;

      /**
       *  \brief Encapsulation of the curl_easy_setopt function.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param option the CURL option (see curl man).
       *  \param parameter the parameter associated to this option (see curl man);
       */
      void  setOpt(CURLoption option, void* parameter) const;

      /**
       *  \brief Encapsulation of the curl_easy_setopt function.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param option the CURL option (see curl man).
       *  \param parameter the parameter associated to this option (see curl man);
       */
      void  setOpt(CURLoption option, size_t parameter) const;

      /**
       *  \brief Encapsulation of the curl_easy_setopt function.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param option the CURL option (see curl man).
       *  \param parameter the parameter associated to this option (see curl man);
       */
      void  setOpt(CURLoption option, size_t (*parameter)(void*, size_t, size_t, void*)) const;

      /**
       *  \brief Encapsulation of the curl_easy_perform function.
       *  \throw curlx::Exception if the underlying function failed.
       */
      void  perform(void) const;

      /**
       *  \brief Gets the status code of the response (200...).
       *  Should only be called after perform returned successfully.
       *  \throw curlx::Exception if the underlying function failed.
       *  \return the status code of the request.
       */
      uint32_t  getStatus(void) const;

      /**
       *  \brief Cleans up the handle.
       */
      void  cleanup(void);

    public:
      /**
       *  \brief Sets the headers of the HTTP request.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param headers the headers.
       */
      void  setHeaders(const std::map<std::string, std::string>& headers);

      /**
       *  \brief Sets the HTTP method (GET, POST, PATCH, PUT, DELETE, ...)
       *  \throw curlx::Exception if the underlying function failed.
       *  \param method the HTTP method.
       */
      void  setMethod(const std::string& method) const;

      /**
       *  \brief Sets the file to upload.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param filepath the path to the file.
       *  \param callback the method used by curl to send the file (see curl upload examples)
       */
      void  setFile(const std::string& filepath, size_t (*callback)(void*, size_t, size_t, void*)) const;

      /**
       *  \brief Sets the body to send.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param ptr pointer to the start of the body.
       *  \param size size of the body in bytes.
       */
      void  setBody(void* ptr, size_t size) const;

      /**
       *  \brief Sets the HTTP port.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param port the port.
       */
      void  setPort(uint16_t port) const;

      /**
       *  \brief Sets the URL. Format must be {protocol}://{hostname}/{path}.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param fullURL the URL.
       */
      void  setURL(const std::string& fullURL) const;

      /**
       *  \brief Sets the user agent string.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param userAgent the user agent string.
       */
      void  setUserAgent(const std::string& userAgent) const;

      /**
       *  \brief Sets the response callbacks.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param responsePtr a pointer to some data used by the given callbacks to save the response.
       *  \param body a method used to save the body in the response data.
       *  \param headers a method used to save the headers of the response in the response data.
       */
      void  setResponseCallbacks(void* responsePtr, size_t (*body)(void*, size_t, size_t, void*), size_t (*headers)(void*, size_t, size_t, void*)) const;

    public:
      /**
       *  \brief Get the underlying CURL easy handle. If the EasyHandle was not init, returns NULL.
       *  \return the CURL easy handle.
       */
      CURL*  getHandle(void) const;

    private:
      /**
       *  \brief Adds a header to the header list.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param key key of the header.
       *  \param value value of the header.
       */
      void  appendHeader(const std::string& header, const std::string& value);
    };
  }
}

#endif    /* __LIBRARY_NETWORK_CURL_EASYHANDLE_HH__ */
