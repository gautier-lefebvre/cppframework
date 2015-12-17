#ifndef   __CORE_NETWORK_HTTP_CONNECTION_HH__
#define   __CORE_NETWORK_HTTP_CONNECTION_HH__

#include  <queue>
#include  <thread>

#include  "Library/Property/AEndable.hh"
#include  "Library/Threading/Lock.hpp"
#include  "Library/Threading/Condition.hpp"
#include  "Core/Network/HTTP/Request.hh"

namespace     Core {
  namespace   Network {
    namespace HTTP {
      class Connection :public Threading::Lockable, public AEndable {
      private:
        struct upload_object {
          const uint8_t*  ptr;
          size_t      length;
        };

      private:
        std::string _host;
        uint16_t  _port;
        uint16_t  _secureport;
        std::string _userAgent;
        std::thread *_thread;
        Threading::TNotifiable<std::queue<Core::Network::HTTP::Request*>>  _pendingRequests;

      public:
        Connection(const std::string &, uint16_t, uint16_t, const std::string& user_agent);
        virtual ~Connection(void);

      public:
        virtual void  end(void);

      public:
        void      run(void);
        const std::string&  getHost(void) const;
        uint16_t  getPort(void) const;
        uint16_t  getSecurePort(void) const;
        void      addRequest(Core::Network::HTTP::Request*);

      private:
        Response* sendRequest(const Core::Network::HTTP::Request*) const;
        void      routine(void);
        Response* exec(const Core::Network::HTTP::Request*) const;

      private:
        static size_t read_callback(void*, size_t, size_t, void*);
        static size_t write_callback(void*, size_t, size_t, void*);
        static size_t header_callback(void*, size_t, size_t, void*);
      };
    }
  }
}

#endif    /* __CORE_NETWORK_HTTP_CONNECTION_HH__ */
