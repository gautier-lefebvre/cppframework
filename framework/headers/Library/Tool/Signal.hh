#ifndef   __LIBRARY_TOOL_SIGNAL_HH__
#define   __LIBRARY_TOOL_SIGNAL_HH__

#include <unordered_map>
#include <functional>
#include <csignal>

#include  "Library/Collection/BidiMap.hpp"
#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Threading/Lock.hpp"

class Signal :public Singleton<Signal>, public Threading::Lockable {
  friend class Singleton<Signal>;
public:
  enum class Type {
    INT
  };

static const BidiMap<Signal::Type, int> SignalTypeToInt;

private:
  Signal(const Signal&) = delete;
  Signal(const Signal&&) = delete;
  Signal& operator=(const Signal&) = delete;

private:
  Signal(void);
  virtual ~Signal(void);

private:
  std::unordered_map<int, std::function<bool (void)>> callbacks;
  std::unordered_map<int, struct sigaction*> oldcallbacks;

public:
  /**
   *  \brief Creates a callback and saves the default one.
   *  If the callback is nullptr and the signal was already handled, removes the old one and sets back the default.
   */
  void setCallback(Signal::Type, const std::function<bool (void)>&);

  /** 
   *  \brief Removes a callback and sets back the default one.
   *  \throws std::out_of_range if this signal was not already handled.
   */
  void delCallback(Signal::Type);

  /**
   *  \brief Calls the callback.
   *  Not meant to be called by anything other than the signal handler (e.g., not by the user).
   */
  void handle(int);
};

#endif    /* __LIBRARY_TOOL_SIGNAL_HH__ */
