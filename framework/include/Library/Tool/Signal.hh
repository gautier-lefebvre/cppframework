#ifndef   __LIBRARY_TOOL_SIGNAL_HH__
#define   __LIBRARY_TOOL_SIGNAL_HH__

#include <unordered_map>
#include <functional>
#include <csignal>

#include  "Library/Collection/BidiMap.hpp"
#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Threading/Lockable.hpp"

namespace fwk {
    /**
     *  \class Signal Library/Tool/Signal.hh
     *  \brief A singleton class used to catch system signals.
     */
    class Signal :public Singleton<fwk::Signal>, public Lockable {
        friend class Singleton<fwk::Signal>;
    public:
        /**
         *  \enum Type Library/Tool/Signal.hh
         *  \brief Enumerations of system signals.
         */
        enum class Type {
            INT /*!< SIGINT */
        };

    static const BidiMap<Signal::Type, int> SignalTypeToInt; /*!< enum to signum. */

    private:
        /**
         *  \brief Deleted copy constructor.
         */
        Signal(const Signal&) = delete;

        /**
         *  \brief Deleted move constructor.
         */
        Signal(const Signal&&) = delete;

        /**
         *  \brief Deleted assignment constructor.
         */
        Signal&  operator=(const Signal&) = delete;

    private:
        /**
         *  \brief Constructor of Signal.
         */
        Signal(void);

        /**
         *  \brief Destructor of Signal.
         *  Deletes the default callbacks.
         */
        virtual ~Signal(void);

    private:
        std::unordered_map<int, std::function<bool (void)>> callbacks; /*!< current callbacks for each signal number. */
        std::unordered_map<int, struct sigaction*> oldcallbacks; /*!< default callbacks for each signal number. */

    public:
        /**
         *  \brief Creates a callback and saves the default one.
         *  If the callback is nullptr and the signal was already handled, removes the old one and sets back the default.
         *  \param signal the signal to catch.
         *  \param callback the function to call when the signal is caught. If this function returns false, stops catching the signal.
         *  The callback cannot call Signal::delCallback for its own number (segfault). Return false instead.
         */
        void  setCallback(Signal::Type signal, const std::function<bool (void)>& callback);

        /** 
         *  \brief Removes a callback and sets back the default one.
         *  \throw std::out_of_range this signal was not already handled.
         *  \param signal the signal whose callback to remove.
         */
        void  delCallback(Signal::Type signal);

        /**
         *  \brief Calls the callback.
         *  Not meant to be called by anything other than the signal handler (e.g., not by the user).
         *  \param signum the signal number.
         */
        void  handle(int signum);
    };
}

#endif    /* __LIBRARY_TOOL_SIGNAL_HH__ */
