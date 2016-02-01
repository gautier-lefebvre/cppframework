#ifndef   __LIBRARY_NETWORK_CURL_METHOD_HH__
#define   __LIBRARY_NETWORK_CURL_METHOD_HH__

#include  "Library/Collection/BidiMap.hpp"

namespace curlxx {
  static const BidiMap<std::string, CURLoption> MethodToOption = {
    {std::string("GET"), CURLOPT_HTTPGET},
    {std::string("POST"), CURLOPT_POST},
    {std::string("PUT"), CURLOPT_PUT},
    {std::string("HEAD"), CURLOPT_NOBODY},
  }; /*!< Methods known by CURL. */
}

#endif    /* __LIBRARY_NETWORK_CURL_METHOD_HH__ */
