#ifndef    __LIBRARY_COLLECTION_BIDIMAP_HPP__
#define    __LIBRARY_COLLECTION_BIDIMAP_HPP__

#include  <map>
#include  <initializer_list>

namespace fwk {
  /**
   *  \class BidiMap Library/Collection/BidiMap.hpp
   *  \brief a class which does a std::map on the keys and the values.
   */
  template<class A, class B>
  class BidiMap {
  public:
    std::map<A, B>  key; /*!< the map on the keys (classic std::map). */
    std::map<B, A>  value; /*!< the map on the values. */

  public:
    /**
     *  \brief Constructor of BidiMap.
     *
     *  The \a key and \a value maps are empty.
     */
    BidiMap(void):
      key(),
      value()
    {}

    /**
     *  \brief Constructor of BidiMap.
     *  \param l a map to copy.
     */
    BidiMap(std::initializer_list<std::pair<const A, B>> l):
      key(l),
      value()
    {
      for (auto &it : this->key) {
        this->value[it.second] = it.first;
      }
    }

    /**
     *  \brief Assignment Constructor of BidiMap.
     *  \param l a map to copy.
     */
    BidiMap&  operator=(std::initializer_list<std::pair<const A, B>> l) {
      this->key = l;
      for (auto &it : this->key) {
        this->value[it.second] = it.first;
      }
      return *this;
    }

    /**
     *  \brief Copy Constructor of BidiMap.
     *  \param oth a BidiMap to copy.
     */
    BidiMap(const BidiMap& oth):
      key(oth.key),
      value(oth.value)
    {}

    /**
     *  \brief Assignment Constructor of BidiMap.
     *  \param oth a BidiMap to copy.
     *  \return a reference on the current object.
     */
    BidiMap&  operator=(const BidiMap& oth) {
      if (this != &oth) {
        this->key = oth.key;
        this->value = oth.value;
      }
      return *this;
    }

    /**
     *  \brief Destructor of BidiMap.
     */
    virtual ~BidiMap(void) {}

    /**
     *  \brief Adds a key/value pair.
     *  \param k the key.
     *  \param v the value.
     */
    void  insert(const A& k, const B& v) {
      this->key[k] = v;
      this->value[v] = k;
    }

    /**
     *  \brief Removes a key/value pair.
     *  \throw std::out_of_range thrown if the key does not exist.
     *  \param k the key.
     */
    void  erase(const A& k) {
      this->value.erase(this->key.at(k));
      this->key.erase(k);
    }
  };
}

#endif    /* __LIBRARY_COLLECTION_BIDIMAP_HPP__ */
