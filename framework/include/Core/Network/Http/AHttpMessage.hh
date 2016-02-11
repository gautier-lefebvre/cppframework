#ifndef   __CORE_NETWORK_HTTP_AHTTPMESSAGE_HH__
#define   __CORE_NETWORK_HTTP_AHTTPMESSAGE_HH__

#include  <map>
#include  <string>

#include  "Library/Collection/ByteArray.hpp"
#include  "Library/Factory/AFactored.hh"

namespace fwk {
  /**
   *  \class AHttpMessage Core/Network/Http/AHttpMessage.hh
   *  \brief Superclass of Requests and Responses.
   */
  struct AHttpMessage :public Factory::AFactored {
  public:
    std::string  version; /*!< HTTP version of the request. Default is 1.1 */
    std::map<std::string, std::string>  headers; /*!< headers of the request. */
    ByteArray  *body; /*!< body of the request. */

  public:
    /**
     *  \brief Constructor of AHttpMessage.
     */
    AHttpMessage(void);

    /**
     *  \brief Destructor of AHttpMessage.
     */
    virtual ~AHttpMessage(void);

  public:
    /**
     *  \brief Reinits the AHttpMessage. Sets back the default HTTP version, clears the headers and returns the body to the pool.
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Takes a ByteArray from the Pool.
     */
    void  init(void);

  public:
    /**
     *  \brief gets a HTTP header value by its key.
     *  \throw std::out_of_range the header key does not exist.
     *  \param header the key of the header. Not case sensitive.
     *  \return the header value.
     */
    const std::string&  getHeader(const std::string& header) const;

    /**
     *  \brief checks if the request has a header.
     *  \param header the header key.
     *  \return true if the header is present.
     */
    bool  hasHeader(const std::string& header) const;
  };
}

#endif    /* __CORE_NETWORK_HTTP_AHTTPMESSAGE_HH__ */
