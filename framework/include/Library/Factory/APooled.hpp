#ifndef    __LIBRARY_FACTORY_APOOLED_HPP__
#define    __LIBRARY_FACTORY_APOOLED_HPP__

#include  <chrono>

#include  "Library/Factory/Pool.hpp"

namespace fwk {
    /**
     *  \class APooled Library/Factory/APooled.hpp
     *  \brief base class from which any object that will be pooled must inherit.
     */
    template<class C>
    class APooled {
    protected:
        static Pool<C> _pool; /*!< the pool of objects. */

    public:
        /**
         *  \brief Initializes the pool if it was not already created.
         *  \param originalSize number of objects created right away.
         *  \param hydrateSize number of objects created when the pool is empty.
         *  \param className the stored objects' class name.
         */
        static void  initPool(size_t originalSize, size_t hydrateSize, const std::string& className) {
            APooled<C>::_pool.init(originalSize, hydrateSize, className);
        }

        /**
         *  \brief Deletes the pool.
         */
        static void  destroyPool(void) {
            APooled<C>::_pool.clear();
        }

        /**
         *  \brief Takes an object from the pool.
         *  \throw PoolNotInitializedException the pool is empty and was never initialized.
         *  The object must have an `init` method with the same arguments.
         *  \return the object.
         */
        template<typename... Args>
        static C*  getFromPool(Args&&... args) {
            return APooled<C>::_pool.get(std::forward<Args>(args)...);
        }

        /**
         *  \brief Puts an object back into the pool.
         *  \param element the object to put back into the pool.
         */
        static void  returnToPool(C* element) {
            if (element != nullptr) {
                APooled<C>::_pool.push(element);
            }
        }

    private:
        bool  _valid; /*!< true if the object is out of the pool. */
        std::chrono::steady_clock::time_point _lastOutOfPoolTimePoint; /*!< set at current time when the object if taken from the pool. */

    public:
        /**
         *  \brief Constructor of APooled.
         *  Will set valid to false, infering that the object is created by the pool itself.
         */
        APooled(void):
            _valid(false),
            _lastOutOfPoolTimePoint()
        {}

        /**
         *  \brief Destructor of APooled.
         */
        virtual ~APooled(void) {}

    public:
        /**
         *  \brief This method is called when the object is returned to the pool.
         *  Subclass object should return any pooled object it owns to its own pool.
         */
        virtual void  reinit(void) = 0;

    public:
        /**
         *  \brief Updates the \a _valid attribute.
         *  \param valid true if the object is out of the pool, elsewise false.
         */
        void  isValid(bool valid) { this->_valid = valid; }

        /**
         *  \brief Checks if the object is inside the pool.
         *  \return true if the object is out of the pool, elsewise false.
         */
        bool  isValid(void) const { return this->_valid; }

        /**
         *  \brief Sets the last out of pool time point to current time.
         *  This uses a steady clock.
         */
        void  updateLastOutOfPoolTimePoint(void) {
            this->_lastOutOfPoolTimePoint = std::chrono::steady_clock::now();
        }

        /**
         *  \brief Retrieves the last out of pool time point.
         *  \return the last time the object was taken out of the pool.
         *
         *  This can be useful to determine if an object was returned to the pool between some actions.
         */
        const std::chrono::steady_clock::time_point&  lastOutOfPoolTimePoint(void) const {
            return this->_lastOutOfPoolTimePoint;
        }
    };

    template<class C>
    Pool<C> APooled<C>::_pool;
}

#endif    /* __LIBRARY_FACTORY_APOOLED_HH__ */
