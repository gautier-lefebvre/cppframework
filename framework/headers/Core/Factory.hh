#ifndef   __CORE_FACTORY_HH__
#define   __CORE_FACTORY_HH__

#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Property/Initializable.hpp"
#include  "Library/Threading/Lock.hpp"

namespace Core {
  class   Factory :public Singleton<Core::Factory>, public Initializable, public Threading::Lockable {
    friend class Singleton<Core::Factory>;
  private:
    Factory(const Factory&) = delete;
    Factory(const Factory&&) = delete;
    Factory& operator=(const Factory&) = delete;

  private:
    Factory(void);
    virtual ~Factory(void);

  public:
    void init(void);
    void end(void);
  };
}

#endif    /* __CORE_FACTORY_HH__ */
