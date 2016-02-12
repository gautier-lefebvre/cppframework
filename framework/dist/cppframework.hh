#ifndef    __LIBRARY_EXCEPTION_HH__
#define    __LIBRARY_EXCEPTION_HH__

#include  <string>
#include  <exception>

namespace fwk {
  /**
   *  \class Exception Library/Exception.hh
   *  \brief Base class of all the custom exceptions of the project.
   */
  class Exception :public std::exception {
  private:
    std::string  _what; /*!< a string describing the error. */

  public:
    /**
     *  \brief Constructor of Exception.
     *  \param what a string describing the error.
     */
    Exception(const std::string& what) noexcept;

    /**
     *  \brief Constructor of Exception.
     *  \param oth an exception to copy.
     */
    Exception(const std::exception& oth) noexcept;

    /**
     *  \brief Constructor of Exception.
     *  \param oth an exception to copy.
     */
    Exception& operator=(const std::exception& oth) noexcept;

    /**
     *  \brief Destructor of Exception.
     */
    virtual ~Exception(void);

    /**
     *  \brief Gets the error thrown.
     *  \return a string describing the error.
     */
    virtual const char*  what(void) const noexcept;
  };
}

#endif    /* __LIBRARY_EXCEPTION_HH__ */

/*
 Formatting library for C++

 Copyright (c) 2012 - 2015, Victor Zverovich
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FMT_FORMAT_H_
#define FMT_FORMAT_H_

#if defined _MSC_VER && _MSC_VER <= 1500
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
typedef long long          intmax_t;
#else
#include <stdint.h>
#endif

#include <cassert>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>
#include <map>

#ifndef FMT_USE_IOSTREAMS
# define FMT_USE_IOSTREAMS 1
#endif

#if FMT_USE_IOSTREAMS
# include <ostream>
#endif

#ifdef _SECURE_SCL
# define FMT_SECURE_SCL _SECURE_SCL
#else
# define FMT_SECURE_SCL 0
#endif

#if FMT_SECURE_SCL
# include <iterator>
#endif

#ifdef _MSC_VER
# include <intrin.h>  // _BitScanReverse, _BitScanReverse64

namespace fmt {
namespace internal {
# pragma intrinsic(_BitScanReverse)
inline uint32_t clz(uint32_t x) {
  unsigned long r = 0;
  _BitScanReverse(&r, x);
  return 31 - r;
}
# define FMT_BUILTIN_CLZ(n) fmt::internal::clz(n)

# ifdef _WIN64
#  pragma intrinsic(_BitScanReverse64)
# endif

inline uint32_t clzll(uint64_t x) {
  unsigned long r = 0;
# ifdef _WIN64
  _BitScanReverse64(&r, x);
# else
  // Scan the high 32 bits.
  if (_BitScanReverse(&r, static_cast<uint32_t>(x >> 32)))
    return 63 - (r + 32);

  // Scan the low 32 bits.
  _BitScanReverse(&r, static_cast<uint32_t>(x));
# endif
  return 63 - r;
}
# define FMT_BUILTIN_CLZLL(n) fmt::internal::clzll(n)
}
}
#endif

#ifdef __GNUC__
# define FMT_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
# define FMT_GCC_EXTENSION __extension__
# if FMT_GCC_VERSION >= 406
#  pragma GCC diagnostic push
// Disable the warning about "long long" which is sometimes reported even
// when using __extension__.
#  pragma GCC diagnostic ignored "-Wlong-long"
// Disable the warning about declaration shadowing because it affects too
// many valid cases.
#  pragma GCC diagnostic ignored "-Wshadow"
// Disable the warning about implicit conversions that may change the sign of
// an integer; silencing it otherwise would require many explicit casts.
#  pragma GCC diagnostic ignored "-Wsign-conversion"
# endif
# if __cplusplus >= 201103L || defined __GXX_EXPERIMENTAL_CXX0X__
#  define FMT_HAS_GXX_CXX11 1
# endif
#else
# define FMT_GCC_EXTENSION
#endif

#if defined(__clang__) && !defined(__INTEL_COMPILER)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdocumentation"
#endif

#ifdef __GNUC_LIBSTD__
# define FMT_GNUC_LIBSTD_VERSION (__GNUC_LIBSTD__ * 100 + __GNUC_LIBSTD_MINOR__)
#endif

#ifdef __has_feature
# define FMT_HAS_FEATURE(x) __has_feature(x)
#else
# define FMT_HAS_FEATURE(x) 0
#endif

#ifdef __has_builtin
# define FMT_HAS_BUILTIN(x) __has_builtin(x)
#else
# define FMT_HAS_BUILTIN(x) 0
#endif

#ifdef __has_cpp_attribute
# define FMT_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
# define FMT_HAS_CPP_ATTRIBUTE(x) 0
#endif

#ifndef FMT_USE_VARIADIC_TEMPLATES
// Variadic templates are available in GCC since version 4.4
// (http://gcc.gnu.org/projects/cxx0x.html) and in Visual C++
// since version 2013.
# define FMT_USE_VARIADIC_TEMPLATES \
   (FMT_HAS_FEATURE(cxx_variadic_templates) || \
       (FMT_GCC_VERSION >= 404 && FMT_HAS_GXX_CXX11) || _MSC_VER >= 1800)
#endif

#ifndef FMT_USE_RVALUE_REFERENCES
// Don't use rvalue references when compiling with clang and an old libstdc++
// as the latter doesn't provide std::move.
# if defined(FMT_GNUC_LIBSTD_VERSION) && FMT_GNUC_LIBSTD_VERSION <= 402
#  define FMT_USE_RVALUE_REFERENCES 0
# else
#  define FMT_USE_RVALUE_REFERENCES \
    (FMT_HAS_FEATURE(cxx_rvalue_references) || \
        (FMT_GCC_VERSION >= 403 && FMT_HAS_GXX_CXX11) || _MSC_VER >= 1600)
# endif
#endif

#if FMT_USE_RVALUE_REFERENCES
# include <utility>  // for std::move
#endif

// Define FMT_USE_NOEXCEPT to make C++ Format use noexcept (C++11 feature).
#ifndef FMT_USE_NOEXCEPT
# define FMT_USE_NOEXCEPT 0
#endif

#ifndef FMT_NOEXCEPT
# if FMT_USE_NOEXCEPT || FMT_HAS_FEATURE(cxx_noexcept) || \
   (FMT_GCC_VERSION >= 408 && FMT_HAS_GXX_CXX11) || \
   _MSC_VER >= 1900
#  define FMT_NOEXCEPT noexcept
# else
#  define FMT_NOEXCEPT throw()
# endif
#endif

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#ifndef FMT_USE_DELETED_FUNCTIONS
# define FMT_USE_DELETED_FUNCTIONS 0
#endif

#if FMT_USE_DELETED_FUNCTIONS || FMT_HAS_FEATURE(cxx_deleted_functions) || \
  (FMT_GCC_VERSION >= 404 && FMT_HAS_GXX_CXX11) || _MSC_VER >= 1800
# define FMT_DELETED_OR_UNDEFINED  = delete
# define FMT_DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete; \
    TypeName& operator=(const TypeName&) = delete
#else
# define FMT_DELETED_OR_UNDEFINED
# define FMT_DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
    TypeName& operator=(const TypeName&)
#endif

#ifndef FMT_USE_USER_DEFINED_LITERALS
// All compilers which support UDLs also support variadic templates. This
// makes the fmt::literals implementation easier. However, an explicit check
// for variadic templates is added here just in case.
# define FMT_USE_USER_DEFINED_LITERALS \
   FMT_USE_VARIADIC_TEMPLATES && FMT_USE_RVALUE_REFERENCES && \
   (FMT_HAS_FEATURE(cxx_user_literals) || \
       (FMT_GCC_VERSION >= 407 && FMT_HAS_GXX_CXX11) || _MSC_VER >= 1900)
#endif

#ifndef FMT_ASSERT
# define FMT_ASSERT(condition, message) assert((condition) && message)
#endif

namespace fmt {
namespace internal {
struct DummyInt {
  int data[2];
  operator int() const { return 0; }
};
typedef std::numeric_limits<fmt::internal::DummyInt> FPUtil;

// Dummy implementations of system functions such as signbit and ecvt called
// if the latter are not available.
inline DummyInt signbit(...) { return DummyInt(); }
inline DummyInt _ecvt_s(...) { return DummyInt(); }
inline DummyInt isinf(...) { return DummyInt(); }
inline DummyInt _finite(...) { return DummyInt(); }
inline DummyInt isnan(...) { return DummyInt(); }
inline DummyInt _isnan(...) { return DummyInt(); }

// A helper function to suppress bogus "conditional expression is constant"
// warnings.
template <typename T>
inline T check(T value) { return value; }
}
}  // namespace fmt

namespace std {
// Standard permits specialization of std::numeric_limits. This specialization
// is used to resolve ambiguity between isinf and std::isinf in glibc:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=48891
// and the same for isnan and signbit.
template <>
class numeric_limits<fmt::internal::DummyInt> :
    public std::numeric_limits<int> {
 public:
  // Portable version of isinf.
  template <typename T>
  static bool isinfinity(T x) {
    using namespace fmt::internal;
    // The resolution "priority" is:
    // isinf macro > std::isinf > ::isinf > fmt::internal::isinf
    if (check(sizeof(isinf(x)) == sizeof(bool) ||
              sizeof(isinf(x)) == sizeof(int))) {
      return !!isinf(x);
    }
    return !_finite(static_cast<double>(x));
  }

  // Portable version of isnan.
  template <typename T>
  static bool isnotanumber(T x) {
    using namespace fmt::internal;
    if (check(sizeof(isnan(x)) == sizeof(bool) ||
              sizeof(isnan(x)) == sizeof(int))) {
      return !!isnan(x);
    }
    return _isnan(static_cast<double>(x)) != 0;
  }

  // Portable version of signbit.
  static bool isnegative(double x) {
    using namespace fmt::internal;
    if (check(sizeof(signbit(x)) == sizeof(int)))
      return !!signbit(x);
    if (x < 0) return true;
    if (!isnotanumber(x)) return false;
    int dec = 0, sign = 0;
    char buffer[2];  // The buffer size must be >= 2 or _ecvt_s will fail.
    _ecvt_s(buffer, sizeof(buffer), x, 0, &dec, &sign);
    return sign != 0;
  }
};
}  // namespace std

namespace fmt {

// Fix the warning about long long on older versions of GCC
// that don't support the diagnostic pragma.
FMT_GCC_EXTENSION typedef long long LongLong;
FMT_GCC_EXTENSION typedef unsigned long long ULongLong;

#if FMT_USE_RVALUE_REFERENCES
using std::move;
#endif

template <typename Char>
class BasicWriter;

typedef BasicWriter<char> Writer;
typedef BasicWriter<wchar_t> WWriter;

template <typename Char>
class BasicFormatter;

template <typename Char, typename T>
void format(BasicFormatter<Char> &f, const Char *&format_str, const T &value);

/**
  \rst
  A string reference. It can be constructed from a C string or ``std::string``.

  You can use one of the following typedefs for common character types:

  +------------+-------------------------+
  | Type       | Definition              |
  +============+=========================+
  | StringRef  | BasicStringRef<char>    |
  +------------+-------------------------+
  | WStringRef | BasicStringRef<wchar_t> |
  +------------+-------------------------+

  This class is most useful as a parameter type to allow passing
  different types of strings to a function, for example::

    template <typename... Args>
    std::string format(StringRef format_str, const Args & ... args);

    format("{}", 42);
    format(std::string("{}"), 42);
  \endrst
 */
template <typename Char>
class BasicStringRef {
 private:
  const Char *data_;
  std::size_t size_;

 public:
  /** Constructs a string reference object from a C string and a size. */
  BasicStringRef(const Char *s, std::size_t size) : data_(s), size_(size) {}

  /**
    \rst
    Constructs a string reference object from a C string computing
    the size with ``std::char_traits<Char>::length``.
    \endrst
   */
  BasicStringRef(const Char *s)
    : data_(s), size_(std::char_traits<Char>::length(s)) {}

  /**
    \rst
    Constructs a string reference from an ``std::string`` object.
    \endrst
   */
  BasicStringRef(const std::basic_string<Char> &s)
  : data_(s.c_str()), size_(s.size()) {}

  /**
    \rst
    Converts a string reference to an ``std::string`` object.
    \endrst
   */
  std::basic_string<Char> to_string() const {
    return std::basic_string<Char>(data_, size_);
  }

  /** Returns the pointer to a C string. */
  const Char *data() const { return data_; }

  /** Returns the string size. */
  std::size_t size() const { return size_; }

  // Lexicographically compare this string reference to other.
  int compare(BasicStringRef other) const {
    std::size_t size = std::min(size_, other.size_);
    int result = std::char_traits<Char>::compare(data_, other.data_, size);
    if (result == 0)
      result = size_ == other.size_ ? 0 : (size_ < other.size_ ? -1 : 1);
    return result;
  }

  friend bool operator==(BasicStringRef lhs, BasicStringRef rhs) {
    return lhs.compare(rhs) == 0;
  }
  friend bool operator!=(BasicStringRef lhs, BasicStringRef rhs) {
    return lhs.compare(rhs) != 0;
  }
  friend bool operator<(BasicStringRef lhs, BasicStringRef rhs) {
    return lhs.compare(rhs) < 0;
  }
  friend bool operator<=(BasicStringRef lhs, BasicStringRef rhs) {
    return lhs.compare(rhs) <= 0;
  }
  friend bool operator>(BasicStringRef lhs, BasicStringRef rhs) {
    return lhs.compare(rhs) > 0;
  }
  friend bool operator>=(BasicStringRef lhs, BasicStringRef rhs) {
    return lhs.compare(rhs) >= 0;
  }
};

typedef BasicStringRef<char> StringRef;
typedef BasicStringRef<wchar_t> WStringRef;

/**
  \rst
  A reference to a null terminated string. It can be constructed from a C
  string or ``std::string``.

  You can use one of the following typedefs for common character types:

  +-------------+--------------------------+
  | Type        | Definition               |
  +=============+==========================+
  | CStringRef  | BasicCStringRef<char>    |
  +-------------+--------------------------+
  | WCStringRef | BasicCStringRef<wchar_t> |
  +-------------+--------------------------+

  This class is most useful as a parameter type to allow passing
  different types of strings to a function, for example::

    template <typename... Args>
    std::string format(CStringRef format_str, const Args & ... args);

    format("{}", 42);
    format(std::string("{}"), 42);
  \endrst
 */
template <typename Char>
class BasicCStringRef {
 private:
  const Char *data_;

 public:
  /** Constructs a string reference object from a C string. */
  BasicCStringRef(const Char *s) : data_(s) {}

  /**
    \rst
    Constructs a string reference from an ``std::string`` object.
    \endrst
   */
  BasicCStringRef(const std::basic_string<Char> &s) : data_(s.c_str()) {}

  /** Returns the pointer to a C string. */
  const Char *c_str() const { return data_; }
};

typedef BasicCStringRef<char> CStringRef;
typedef BasicCStringRef<wchar_t> WCStringRef;

/**
  A formatting error such as invalid format string.
*/
class FormatError : public std::runtime_error {
 public:
  explicit FormatError(CStringRef message)
  : std::runtime_error(message.c_str()) {}
};

namespace internal {
// The number of characters to store in the MemoryBuffer object itself
// to avoid dynamic memory allocation.
enum { INLINE_BUFFER_SIZE = 500 };

#if FMT_SECURE_SCL
// Use checked iterator to avoid warnings on MSVC.
template <typename T>
inline stdext::checked_array_iterator<T*> make_ptr(T *ptr, std::size_t size) {
  return stdext::checked_array_iterator<T*>(ptr, size);
}
#else
template <typename T>
inline T *make_ptr(T *ptr, std::size_t) { return ptr; }
#endif
}  // namespace internal

/**
  \rst
  A buffer supporting a subset of ``std::vector``'s operations.
  \endrst
 */
template <typename T>
class Buffer {
 private:
  FMT_DISALLOW_COPY_AND_ASSIGN(Buffer);

 protected:
  T *ptr_;
  std::size_t size_;
  std::size_t capacity_;

  Buffer(T *ptr = 0, std::size_t capacity = 0)
    : ptr_(ptr), size_(0), capacity_(capacity) {}

  /**
    \rst
    Increases the buffer capacity to hold at least *size* elements updating
    ``ptr_`` and ``capacity_``.
    \endrst
   */
  virtual void grow(std::size_t size) = 0;

 public:
  virtual ~Buffer() {}

  /** Returns the size of this buffer. */
  std::size_t size() const { return size_; }

  /** Returns the capacity of this buffer. */
  std::size_t capacity() const { return capacity_; }

  /**
    Resizes the buffer. If T is a POD type new elements may not be initialized.
   */
  void resize(std::size_t new_size) {
    if (new_size > capacity_)
      grow(new_size);
    size_ = new_size;
  }

  /**
    \rst
    Reserves space to store at least *capacity* elements.
    \endrst
   */
  void reserve(std::size_t capacity) {
    if (capacity > capacity_)
      grow(capacity);
  }

  void clear() FMT_NOEXCEPT { size_ = 0; }

  void push_back(const T &value) {
    if (size_ == capacity_)
      grow(size_ + 1);
    ptr_[size_++] = value;
  }

  /** Appends data to the end of the buffer. */
  template <typename U>
  void append(const U *begin, const U *end);

  T &operator[](std::size_t index) { return ptr_[index]; }
  const T &operator[](std::size_t index) const { return ptr_[index]; }
};

template <typename T>
template <typename U>
void Buffer<T>::append(const U *begin, const U *end) {
  assert(begin <= end);
  std::size_t new_size = size_ + (end - begin);
  if (new_size > capacity_)
    grow(new_size);
  std::copy(begin, end, internal::make_ptr(ptr_, capacity_) + size_);
  size_ = new_size;
}

namespace internal {

// A memory buffer for POD types with the first SIZE elements stored in
// the object itself.
template <typename T, std::size_t SIZE, typename Allocator = std::allocator<T> >
class MemoryBuffer : private Allocator, public Buffer<T> {
 private:
  T data_[SIZE];

  // Deallocate memory allocated by the buffer.
  void deallocate() {
    if (this->ptr_ != data_) Allocator::deallocate(this->ptr_, this->capacity_);
  }

 protected:
  void grow(std::size_t size);

 public:
  explicit MemoryBuffer(const Allocator &alloc = Allocator())
      : Allocator(alloc), Buffer<T>(data_, SIZE) {}
  ~MemoryBuffer() { deallocate(); }

#if FMT_USE_RVALUE_REFERENCES
 private:
  // Move data from other to this buffer.
  void move(MemoryBuffer &other) {
    Allocator &this_alloc = *this, &other_alloc = other;
    this_alloc = std::move(other_alloc);
    this->size_ = other.size_;
    this->capacity_ = other.capacity_;
    if (other.ptr_ == other.data_) {
      this->ptr_ = data_;
      std::copy(other.data_,
                other.data_ + this->size_, make_ptr(data_, this->capacity_));
    } else {
      this->ptr_ = other.ptr_;
      // Set pointer to the inline array so that delete is not called
      // when deallocating.
      other.ptr_ = other.data_;
    }
  }

 public:
  MemoryBuffer(MemoryBuffer &&other) {
    move(other);
  }

  MemoryBuffer &operator=(MemoryBuffer &&other) {
    assert(this != &other);
    deallocate();
    move(other);
    return *this;
  }
#endif

  // Returns a copy of the allocator associated with this buffer.
  Allocator get_allocator() const { return *this; }
};

template <typename T, std::size_t SIZE, typename Allocator>
void MemoryBuffer<T, SIZE, Allocator>::grow(std::size_t size) {
  std::size_t new_capacity =
      (std::max)(size, this->capacity_ + this->capacity_ / 2);
  T *new_ptr = this->allocate(new_capacity);
  // The following code doesn't throw, so the raw pointer above doesn't leak.
  std::copy(this->ptr_,
            this->ptr_ + this->size_, make_ptr(new_ptr, new_capacity));
  std::size_t old_capacity = this->capacity_;
  T *old_ptr = this->ptr_;
  this->capacity_ = new_capacity;
  this->ptr_ = new_ptr;
  // deallocate may throw (at least in principle), but it doesn't matter since
  // the buffer already uses the new storage and will deallocate it in case
  // of exception.
  if (old_ptr != data_)
    Allocator::deallocate(old_ptr, old_capacity);
}

// A fixed-size buffer.
template <typename Char>
class FixedBuffer : public fmt::Buffer<Char> {
 public:
  FixedBuffer(Char *array, std::size_t size) : fmt::Buffer<Char>(array, size) {}

 protected:
  void grow(std::size_t size);
};

template <typename Char>
class BasicCharTraits {
 public:
#if FMT_SECURE_SCL
  typedef stdext::checked_array_iterator<Char*> CharPtr;
#else
  typedef Char *CharPtr;
#endif
  static Char cast(wchar_t value) { return static_cast<Char>(value); }
};

template <typename Char>
class CharTraits;

template <>
class CharTraits<char> : public BasicCharTraits<char> {
 private:
  // Conversion from wchar_t to char is not allowed.
  static char convert(wchar_t);

 public:
  static char convert(char value) { return value; }

  // Formats a floating-point number.
  template <typename T>
  static int format_float(char *buffer, std::size_t size,
      const char *format, unsigned width, int precision, T value);
};

template <>
class CharTraits<wchar_t> : public BasicCharTraits<wchar_t> {
 public:
  static wchar_t convert(char value) { return value; }
  static wchar_t convert(wchar_t value) { return value; }

  template <typename T>
  static int format_float(wchar_t *buffer, std::size_t size,
      const wchar_t *format, unsigned width, int precision, T value);
};

// Checks if a number is negative - used to avoid warnings.
template <bool IsSigned>
struct SignChecker {
  template <typename T>
  static bool is_negative(T value) { return value < 0; }
};

template <>
struct SignChecker<false> {
  template <typename T>
  static bool is_negative(T) { return false; }
};

// Returns true if value is negative, false otherwise.
// Same as (value < 0) but doesn't produce warnings if T is an unsigned type.
template <typename T>
inline bool is_negative(T value) {
  return SignChecker<std::numeric_limits<T>::is_signed>::is_negative(value);
}

// Selects uint32_t if FitsIn32Bits is true, uint64_t otherwise.
template <bool FitsIn32Bits>
struct TypeSelector { typedef uint32_t Type; };

template <>
struct TypeSelector<false> { typedef uint64_t Type; };

template <typename T>
struct IntTraits {
  // Smallest of uint32_t and uint64_t that is large enough to represent
  // all values of T.
  typedef typename
    TypeSelector<std::numeric_limits<T>::digits <= 32>::Type MainType;
};

// MakeUnsigned<T>::Type gives an unsigned type corresponding to integer type T.
template <typename T>
struct MakeUnsigned { typedef T Type; };

#define FMT_SPECIALIZE_MAKE_UNSIGNED(T, U) \
  template <> \
  struct MakeUnsigned<T> { typedef U Type; }

FMT_SPECIALIZE_MAKE_UNSIGNED(char, unsigned char);
FMT_SPECIALIZE_MAKE_UNSIGNED(signed char, unsigned char);
FMT_SPECIALIZE_MAKE_UNSIGNED(short, unsigned short);
FMT_SPECIALIZE_MAKE_UNSIGNED(int, unsigned);
FMT_SPECIALIZE_MAKE_UNSIGNED(long, unsigned long);
FMT_SPECIALIZE_MAKE_UNSIGNED(LongLong, ULongLong);

void report_unknown_type(char code, const char *type);

// Static data is placed in this class template to allow header-only
// configuration.
template <typename T = void>
struct BasicData {
  static const uint32_t POWERS_OF_10_32[];
  static const uint64_t POWERS_OF_10_64[];
  static const char DIGITS[];
};

typedef BasicData<> Data;

#if FMT_GCC_VERSION >= 400 || FMT_HAS_BUILTIN(__builtin_clz)
# define FMT_BUILTIN_CLZ(n) __builtin_clz(n)
#endif

#if FMT_GCC_VERSION >= 400 || FMT_HAS_BUILTIN(__builtin_clzll)
# define FMT_BUILTIN_CLZLL(n) __builtin_clzll(n)
#endif

#ifdef FMT_BUILTIN_CLZLL
// Returns the number of decimal digits in n. Leading zeros are not counted
// except for n == 0 in which case count_digits returns 1.
inline unsigned count_digits(uint64_t n) {
  // Based on http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog10
  // and the benchmark https://github.com/localvoid/cxx-benchmark-count-digits.
  unsigned t = (64 - FMT_BUILTIN_CLZLL(n | 1)) * 1233 >> 12;
  return t - (n < Data::POWERS_OF_10_64[t]) + 1;
}
#else
// Fallback version of count_digits used when __builtin_clz is not available.
inline unsigned count_digits(uint64_t n) {
  unsigned count = 1;
  for (;;) {
    // Integer division is slow so do it for a group of four digits instead
    // of for every digit. The idea comes from the talk by Alexandrescu
    // "Three Optimization Tips for C++". See speed-test for a comparison.
    if (n < 10) return count;
    if (n < 100) return count + 1;
    if (n < 1000) return count + 2;
    if (n < 10000) return count + 3;
    n /= 10000u;
    count += 4;
  }
}
#endif

#ifdef FMT_BUILTIN_CLZ
// Optional version of count_digits for better performance on 32-bit platforms.
inline unsigned count_digits(uint32_t n) {
  uint32_t t = (32 - FMT_BUILTIN_CLZ(n | 1)) * 1233 >> 12;
  return t - (n < Data::POWERS_OF_10_32[t]) + 1;
}
#endif

// Formats a decimal unsigned integer value writing into buffer.
template <typename UInt, typename Char>
inline void format_decimal(Char *buffer, UInt value, unsigned num_digits) {
  buffer += num_digits;
  while (value >= 100) {
    // Integer division is slow so do it for a group of two digits instead
    // of for every digit. The idea comes from the talk by Alexandrescu
    // "Three Optimization Tips for C++". See speed-test for a comparison.
    unsigned index = static_cast<unsigned>((value % 100) * 2);
    value /= 100;
    *--buffer = Data::DIGITS[index + 1];
    *--buffer = Data::DIGITS[index];
  }
  if (value < 10) {
    *--buffer = static_cast<char>('0' + value);
    return;
  }
  unsigned index = static_cast<unsigned>(value * 2);
  *--buffer = Data::DIGITS[index + 1];
  *--buffer = Data::DIGITS[index];
}

#ifndef _WIN32
# define FMT_USE_WINDOWS_H 0
#elif !defined(FMT_USE_WINDOWS_H)
# define FMT_USE_WINDOWS_H 1
#endif

// Define FMT_USE_WINDOWS_H to 0 to disable use of windows.h.
// All the functionality that relies on it will be disabled too.
#if FMT_USE_WINDOWS_H
// A converter from UTF-8 to UTF-16.
// It is only provided for Windows since other systems support UTF-8 natively.
class UTF8ToUTF16 {
 private:
  MemoryBuffer<wchar_t, INLINE_BUFFER_SIZE> buffer_;

 public:
  explicit UTF8ToUTF16(StringRef s);
  operator WStringRef() const { return WStringRef(&buffer_[0], size()); }
  size_t size() const { return buffer_.size() - 1; }
  const wchar_t *c_str() const { return &buffer_[0]; }
  std::wstring str() const { return std::wstring(&buffer_[0], size()); }
};

// A converter from UTF-16 to UTF-8.
// It is only provided for Windows since other systems support UTF-8 natively.
class UTF16ToUTF8 {
 private:
  MemoryBuffer<char, INLINE_BUFFER_SIZE> buffer_;

 public:
  UTF16ToUTF8() {}
  explicit UTF16ToUTF8(WStringRef s);
  operator StringRef() const { return StringRef(&buffer_[0], size()); }
  size_t size() const { return buffer_.size() - 1; }
  const char *c_str() const { return &buffer_[0]; }
  std::string str() const { return std::string(&buffer_[0], size()); }

  // Performs conversion returning a system error code instead of
  // throwing exception on conversion error. This method may still throw
  // in case of memory allocation error.
  int convert(WStringRef s);
};

void format_windows_error(fmt::Writer &out, int error_code,
                          fmt::StringRef message) FMT_NOEXCEPT;
#endif

void format_system_error(fmt::Writer &out, int error_code,
                         fmt::StringRef message) FMT_NOEXCEPT;

// A formatting argument value.
struct Value {
  template <typename Char>
  struct StringValue {
    const Char *value;
    std::size_t size;
  };

  typedef void (*FormatFunc)(
      void *formatter, const void *arg, void *format_str_ptr);

  struct CustomValue {
    const void *value;
    FormatFunc format;
  };

  union {
    int int_value;
    unsigned uint_value;
    LongLong long_long_value;
    ULongLong ulong_long_value;
    double double_value;
    long double long_double_value;
    const void *pointer;
    StringValue<char> string;
    StringValue<signed char> sstring;
    StringValue<unsigned char> ustring;
    StringValue<wchar_t> wstring;
    CustomValue custom;
  };

  enum Type {
    NONE, NAMED_ARG,
    // Integer types should go first,
    INT, UINT, LONG_LONG, ULONG_LONG, BOOL, CHAR, LAST_INTEGER_TYPE = CHAR,
    // followed by floating-point types.
    DOUBLE, LONG_DOUBLE, LAST_NUMERIC_TYPE = LONG_DOUBLE,
    CSTRING, STRING, WSTRING, POINTER, CUSTOM
  };
};

// A formatting argument. It is a POD type to allow storage in
// internal::MemoryBuffer.
struct Arg : Value {
  Type type;
};

template <typename Char>
struct NamedArg;

template <typename T = void>
struct Null {};

// A helper class template to enable or disable overloads taking wide
// characters and strings in MakeValue.
template <typename T, typename Char>
struct WCharHelper {
  typedef Null<T> Supported;
  typedef T Unsupported;
};

template <typename T>
struct WCharHelper<T, wchar_t> {
  typedef T Supported;
  typedef Null<T> Unsupported;
};

typedef char Yes[1];
typedef char No[2];

// These are non-members to workaround an overload resolution bug in bcc32.
Yes &convert(fmt::ULongLong);
Yes &convert(std::ostream &);
No &convert(...);

template <typename T>
T &get();

struct DummyStream : std::ostream {
  // Hide all operator<< overloads from std::ostream.
  void operator<<(Null<>);
};

No &operator<<(std::ostream &, int);

template<typename T, bool ENABLE_CONVERSION>
struct ConvertToIntImpl {
  enum { value = false };
};

template<typename T>
struct ConvertToIntImpl<T, true> {
  // Convert to int only if T doesn't have an overloaded operator<<.
  enum {
    value = sizeof(convert(get<DummyStream>() << get<T>())) == sizeof(No)
  };
};

template<typename T, bool ENABLE_CONVERSION>
struct ConvertToIntImpl2 {
  enum { value = false };
};

template<typename T>
struct ConvertToIntImpl2<T, true> {
  enum {
    // Don't convert numeric types.
    value = ConvertToIntImpl<T, !std::numeric_limits<T>::is_specialized>::value
  };
};

template<typename T>
struct ConvertToInt {
  enum { enable_conversion = sizeof(convert(get<T>())) == sizeof(Yes) };
  enum { value = ConvertToIntImpl2<T, enable_conversion>::value };
};

#define FMT_DISABLE_CONVERSION_TO_INT(Type) \
  template <> \
  struct ConvertToInt<Type> {  enum { value = 0 }; }

// Silence warnings about convering float to int.
FMT_DISABLE_CONVERSION_TO_INT(float);
FMT_DISABLE_CONVERSION_TO_INT(double);
FMT_DISABLE_CONVERSION_TO_INT(long double);

template<bool B, class T = void>
struct EnableIf {};

template<class T>
struct EnableIf<true, T> { typedef T type; };

template<bool B, class T, class F>
struct Conditional { typedef T type; };

template<class T, class F>
struct Conditional<false, T, F> { typedef F type; };

// For bcc32 which doesn't understand ! in template arguments.
template<bool>
struct Not { enum { value = 0 }; };

template<>
struct Not<false> { enum { value = 1 }; };

// Makes an Arg object from any type.
template <typename Char>
class MakeValue : public Arg {
 private:
  // The following two methods are private to disallow formatting of
  // arbitrary pointers. If you want to output a pointer cast it to
  // "void *" or "const void *". In particular, this forbids formatting
  // of "[const] volatile char *" which is printed as bool by iostreams.
  // Do not implement!
  template <typename T>
  MakeValue(const T *value);
  template <typename T>
  MakeValue(T *value);

  // The following methods are private to disallow formatting of wide
  // characters and strings into narrow strings as in
  //   fmt::format("{}", L"test");
  // To fix this, use a wide format string: fmt::format(L"{}", L"test").
#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED)
  MakeValue(typename WCharHelper<wchar_t, Char>::Unsupported);
#endif
  MakeValue(typename WCharHelper<wchar_t *, Char>::Unsupported);
  MakeValue(typename WCharHelper<const wchar_t *, Char>::Unsupported);
  MakeValue(typename WCharHelper<const std::wstring &, Char>::Unsupported);
  MakeValue(typename WCharHelper<WStringRef, Char>::Unsupported);

  void set_string(StringRef str) {
    string.value = str.data();
    string.size = str.size();
  }

  void set_string(WStringRef str) {
    wstring.value = str.data();
    wstring.size = str.size();
  }

  // Formats an argument of a custom type, such as a user-defined class.
  template <typename T>
  static void format_custom_arg(
      void *formatter, const void *arg, void *format_str_ptr) {
    format(*static_cast<BasicFormatter<Char>*>(formatter),
           *static_cast<const Char**>(format_str_ptr),
           *static_cast<const T*>(arg));
  }

 public:
  MakeValue() {}

#define FMT_MAKE_VALUE_(Type, field, TYPE, rhs) \
  MakeValue(Type value) { field = rhs; } \
  static uint64_t type(Type) { return Arg::TYPE; }

#define FMT_MAKE_VALUE(Type, field, TYPE) \
  FMT_MAKE_VALUE_(Type, field, TYPE, value)

  FMT_MAKE_VALUE(bool, int_value, BOOL)
  FMT_MAKE_VALUE(short, int_value, INT)
  FMT_MAKE_VALUE(unsigned short, uint_value, UINT)
  FMT_MAKE_VALUE(int, int_value, INT)
  FMT_MAKE_VALUE(unsigned, uint_value, UINT)

  MakeValue(long value) {
    // To minimize the number of types we need to deal with, long is
    // translated either to int or to long long depending on its size.
    if (check(sizeof(long) == sizeof(int)))
      int_value = static_cast<int>(value);
    else
      long_long_value = value;
  }
  static uint64_t type(long) {
    return sizeof(long) == sizeof(int) ? Arg::INT : Arg::LONG_LONG;
  }

  MakeValue(unsigned long value) {
    if (check(sizeof(unsigned long) == sizeof(unsigned)))
      uint_value = static_cast<unsigned>(value);
    else
      ulong_long_value = value;
  }
  static uint64_t type(unsigned long) {
    return sizeof(unsigned long) == sizeof(unsigned) ?
          Arg::UINT : Arg::ULONG_LONG;
  }

  FMT_MAKE_VALUE(LongLong, long_long_value, LONG_LONG)
  FMT_MAKE_VALUE(ULongLong, ulong_long_value, ULONG_LONG)
  FMT_MAKE_VALUE(float, double_value, DOUBLE)
  FMT_MAKE_VALUE(double, double_value, DOUBLE)
  FMT_MAKE_VALUE(long double, long_double_value, LONG_DOUBLE)
  FMT_MAKE_VALUE(signed char, int_value, INT)
  FMT_MAKE_VALUE(unsigned char, uint_value, UINT)
  FMT_MAKE_VALUE(char, int_value, CHAR)

#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED)
  MakeValue(typename WCharHelper<wchar_t, Char>::Supported value) {
    int_value = value;
  }
  static uint64_t type(wchar_t) { return Arg::CHAR; }
#endif

#define FMT_MAKE_STR_VALUE(Type, TYPE) \
  MakeValue(Type value) { set_string(value); } \
  static uint64_t type(Type) { return Arg::TYPE; }

  FMT_MAKE_VALUE(char *, string.value, CSTRING)
  FMT_MAKE_VALUE(const char *, string.value, CSTRING)
  FMT_MAKE_VALUE(const signed char *, sstring.value, CSTRING)
  FMT_MAKE_VALUE(const unsigned char *, ustring.value, CSTRING)
  FMT_MAKE_STR_VALUE(const std::string &, STRING)
  FMT_MAKE_STR_VALUE(StringRef, STRING)
  FMT_MAKE_VALUE_(CStringRef, string.value, CSTRING, value.c_str())

#define FMT_MAKE_WSTR_VALUE(Type, TYPE) \
  MakeValue(typename WCharHelper<Type, Char>::Supported value) { \
    set_string(value); \
  } \
  static uint64_t type(Type) { return Arg::TYPE; }

  FMT_MAKE_WSTR_VALUE(wchar_t *, WSTRING)
  FMT_MAKE_WSTR_VALUE(const wchar_t *, WSTRING)
  FMT_MAKE_WSTR_VALUE(const std::wstring &, WSTRING)
  FMT_MAKE_WSTR_VALUE(WStringRef, WSTRING)

  FMT_MAKE_VALUE(void *, pointer, POINTER)
  FMT_MAKE_VALUE(const void *, pointer, POINTER)

  template <typename T>
  MakeValue(const T &value,
            typename EnableIf<Not<
              ConvertToInt<T>::value>::value, int>::type = 0) {
    custom.value = &value;
    custom.format = &format_custom_arg<T>;
  }

  template <typename T>
  MakeValue(const T &value,
            typename EnableIf<ConvertToInt<T>::value, int>::type = 0) {
    int_value = value;
  }

  template <typename T>
  static uint64_t type(const T &) {
    return ConvertToInt<T>::value ? Arg::INT : Arg::CUSTOM;
  }

  // Additional template param `Char_` is needed here because make_type always
  // uses MakeValue<char>.
  template <typename Char_>
  MakeValue(const NamedArg<Char_> &value) { pointer = &value; }

  template <typename Char_>
  static uint64_t type(const NamedArg<Char_> &) { return Arg::NAMED_ARG; }
};

template <typename Char>
struct NamedArg : Arg {
  BasicStringRef<Char> name;

  template <typename T>
  NamedArg(BasicStringRef<Char> argname, const T &value)
  : Arg(MakeValue<Char>(value)), name(argname) {
    type = static_cast<internal::Arg::Type>(MakeValue<Char>::type(value));
  }
};

#define FMT_DISPATCH(call) static_cast<Impl*>(this)->call

// An argument visitor.
// To use ArgVisitor define a subclass that implements some or all of the
// visit methods with the same signatures as the methods in ArgVisitor,
// for example, visit_int(int).
// Specify the subclass name as the Impl template parameter. Then calling
// ArgVisitor::visit for some argument will dispatch to a visit method
// specific to the argument type. For example, if the argument type is
// double then visit_double(double) method of a subclass will be called.
// If the subclass doesn't contain a method with this signature, then
// a corresponding method of ArgVisitor will be called.
//
// Example:
//  class MyArgVisitor : public ArgVisitor<MyArgVisitor, void> {
//   public:
//    void visit_int(int value) { print("{}", value); }
//    void visit_double(double value) { print("{}", value ); }
//  };
//
// ArgVisitor uses the curiously recurring template pattern:
// http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
template <typename Impl, typename Result>
class ArgVisitor {
 public:
  void report_unhandled_arg() {}

  Result visit_unhandled_arg() {
    FMT_DISPATCH(report_unhandled_arg());
    return Result();
  }

  Result visit_int(int value) {
    return FMT_DISPATCH(visit_any_int(value));
  }
  Result visit_long_long(LongLong value) {
    return FMT_DISPATCH(visit_any_int(value));
  }
  Result visit_uint(unsigned value) {
    return FMT_DISPATCH(visit_any_int(value));
  }
  Result visit_ulong_long(ULongLong value) {
    return FMT_DISPATCH(visit_any_int(value));
  }
  Result visit_bool(bool value) {
    return FMT_DISPATCH(visit_any_int(value));
  }
  Result visit_char(int value) {
    return FMT_DISPATCH(visit_any_int(value));
  }
  template <typename T>
  Result visit_any_int(T) {
    return FMT_DISPATCH(visit_unhandled_arg());
  }

  Result visit_double(double value) {
    return FMT_DISPATCH(visit_any_double(value));
  }
  Result visit_long_double(long double value) {
    return FMT_DISPATCH(visit_any_double(value));
  }
  template <typename T>
  Result visit_any_double(T) {
    return FMT_DISPATCH(visit_unhandled_arg());
  }

  Result visit_cstring(const char *) {
    return FMT_DISPATCH(visit_unhandled_arg());
  }
  Result visit_string(Arg::StringValue<char>) {
    return FMT_DISPATCH(visit_unhandled_arg());
  }
  Result visit_wstring(Arg::StringValue<wchar_t>) {
    return FMT_DISPATCH(visit_unhandled_arg());
  }
  Result visit_pointer(const void *) {
    return FMT_DISPATCH(visit_unhandled_arg());
  }
  Result visit_custom(Arg::CustomValue) {
    return FMT_DISPATCH(visit_unhandled_arg());
  }

  Result visit(const Arg &arg) {
    switch (arg.type) {
    default:
      FMT_ASSERT(false, "invalid argument type");
      return Result();
    case Arg::INT:
      return FMT_DISPATCH(visit_int(arg.int_value));
    case Arg::UINT:
      return FMT_DISPATCH(visit_uint(arg.uint_value));
    case Arg::LONG_LONG:
      return FMT_DISPATCH(visit_long_long(arg.long_long_value));
    case Arg::ULONG_LONG:
      return FMT_DISPATCH(visit_ulong_long(arg.ulong_long_value));
    case Arg::BOOL:
      return FMT_DISPATCH(visit_bool(arg.int_value != 0));
    case Arg::CHAR:
      return FMT_DISPATCH(visit_char(arg.int_value));
    case Arg::DOUBLE:
      return FMT_DISPATCH(visit_double(arg.double_value));
    case Arg::LONG_DOUBLE:
      return FMT_DISPATCH(visit_long_double(arg.long_double_value));
    case Arg::CSTRING:
      return FMT_DISPATCH(visit_cstring(arg.string.value));
    case Arg::STRING:
      return FMT_DISPATCH(visit_string(arg.string));
    case Arg::WSTRING:
      return FMT_DISPATCH(visit_wstring(arg.wstring));
    case Arg::POINTER:
      return FMT_DISPATCH(visit_pointer(arg.pointer));
    case Arg::CUSTOM:
      return FMT_DISPATCH(visit_custom(arg.custom));
    }
  }
};

class RuntimeError : public std::runtime_error {
 protected:
  RuntimeError() : std::runtime_error("") {}
};

template <typename Impl, typename Char>
class BasicArgFormatter;

template <typename Char>
class PrintfArgFormatter;

template <typename Char>
class ArgMap;
}  // namespace internal

/** An argument list. */
class ArgList {
 private:
  // To reduce compiled code size per formatting function call, types of first
  // MAX_PACKED_ARGS arguments are passed in the types_ field.
  uint64_t types_;
  union {
    // If the number of arguments is less than MAX_PACKED_ARGS, the argument
    // values are stored in values_, otherwise they are stored in args_.
    // This is done to reduce compiled code size as storing larger objects
    // may require more code (at least on x86-64) even if the same amount of
    // data is actually copied to stack. It saves ~10% on the bloat test.
    const internal::Value *values_;
    const internal::Arg *args_;
  };

  internal::Arg::Type type(unsigned index) const {
    unsigned shift = index * 4;
    uint64_t mask = 0xf;
    return static_cast<internal::Arg::Type>(
          (types_ & (mask << shift)) >> shift);
  }

  template <typename Char>
  friend class internal::ArgMap;

 public:
  // Maximum number of arguments with packed types.
  enum { MAX_PACKED_ARGS = 16 };

  ArgList() : types_(0) {}

  ArgList(ULongLong types, const internal::Value *values)
  : types_(types), values_(values) {}
  ArgList(ULongLong types, const internal::Arg *args)
  : types_(types), args_(args) {}

  /** Returns the argument at specified index. */
  internal::Arg operator[](unsigned index) const {
    using internal::Arg;
    Arg arg;
    bool use_values = type(MAX_PACKED_ARGS - 1) == Arg::NONE;
    if (index < MAX_PACKED_ARGS) {
      Arg::Type arg_type = type(index);
      internal::Value &val = arg;
      if (arg_type != Arg::NONE)
        val = use_values ? values_[index] : args_[index];
      arg.type = arg_type;
      return arg;
    }
    if (use_values) {
      // The index is greater than the number of arguments that can be stored
      // in values, so return a "none" argument.
      arg.type = Arg::NONE;
      return arg;
    }
    for (unsigned i = MAX_PACKED_ARGS; i <= index; ++i) {
      if (args_[i].type == Arg::NONE)
        return args_[i];
    }
    return args_[index];
  }
};

struct FormatSpec;

namespace internal {

template <typename Char>
class ArgMap {
 private:
  typedef std::map<fmt::BasicStringRef<Char>, internal::Arg> MapType;
  typedef typename MapType::value_type Pair;

  MapType map_;

 public:
  void init(const ArgList &args);

  const internal::Arg* find(const fmt::BasicStringRef<Char> &name) const {
    typename MapType::const_iterator it = map_.find(name);
    return it != map_.end() ? &it->second : 0;
  }
};

class FormatterBase {
 private:
  ArgList args_;
  int next_arg_index_;

  // Returns the argument with specified index.
  Arg do_get_arg(unsigned arg_index, const char *&error);

 protected:
  const ArgList &args() const { return args_; }

  explicit FormatterBase(const ArgList &args) {
    args_ = args;
    next_arg_index_ = 0;
  }

  // Returns the next argument.
  Arg next_arg(const char *&error);

  // Checks if manual indexing is used and returns the argument with
  // specified index.
  Arg get_arg(unsigned arg_index, const char *&error);

  bool check_no_auto_index(const char *&error);

  template <typename Char>
  void write(BasicWriter<Char> &w, const Char *start, const Char *end) {
    if (start != end)
      w << BasicStringRef<Char>(start, end - start);
  }
};

// A printf formatter.
template <typename Char>
class PrintfFormatter : private FormatterBase {
 private:
  void parse_flags(FormatSpec &spec, const Char *&s);

  // Returns the argument with specified index or, if arg_index is equal
  // to the maximum unsigned value, the next argument.
  Arg get_arg(const Char *s,
      unsigned arg_index = (std::numeric_limits<unsigned>::max)());

  // Parses argument index, flags and width and returns the argument index.
  unsigned parse_header(const Char *&s, FormatSpec &spec);

 public:
  explicit PrintfFormatter(const ArgList &args) : FormatterBase(args) {}
  void format(BasicWriter<Char> &writer, BasicCStringRef<Char> format_str);
};
}  // namespace internal

// A formatter.
template <typename Char>
class BasicFormatter : private internal::FormatterBase {
 private:
  BasicWriter<Char> &writer_;
  internal::ArgMap<Char> map_;

  FMT_DISALLOW_COPY_AND_ASSIGN(BasicFormatter);

  using internal::FormatterBase::get_arg;

  // Checks if manual indexing is used and returns the argument with
  // specified name.
  internal::Arg get_arg(BasicStringRef<Char> arg_name, const char *&error);

  // Parses argument index and returns corresponding argument.
  internal::Arg parse_arg_index(const Char *&s);

  // Parses argument name and returns corresponding argument.
  internal::Arg parse_arg_name(const Char *&s);

 public:
  BasicFormatter(const ArgList &args, BasicWriter<Char> &w)
    : internal::FormatterBase(args), writer_(w) {}

  BasicWriter<Char> &writer() { return writer_; }

  void format(BasicCStringRef<Char> format_str);

  const Char *format(const Char *&format_str, const internal::Arg &arg);
};

enum Alignment {
  ALIGN_DEFAULT, ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER, ALIGN_NUMERIC
};

// Flags.
enum {
  SIGN_FLAG = 1, PLUS_FLAG = 2, MINUS_FLAG = 4, HASH_FLAG = 8,
  CHAR_FLAG = 0x10  // Argument has char type - used in error reporting.
};

// An empty format specifier.
struct EmptySpec {};

// A type specifier.
template <char TYPE>
struct TypeSpec : EmptySpec {
  Alignment align() const { return ALIGN_DEFAULT; }
  unsigned width() const { return 0; }
  int precision() const { return -1; }
  bool flag(unsigned) const { return false; }
  char type() const { return TYPE; }
  char fill() const { return ' '; }
};

// A width specifier.
struct WidthSpec {
  unsigned width_;
  // Fill is always wchar_t and cast to char if necessary to avoid having
  // two specialization of WidthSpec and its subclasses.
  wchar_t fill_;

  WidthSpec(unsigned width, wchar_t fill) : width_(width), fill_(fill) {}

  unsigned width() const { return width_; }
  wchar_t fill() const { return fill_; }
};

// An alignment specifier.
struct AlignSpec : WidthSpec {
  Alignment align_;

  AlignSpec(unsigned width, wchar_t fill, Alignment align = ALIGN_DEFAULT)
  : WidthSpec(width, fill), align_(align) {}

  Alignment align() const { return align_; }

  int precision() const { return -1; }
};

// An alignment and type specifier.
template <char TYPE>
struct AlignTypeSpec : AlignSpec {
  AlignTypeSpec(unsigned width, wchar_t fill) : AlignSpec(width, fill) {}

  bool flag(unsigned) const { return false; }
  char type() const { return TYPE; }
};

// A full format specifier.
struct FormatSpec : AlignSpec {
  unsigned flags_;
  int precision_;
  char type_;

  FormatSpec(
    unsigned width = 0, char type = 0, wchar_t fill = ' ')
  : AlignSpec(width, fill), flags_(0), precision_(-1), type_(type) {}

  bool flag(unsigned f) const { return (flags_ & f) != 0; }
  int precision() const { return precision_; }
  char type() const { return type_; }
};

// An integer format specifier.
template <typename T, typename SpecT = TypeSpec<0>, typename Char = char>
class IntFormatSpec : public SpecT {
 private:
  T value_;

 public:
  IntFormatSpec(T val, const SpecT &spec = SpecT())
  : SpecT(spec), value_(val) {}

  T value() const { return value_; }
};

// A string format specifier.
template <typename Char>
class StrFormatSpec : public AlignSpec {
 private:
  const Char *str_;

 public:
  template <typename FillChar>
  StrFormatSpec(const Char *str, unsigned width, FillChar fill)
  : AlignSpec(width, fill), str_(str) {
    internal::CharTraits<Char>::convert(FillChar());
  }

  const Char *str() const { return str_; }
};

/**
  Returns an integer format specifier to format the value in base 2.
 */
IntFormatSpec<int, TypeSpec<'b'> > bin(int value);

/**
  Returns an integer format specifier to format the value in base 8.
 */
IntFormatSpec<int, TypeSpec<'o'> > oct(int value);

/**
  Returns an integer format specifier to format the value in base 16 using
  lower-case letters for the digits above 9.
 */
IntFormatSpec<int, TypeSpec<'x'> > hex(int value);

/**
  Returns an integer formatter format specifier to format in base 16 using
  upper-case letters for the digits above 9.
 */
IntFormatSpec<int, TypeSpec<'X'> > hexu(int value);

/**
  \rst
  Returns an integer format specifier to pad the formatted argument with the
  fill character to the specified width using the default (right) numeric
  alignment.

  **Example**::

    MemoryWriter out;
    out << pad(hex(0xcafe), 8, '0');
    // out.str() == "0000cafe"

  \endrst
 */
template <char TYPE_CODE, typename Char>
IntFormatSpec<int, AlignTypeSpec<TYPE_CODE>, Char> pad(
    int value, unsigned width, Char fill = ' ');

#define FMT_DEFINE_INT_FORMATTERS(TYPE) \
inline IntFormatSpec<TYPE, TypeSpec<'b'> > bin(TYPE value) { \
  return IntFormatSpec<TYPE, TypeSpec<'b'> >(value, TypeSpec<'b'>()); \
} \
 \
inline IntFormatSpec<TYPE, TypeSpec<'o'> > oct(TYPE value) { \
  return IntFormatSpec<TYPE, TypeSpec<'o'> >(value, TypeSpec<'o'>()); \
} \
 \
inline IntFormatSpec<TYPE, TypeSpec<'x'> > hex(TYPE value) { \
  return IntFormatSpec<TYPE, TypeSpec<'x'> >(value, TypeSpec<'x'>()); \
} \
 \
inline IntFormatSpec<TYPE, TypeSpec<'X'> > hexu(TYPE value) { \
  return IntFormatSpec<TYPE, TypeSpec<'X'> >(value, TypeSpec<'X'>()); \
} \
 \
template <char TYPE_CODE> \
inline IntFormatSpec<TYPE, AlignTypeSpec<TYPE_CODE> > pad( \
    IntFormatSpec<TYPE, TypeSpec<TYPE_CODE> > f, unsigned width) { \
  return IntFormatSpec<TYPE, AlignTypeSpec<TYPE_CODE> >( \
      f.value(), AlignTypeSpec<TYPE_CODE>(width, ' ')); \
} \
 \
/* For compatibility with older compilers we provide two overloads for pad, */ \
/* one that takes a fill character and one that doesn't. In the future this */ \
/* can be replaced with one overload making the template argument Char      */ \
/* default to char (C++11). */ \
template <char TYPE_CODE, typename Char> \
inline IntFormatSpec<TYPE, AlignTypeSpec<TYPE_CODE>, Char> pad( \
    IntFormatSpec<TYPE, TypeSpec<TYPE_CODE>, Char> f, \
    unsigned width, Char fill) { \
  return IntFormatSpec<TYPE, AlignTypeSpec<TYPE_CODE>, Char>( \
      f.value(), AlignTypeSpec<TYPE_CODE>(width, fill)); \
} \
 \
inline IntFormatSpec<TYPE, AlignTypeSpec<0> > pad( \
    TYPE value, unsigned width) { \
  return IntFormatSpec<TYPE, AlignTypeSpec<0> >( \
      value, AlignTypeSpec<0>(width, ' ')); \
} \
 \
template <typename Char> \
inline IntFormatSpec<TYPE, AlignTypeSpec<0>, Char> pad( \
   TYPE value, unsigned width, Char fill) { \
 return IntFormatSpec<TYPE, AlignTypeSpec<0>, Char>( \
     value, AlignTypeSpec<0>(width, fill)); \
}

FMT_DEFINE_INT_FORMATTERS(int)
FMT_DEFINE_INT_FORMATTERS(long)
FMT_DEFINE_INT_FORMATTERS(unsigned)
FMT_DEFINE_INT_FORMATTERS(unsigned long)
FMT_DEFINE_INT_FORMATTERS(LongLong)
FMT_DEFINE_INT_FORMATTERS(ULongLong)

/**
  \rst
  Returns a string formatter that pads the formatted argument with the fill
  character to the specified width using the default (left) string alignment.

  **Example**::

    std::string s = str(MemoryWriter() << pad("abc", 8));
    // s == "abc     "

  \endrst
 */
template <typename Char>
inline StrFormatSpec<Char> pad(
    const Char *str, unsigned width, Char fill = ' ') {
  return StrFormatSpec<Char>(str, width, fill);
}

inline StrFormatSpec<wchar_t> pad(
    const wchar_t *str, unsigned width, char fill = ' ') {
  return StrFormatSpec<wchar_t>(str, width, fill);
}

// Generates a comma-separated list with results of applying f to
// numbers 0..n-1.
# define FMT_GEN(n, f) FMT_GEN##n(f)
# define FMT_GEN1(f)  f(0)
# define FMT_GEN2(f)  FMT_GEN1(f),  f(1)
# define FMT_GEN3(f)  FMT_GEN2(f),  f(2)
# define FMT_GEN4(f)  FMT_GEN3(f),  f(3)
# define FMT_GEN5(f)  FMT_GEN4(f),  f(4)
# define FMT_GEN6(f)  FMT_GEN5(f),  f(5)
# define FMT_GEN7(f)  FMT_GEN6(f),  f(6)
# define FMT_GEN8(f)  FMT_GEN7(f),  f(7)
# define FMT_GEN9(f)  FMT_GEN8(f),  f(8)
# define FMT_GEN10(f) FMT_GEN9(f),  f(9)
# define FMT_GEN11(f) FMT_GEN10(f), f(10)
# define FMT_GEN12(f) FMT_GEN11(f), f(11)
# define FMT_GEN13(f) FMT_GEN12(f), f(12)
# define FMT_GEN14(f) FMT_GEN13(f), f(13)
# define FMT_GEN15(f) FMT_GEN14(f), f(14)

namespace internal {
inline uint64_t make_type() { return 0; }

template <typename T>
inline uint64_t make_type(const T &arg) { return MakeValue<char>::type(arg); }

template <unsigned N>
struct ArgArray {
  // Computes the argument array size by adding 1 to N, which is the number of
  // arguments, if N is zero, because array of zero size is invalid, or if N
  // is greater than ArgList::MAX_PACKED_ARGS to accommodate for an extra
  // argument that marks the end of the list.
  enum { SIZE = N + (N == 0 || N >= ArgList::MAX_PACKED_ARGS ? 1 : 0) };

  typedef typename Conditional<
    (N < ArgList::MAX_PACKED_ARGS), Value, Arg>::type Type[SIZE];
};

#if FMT_USE_VARIADIC_TEMPLATES
template <typename Arg, typename... Args>
inline uint64_t make_type(const Arg &first, const Args & ... tail) {
  return make_type(first) | (make_type(tail...) << 4);
}

inline void do_set_types(Arg *) {}

template <typename T, typename... Args>
inline void do_set_types(Arg *args, const T &arg, const Args & ... tail) {
  args->type = static_cast<Arg::Type>(MakeValue<T>::type(arg));
  do_set_types(args + 1, tail...);
}

template <typename... Args>
inline void set_types(Arg *array, const Args & ... args) {
  if (check(sizeof...(Args) > ArgList::MAX_PACKED_ARGS))
    do_set_types(array, args...);
  array[sizeof...(Args)].type = Arg::NONE;
}

template <typename... Args>
inline void set_types(Value *, const Args & ...) {
  // Do nothing as types are passed separately from values.
}

template <typename Char, typename Value>
inline void store_args(Value *) {}

template <typename Char, typename Arg, typename T, typename... Args>
inline void store_args(Arg *args, const T &arg, const Args & ... tail) {
  // Assign only the Value subobject of Arg and don't overwrite type (if any)
  // that is assigned by set_types.
  Value &value = *args;
  value = MakeValue<Char>(arg);
  store_args<Char>(args + 1, tail...);
}

template <typename Char, typename... Args>
ArgList make_arg_list(typename ArgArray<sizeof...(Args)>::Type array,
                      const Args & ... args) {
  if (check(sizeof...(Args) >= ArgList::MAX_PACKED_ARGS))
    set_types(array, args...);
  store_args<Char>(array, args...);
  return ArgList(make_type(args...), array);
}
#else

struct ArgType {
  uint64_t type;

  ArgType() : type(0) {}

  template <typename T>
  ArgType(const T &arg) : type(make_type(arg)) {}
};

# define FMT_ARG_TYPE_DEFAULT(n) ArgType t##n = ArgType()

inline uint64_t make_type(FMT_GEN15(FMT_ARG_TYPE_DEFAULT)) {
  return t0.type | (t1.type << 4) | (t2.type << 8) | (t3.type << 12) |
      (t4.type << 16) | (t5.type << 20) | (t6.type << 24) | (t7.type << 28) |
      (t8.type << 32) | (t9.type << 36) | (t10.type << 40) | (t11.type << 44) |
      (t12.type << 48) | (t13.type << 52) | (t14.type << 56);
}
#endif

template <class Char>
class FormatBuf : public std::basic_streambuf<Char> {
 private:
  typedef typename std::basic_streambuf<Char>::int_type int_type;
  typedef typename std::basic_streambuf<Char>::traits_type traits_type;

  Buffer<Char> &buffer_;
  Char *start_;

 public:
  FormatBuf(Buffer<Char> &buffer) : buffer_(buffer), start_(&buffer[0]) {
    this->setp(start_, start_ + buffer_.capacity());
  }

  int_type overflow(int_type ch = traits_type::eof()) {
    if (!traits_type::eq_int_type(ch, traits_type::eof())) {
      size_t size = this->pptr() - start_;
      buffer_.resize(size);
      buffer_.reserve(size * 2);

      start_ = &buffer_[0];
      start_[size] = traits_type::to_char_type(ch);
      this->setp(start_+ size + 1, start_ + size * 2);
    }
    return ch;
  }

  size_t size() const {
    return this->pptr() - start_;
  }
};
}  // namespace internal

# define FMT_MAKE_TEMPLATE_ARG(n) typename T##n
# define FMT_MAKE_ARG_TYPE(n) T##n
# define FMT_MAKE_ARG(n) const T##n &v##n
# define FMT_ASSIGN_char(n) arr[n] = fmt::internal::MakeValue<char>(v##n)
# define FMT_ASSIGN_wchar_t(n) arr[n] = fmt::internal::MakeValue<wchar_t>(v##n)

#if FMT_USE_VARIADIC_TEMPLATES
// Defines a variadic function returning void.
# define FMT_VARIADIC_VOID(func, arg_type) \
  template <typename... Args> \
  void func(arg_type arg0, const Args & ... args) { \
    typename fmt::internal::ArgArray<sizeof...(Args)>::Type array; \
    func(arg0, fmt::internal::make_arg_list<Char>(array, args...)); \
  }

// Defines a variadic constructor.
# define FMT_VARIADIC_CTOR(ctor, func, arg0_type, arg1_type) \
  template <typename... Args> \
  ctor(arg0_type arg0, arg1_type arg1, const Args & ... args) { \
    typename fmt::internal::ArgArray<sizeof...(Args)>::Type array; \
    func(arg0, arg1, fmt::internal::make_arg_list<Char>(array, args...)); \
  }

#else

# define FMT_MAKE_REF(n) fmt::internal::MakeValue<Char>(v##n)
# define FMT_MAKE_REF2(n) v##n

// Defines a wrapper for a function taking one argument of type arg_type
// and n additional arguments of arbitrary types.
# define FMT_WRAP1(func, arg_type, n) \
  template <FMT_GEN(n, FMT_MAKE_TEMPLATE_ARG)> \
  inline void func(arg_type arg1, FMT_GEN(n, FMT_MAKE_ARG)) { \
    const fmt::internal::ArgArray<n>::Type array = {FMT_GEN(n, FMT_MAKE_REF)}; \
    func(arg1, fmt::ArgList( \
      fmt::internal::make_type(FMT_GEN(n, FMT_MAKE_REF2)), array)); \
  }

// Emulates a variadic function returning void on a pre-C++11 compiler.
# define FMT_VARIADIC_VOID(func, arg_type) \
  inline void func(arg_type arg) { func(arg, fmt::ArgList()); } \
  FMT_WRAP1(func, arg_type, 1) FMT_WRAP1(func, arg_type, 2) \
  FMT_WRAP1(func, arg_type, 3) FMT_WRAP1(func, arg_type, 4) \
  FMT_WRAP1(func, arg_type, 5) FMT_WRAP1(func, arg_type, 6) \
  FMT_WRAP1(func, arg_type, 7) FMT_WRAP1(func, arg_type, 8) \
  FMT_WRAP1(func, arg_type, 9) FMT_WRAP1(func, arg_type, 10)

# define FMT_CTOR(ctor, func, arg0_type, arg1_type, n) \
  template <FMT_GEN(n, FMT_MAKE_TEMPLATE_ARG)> \
  ctor(arg0_type arg0, arg1_type arg1, FMT_GEN(n, FMT_MAKE_ARG)) { \
    const fmt::internal::ArgArray<n>::Type array = {FMT_GEN(n, FMT_MAKE_REF)}; \
    func(arg0, arg1, fmt::ArgList( \
      fmt::internal::make_type(FMT_GEN(n, FMT_MAKE_REF2)), array)); \
  }

// Emulates a variadic constructor on a pre-C++11 compiler.
# define FMT_VARIADIC_CTOR(ctor, func, arg0_type, arg1_type) \
  FMT_CTOR(ctor, func, arg0_type, arg1_type, 1) \
  FMT_CTOR(ctor, func, arg0_type, arg1_type, 2) \
  FMT_CTOR(ctor, func, arg0_type, arg1_type, 3) \
  FMT_CTOR(ctor, func, arg0_type, arg1_type, 4) \
  FMT_CTOR(ctor, func, arg0_type, arg1_type, 5) \
  FMT_CTOR(ctor, func, arg0_type, arg1_type, 6) \
  FMT_CTOR(ctor, func, arg0_type, arg1_type, 7) \
  FMT_CTOR(ctor, func, arg0_type, arg1_type, 8) \
  FMT_CTOR(ctor, func, arg0_type, arg1_type, 9) \
  FMT_CTOR(ctor, func, arg0_type, arg1_type, 10)
#endif

// Generates a comma-separated list with results of applying f to pairs
// (argument, index).
#define FMT_FOR_EACH1(f, x0) f(x0, 0)
#define FMT_FOR_EACH2(f, x0, x1) \
  FMT_FOR_EACH1(f, x0), f(x1, 1)
#define FMT_FOR_EACH3(f, x0, x1, x2) \
  FMT_FOR_EACH2(f, x0 ,x1), f(x2, 2)
#define FMT_FOR_EACH4(f, x0, x1, x2, x3) \
  FMT_FOR_EACH3(f, x0, x1, x2), f(x3, 3)
#define FMT_FOR_EACH5(f, x0, x1, x2, x3, x4) \
  FMT_FOR_EACH4(f, x0, x1, x2, x3), f(x4, 4)
#define FMT_FOR_EACH6(f, x0, x1, x2, x3, x4, x5) \
  FMT_FOR_EACH5(f, x0, x1, x2, x3, x4), f(x5, 5)
#define FMT_FOR_EACH7(f, x0, x1, x2, x3, x4, x5, x6) \
  FMT_FOR_EACH6(f, x0, x1, x2, x3, x4, x5), f(x6, 6)
#define FMT_FOR_EACH8(f, x0, x1, x2, x3, x4, x5, x6, x7) \
  FMT_FOR_EACH7(f, x0, x1, x2, x3, x4, x5, x6), f(x7, 7)
#define FMT_FOR_EACH9(f, x0, x1, x2, x3, x4, x5, x6, x7, x8) \
  FMT_FOR_EACH8(f, x0, x1, x2, x3, x4, x5, x6, x7), f(x8, 8)
#define FMT_FOR_EACH10(f, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9) \
  FMT_FOR_EACH9(f, x0, x1, x2, x3, x4, x5, x6, x7, x8), f(x9, 9)

/**
 An error returned by an operating system or a language runtime,
 for example a file opening error.
*/
class SystemError : public internal::RuntimeError {
 private:
  void init(int err_code, CStringRef format_str, ArgList args);

 protected:
  int error_code_;

  typedef char Char;  // For FMT_VARIADIC_CTOR.

  SystemError() {}

 public:
  /**
   \rst
   Constructs a :class:`fmt::SystemError` object with the description
   of the form

   .. parsed-literal::
     *<message>*: *<system-message>*

   where *<message>* is the formatted message and *<system-message>* is
   the system message corresponding to the error code.
   *error_code* is a system error code as given by ``errno``.
   If *error_code* is not a valid error code such as -1, the system message
   may look like "Unknown error -1" and is platform-dependent.

   **Example**::

     // This throws a SystemError with the description
     //   cannot open file 'madeup': No such file or directory
     // or similar (system message may vary).
     const char *filename = "madeup";
     std::FILE *file = std::fopen(filename, "r");
     if (!file)
       throw fmt::SystemError(errno, "cannot open file '{}'", filename);
   \endrst
  */
  SystemError(int error_code, CStringRef message) {
    init(error_code, message, ArgList());
  }
  FMT_VARIADIC_CTOR(SystemError, init, int, CStringRef)

  int error_code() const { return error_code_; }
};

/**
  \rst
  This template provides operations for formatting and writing data into
  a character stream. The output is stored in a buffer provided by a subclass
  such as :class:`fmt::BasicMemoryWriter`.

  You can use one of the following typedefs for common character types:

  +---------+----------------------+
  | Type    | Definition           |
  +=========+======================+
  | Writer  | BasicWriter<char>    |
  +---------+----------------------+
  | WWriter | BasicWriter<wchar_t> |
  +---------+----------------------+

  \endrst
 */
template <typename Char>
class BasicWriter {
 private:
  // Output buffer.
  Buffer<Char> &buffer_;

  FMT_DISALLOW_COPY_AND_ASSIGN(BasicWriter);

  typedef typename internal::CharTraits<Char>::CharPtr CharPtr;

#if FMT_SECURE_SCL
  // Returns pointer value.
  static Char *get(CharPtr p) { return p.base(); }
#else
  static Char *get(Char *p) { return p; }
#endif

  // Fills the padding around the content and returns the pointer to the
  // content area.
  static CharPtr fill_padding(CharPtr buffer,
      unsigned total_size, std::size_t content_size, wchar_t fill);

  // Grows the buffer by n characters and returns a pointer to the newly
  // allocated area.
  CharPtr grow_buffer(std::size_t n) {
    std::size_t size = buffer_.size();
    buffer_.resize(size + n);
    return internal::make_ptr(&buffer_[size], n);
  }

  // Writes an unsigned decimal integer.
  template <typename UInt>
  Char *write_unsigned_decimal(UInt value, unsigned prefix_size = 0) {
    unsigned num_digits = internal::count_digits(value);
    Char *ptr = get(grow_buffer(prefix_size + num_digits));
    internal::format_decimal(ptr + prefix_size, value, num_digits);
    return ptr;
  }

  // Writes a decimal integer.
  template <typename Int>
  void write_decimal(Int value) {
    typename internal::IntTraits<Int>::MainType abs_value = value;
    if (internal::is_negative(value)) {
      abs_value = 0 - abs_value;
      *write_unsigned_decimal(abs_value, 1) = '-';
    } else {
      write_unsigned_decimal(abs_value, 0);
    }
  }

  // Prepare a buffer for integer formatting.
  CharPtr prepare_int_buffer(unsigned num_digits,
      const EmptySpec &, const char *prefix, unsigned prefix_size) {
    unsigned size = prefix_size + num_digits;
    CharPtr p = grow_buffer(size);
    std::copy(prefix, prefix + prefix_size, p);
    return p + size - 1;
  }

  template <typename Spec>
  CharPtr prepare_int_buffer(unsigned num_digits,
    const Spec &spec, const char *prefix, unsigned prefix_size);

  // Formats an integer.
  template <typename T, typename Spec>
  void write_int(T value, Spec spec);

  // Formats a floating-point number (double or long double).
  template <typename T>
  void write_double(T value, const FormatSpec &spec);

  // Writes a formatted string.
  template <typename StrChar>
  CharPtr write_str(
      const StrChar *s, std::size_t size, const AlignSpec &spec);

  template <typename StrChar>
  void write_str(
      const internal::Arg::StringValue<StrChar> &str, const FormatSpec &spec);

  // This following methods are private to disallow writing wide characters
  // and strings to a char stream. If you want to print a wide string as a
  // pointer as std::ostream does, cast it to const void*.
  // Do not implement!
  void operator<<(typename internal::WCharHelper<wchar_t, Char>::Unsupported);
  void operator<<(
      typename internal::WCharHelper<const wchar_t *, Char>::Unsupported);

  // Appends floating-point length specifier to the format string.
  // The second argument is only used for overload resolution.
  void append_float_length(Char *&format_ptr, long double) {
    *format_ptr++ = 'L';
  }

  template<typename T>
  void append_float_length(Char *&, T) {}

  template <typename Impl, typename Char_>
  friend class internal::BasicArgFormatter;

  friend class internal::PrintfArgFormatter<Char>;

 protected:
  /**
    Constructs a ``BasicWriter`` object.
   */
  explicit BasicWriter(Buffer<Char> &b) : buffer_(b) {}

 public:
  /**
    \rst
    Destroys a ``BasicWriter`` object.
    \endrst
   */
  virtual ~BasicWriter() {}

  /**
    Returns the total number of characters written.
   */
  std::size_t size() const { return buffer_.size(); }

  /**
    Returns a pointer to the output buffer content. No terminating null
    character is appended.
   */
  const Char *data() const FMT_NOEXCEPT { return &buffer_[0]; }

  /**
    Returns a pointer to the output buffer content with terminating null
    character appended.
   */
  const Char *c_str() const {
    std::size_t size = buffer_.size();
    buffer_.reserve(size + 1);
    buffer_[size] = '\0';
    return &buffer_[0];
  }

  /**
    \rst
    Returns the content of the output buffer as an `std::string`.
    \endrst
   */
  std::basic_string<Char> str() const {
    return std::basic_string<Char>(&buffer_[0], buffer_.size());
  }

  /**
    \rst
    Writes formatted data.

    *args* is an argument list representing arbitrary arguments.

    **Example**::

       MemoryWriter out;
       out.write("Current point:\n");
       out.write("({:+f}, {:+f})", -3.14, 3.14);

    This will write the following output to the ``out`` object:

    .. code-block:: none

       Current point:
       (-3.140000, +3.140000)

    The output can be accessed using :func:`data()`, :func:`c_str` or
    :func:`str` methods.

    See also :ref:`syntax`.
    \endrst
   */
  void write(BasicCStringRef<Char> format, ArgList args) {
    BasicFormatter<Char>(args, *this).format(format);
  }
  FMT_VARIADIC_VOID(write, BasicCStringRef<Char>)

  BasicWriter &operator<<(int value) {
    write_decimal(value);
    return *this;
  }
  BasicWriter &operator<<(unsigned value) {
    return *this << IntFormatSpec<unsigned>(value);
  }
  BasicWriter &operator<<(long value) {
    write_decimal(value);
    return *this;
  }
  BasicWriter &operator<<(unsigned long value) {
    return *this << IntFormatSpec<unsigned long>(value);
  }
  BasicWriter &operator<<(LongLong value) {
    write_decimal(value);
    return *this;
  }

  /**
    \rst
    Formats *value* and writes it to the stream.
    \endrst
   */
  BasicWriter &operator<<(ULongLong value) {
    return *this << IntFormatSpec<ULongLong>(value);
  }

  BasicWriter &operator<<(double value) {
    write_double(value, FormatSpec());
    return *this;
  }

  /**
    \rst
    Formats *value* using the general format for floating-point numbers
    (``'g'``) and writes it to the stream.
    \endrst
   */
  BasicWriter &operator<<(long double value) {
    write_double(value, FormatSpec());
    return *this;
  }

  /**
    Writes a character to the stream.
   */
  BasicWriter &operator<<(char value) {
    buffer_.push_back(value);
    return *this;
  }

  BasicWriter &operator<<(
      typename internal::WCharHelper<wchar_t, Char>::Supported value) {
    buffer_.push_back(value);
    return *this;
  }

  /**
    \rst
    Writes *value* to the stream.
    \endrst
   */
  BasicWriter &operator<<(fmt::BasicStringRef<Char> value) {
    const Char *str = value.data();
    buffer_.append(str, str + value.size());
    return *this;
  }

  BasicWriter &operator<<(
      typename internal::WCharHelper<StringRef, Char>::Supported value) {
    const char *str = value.data();
    buffer_.append(str, str + value.size());
    return *this;
  }

  template <typename T, typename Spec, typename FillChar>
  BasicWriter &operator<<(IntFormatSpec<T, Spec, FillChar> spec) {
    internal::CharTraits<Char>::convert(FillChar());
    write_int(spec.value(), spec);
    return *this;
  }

  template <typename StrChar>
  BasicWriter &operator<<(const StrFormatSpec<StrChar> &spec) {
    const StrChar *s = spec.str();
    write_str(s, std::char_traits<Char>::length(s), spec);
    return *this;
  }

  void clear() FMT_NOEXCEPT { buffer_.clear(); }
};

template <typename Char>
template <typename StrChar>
typename BasicWriter<Char>::CharPtr BasicWriter<Char>::write_str(
      const StrChar *s, std::size_t size, const AlignSpec &spec) {
  CharPtr out = CharPtr();
  if (spec.width() > size) {
    out = grow_buffer(spec.width());
    Char fill = internal::CharTraits<Char>::cast(spec.fill());
    if (spec.align() == ALIGN_RIGHT) {
      std::fill_n(out, spec.width() - size, fill);
      out += spec.width() - size;
    } else if (spec.align() == ALIGN_CENTER) {
      out = fill_padding(out, spec.width(), size, fill);
    } else {
      std::fill_n(out + size, spec.width() - size, fill);
    }
  } else {
    out = grow_buffer(size);
  }
  std::copy(s, s + size, out);
  return out;
}

template <typename Char>
typename BasicWriter<Char>::CharPtr
  BasicWriter<Char>::fill_padding(
    CharPtr buffer, unsigned total_size,
    std::size_t content_size, wchar_t fill) {
  std::size_t padding = total_size - content_size;
  std::size_t left_padding = padding / 2;
  Char fill_char = internal::CharTraits<Char>::cast(fill);
  std::fill_n(buffer, left_padding, fill_char);
  buffer += left_padding;
  CharPtr content = buffer;
  std::fill_n(buffer + content_size, padding - left_padding, fill_char);
  return content;
}

template <typename Char>
template <typename Spec>
typename BasicWriter<Char>::CharPtr
  BasicWriter<Char>::prepare_int_buffer(
    unsigned num_digits, const Spec &spec,
    const char *prefix, unsigned prefix_size) {
  unsigned width = spec.width();
  Alignment align = spec.align();
  Char fill = internal::CharTraits<Char>::cast(spec.fill());
  if (spec.precision() > static_cast<int>(num_digits)) {
    // Octal prefix '0' is counted as a digit, so ignore it if precision
    // is specified.
    if (prefix_size > 0 && prefix[prefix_size - 1] == '0')
      --prefix_size;
    unsigned number_size = prefix_size + spec.precision();
    AlignSpec subspec(number_size, '0', ALIGN_NUMERIC);
    if (number_size >= width)
      return prepare_int_buffer(num_digits, subspec, prefix, prefix_size);
    buffer_.reserve(width);
    unsigned fill_size = width - number_size;
    if (align != ALIGN_LEFT) {
      CharPtr p = grow_buffer(fill_size);
      std::fill(p, p + fill_size, fill);
    }
    CharPtr result = prepare_int_buffer(
        num_digits, subspec, prefix, prefix_size);
    if (align == ALIGN_LEFT) {
      CharPtr p = grow_buffer(fill_size);
      std::fill(p, p + fill_size, fill);
    }
    return result;
  }
  unsigned size = prefix_size + num_digits;
  if (width <= size) {
    CharPtr p = grow_buffer(size);
    std::copy(prefix, prefix + prefix_size, p);
    return p + size - 1;
  }
  CharPtr p = grow_buffer(width);
  CharPtr end = p + width;
  if (align == ALIGN_LEFT) {
    std::copy(prefix, prefix + prefix_size, p);
    p += size;
    std::fill(p, end, fill);
  } else if (align == ALIGN_CENTER) {
    p = fill_padding(p, width, size, fill);
    std::copy(prefix, prefix + prefix_size, p);
    p += size;
  } else {
    if (align == ALIGN_NUMERIC) {
      if (prefix_size != 0) {
        p = std::copy(prefix, prefix + prefix_size, p);
        size -= prefix_size;
      }
    } else {
      std::copy(prefix, prefix + prefix_size, end - size);
    }
    std::fill(p, end - size, fill);
    p = end;
  }
  return p - 1;
}

template <typename Char>
template <typename T, typename Spec>
void BasicWriter<Char>::write_int(T value, Spec spec) {
  unsigned prefix_size = 0;
  typedef typename internal::IntTraits<T>::MainType UnsignedType;
  UnsignedType abs_value = value;
  char prefix[4] = "";
  if (internal::is_negative(value)) {
    prefix[0] = '-';
    ++prefix_size;
    abs_value = 0 - abs_value;
  } else if (spec.flag(SIGN_FLAG)) {
    prefix[0] = spec.flag(PLUS_FLAG) ? '+' : ' ';
    ++prefix_size;
  }
  switch (spec.type()) {
  case 0: case 'd': {
    unsigned num_digits = internal::count_digits(abs_value);
    CharPtr p = prepare_int_buffer(
      num_digits, spec, prefix, prefix_size) + 1 - num_digits;
    internal::format_decimal(get(p), abs_value, num_digits);
    break;
  }
  case 'x': case 'X': {
    UnsignedType n = abs_value;
    if (spec.flag(HASH_FLAG)) {
      prefix[prefix_size++] = '0';
      prefix[prefix_size++] = spec.type();
    }
    unsigned num_digits = 0;
    do {
      ++num_digits;
    } while ((n >>= 4) != 0);
    Char *p = get(prepare_int_buffer(
      num_digits, spec, prefix, prefix_size));
    n = abs_value;
    const char *digits = spec.type() == 'x' ?
        "0123456789abcdef" : "0123456789ABCDEF";
    do {
      *p-- = digits[n & 0xf];
    } while ((n >>= 4) != 0);
    break;
  }
  case 'b': case 'B': {
    UnsignedType n = abs_value;
    if (spec.flag(HASH_FLAG)) {
      prefix[prefix_size++] = '0';
      prefix[prefix_size++] = spec.type();
    }
    unsigned num_digits = 0;
    do {
      ++num_digits;
    } while ((n >>= 1) != 0);
    Char *p = get(prepare_int_buffer(num_digits, spec, prefix, prefix_size));
    n = abs_value;
    do {
      *p-- = static_cast<Char>('0' + (n & 1));
    } while ((n >>= 1) != 0);
    break;
  }
  case 'o': {
    UnsignedType n = abs_value;
    if (spec.flag(HASH_FLAG))
      prefix[prefix_size++] = '0';
    unsigned num_digits = 0;
    do {
      ++num_digits;
    } while ((n >>= 3) != 0);
    Char *p = get(prepare_int_buffer(num_digits, spec, prefix, prefix_size));
    n = abs_value;
    do {
      *p-- = static_cast<Char>('0' + (n & 7));
    } while ((n >>= 3) != 0);
    break;
  }
  default:
    internal::report_unknown_type(
      spec.type(), spec.flag(CHAR_FLAG) ? "char" : "integer");
    break;
  }
}

template <typename Char>
template <typename T>
void BasicWriter<Char>::write_double(
    T value, const FormatSpec &spec) {
  // Check type.
  char type = spec.type();
  bool upper = false;
  switch (type) {
  case 0:
    type = 'g';
    break;
  case 'e': case 'f': case 'g': case 'a':
    break;
  case 'F':
#ifdef _MSC_VER
    // MSVC's printf doesn't support 'F'.
    type = 'f';
#endif
    // Fall through.
  case 'E': case 'G': case 'A':
    upper = true;
    break;
  default:
    internal::report_unknown_type(type, "double");
    break;
  }

  char sign = 0;
  // Use isnegative instead of value < 0 because the latter is always
  // false for NaN.
  if (internal::FPUtil::isnegative(static_cast<double>(value))) {
    sign = '-';
    value = -value;
  } else if (spec.flag(SIGN_FLAG)) {
    sign = spec.flag(PLUS_FLAG) ? '+' : ' ';
  }

  if (internal::FPUtil::isnotanumber(value)) {
    // Format NaN ourselves because sprintf's output is not consistent
    // across platforms.
    std::size_t nan_size = 4;
    const char *nan = upper ? " NAN" : " nan";
    if (!sign) {
      --nan_size;
      ++nan;
    }
    CharPtr out = write_str(nan, nan_size, spec);
    if (sign)
      *out = sign;
    return;
  }

  if (internal::FPUtil::isinfinity(value)) {
    // Format infinity ourselves because sprintf's output is not consistent
    // across platforms.
    std::size_t inf_size = 4;
    const char *inf = upper ? " INF" : " inf";
    if (!sign) {
      --inf_size;
      ++inf;
    }
    CharPtr out = write_str(inf, inf_size, spec);
    if (sign)
      *out = sign;
    return;
  }

  std::size_t offset = buffer_.size();
  unsigned width = spec.width();
  if (sign) {
    buffer_.reserve(buffer_.size() + (std::max)(width, 1u));
    if (width > 0)
      --width;
    ++offset;
  }

  // Build format string.
  enum { MAX_FORMAT_SIZE = 10}; // longest format: %#-*.*Lg
  Char format[MAX_FORMAT_SIZE];
  Char *format_ptr = format;
  *format_ptr++ = '%';
  unsigned width_for_sprintf = width;
  if (spec.flag(HASH_FLAG))
    *format_ptr++ = '#';
  if (spec.align() == ALIGN_CENTER) {
    width_for_sprintf = 0;
  } else {
    if (spec.align() == ALIGN_LEFT)
      *format_ptr++ = '-';
    if (width != 0)
      *format_ptr++ = '*';
  }
  if (spec.precision() >= 0) {
    *format_ptr++ = '.';
    *format_ptr++ = '*';
  }

  append_float_length(format_ptr, value);
  *format_ptr++ = type;
  *format_ptr = '\0';

  // Format using snprintf.
  Char fill = internal::CharTraits<Char>::cast(spec.fill());
  for (;;) {
    std::size_t buffer_size = buffer_.capacity() - offset;
#ifdef _MSC_VER
    // MSVC's vsnprintf_s doesn't work with zero size, so reserve
    // space for at least one extra character to make the size non-zero.
    // Note that the buffer's capacity will increase by more than 1.
    if (buffer_size == 0) {
      buffer_.reserve(offset + 1);
      buffer_size = buffer_.capacity() - offset;
    }
#endif
    Char *start = &buffer_[offset];
    int n = internal::CharTraits<Char>::format_float(
        start, buffer_size, format, width_for_sprintf, spec.precision(), value);
    if (n >= 0 && offset + n < buffer_.capacity()) {
      if (sign) {
        if ((spec.align() != ALIGN_RIGHT && spec.align() != ALIGN_DEFAULT) ||
            *start != ' ') {
          *(start - 1) = sign;
          sign = 0;
        } else {
          *(start - 1) = fill;
        }
        ++n;
      }
      if (spec.align() == ALIGN_CENTER &&
          spec.width() > static_cast<unsigned>(n)) {
        width = spec.width();
        CharPtr p = grow_buffer(width);
        std::copy(p, p + n, p + (width - n) / 2);
        fill_padding(p, spec.width(), n, fill);
        return;
      }
      if (spec.fill() != ' ' || sign) {
        while (*start == ' ')
          *start++ = fill;
        if (sign)
          *(start - 1) = sign;
      }
      grow_buffer(n);
      return;
    }
    // If n is negative we ask to increase the capacity by at least 1,
    // but as std::vector, the buffer grows exponentially.
    buffer_.reserve(n >= 0 ? offset + n + 1 : buffer_.capacity() + 1);
  }
}

/**
  \rst
  This class template provides operations for formatting and writing data
  into a character stream. The output is stored in a memory buffer that grows
  dynamically.

  You can use one of the following typedefs for common character types
  and the standard allocator:

  +---------------+-----------------------------------------------------+
  | Type          | Definition                                          |
  +===============+=====================================================+
  | MemoryWriter  | BasicMemoryWriter<char, std::allocator<char>>       |
  +---------------+-----------------------------------------------------+
  | WMemoryWriter | BasicMemoryWriter<wchar_t, std::allocator<wchar_t>> |
  +---------------+-----------------------------------------------------+

  **Example**::

     MemoryWriter out;
     out << "The answer is " << 42 << "\n";
     out.write("({:+f}, {:+f})", -3.14, 3.14);

  This will write the following output to the ``out`` object:

  .. code-block:: none

     The answer is 42
     (-3.140000, +3.140000)

  The output can be converted to an ``std::string`` with ``out.str()`` or
  accessed as a C string with ``out.c_str()``.
  \endrst
 */
template <typename Char, typename Allocator = std::allocator<Char> >
class BasicMemoryWriter : public BasicWriter<Char> {
 private:
  internal::MemoryBuffer<Char, internal::INLINE_BUFFER_SIZE, Allocator> buffer_;

 public:
  explicit BasicMemoryWriter(const Allocator& alloc = Allocator())
    : BasicWriter<Char>(buffer_), buffer_(alloc) {}

#if FMT_USE_RVALUE_REFERENCES
  /**
    \rst
    Constructs a :class:`fmt::BasicMemoryWriter` object moving the content
    of the other object to it.
    \endrst
   */
  BasicMemoryWriter(BasicMemoryWriter &&other)
    : BasicWriter<Char>(buffer_), buffer_(std::move(other.buffer_)) {
  }

  /**
    \rst
    Moves the content of the other ``BasicMemoryWriter`` object to this one.
    \endrst
   */
  BasicMemoryWriter &operator=(BasicMemoryWriter &&other) {
    buffer_ = std::move(other.buffer_);
    return *this;
  }
#endif
};

typedef BasicMemoryWriter<char> MemoryWriter;
typedef BasicMemoryWriter<wchar_t> WMemoryWriter;

/**
  \rst
  This class template provides operations for formatting and writing data
  into a fixed-size array. For writing into a dynamically growing buffer
  use :class:`fmt::BasicMemoryWriter`.

  Any write method will throw ``std::runtime_error`` if the output doesn't fit
  into the array.

  You can use one of the following typedefs for common character types:

  +--------------+---------------------------+
  | Type         | Definition                |
  +==============+===========================+
  | ArrayWriter  | BasicArrayWriter<char>    |
  +--------------+---------------------------+
  | WArrayWriter | BasicArrayWriter<wchar_t> |
  +--------------+---------------------------+
  \endrst
 */
template <typename Char>
class BasicArrayWriter : public BasicWriter<Char> {
 private:
  internal::FixedBuffer<Char> buffer_;

 public:
  /**
   \rst
   Constructs a :class:`fmt::BasicArrayWriter` object for *array* of the
   given size.
   \endrst
   */
  BasicArrayWriter(Char *array, std::size_t size)
    : BasicWriter<Char>(buffer_), buffer_(array, size) {}

  /**
   \rst
   Constructs a :class:`fmt::BasicArrayWriter` object for *array* of the
   size known at compile time.
   \endrst
   */
  template <std::size_t SIZE>
  explicit BasicArrayWriter(Char (&array)[SIZE])
    : BasicWriter<Char>(buffer_), buffer_(array, SIZE) {}
};

typedef BasicArrayWriter<char> ArrayWriter;
typedef BasicArrayWriter<wchar_t> WArrayWriter;

// Formats a value.
template <typename Char, typename T>
void format(BasicFormatter<Char> &f, const Char *&format_str, const T &value) {
  internal::MemoryBuffer<Char, internal::INLINE_BUFFER_SIZE> buffer;

  internal::FormatBuf<Char> format_buf(buffer);
  std::basic_ostream<Char> output(&format_buf);
  output << value;

  BasicStringRef<Char> str(&buffer[0], format_buf.size());
  internal::Arg arg = internal::MakeValue<Char>(str);
  arg.type = static_cast<internal::Arg::Type>(
        internal::MakeValue<Char>::type(str));
  format_str = f.format(format_str, arg);
}

// Reports a system error without throwing an exception.
// Can be used to report errors from destructors.
void report_system_error(int error_code, StringRef message) FMT_NOEXCEPT;

#if FMT_USE_WINDOWS_H

/** A Windows error. */
class WindowsError : public SystemError {
 private:
  void init(int error_code, CStringRef format_str, ArgList args);

 public:
  /**
   \rst
   Constructs a :class:`fmt::WindowsError` object with the description
   of the form

   .. parsed-literal::
     *<message>*: *<system-message>*

   where *<message>* is the formatted message and *<system-message>* is the
   system message corresponding to the error code.
   *error_code* is a Windows error code as given by ``GetLastError``.
   If *error_code* is not a valid error code such as -1, the system message
   will look like "error -1".

   **Example**::

     // This throws a WindowsError with the description
     //   cannot open file 'madeup': The system cannot find the file specified.
     // or similar (system message may vary).
     const char *filename = "madeup";
     LPOFSTRUCT of = LPOFSTRUCT();
     HFILE file = OpenFile(filename, &of, OF_READ);
     if (file == HFILE_ERROR) {
       throw fmt::WindowsError(GetLastError(),
                               "cannot open file '{}'", filename);
     }
   \endrst
  */
  WindowsError(int error_code, CStringRef message) {
    init(error_code, message, ArgList());
  }
  FMT_VARIADIC_CTOR(WindowsError, init, int, CStringRef)
};

// Reports a Windows error without throwing an exception.
// Can be used to report errors from destructors.
void report_windows_error(int error_code, StringRef message) FMT_NOEXCEPT;

#endif

enum Color { BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE };

/**
  Formats a string and prints it to stdout using ANSI escape sequences
  to specify color (experimental).
  Example:
    print_colored(fmt::RED, "Elapsed time: {0:.2f} seconds", 1.23);
 */
void print_colored(Color c, CStringRef format, ArgList args);

/**
  \rst
  Formats arguments and returns the result as a string.

  **Example**::

    std::string message = format("The answer is {}", 42);
  \endrst
*/
inline std::string format(CStringRef format_str, ArgList args) {
  MemoryWriter w;
  w.write(format_str, args);
  return w.str();
}

inline std::wstring format(WCStringRef format_str, ArgList args) {
  WMemoryWriter w;
  w.write(format_str, args);
  return w.str();
}

/**
  \rst
  Prints formatted data to the file *f*.

  **Example**::

    print(stderr, "Don't {}!", "panic");
  \endrst
 */
void print(std::FILE *f, CStringRef format_str, ArgList args);

/**
  \rst
  Prints formatted data to ``stdout``.

  **Example**::

    print("Elapsed time: {0:.2f} seconds", 1.23);
  \endrst
 */
void print(CStringRef format_str, ArgList args);

template <typename Char>
void printf(BasicWriter<Char> &w, BasicCStringRef<Char> format, ArgList args) {
  internal::PrintfFormatter<Char>(args).format(w, format);
}

/**
  \rst
  Formats arguments and returns the result as a string.

  **Example**::

    std::string message = fmt::sprintf("The answer is %d", 42);
  \endrst
*/
inline std::string sprintf(CStringRef format, ArgList args) {
  MemoryWriter w;
  printf(w, format, args);
  return w.str();
}

inline std::wstring sprintf(WCStringRef format, ArgList args) {
  WMemoryWriter w;
  printf(w, format, args);
  return w.str();
}

/**
  \rst
  Prints formatted data to the file *f*.

  **Example**::

    fmt::fprintf(stderr, "Don't %s!", "panic");
  \endrst
 */
int fprintf(std::FILE *f, CStringRef format, ArgList args);

/**
  \rst
  Prints formatted data to ``stdout``.

  **Example**::

    fmt::printf("Elapsed time: %.2f seconds", 1.23);
  \endrst
 */
inline int printf(CStringRef format, ArgList args) {
  return fprintf(stdout, format, args);
}

/**
  Fast integer formatter.
 */
class FormatInt {
 private:
  // Buffer should be large enough to hold all digits (digits10 + 1),
  // a sign and a null character.
  enum {BUFFER_SIZE = std::numeric_limits<ULongLong>::digits10 + 3};
  mutable char buffer_[BUFFER_SIZE];
  char *str_;

  // Formats value in reverse and returns the number of digits.
  char *format_decimal(ULongLong value) {
    char *buffer_end = buffer_ + BUFFER_SIZE - 1;
    while (value >= 100) {
      // Integer division is slow so do it for a group of two digits instead
      // of for every digit. The idea comes from the talk by Alexandrescu
      // "Three Optimization Tips for C++". See speed-test for a comparison.
      unsigned index = static_cast<unsigned>((value % 100) * 2);
      value /= 100;
      *--buffer_end = internal::Data::DIGITS[index + 1];
      *--buffer_end = internal::Data::DIGITS[index];
    }
    if (value < 10) {
      *--buffer_end = static_cast<char>('0' + value);
      return buffer_end;
    }
    unsigned index = static_cast<unsigned>(value * 2);
    *--buffer_end = internal::Data::DIGITS[index + 1];
    *--buffer_end = internal::Data::DIGITS[index];
    return buffer_end;
  }

  void FormatSigned(LongLong value) {
    ULongLong abs_value = static_cast<ULongLong>(value);
    bool negative = value < 0;
    if (negative)
      abs_value = 0 - abs_value;
    str_ = format_decimal(abs_value);
    if (negative)
      *--str_ = '-';
  }

 public:
  explicit FormatInt(int value) { FormatSigned(value); }
  explicit FormatInt(long value) { FormatSigned(value); }
  explicit FormatInt(LongLong value) { FormatSigned(value); }
  explicit FormatInt(unsigned value) : str_(format_decimal(value)) {}
  explicit FormatInt(unsigned long value) : str_(format_decimal(value)) {}
  explicit FormatInt(ULongLong value) : str_(format_decimal(value)) {}

  /**
    Returns the number of characters written to the output buffer.
   */
  std::size_t size() const { return buffer_ - str_ + BUFFER_SIZE - 1; }

  /**
    Returns a pointer to the output buffer content. No terminating null
    character is appended.
   */
  const char *data() const { return str_; }

  /**
    Returns a pointer to the output buffer content with terminating null
    character appended.
   */
  const char *c_str() const {
    buffer_[BUFFER_SIZE - 1] = '\0';
    return str_;
  }

  /**
    \rst
    Returns the content of the output buffer as an ``std::string``.
    \endrst
   */
  std::string str() const { return std::string(str_, size()); }
};

// Formats a decimal integer value writing into buffer and returns
// a pointer to the end of the formatted string. This function doesn't
// write a terminating null character.
template <typename T>
inline void format_decimal(char *&buffer, T value) {
  typename internal::IntTraits<T>::MainType abs_value = value;
  if (internal::is_negative(value)) {
    *buffer++ = '-';
    abs_value = 0 - abs_value;
  }
  if (abs_value < 100) {
    if (abs_value < 10) {
      *buffer++ = static_cast<char>('0' + abs_value);
      return;
    }
    unsigned index = static_cast<unsigned>(abs_value * 2);
    *buffer++ = internal::Data::DIGITS[index];
    *buffer++ = internal::Data::DIGITS[index + 1];
    return;
  }
  unsigned num_digits = internal::count_digits(abs_value);
  internal::format_decimal(buffer, abs_value, num_digits);
  buffer += num_digits;
}

/**
  \rst
  Returns a named argument for formatting functions.

  **Example**::

    print("Elapsed time: {s:.2f} seconds", arg("s", 1.23));

  \endrst
 */
template <typename T>
inline internal::NamedArg<char> arg(StringRef name, const T &arg) {
  return internal::NamedArg<char>(name, arg);
}

template <typename T>
inline internal::NamedArg<wchar_t> arg(WStringRef name, const T &arg) {
  return internal::NamedArg<wchar_t>(name, arg);
}

// The following two functions are deleted intentionally to disable
// nested named arguments as in ``format("{}", arg("a", arg("b", 42)))``.
template <typename Char>
void arg(StringRef, const internal::NamedArg<Char>&) FMT_DELETED_OR_UNDEFINED;
template <typename Char>
void arg(WStringRef, const internal::NamedArg<Char>&) FMT_DELETED_OR_UNDEFINED;
}

#if FMT_GCC_VERSION
// Use the system_header pragma to suppress warnings about variadic macros
// because suppressing -Wvariadic-macros with the diagnostic pragma doesn't
// work. It is used at the end because we want to suppress as little warnings
// as possible.
# pragma GCC system_header
#endif

// This is used to work around VC++ bugs in handling variadic macros.
#define FMT_EXPAND(args) args

// Returns the number of arguments.
// Based on https://groups.google.com/forum/#!topic/comp.std.c/d-6Mj5Lko_s.
#define FMT_NARG(...) FMT_NARG_(__VA_ARGS__, FMT_RSEQ_N())
#define FMT_NARG_(...) FMT_EXPAND(FMT_ARG_N(__VA_ARGS__))
#define FMT_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define FMT_RSEQ_N() 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define FMT_CONCAT(a, b) a##b
#define FMT_FOR_EACH_(N, f, ...) \
  FMT_EXPAND(FMT_CONCAT(FMT_FOR_EACH, N)(f, __VA_ARGS__))
#define FMT_FOR_EACH(f, ...) \
  FMT_EXPAND(FMT_FOR_EACH_(FMT_NARG(__VA_ARGS__), f, __VA_ARGS__))

#define FMT_ADD_ARG_NAME(type, index) type arg##index
#define FMT_GET_ARG_NAME(type, index) arg##index

#if FMT_USE_VARIADIC_TEMPLATES
# define FMT_VARIADIC_(Char, ReturnType, func, call, ...) \
  template <typename... Args> \
  ReturnType func(FMT_FOR_EACH(FMT_ADD_ARG_NAME, __VA_ARGS__), \
      const Args & ... args) { \
    typename fmt::internal::ArgArray<sizeof...(Args)>::Type array; \
    call(FMT_FOR_EACH(FMT_GET_ARG_NAME, __VA_ARGS__), \
      fmt::internal::make_arg_list<Char>(array, args...)); \
  }
#else
// Defines a wrapper for a function taking __VA_ARGS__ arguments
// and n additional arguments of arbitrary types.
# define FMT_WRAP(Char, ReturnType, func, call, n, ...) \
  template <FMT_GEN(n, FMT_MAKE_TEMPLATE_ARG)> \
  inline ReturnType func(FMT_FOR_EACH(FMT_ADD_ARG_NAME, __VA_ARGS__), \
      FMT_GEN(n, FMT_MAKE_ARG)) { \
    fmt::internal::ArgArray<n>::Type arr; \
    FMT_GEN(n, FMT_ASSIGN_##Char); \
    call(FMT_FOR_EACH(FMT_GET_ARG_NAME, __VA_ARGS__), fmt::ArgList( \
      fmt::internal::make_type(FMT_GEN(n, FMT_MAKE_REF2)), arr)); \
  }

# define FMT_VARIADIC_(Char, ReturnType, func, call, ...) \
  inline ReturnType func(FMT_FOR_EACH(FMT_ADD_ARG_NAME, __VA_ARGS__)) { \
    call(FMT_FOR_EACH(FMT_GET_ARG_NAME, __VA_ARGS__), fmt::ArgList()); \
  } \
  FMT_WRAP(Char, ReturnType, func, call, 1, __VA_ARGS__) \
  FMT_WRAP(Char, ReturnType, func, call, 2, __VA_ARGS__) \
  FMT_WRAP(Char, ReturnType, func, call, 3, __VA_ARGS__) \
  FMT_WRAP(Char, ReturnType, func, call, 4, __VA_ARGS__) \
  FMT_WRAP(Char, ReturnType, func, call, 5, __VA_ARGS__) \
  FMT_WRAP(Char, ReturnType, func, call, 6, __VA_ARGS__) \
  FMT_WRAP(Char, ReturnType, func, call, 7, __VA_ARGS__) \
  FMT_WRAP(Char, ReturnType, func, call, 8, __VA_ARGS__) \
  FMT_WRAP(Char, ReturnType, func, call, 9, __VA_ARGS__) \
  FMT_WRAP(Char, ReturnType, func, call, 10, __VA_ARGS__) \
  FMT_WRAP(Char, ReturnType, func, call, 11, __VA_ARGS__) \
  FMT_WRAP(Char, ReturnType, func, call, 12, __VA_ARGS__) \
  FMT_WRAP(Char, ReturnType, func, call, 13, __VA_ARGS__) \
  FMT_WRAP(Char, ReturnType, func, call, 14, __VA_ARGS__) \
  FMT_WRAP(Char, ReturnType, func, call, 15, __VA_ARGS__)
#endif  // FMT_USE_VARIADIC_TEMPLATES

/**
  \rst
  Defines a variadic function with the specified return type, function name
  and argument types passed as variable arguments to this macro.

  **Example**::

    void print_error(const char *file, int line, const char *format,
                     fmt::ArgList args) {
      fmt::print("{}: {}: ", file, line);
      fmt::print(format, args);
    }
    FMT_VARIADIC(void, print_error, const char *, int, const char *)

  ``FMT_VARIADIC`` is used for compatibility with legacy C++ compilers that
  don't implement variadic templates. You don't have to use this macro if
  you don't need legacy compiler support and can use variadic templates
  directly::

    template <typename... Args>
    void print_error(const char *file, int line, const char *format,
                     const Args & ... args) {
      fmt::print("{}: {}: ", file, line);
      fmt::print(format, args...);
    }
  \endrst
 */
#define FMT_VARIADIC(ReturnType, func, ...) \
  FMT_VARIADIC_(char, ReturnType, func, return func, __VA_ARGS__)

#define FMT_VARIADIC_W(ReturnType, func, ...) \
  FMT_VARIADIC_(wchar_t, ReturnType, func, return func, __VA_ARGS__)

#define FMT_CAPTURE_ARG_(id, index) ::fmt::arg(#id, id)

#define FMT_CAPTURE_ARG_W_(id, index) ::fmt::arg(L###id, id)

/**
  \rst
  Convenient macro to capture the arguments' names and values into several
  ``fmt::arg(name, value)``.

  **Example**::

    int x = 1, y = 2;
    print("point: ({x}, {y})", FMT_CAPTURE(x, y));
    // same as:
    // print("point: ({x}, {y})", arg("x", x), arg("y", y));

  \endrst
 */
#define FMT_CAPTURE(...) FMT_FOR_EACH(FMT_CAPTURE_ARG_, __VA_ARGS__)

#define FMT_CAPTURE_W(...) FMT_FOR_EACH(FMT_CAPTURE_ARG_W_, __VA_ARGS__)

namespace fmt {
FMT_VARIADIC(std::string, format, CStringRef)
FMT_VARIADIC_W(std::wstring, format, WCStringRef)
FMT_VARIADIC(void, print, CStringRef)
FMT_VARIADIC(void, print, std::FILE *, CStringRef)

FMT_VARIADIC(void, print_colored, Color, CStringRef)
FMT_VARIADIC(std::string, sprintf, CStringRef)
FMT_VARIADIC_W(std::wstring, sprintf, WCStringRef)
FMT_VARIADIC(int, printf, CStringRef)
FMT_VARIADIC(int, fprintf, std::FILE *, CStringRef)

#if FMT_USE_IOSTREAMS
/**
  \rst
  Prints formatted data to the stream *os*.

  **Example**::

    print(cerr, "Don't {}!", "panic");
  \endrst
 */
void print(std::ostream &os, CStringRef format_str, ArgList args);
FMT_VARIADIC(void, print, std::ostream &, CStringRef)
#endif
}  // namespace fmt

#if FMT_USE_USER_DEFINED_LITERALS
namespace fmt {
namespace internal {

template <typename Char>
struct UdlFormat {
  const Char *str;

  template <typename... Args>
  auto operator()(Args && ... args) const
                  -> decltype(format(str, std::forward<Args>(args)...)) {
    return format(str, std::forward<Args>(args)...);
  }
};

template <typename Char>
struct UdlArg {
  const Char *str;

  template <typename T>
  NamedArg<Char> operator=(T &&value) const {
    return {str, std::forward<T>(value)};
  }
};

} // namespace internal

inline namespace literals {

/**
  \rst
  C++11 literal equivalent of :func:`fmt::format`.

  **Example**::

    using namespace fmt::literals;
    std::string message = "The answer is {}"_format(42);
  \endrst
 */
inline internal::UdlFormat<char>
operator"" _format(const char *s, std::size_t) { return {s}; }
inline internal::UdlFormat<wchar_t>
operator"" _format(const wchar_t *s, std::size_t) { return {s}; }

/**
  \rst
  C++11 literal equivalent of :func:`fmt::arg`.

  **Example**::

    using namespace fmt::literals;
    print("Elapsed time: {s:.2f} seconds", "s"_a=1.23);
  \endrst
 */
inline internal::UdlArg<char>
operator"" _a(const char *s, std::size_t) { return {s}; }
inline internal::UdlArg<wchar_t>
operator"" _a(const wchar_t *s, std::size_t) { return {s}; }

} // inline namespace literals
} // namespace fmt
#endif // FMT_USE_USER_DEFINED_LITERALS

// Restore warnings.
#if FMT_GCC_VERSION >= 406
# pragma GCC diagnostic pop
#endif

#if defined(__clang__) && !defined(__INTEL_COMPILER)
# pragma clang diagnostic pop
#endif

#ifdef FMT_HEADER_ONLY
# include "format.cc"
#endif

#endif  // FMT_FORMAT_H_

/*
 A C++ interface to POSIX functions.

 Copyright (c) 2014 - 2015, Victor Zverovich
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FMT_POSIX_H_
#define FMT_POSIX_H_

#ifdef __MINGW32__
// Workaround MinGW bug https://sourceforge.net/p/mingw/bugs/2024/.
# undef __STRICT_ANSI__
#endif

#include <errno.h>
#include <fcntl.h>  // for O_RDONLY
#include <stdio.h>

#include <cstddef>


#ifndef FMT_POSIX
# if defined(_WIN32) && !defined(__MINGW32__)
// Fix warnings about deprecated symbols.
#  define FMT_POSIX(call) _##call
# else
#  define FMT_POSIX(call) call
# endif
#endif

// Calls to system functions are wrapped in FMT_SYSTEM for testability.
#ifdef FMT_SYSTEM
# define FMT_POSIX_CALL(call) FMT_SYSTEM(call)
#else
# define FMT_SYSTEM(call) call
# ifdef _WIN32
// Fix warnings about deprecated symbols.
#  define FMT_POSIX_CALL(call) ::_##call
# else
#  define FMT_POSIX_CALL(call) ::call
# endif
#endif

#if FMT_GCC_VERSION >= 407
# define FMT_UNUSED __attribute__((unused))
#else
# define FMT_UNUSED
#endif

#ifndef FMT_USE_STATIC_ASSERT
# define FMT_USE_STATIC_ASSERT 0
#endif

#if FMT_USE_STATIC_ASSERT || FMT_HAS_FEATURE(cxx_static_assert) || \
  (FMT_GCC_VERSION >= 403 && FMT_HAS_GXX_CXX11) || _MSC_VER >= 1600
# define FMT_STATIC_ASSERT(cond, message) static_assert(cond, message)
#else
# define FMT_CONCAT_(a, b) FMT_CONCAT(a, b)
# define FMT_STATIC_ASSERT(cond, message) \
  typedef int FMT_CONCAT_(Assert, __LINE__)[(cond) ? 1 : -1] FMT_UNUSED
#endif

// Retries the expression while it evaluates to error_result and errno
// equals to EINTR.
#ifndef _WIN32
# define FMT_RETRY_VAL(result, expression, error_result) \
  do { \
    result = (expression); \
  } while (result == error_result && errno == EINTR)
#else
# define FMT_RETRY_VAL(result, expression, error_result) result = (expression)
#endif

#define FMT_RETRY(result, expression) FMT_RETRY_VAL(result, expression, -1)

namespace fmt {

// An error code.
class ErrorCode {
 private:
  int value_;

 public:
  explicit ErrorCode(int value = 0) FMT_NOEXCEPT : value_(value) {}

  int get() const FMT_NOEXCEPT { return value_; }
};

// A buffered file.
class BufferedFile {
 private:
  FILE *file_;

  friend class File;

  explicit BufferedFile(FILE *f) : file_(f) {}

 public:
  // Constructs a BufferedFile object which doesn't represent any file.
  BufferedFile() FMT_NOEXCEPT : file_(0) {}

  // Destroys the object closing the file it represents if any.
  ~BufferedFile() FMT_NOEXCEPT;

#if !FMT_USE_RVALUE_REFERENCES
  // Emulate a move constructor and a move assignment operator if rvalue
  // references are not supported.

 private:
  // A proxy object to emulate a move constructor.
  // It is private to make it impossible call operator Proxy directly.
  struct Proxy {
    FILE *file;
  };

public:
  // A "move constructor" for moving from a temporary.
  BufferedFile(Proxy p) FMT_NOEXCEPT : file_(p.file) {}

  // A "move constructor" for for moving from an lvalue.
  BufferedFile(BufferedFile &f) FMT_NOEXCEPT : file_(f.file_) {
    f.file_ = 0;
  }

  // A "move assignment operator" for moving from a temporary.
  BufferedFile &operator=(Proxy p) {
    close();
    file_ = p.file;
    return *this;
  }

  // A "move assignment operator" for moving from an lvalue.
  BufferedFile &operator=(BufferedFile &other) {
    close();
    file_ = other.file_;
    other.file_ = 0;
    return *this;
  }

  // Returns a proxy object for moving from a temporary:
  //   BufferedFile file = BufferedFile(...);
  operator Proxy() FMT_NOEXCEPT {
    Proxy p = {file_};
    file_ = 0;
    return p;
  }

#else
 private:
  FMT_DISALLOW_COPY_AND_ASSIGN(BufferedFile);

 public:
  BufferedFile(BufferedFile &&other) FMT_NOEXCEPT : file_(other.file_) {
    other.file_ = 0;
  }

  BufferedFile& operator=(BufferedFile &&other) {
    close();
    file_ = other.file_;
    other.file_ = 0;
    return *this;
  }
#endif

  // Opens a file.
  BufferedFile(CStringRef filename, CStringRef mode);

  // Closes the file.
  void close();

  // Returns the pointer to a FILE object representing this file.
  FILE *get() const FMT_NOEXCEPT { return file_; }

  // We place parentheses around fileno to workaround a bug in some versions
  // of MinGW that define fileno as a macro.
  int (fileno)() const;

  void print(CStringRef format_str, const ArgList &args) {
    fmt::print(file_, format_str, args);
  }
  FMT_VARIADIC(void, print, CStringRef)
};

// A file. Closed file is represented by a File object with descriptor -1.
// Methods that are not declared with FMT_NOEXCEPT may throw
// fmt::SystemError in case of failure. Note that some errors such as
// closing the file multiple times will cause a crash on Windows rather
// than an exception. You can get standard behavior by overriding the
// invalid parameter handler with _set_invalid_parameter_handler.
class File {
 private:
  int fd_;  // File descriptor.

  // Constructs a File object with a given descriptor.
  explicit File(int fd) : fd_(fd) {}

 public:
  // Possible values for the oflag argument to the constructor.
  enum {
    RDONLY = FMT_POSIX(O_RDONLY), // Open for reading only.
    WRONLY = FMT_POSIX(O_WRONLY), // Open for writing only.
    RDWR   = FMT_POSIX(O_RDWR)    // Open for reading and writing.
  };

  // Constructs a File object which doesn't represent any file.
  File() FMT_NOEXCEPT : fd_(-1) {}

  // Opens a file and constructs a File object representing this file.
  File(CStringRef path, int oflag);

#if !FMT_USE_RVALUE_REFERENCES
  // Emulate a move constructor and a move assignment operator if rvalue
  // references are not supported.

 private:
  // A proxy object to emulate a move constructor.
  // It is private to make it impossible call operator Proxy directly.
  struct Proxy {
    int fd;
  };

 public:
  // A "move constructor" for moving from a temporary.
  File(Proxy p) FMT_NOEXCEPT : fd_(p.fd) {}

  // A "move constructor" for for moving from an lvalue.
  File(File &other) FMT_NOEXCEPT : fd_(other.fd_) {
    other.fd_ = -1;
  }

  // A "move assignment operator" for moving from a temporary.
  File &operator=(Proxy p) {
    close();
    fd_ = p.fd;
    return *this;
  }

  // A "move assignment operator" for moving from an lvalue.
  File &operator=(File &other) {
    close();
    fd_ = other.fd_;
    other.fd_ = -1;
    return *this;
  }

  // Returns a proxy object for moving from a temporary:
  //   File file = File(...);
  operator Proxy() FMT_NOEXCEPT {
    Proxy p = {fd_};
    fd_ = -1;
    return p;
  }

#else
 private:
  FMT_DISALLOW_COPY_AND_ASSIGN(File);

 public:
  File(File &&other) FMT_NOEXCEPT : fd_(other.fd_) {
    other.fd_ = -1;
  }

  File& operator=(File &&other) {
    close();
    fd_ = other.fd_;
    other.fd_ = -1;
    return *this;
  }
#endif

  // Destroys the object closing the file it represents if any.
  ~File() FMT_NOEXCEPT;

  // Returns the file descriptor.
  int descriptor() const FMT_NOEXCEPT { return fd_; }

  // Closes the file.
  void close();

  // Returns the file size.
  LongLong size() const;

  // Attempts to read count bytes from the file into the specified buffer.
  std::size_t read(void *buffer, std::size_t count);

  // Attempts to write count bytes from the specified buffer to the file.
  std::size_t write(const void *buffer, std::size_t count);

  // Duplicates a file descriptor with the dup function and returns
  // the duplicate as a file object.
  static File dup(int fd);

  // Makes fd be the copy of this file descriptor, closing fd first if
  // necessary.
  void dup2(int fd);

  // Makes fd be the copy of this file descriptor, closing fd first if
  // necessary.
  void dup2(int fd, ErrorCode &ec) FMT_NOEXCEPT;

  // Creates a pipe setting up read_end and write_end file objects for reading
  // and writing respectively.
  static void pipe(File &read_end, File &write_end);

  // Creates a BufferedFile object associated with this file and detaches
  // this File object from the file.
  BufferedFile fdopen(const char *mode);
};

// Returns the memory page size.
long getpagesize();
}  // namespace fmt

#if !FMT_USE_RVALUE_REFERENCES
namespace std {
// For compatibility with C++98.
inline fmt::BufferedFile &move(fmt::BufferedFile &f) { return f; }
inline fmt::File &move(fmt::File &f) { return f; }
}
#endif

#endif  // FMT_POSIX_H_

#ifndef    __LIBRARY_ENCRYPTION_XOR_HH__
#define    __LIBRARY_ENCRYPTION_XOR_HH__

#include  <cstdint>

namespace fwk {
  namespace Encryption {
    /**
     *  \class XOR Library/Encryption/XOR.hh
     *  \brief a class which implements a XOR encryption algorithm (a^b).
     */
    class XOR {
    public:
      /**
       *  \brief Encrypts data with the current encryption key.
       *  \param data a pointer to the start of the data to encrypt.
       *  \param dataSize size of the data to encrypt in bytes.
       *  \param key a pointer to the start of the key used to encrypt.
       *  \param keySize size of the key in bytes.
       */
      static void  xorify(void* data, size_t dataSize, const void* key, size_t keySize);
    };
  }
}

#endif    /* __LIBRARY_ENCRYPTION_XOR_HH__ */

#ifndef   __LIBRARY_NETWORK_CURL_METHOD_HH__
#define   __LIBRARY_NETWORK_CURL_METHOD_HH__

#include  <curl/curl.h>

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


namespace fwk {
  namespace curlxx {
    static const BidiMap<std::string, CURLoption> MethodToOption = {
      {std::string("GET"), CURLOPT_HTTPGET},
      {std::string("POST"), CURLOPT_POST},
      {std::string("PUT"), CURLOPT_PUT},
      {std::string("HEAD"), CURLOPT_NOBODY},
    }; /*!< Methods known by CURL. */
  }
}

#endif    /* __LIBRARY_NETWORK_CURL_METHOD_HH__ */

#ifndef   __LIBRARY_NETWORK_CURL_EASYHANDLE_HH__
#define   __LIBRARY_NETWORK_CURL_EASYHANDLE_HH__

#include  <curl/curl.h>
#include  <string>
#include  <map>

#ifndef    __LIBRARY_FACTORY_APOOLED_HPP__
#define    __LIBRARY_FACTORY_APOOLED_HPP__

#include  <chrono>

#ifndef    __LIBRARY_FACTORY_POOL_HPP__
#define    __LIBRARY_FACTORY_POOL_HPP__

#include  <queue>
#include  <utility>
#include  <string>

#ifndef   __LIBRARY_FACTORY_EXCEPTION_HH__
#define   __LIBRARY_FACTORY_EXCEPTION_HH__


namespace fwk {
  /**
   *  \class PoolNotInitializedException Library/Factory/Exception.hh
   *  \brief Exception thrown when trying to get or return an object to a null Pool.
   */
  class PoolNotInitializedException :public fwk::Exception {
  public:
    /**
     *  \brief Constructor of PoolNotInitializedException.
     *  \param message the error message.
     */
    PoolNotInitializedException(const std::string& message = "This pool was not initialized") noexcept;

    /**
     *  \brief Destructor of PoolNotInitializedException.
     */
    virtual ~PoolNotInitializedException(void) noexcept;
  };

  /**
   *  \class PoolInvalidArgumentsException Library/Factory/Exception.hh
   *  \brief Exception thrown when invalid values are given to initialize a Pool.
   */
  class PoolInvalidArgumentsException :public fwk::Exception {
  public:
    /**
     *  \brief Constructor of PoolInvalidArgumentsException.
     *  \param message the error message.
     */
    PoolInvalidArgumentsException(const std::string& message) noexcept;

    /**
     *  \brief Destructor of PoolInvalidArgumentsException.
     */
    virtual ~PoolInvalidArgumentsException(void) noexcept;
  };

  /**
   *  \class PoolMemoryExhaustedException Library/Factory/Exception.hh
   *  \brief Exception thrown when catching a std::bad_alloc exception.
   */
  class PoolMemoryExhaustedException :public fwk::Exception {
  public:
    /**
     *  \brief Constructor of PoolMemoryExhaustedException.
     *  \param message the error message.
     */
    PoolMemoryExhaustedException(const std::string& message) noexcept;

    /**
     *  \brief Destructor of PoolMemoryExhaustedException.
     */
    virtual ~PoolMemoryExhaustedException(void) noexcept;
  };
}

#endif    /* __LIBRARY_FACTORY_EXCEPTION_HH__ */

#ifndef    __LIBRARY_PROPERTY_INITIALIZABLE_HPP__
#define    __LIBRARY_PROPERTY_INITIALIZABLE_HPP__

namespace fwk {
  /**
   *  \class Initializable Library/Property/Initializable.hpp
   *  \brief Abstract class from which all classes than can be init only once inherit.
   */
  class Initializable {
  protected:
    bool  _initialized; /*!< set to true when the child class has been initialized. */

  public:
    /**
     *  \brief Constructor of Initializable.
     *
     *  Sets the init state to false.
     */
    Initializable(void);

    /**
     *  \brief Destructor of Initializable.
     */
    virtual ~Initializable(void);

  public:
    /**
     *  \brief Checks if the child class has been initialized.
     *  \return true if the child class has been initialized.
     */
    bool  isInitialized(void) const;

  protected:
    /**
     *  \brief Changes the init state of the object.
     *  \param state the new state.
     */
    void  isInitialized(bool state);
  };

  /**
   *  \class TInitializable Library/Property/Initializable.hpp
   *  \brief Templated class to create an Initializable object more easily.
   */
  template<class C>
  class TInitializable :public C, public Initializable {
  public:
    /**
     *  \brief Constructor of TInitializable.
     *
     *  Whatever arguments are passed to the constructor are passed to the templated class.
     */
    template<typename... Args>
    TInitializable(const Args&... args):
      C(args...),
      Initializable()
    {}

    /**
     *  \brief Destructor of TInitializable.
     */
    virtual ~TInitializable(void) {}
  };
}

#endif    /* __LIBRARY_PROPERTY_INITIALIZABLE_HPP__ */

#ifndef    __LIBRARY_THREADING_LOCKABLE_HPP__
#define    __LIBRARY_THREADING_LOCKABLE_HPP__

#include  <mutex>

namespace fwk {
  /**
   *  \class Lockable Library/Threading/Lock.hpp
   *  \brief A reentrant lock class.
   */
  class  Lockable {
  protected:
    std::recursive_mutex _lock; /*!< the mutex used for locking */

  public:
    /**
     *  \brief Constructor of Lockable.
     *  Inits the mutex.
     */
    Lockable(void);

    /**
     *  \brief Destructor of Lockable.
     */
    virtual ~Lockable(void);

  public:
    /**
     *  \brief Locks the mutex.
     */
    void  lock(void);

    /**
     *  \brief Unlocks the mutex.
     */
    void  unlock(void);
  };

  /**
   *  \class TLockable Library/Threading/Lock.hpp
   *  \brief A templated class to use when setting an object lockable.
   *  Can be useful for locking object of the STL, like `TLockable<std::list<int>>`.
   */
  template<class C>
  class  TLockable :public C, public Lockable {
  public:
    /**
     *  \brief Copy constructor of TLockable.
     *  \param oth the TLockable object to copy.
     */
    TLockable(const TLockable<C>& oth):
      C(oth),
      Lockable()
    {}

    /**
     *  \brief Default constructor of TLockable.
     *  \param args the arguments which will be passed to the constructor of the templated class.
     */
    template<typename... Args>
    TLockable(const Args&... args):
      C(args...),
      Lockable()
    {}

    /**
     *  \brief Destructor of TLockable.
     */
    virtual ~TLockable(void) {}
  };

  /**
   *  \class ReadWriteLock Library/Threading/Lock.hpp
   *  \brief A reader writer lock with writer priority.
   */
  class  ReadWriteLock {
  public:
    /**
     *  \class WriterGuard Library/Threading/Lock.hpp
     *  \brief A guard class which locks a ReadWriteLock as a writer in its constructor and unlocks it in its destructor.
     */
    class WriterGuard {
    private:
      ReadWriteLock *_lock; /*!< the read write lock. */

    public:
      /**
       *  \brief Constructor of WriterGuard. Locks the lock passed in parameter as writer.
       *  \param lock the lock.
       */
      WriterGuard(ReadWriteLock* lock);

      /**
       *  \brief Destructor of WriterGuard. Unlocks the lock given at creation as writer.
       */
      ~WriterGuard(void);
    };

    /**
     *  \class ReaderGuard Library/Threading/Lock.hpp
     *  \brief A guard class which locks a ReadWriteLock as a reader in its constructor and unlocks it in its destructor.
     */
    class ReaderGuard {
    private:
      ReadWriteLock *_lock; /*!< the read write lock. */

    public:
      /**
       *  \brief Constructor of ReaderGuard. Locks the lock passed in parameter as reader.
       *  \param lock the lock.
       */
      ReaderGuard(ReadWriteLock* lock);

      /**
       *  \brief Destructor of ReaderGuard. Unlocks the lock given at creation as reader.
       */
       ~ReaderGuard(void);
    };

  private:
    /**
     *  \class LightSwitch Library/Threading/Lock.hpp
     *  \brief an object used to count the number of times another mutex was locked.
     */
    class  LightSwitch {
    private:
      std::mutex _mutex;
      size_t     _counter; /*!< the number of times the mutex was locked. */

    public:
      /**
       *  \brief Constructor of LightSwitch. Sets the counter to 0.
       */
      LightSwitch(void);

    public:
      /**
       *  \brief Increments the counter and locks the mutex if it hits 1.
       *  \param mutex the mutex to lock.
       */
      void  acquire(std::mutex& mutex);

      /**
       *  \brief Decrements the counter and unlocks the mutex if it hits 0.
       *  \param mutex the mutex to lock.
       */
      void  release(std::mutex& mutex);
    };

  private:
    LightSwitch  _readSwitch; /*!< the switch used for the _noWriters mutex. */
    LightSwitch  _writeSwitch; /*!< the switch used for the _noReaders mutex. */
    std::mutex  _noReaders; /*!< the mutex used to check if there are no readers. */
    std::mutex  _noWriters; /*!< the mutex used to check if there are no writers. */
    std::mutex  _readersQueue; /*!< the mutex used to make readers wait to lock the ReadWriteLock. */

  public:
    /**
     *  \brief Constructor of ReadWriteLock.
     */
    ReadWriteLock(void);

    /**
     *  \brief Locks the ReadWriteLock as a reader.
     */
    void  readerAcquire(void);

    /**
     *  \brief Unlocks the ReadWriteLock as a reader.
     */
    void  readerRelease(void);

    /**
     *  \brief Locks the ReadWriteLock as a writer.
     */
    void  writerAcquire(void);

    /**
     *  \brief Unlocks the ReadWriteLock as a writer.
     */
    void  writerRelease(void);
  };

  typedef std::lock_guard<fwk::Lockable> ScopeLock;
  typedef std::lock_guard<std::mutex>    ScopeLockMutex;
}

#define SCOPELOCK(x)       std::lock_guard<fwk::Lockable>  lockguard(*(x));
#define SCOPELOCK_MUTEX(x) std::lock_guard<std::mutex>     lockguard_mutex(x);

#endif    /* __LIBRARY_THREADING_LOCKABLE_HPP__ */

#ifndef    __LIBRARY_TOOL_LOGGER_HPP__
#define    __LIBRARY_TOOL_LOGGER_HPP__

#include  <map>
#include  <unordered_map>
#include  <string>
#include  <fstream>
#include  <iostream>
#include  <sstream>

#ifndef    __LIBRARY_DESIGNPATTERN_SINGLETON_HPP__
#define    __LIBRARY_DESIGNPATTERN_SINGLETON_HPP__

#include  <cstddef>

namespace fwk {
  /**
   *  \class Singleton Library/DesignPattern/Singleton.hpp
   *  \brief Base class of all singleton classes.
   *
   *  Implements the singleton design pattern.
   */
  template<class T>
  class Singleton {
  private:
    static T  *_instance; /*!< A pointer on the instance of the subclass. */

  private:
    /**
     *  \brief deleted copy constructor.
     */
    Singleton(const Singleton&) = delete;

    /**
     *  \brief Deleted move constructor of Singleton.
     */
    Singleton(const Singleton&&) = delete;

    /**
     *  \brief deleted assignment constructor.
     */
    Singleton& operator=(const Singleton&) = delete;

  protected:
    /**
     *  \brief Constructor of Singleton.
     */
    Singleton(void) {}

    /**
     *  \brief Destructor of Singleton.
     */
    virtual ~Singleton(void) {}

  public:
    /**
     *  \brief Returns a reference on a Singleton subclass instance. Creates it before returning if it does not exist.
     *  \return a reference on the subclass instance.
     */
    static T&  get(void) {
      if (_instance == nullptr) {
        _instance = new T();
      }
      return (*_instance);
    }

    /**
     *  \brief Returns a pointer on a Singleton subclass instance. Creates it before returning if it does not exist.
     *  \return a pointer on the subclass instance.
     */
    static T*  asPtr(void) {
      return &(get());
    }

    /**
     *  \brief Destroys the singleton sublass instance.
     */
    static void  destroy(void) {
      if (_instance) {
        delete (_instance);
      }
      _instance = nullptr;
    }
  };

  template<class T>
  T*  Singleton<T>::_instance = nullptr;
}

#endif    /* __LIBRARY_DESIGNPATTERN_SINGLETON_HPP__ */

#ifndef    __LIBRARY_TOOL_DATE_HH__
#define    __LIBRARY_TOOL_DATE_HH__

#include  <iostream>
#include  <ctime>

namespace fwk {
  /**
   *  \class Date Library/Tool/Date.hh
   *  \brief a class containing static methods concerning the time.
   */
  class Date {
  public:
    /**
     *  \brief cross-platform inplementation of the UNIX gettimeofday function.
     *
     *  On UNIX, simply does a gettimeofday.
     *  \return the return value of gettimeofday, or 0 on Windows.
     */
    static int  gettimeofday(struct timeval *);

    /**
     *  \brief returns the time in hh:mm:ss:µµµµµµ format.
     *  \return a string representing the current time.
     */
    static const std::string  getTime(void);
  };
}

#endif    /* __LIBRARY_TOOL_DATE_HH__ */


namespace fwk {
  /**
   *  \class Logger Library/Tool/Logger.hpp
   *  \brief A logger class, which can write logs to a file or to the terminal output.
   */
  class Logger :public Lockable {
  public:
    /*! Logging levels. */
    enum class Level {
      DEBUG    = 0, /*!< Used to debug. */
      INFO     = 1, /*!< Used to log information. */
      WARNING  = 2, /*!< Used to log minor errors. */
      ERROR    = 3, /*!< Used to log non-critical errors. */
      CRITICAL = 4  /*!< Used to log critical errors. */
    };

  private:
    /*!< Colors usable in a terminal. */
    enum class Color {
      WHITE,
      SKYBLUE,
      PURPLE,
      BLUE,
      YELLOW,
      GREEN,
      RED,
      NONE
    };

    const std::map<Logger::Color, std::string> ColorToString = {
      {Logger::Color::WHITE, std::string("\033[97m")},
      {Logger::Color::SKYBLUE, std::string("\033[96m")},
      {Logger::Color::PURPLE, std::string("\033[95m")},
      {Logger::Color::BLUE, std::string("\033[94m")},
      {Logger::Color::YELLOW, std::string("\033[93m")},
      {Logger::Color::GREEN, std::string("\033[92m")},
      {Logger::Color::RED, std::string("\033[91m")},
      {Logger::Color::NONE, std::string("\033[0m")}
    }; /*!< Used to init colors inside the terminal. */

    const std::map<Logger::Level, Logger::Color> LevelToColor = {
      {Logger::Level::CRITICAL, Logger::Color::RED},
      {Logger::Level::ERROR, Logger::Color::PURPLE},
      {Logger::Level::WARNING, Logger::Color::YELLOW},
      {Logger::Level::INFO, Logger::Color::GREEN},
      {Logger::Level::DEBUG, Logger::Color::WHITE}
    }; /*!< associates a level to a color. */

  private:
    std::string     _name; /*!< name of the logger. */
    Logger::Level   _level; /*!< lowest level of logging. */
    size_t          _offset; /*!< current number of tabulations between the timestamp and the message. */
    std::ofstream*  _file; /*!< file where the logs are written. */

  public:
    /**
     *  \brief Constructor of Logger.
     *  Default lowest level of logging is INFO.
     *  \param name name of the logger.
     */
    Logger(const std::string& name);

    /**
     *  \brief Copy constructor of Logger.
     *  \param oth logger to copy.
     */
    Logger(const Logger& oth);

    /**
     *  \brief Assignation constructor of Logger.
     *  \param oth logger to copy.
     */
    Logger& operator=(const Logger& oth);

    /**
     *  \brief Destructor of Logger.
     *  Closes the file if it is open.
     */
    ~Logger(void);

  public:
    /**
     *  \brief Logs the message only if its level is greater or equal to the lowest debugging level. Prints the message inside the file if specified in \a init.
     *  Prints in a "hh:mm:ss:µµµµµµ -- msg" format.
     *  \param msg the message to log.
     *  \param level the logging level.
     */
    template<typename T>
    void  log(const T &msg, Logger::Level level) {
      SCOPELOCK(this);
      if (level >= this->_level) {
        std::ostream& os = (this->_file != nullptr ? *(this->_file) : (level >= Logger::Level::ERROR ? std::cerr : std::cout));

        os << Date::getTime() << " -- ";
        os << this->_name << " -- ";

        for (size_t i = 0 ; i < this->_offset ; ++i) {
          os << "\t";
        }

        try {
          os << Logger::ColorToString.at(Logger::LevelToColor.at(level));
        } catch (const std::out_of_range&) {
          os << Logger::ColorToString.at(Logger::Color::NONE);
        }

        os << msg << Logger::ColorToString.at(Logger::Color::NONE) << std::endl;
      }
    }

    /**
     *  \brief Sets the offset to an absolute number.
     *  \param off the offset.
     */
    void  setOffset(size_t off);

    /**
     *  \brief Increments the offset by the given number.
     *  \param off the incrementation.
     */
    void  addOffset(size_t off);

    /**
     *  \brief Decrements the offset by the given number.
     *  \param off the decrementation.
     */
    void  delOffset(size_t off);

    /**
     *  \brief Sets the lowest level of logging.
     *  \param level lowest level of logging.
     */
    void  setLevel(Logger::Level level);

    /**
     *  \brief Sets the logging file.
     *  \throw Exception if the file could not be opened.
     *  \param filepath path to the logging file.
     */
    void  setFile(const std::string& filepath);

    /**
     *  \brief Closes the file. Any log will be written to the std output.
     *  If no file is attached to the logger, nothing happens.
     */
    void closeFile(void);
  };

  /**
   *  \class LoggerManager Library/Tool/Logger.hpp
   *  \brief Loggers module.
   */
  class LoggerManager :public Singleton<fwk::LoggerManager>, public Lockable {
    friend class Singleton<fwk::LoggerManager>;
  public:
    typedef std::unordered_map<std::string, Logger> NameLoggerMap;

  private:
    NameLoggerMap _loggers; /*!< Current list of loggers. */

  private:
    /*! Deleted copy constructor of LoggerManager. */
    LoggerManager(const LoggerManager&) = delete;

    /*! Deleted move constructor of LoggerManager. */
    LoggerManager(const LoggerManager&&) = delete;

    /*! Deleted assignement constructor of LoggerManager. */
    LoggerManager&  operator=(const LoggerManager&) = delete;

  private:
    /*! Default constructor of LoggerManager. */
    LoggerManager(void);

    /*! Destructor of LoggerManager. */
    virtual ~LoggerManager(void);

  public:
    /**
     *  \brief Inits a new logger or changes the info of a current logger.
     *  \param loggerName name of the logger.
     *  \param level the lowest level of logging for this logger.
     */
    Logger&  init(const std::string& loggerName, Logger::Level level);

    /**
     *  \brief Inits a new logger or changes the info of a current logger.
     *  \throw Exception if the file could not be opened.
     *  \param loggerName name of the logger.
     *  \param level the lowest level of logging.
     *  \param filepath the filepath of the logging file.
     */
    Logger&  init(const std::string& loggerName, Logger::Level level, const std::string& filepath);

    /**
     *  \brief Gets the Logger matching the specified name, or creates it and returns it if asked.
     *  \throw std::out_of_range if create is false and the logger was not initialized.
     *  \param name the logger name.
     *  \return the logger.
     */
    Logger&  getLogger(const std::string& name, bool create = false);

    /**
     *  \brief Deletes a Logger.
     *  \param name name of the logger to remove.
     */
    void  endLogger(const std::string& name);

    /**
     *  \brief Logs the message to the specified logger. If the logger does not exist, creates it.
     *  \param msg the message to log.
     *  \param level the level of logging.
     *  \param loggerName the name of the logger.
     */
    template<typename T>
    void  log(const std::string& loggerName, const T &msg, Logger::Level level) {
      SCOPELOCK(this);
      try {
        // only log if the logger was initialized
        this->getLogger(loggerName).log(msg, level);
      } catch (const std::out_of_range&) {}
    }
  };

  static const BidiMap<Logger::Level, const std::string> LoggerLevelToString = {
    {Logger::Level::DEBUG, std::string("DEBUG")},
    {Logger::Level::INFO, std::string("INFO")},
    {Logger::Level::WARNING, std::string("WARNING")},
    {Logger::Level::ERROR, std::string("ERROR")},
    {Logger::Level::CRITICAL, std::string("CRITICAL")}
  }; /*!< Used to translate the enum Logging::Level to a string. */
}

#define LOG(x, y)              fwk::LoggerManager::get().log("cppframework", (x), (y))
#define DEBUG(x)               fwk::LoggerManager::get().log("cppframework", (x), fwk::Logger::Level::DEBUG)
#define INFO(x)                fwk::LoggerManager::get().log("cppframework", (x), fwk::Logger::Level::INFO)
#define WARNING(x)             fwk::LoggerManager::get().log("cppframework", (x), fwk::Logger::Level::WARNING)
#define ERROR(x)               fwk::LoggerManager::get().log("cppframework", (x), fwk::Logger::Level::ERROR)
#define CRITICAL(x)            fwk::LoggerManager::get().log("cppframework", (x), fwk::Logger::Level::CRITICAL)
#define LOGGER_SET_OFFSET(x)   fwk::LoggerManager::get().getLogger("cppframework").setOffset((x))
#define LOGGER_ADD_OFFSET(x)   fwk::LoggerManager::get().getLogger("cppframework").addOffset((x))
#define LOGGER_DEL_OFFSET(x)   fwk::LoggerManager::get().getLogger("cppframework").delOffset((x))
#define LOGGER_INIT(x)         fwk::LoggerManager::get().init("cppframework", (x))
#define LOGGER_INIT_FILE(x, y) fwk::LoggerManager::get().init("cppframework", (x), (y))
#define LOGGER_END(x)          fwk::LoggerManager::get().endLogger((x))
#define LOGGER_DESTROY         fwk::LoggerManager::destroy()

#endif    /* __LIBRARY_TOOL_LOGGER_HPP__ */

#ifndef    __LIBRARY_CONVERTER_HPP__
#define    __LIBRARY_CONVERTER_HPP__

#include  <string>
#include  <sstream>

namespace fwk {
  /**
   *  \class Converter Library/Tool/Converter.hpp
   *  \brief A class with static methods to convert any type to a string, or from a string, given that there is a `std::ostream& operator<<(std::ostream&, const T&)` and `std::ostream& operator>>(std::ostream&, T&)` overload somewhere.
   */
  class Converter {
  public:
    /**
     *  \brief Converts an object to a string representation.
     *  The type must have a `std::ostream& operator<<(std::ostream&, const T&)` function somewhere.
     *  \param val the value to convert.
     *  \return the string.
     */
    template<typename T>
    static std::string  StringOf(const T&val) {
      std::stringstream ss;

      ss << std::fixed << val;
      return (ss.str());
    }

    /**
     *  \brief Use a string to create an object.
     *  The type must have a `std::ostream& operator>>(std::ostream&, T&)` function somewhere.
     *  \param val the string representation of an object.
     *  \return the object.
     */
    template<typename T>
    static T  StringTo(const std::string &val) {
      std::stringstream ss;
      T      ret;

      ss << val;
      ss >> ret;
      return (ret);
    }

    /**
     *  \brief converts a type to another type, using their string representation in the middle.
     *  \param val the value to convert.
     *  \return the converted value.
     */
    template<typename T, typename U>
    static U  convert(const T& val) {
      U      ret;
      std::stringstream  ss;

      ss << val;
      ss >> ret;
      return (ret);
    }
  };
}

#define    StringOfInt32(x)   fwk::Converter::StringOf<uint32_t>(x)
#define    StringToInt32(x)   fwk::Converter::StringTo<uint32_t>(x)

#define    StringOfUInt32(x)  fwk::Converter::StringOf<uint32_t>(x)
#define    StringToUInt32(x)  fwk::Converter::StringTo<uint32_t>(x)

#define    StringOfUInt16(x)  fwk::Converter::StringOf<uint16_t>(x)
#define    StringToUInt16(x)  fwk::Converter::StringTo<uint16_t>(x)

#define    StringOfSize(x)    fwk::Converter::StringOf<size_t>(x)
#define    StringToSize(x)    fwk::Converter::StringTo<size_t>(x)

#define    STROFBOOL(x)       ((x)  ? "true" : "false")
#define    STRTOBOOL(x)       (std::string(x) == "true" ? true : false);


#endif    /* __LIBRARY_CONVERTER_HPP__ */


namespace fwk {
  /**
   *  \class Pool Library/Factory/Pool.hpp
   *  \brief a templated pool to store objects without having to dynamically create them.
   */
  template<class C>
  class Pool :public Lockable, public Initializable {
  private:
    std::queue<C*> _pool; /*!< the collection of objects. */
    size_t         _hydrate; /*!< the number of objects to create when the pool is empty. */
    std::string    _name; /*!< the stored objects' class name. */

  public:
    /**
     *  \brief Constructor of Pool.
     */
    Pool(void):
      Lockable(),
      Initializable(),
      _pool(),
      _hydrate(0),
      _name()
    {}

    /**
     *  \brief Destructor of Pool.
     *
     *  Deletes every object still inside the pool.
     */
    virtual ~Pool(void) {
      this->clear();
    }

  public:
    /**
     *  \brief Initializes the pool.
     *  \throw PoolMemoryExhaustedException the memory is exhausted.
     *  \throw PoolInvalidArgumentsException size and hydrate must be greater than 0, or the memory is exhausted.
     *  Will create a first batch of objects.
     *  \param size the number of objects to create now.
     *  \param hydrate the number of objects to create when the pool is empty.
     *  \param classname the stored objects' classname.
     */
    void init(size_t size, size_t hydrate, const std::string& classname) {
      SCOPELOCK(this);

      if (!this->isInitialized()) {
        this->_hydrate = hydrate;
        this->_name = classname;

        if (!size) {
          throw PoolInvalidArgumentsException(fmt::format("{0}: Pool original size must be greater than 0", classname));
        } else if (!hydrate) {
          throw PoolInvalidArgumentsException(fmt::format("{0}: Pool hydrate size must be greater than 0", classname));
        } else {
          this->hydrate(size);
        }

        this->isInitialized(true);
      }
    }

    /**
     *  \brief Pops an object from the pool.
     *  \throw PoolMemoryExhaustedException the pool is empty and the memory is exhausted.
     *  \throw PoolNotInitializedException the pool is empty and was never initialized.
     *  If the pool is empty, will create the number of objects asked at initialization.
     *  Will set the object's validity to true and its last out of pool time point to current time.
     *  \return the object popped from the pool.
     */
    C*  get(void) {
      SCOPELOCK(this);

      // if the pool is empty, try to hydrate
      if (this->_pool.empty()) {
        // if the pool was never initialized, throw exception.
        if (this->isInitialized()) {
          this->hydrate(this->_hydrate);
        } else {
          throw PoolNotInitializedException();
        }
      }

      C*  element = this->_pool.front();
      this->_pool.pop();
      element->isValid(true);
      element->updateLastOutOfPoolTimePoint();

      return element;
    }

    /**
     *  \brief Does the same as the get method without arguments, and calls the init method of the object with the given arguments.
     *  \throw PoolMemoryExhaustedException the pool is empty and the memory is exhausted.
     *  \throw PoolNotInitializedException the pool is empty and was never initialized.
     *  \param args Arguments passed to the init method of the pooled object. If the pooled object has no init method, this won't compile.
     *  \return the object popped from the pool, initialized.
     */
    template<typename... Args>
    C*  get(const Args&... args) {
      C* item = this->get();
      item->init(args...);
      return item;
    }

    /**
     *  \brief Puts an object back into the pool.
     *  Will reinit the object and set its validity to false.
     *  If the \a element is nullptr, does nothing.
     *  \param element the object to put back into the pool.
     *  \param force true if the object is certified valid to be put into the pool. A null object still won't be added.
     */
    void  push(C* element, bool force = false) {
      if (element != nullptr && (force || element->isValid())) {
        element->reinit();
        element->isValid(false);
        {
          SCOPELOCK(this);
          this->_pool.push(element);
        }
      }
    }

    void clear(void) {
      SCOPELOCK(this);
      while (!(this->_pool.empty())) {
        delete this->_pool.front();
        this->_pool.pop();
      }
    }

  private:
    /**
     *  \brief Creates the specified number of objects and puts them into the pool.
     *  \param size the number of objects to create.
     *  \throw PoolMemoryExhaustedException memory exhausted.
     */
    void  hydrate(size_t size) {
      SCOPELOCK(this);
      try {
        for (size_t i = 0 ; i < size ; ++i) {
          C* element = new C();
          this->push(element, true);
        }
      } catch (const std::bad_alloc&) {
        throw PoolMemoryExhaustedException(fmt::format("{0}: Memory exhausted while hydrating pool", this->_name));
      }
    }
  };
}

#endif    /* __LIBRARY_FACTORY_POOL_HPP__ */


namespace fwk {
  /**
   *  \class APooled Library/Factory/APooled.hh
   *  \brief base class from which any object that will be pooled must inherit.
   */
  template<class C>
  class APooled {
  protected:
    static Pool<C> _pool; /*!< the pool of objects. */

  public:
    /**
     *  \brief Initializes the pool if it was not already created.
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
    static C*  getFromPool(const Args&... args) {
      return APooled<C>::_pool.get(args...);
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


namespace fwk {
  /**
   *  \namespace curlxx
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

#ifndef   __LIBRARY_NETWORK_CURL_MULTIHANDLE_HH__
#define   __LIBRARY_NETWORK_CURL_MULTIHANDLE_HH__

#include  <vector>
#include  <curl/curl.h>


namespace fwk {
  namespace curlxx {
    /**
     *  \class MultiHandle Library/Network/CURL/MultiHandle.hh
     *  \brief Encapsulation of the curl multi handle (see curl man).
     */
    class MultiHandle {
    public:
      static const size_t PIPELINED_REQUESTS_MAX_NB; /*!< max number of requests sent at once (arbitrary, should be small (~10) not to ddos the remote server by mistake). */

    private:
      CURLM* _multiHandle; /*!< the underlying curl multi handle. */
      std::vector<curlxx::EasyHandle*> _easyHandles; /*!< the easy handles used by this multi handle. */

    public:
      /**
       *  \brief Constructor of MultiHandle.
       */
      MultiHandle(void);

      /**
       *  \brief Destructor of MultiHandle.
       */
      virtual ~MultiHandle(void);

    public:
      /**
       *  \brief Finds an EasyHandle given a curl easy handle object.
       *  \throw curlxx::Exception if the curl easy handle object is nullptr.
       *  \param curlHandle the curl easy handle object.
       *  \return the found EasyHandle, or nullptr if not found.
       */
      EasyHandle*  findHandle(CURL* curlHandle);

      /**
       *  \brief Returns true if the multi handle has easy handles.
       *  \return true if the multi handle has easy handles;
       */
      bool  hasHandles(void) const;

    public:
      /**
       *  \brief Encapsulation of the curl_multi_init function.
       *  \throw curlx::Exception if the underlying function failed.
       */
      void  init(void);

      /**
       *  \brief Encapsulation of the curl_multi_add_handle function.
       *  \throw curlx::Exception if the underlying function failed, or if the maximum number of handles was reached.
       *  \param handle the easy handle to add.
       */
      void  addHandle(EasyHandle* handle);

      /**
       *  \brief Encapsulation of the curl_multi_remove_handle function.
       *  \throw curlx::Exception if the underlying function failed, or if the given handle was nullptr.
       *  \param handle the easy handle to remove. If it isn't in the MultiHandle, does nothing.
       */
      void  removeHandle(EasyHandle* handle);

      /**
       *  \brief Encapsulation of the curl_multi_perform function.
       *  \throw curlx::Exception if the underlying function failed.
       *  \return true if the method must be called again (the operation is not done).
       */
      bool  perform(void) const;

      /**
       *  \brief Encapsulation of the curl_multi_timeout function.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param interval a timeval structure which will be filled with the recommended duration of timeout for select.
       */
      void  timeout(struct timeval& interval) const;

      /**
       *  \brief Encapsulation of the curl_multi_fdset function.
       *  If the fdmax is -1 after this function, sleep instead of calling select.
       *  \throw curlx::Exception if the underlying function failed.
       *  \param rset empty read fd_set, or NULL.
       *  \param wset empty write fd_set, or NULL.
       *  \param eset empty exception fd_set, or NULL.
       *  \param fdmax an int which will be set to the maximum fd, or -1 if no socket is yet ready.
       */
      void  fdset(fd_set* rset, fd_set* wset, fd_set* eset, int& fdmax) const;

      /**
       *  \brief Encapsulation of the curl_multi_info_read function.
       *  \return the next CURLMsg in the queue, or NULL (see return value of the curl_multi_info_read function).
       */
      CURLMsg*  infoRead(void) const;

      /**
       *  \brief Cleans up the multi handle and all easy handles.
       */
      void  cleanup(void);
    };
  }
}

#endif    /* __LIBRARY_NETWORK_CURL_MULTIHANDLE_HH__ */

#ifndef    __LIBRARY_NETWORK_CURL_EXCEPTION_HH__
#define    __LIBRARY_NETWORK_CURL_EXCEPTION_HH__


namespace fwk {
  namespace curlxx {
    /**
     *  \class Exception Library/Network/CURL/Exception.hh
     *  \brief Superclass of every exception thrown in CURLxx.
     */
    class Exception :public fwk::Exception {
    public:
      /**
       *  \brief Constructor of Exception.
       *  \param message the error message.
       */
      Exception(const std::string& message) noexcept;

      /**
       *  \brief Destructor of Exception.
       */
      virtual ~Exception(void) noexcept;
    };
  }
}

#endif    /* __LIBRARY_NETWORK_CURL_EXCEPTION_HH__ */

#ifndef   __LIBRARY_TOOL_SIGNAL_HH__
#define   __LIBRARY_TOOL_SIGNAL_HH__

#include <unordered_map>
#include <functional>
#include <csignal>


namespace fwk {
  /**
   *  \class Signal Library/Tool/Signal.hh
   *  \brief A singleton class used to catch system signals.
   */
  class Signal :public Singleton<fwk::Signal>, public Lockable {
    friend class Singleton<fwk::Signal>;
  public:
    /**
     *  \class Type Library/Tool/Signal.hh
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

#ifndef    __LIBRARY_CONSTANT_HH__
#define    __LIBRARY_CONSTANT_HH__

#include  <string>

namespace fwk {
  /**
   *  \namespace Constant Library/Constant.hh
   *  \brief namespace containing useful constant variables.
   */
  namespace Constant {
    static const std::string  undefined = "undefined"; /*!< useful to return a const std::string& */
  }
}

#endif    /* __LIBRARY_CONSTANT_HH__ */

#ifndef    __LIBRARY_TOOL_MACRO_HH__
#define    __LIBRARY_TOOL_MACRO_HH__

#define    MAX(x, y)    ((x) > (y) ? (x) : (y))
#define    MIN(x, y)    ((x) < (y) ? (x) : (y))
#define    ABS(x)       ((x) > 0 ? (x) : (x) * -1)

#endif    /* __LIBRARY_TOOL_MACRO_HH__ */

#ifndef    __LIBRARY_TOOL_RANDOM_HH__
#define    __LIBRARY_TOOL_RANDOM_HH__

namespace fwk {
  /**
   *  \class Random Library/Tool/Random.hh
   *  \brief a class with static methods to generate pseudo-random numbers.
   */
  class Random {
  public:
    /**
     *  \brief Sets the seed for generating pseudo-random numbers.
     *  \param seed the seed.
     */
    static void  setSeed(unsigned int seed);

    /**
     *  \brief Sets the seed to the current timestamp.
     */
    static void  init(void);

    /**
     *  \brief Generates a pseudo-random signed 32 bits integer, between two values.
     *  \param min the minimum value.
     *  \param max the maximum value.
     *  \return the random value.
     */
    static int32_t  getInt(int32_t min, int32_t max);
  };
}

#endif    /* __LIBRARY_TOOL_RANDOM_HH__ */

#ifndef    __LIBRARY_TOOL_ARGUMENTS_HH__
#define    __LIBRARY_TOOL_ARGUMENTS_HH__

#include  <string>
#include  <vector>
#include  <list>
#include  <functional>

namespace fwk {
  /**
   *  \class Arguments Library/Tool/Arguments.hh
   *  \brief Parses the program arguments.
   */
  class Arguments {
  public:

    /**
     *  \struct Arg Library/Tool/Arguments.hh
     *  \brief Structure which contains an argument's flag and list of values.
     */
    struct Arg {
      std::string flag;  /*!< flag of the argument (ex: \a -port). */
      std::vector<std::string> values;  /*!< list of values. */

      /**
       *  \brief Constructor of Arg.
       *
       *  Sets the values to an empty vector.
       *
       *  \param  flag  flag of the argument.
       */
      Arg(const std::string& flag);

      /**
       *  \brief Constructor of Arg.
       *
       *  Copies the values given as parameter.
       *
       *  \param flag flag of the argument.
       *  \param values values of the argument.
       */
      Arg(const std::string& flag, const std::vector<std::string>& values);

      /**
       *  \brief Gets a value at a given index.
       *  \exception std::out_of_range thrown in case the index is not valid.
       *  \param idx index of the value to get.
       *  \return a constant reference on the value.
       */
      const std::string&  operator[](size_t idx) const;
    };

    /**
     *  \struct Expected   Library/Tool/Arguments.hh
     *  \brief Structure which contains the information of an argument that is expected by the program.
     */
    struct Expected {
      std::string                 flag; /*!< flag of the argument. */
      std::string                 type; /*!< type of the argument as a string. */
      std::string                  description; /*!< short description of the argument. */
      bool                        mandatory; /*!< \a true if the argument must be given. */
      std::function<bool (const std::string&)>  typechecker; /*!< a function which tells if the argument is valid (\a nullptr if no verification). */
      size_t                      nbmin; /*!< the minimum number of values of the argument. */
      ssize_t                     nbmax; /*!< the maximum number of values of the argument. Value must be >= to \a nbmin, or \a -1 if no limit. */

      /**
       *  \brief Constructor of Expected argument.
       *
       *  \param flag flag of the argument.
       *  \param type type of the argument as a string.
       *  \param description short description of the argument. Will be used to print the usage.
       *  \param mandatory \a true if the absence of the argument must stop the program.
       *  \param typechecker a function which checks if the argument is valid, or \a nullptr for no verification.
       *  \param nbmin minimum number of values of the argument.
       *  \param nbmax maximal number of values of the argument. Must be >= to \a nbmin or set to \a -1 if no limit.
       */
      Expected(const std::string& flag, const std::string& type, const std::string& description, bool mandatory = false, const std::function<bool (const std::string&)>& typechecker = nullptr, size_t nbmin = 1, ssize_t nbmax = 1);
    };

    /**
     *  \struct Incompatible   Library/Tool/Arguments.hh
     *  \brief Structure which contains a set of incompatible flags.
     */
    struct Incompatible {
      std::list<std::string>  flags; /*!< list of flags that are mutually incompatible. */
      bool          oneMandatory; /*!< \a true if one of the flags must be given. */

      /**
       *  \brief Constructor of Incompatible arguments.
       *
       *  \param flags list of flags that are mutually incompatible.
       *  \param mandatory \a true if one of the flags must be present in the command line.
       */
      Incompatible(const std::list<std::string>& flags, bool mandatory);
    };

  private:
    int                      _ac; /*!< number of arguments of the program. */
    char**                   _av; /*!< unparsed arguments of the program. */
    std::list<Arg*>          _arguments; /*!< parsed arguments of the program. */
    std::list<Arg*>          _defaultValues; /*!< default values to give to arguments if not present. */
    std::list<Expected*>     _expected;  /*!< list of expected arguments of the program. Arguments which are given in the arguments list but are not expected will be ignored. */
    std::list<Incompatible*> _incompatibleArguments; /*!< list of incompatible list of flags. */

  public:
    /**
     *  \brief Constructor of Arguments.
     */
    Arguments(void);

    /**
     *  \brief Destructor of Arguments.
     */
    virtual ~Arguments(void);

  public:
    /**
     *  \brief Inits the class.
     *
     *  Sets the number and list of arguments of the program which will be parsed.
     *
     *  \param ac number of arguments of the program.
     *  \param av arguments of the program.
     */
    void  init(int ac, char **av);

    /**
     *  \brief Adds a default value to an argument.
     *
     *  Adds a default value to an argument, which is overwritten if the argument is given to the program.
     *
     *  \param flag flag of the argument.
     *  \param value value to add to the list of values of the argument.
     */
    void  defaultValue(const std::string& flag, const std::string& value);

    /**
     *  \brief Adds an expected argument.
     *
     *  \param expected an expected argument.
     */
    void  expected(Expected *expected);

    /**
     *  \brief Adds a expected argument.
     *
     *  The expected argument is created then added to the list of expected arguments.
     *  
     *  \param flag flag of the argument.
     *  \param type type of the argument as a string.
     *  \param description short description of the argument. Will be used to print the usage.
     *  \param mandatory \a true if the absence of the argument must stop the program.
     *  \param typechecker a function which checks if the argument is valid, or \a nullptr for no verification.
     *  \param nbmin minimum number of values of the argument.
     *  \param nbmax maximal number of values of the argument. Must be >= to \a nbmin or set to \a -1 if no limit.
     */
    void  expected(const std::string& flag, const std::string& type, const std::string& description, bool mandatory = false, const std::function<bool (const std::string&)>& typechecker = nullptr, size_t nbmin = 1, ssize_t nbmax = 1);

    /**
     *  \brief Adds a list of incompatible flags.
     *
     *  \exception ::Exception thrown if one of the flag is not in the expected list.
     *  \param incompatible Incompatible flags.
     */
    void  incompatible(Incompatible* incompatible);

    /**
     *  \brief Adds a list of incompatible flags.
     *
     *  \exception ::Exception thrown if one of the flag is not in the expected list.
     *  \param flags list of flags that are mutually incompatible.
     *  \param mandatory \a true if one of the flags must be present in the command line.
     */
    void  incompatible(const std::list<std::string>& flags, bool mandatory);

    /**
     *  \brief Parses the arguments and checks their validity.
     *
     *  \return \a true if the arguments are valid.
     */
    bool  run(void);

    /**
     *  \brief Gets an argument.
     *  \exception std::out_of_range thrown if the argument is not present.
     *  \param flag flag of the argument to return.
     *  \return a constant pointer on the argument.
     */
    const Arg*  arg(const std::string& flag) const;

    /**
     *  \brief Gets an argument.
     *  \exception std::out_of_range thrown if the argument is not present.
     *  \param flag flag of the argument to return.
     *  \return a constant reference on the argument.
     */
    const Arg&  operator[](const std::string& flag) const;

    /**
     *  \brief Usage of the program.
     *  \return a string describing the usage of the program.
     */
    const std::string  usage(void) const;

  private:
    /**
     *  \brief Checks the validity of the arguments.
     *  \return \a true if the arguments are valid.
     */
    bool  check(void);

    /**
     *  \brief Finds an argument.
     *
     *  Finds an argument and may create it if it is not found.
     *
     *  \param args the arguments list to search in.
     *  \param flag flag of the argument to find.
     *  \param create \a true if the argument must be created if it is not found.
     *  \return a pointer on the Arg instance, or \a nullptr if not found and \a create is \a false.
     */
    Arg* find(std::list<Arg*>& args, const std::string& flag, bool create);

    /**
     *  \brief Finds an argument.
     *  \param args the arguments list to search in.
     *  \param flag flag of the argument to find.
     *  \return a constant pointer on the Arg instance, or \a nullptr if not found.
     */
    const Arg* find(const std::list<Arg*>& args, const std::string& flag) const;

    /**
     *  \brief Checks if a flag is expected.
     *  \param flag flag of the argument.
     *  \return \a true if the argument is expected.
     */
    bool isExpected(const std::string& flag) const;

    /**
     *  \brief Checks if a flag is part of an incompatible set of flags.
     *  \param flag flag of the argument.
     *  \return \a true if the argument is part of an incompatible set of flags.
     */
    bool isIncompatible(const std::string& flag) const;

    /**
     *  \brief Finds an expected argument from its flag.
     *  \param flag flag of the argument.
     *  \return a constant pointer on the expected argument, or \a nullptr if it is not found.
     */
    const Expected* getExpected(const std::string& flag) const;
  };
}

#endif    /* __LIBRARY_TOOL_ARGUMENTS_HH__ */

#ifndef    __LIBRARY_TOOL_STRING_HH__
#define    __LIBRARY_TOOL_STRING_HH__

#include  <string>

namespace fwk {
  /**
   *  \brief Removes all white spaces at the beginning and the end of the string.
   *  \param str a reference on the string to trim.
   */
  void  trim(std::string& str);

  /**
   *  \brief Removes all white spaces at the beginning of the string.
   *  \param str a reference on the string to trim.
   */
  void  ltrim(std::string& str);

  /**
   *  \brief Removes all white spaces at the end of the string.
   *  \param str a reference on the string to trim.
   */
  void  rtrim(std::string& str);
}

#endif    /* __LIBRARY_TOOL_STRING_HH__ */

#ifndef    __LIBRARY_TOOL_TYPECHECKER_HH__
#define    __LIBRARY_TOOL_TYPECHECKER_HH__

#include  <string>

namespace fwk {
  /**
   *  \class TypeChecker Library/Tool/TypeChecker.hh
   *  \brief a class with static methods to check if a string represents specific types.
   */
  class TypeChecker {
  public:
    /**
     *  \brief checks if a string represents a signed integer.
     *  \param str the string.
     *  \return true if the string represents a signed integer.
     */
    static bool  isInteger(const std::string& str);

    /**
     *  \brief checks if a string represents an unsigned integer.
     *  \param str the string.
     *  \return true if the string represents an unsigned integer.
     */
    static bool  isUnsignedInteger(const std::string& str);

    /**
     *  \brief checks if a string represents an unsigned hexadecimal integer, without the "0x" prefix.
     *  \param str the string.
     *  \return true if the string represents an hexadecimal integer.
     */
    static bool  isHexadecimal(const std::string& str);

    /**
     *  \brief checks if a string represents an MD5 hash (32 characters hexadecimal value).
     *  \param str the string.
     *  \return true if the string represents an MD5 hash.
     */
    static bool  isMD5Hash(const std::string& str);

  private:
    /**
     *  \brief Checks if every character of str are part of the characters string.
     *  \param str the string.
     *  \param characters the characters.
     *  \return true if every character of str are part of the characters string.
     */
    static bool  charactersIn(const std::string& str, const std::string& characters);
  };
}

#endif    /* __LIBRARY_TOOL_TYPECHECKER_HH__ */

#ifndef    __LIBRARY_PROPERTY_AENDABLE_HH__
#define    __LIBRARY_PROPERTY_AENDABLE_HH__

namespace fwk {
  /**
   *  \class AEndable Library/Property/AEndable.hh
   *  \brief Abstract class from which all classes than can be stopped by SIGINT inherit.
   */
  class AEndable {
  protected:
    bool  _end; /*!< set to true when the child class must end. */

  public:
    /**
     *  \brief Constructor of AEndable.
     *
     *  Sets the end state to false.
     */
    AEndable(void);

    /**
     *  \brief Destructor of AEndable.
     */
    virtual ~AEndable(void);

  public:
    /**
     *  \brief Tells the child class that it must stop.
     */
    virtual void  end(void) = 0;

  public:
    /**
     *  \brief Checks if the end method has been called.
     *  \return true if the end method has been called.
     */
    bool  mustEnd(void) const;

    /**
     *  \brief Changes the end states of the object.
     *  \param state the new state.
     */
    void  mustEnd(bool state);
  };
}

#endif    /* __LIBRARY_PROPERTY_AENDABLE_HH__ */

#ifndef    __LIBRARY_THREADING_NOTIFIABLE_HPP__
#define    __LIBRARY_THREADING_NOTIFIABLE_HPP__

#include  <condition_variable>
#include  <chrono>
#include  <thread>


namespace fwk {
  /**
   *  \class Notifiable Library/Threading/Notifiable.hpp
   *  \brief A condition variable (synchronization primitive) using a reentrant lock (or recursive mutex).
   */
  class Notifiable :public Lockable {
  private:
    std::condition_variable_any _condvar; /*!< the basic condition variable. */

  public:
    /**
     *  \brief Constructor of Notifiable.
     */
    Notifiable(void);

    /**
     *  \brief Destructor of Notifiable.
     */
    virtual ~Notifiable(void);

  public:
    /**
     *  \brief Waits for the condition variable to be notified. Will unlock the mutex, wait, and lock it again before returning.
     */
    void  wait(void);

    /**
     *  \brief Wakes one thread waiting on this condition variable.
     */
    void  notify(void);

    /**
     *  \brief Wakes all threads waiting on this condition variable.
     */
    void  notify_all(void);

    /**
     *  \brief Waits until this condition variable is notified and the predicate is true.
     *  \param pred the predicate.
     */
    template <class Predicate>
    void  wait(Predicate pred) {
      this->_condvar.wait(*this, pred);
    }

    /**
     *  \brief Waits until this condition variable is notified, with a timeout.
     *  \param tp the point in time when the condition variable must be woken.
     */
    template<class C, class D>
    std::cv_status  wait_until(const std::chrono::time_point<C, D>& tp) {
      return this->_condvar.wait_until(*this, tp);
    }

    /**
     *  \brief Waits until this condition variable is notified, with a timeout, and checking that a predicate is satisfied.
     *  \param timeout_time the point in time when the condition variable must be woken.
     *  \param pred the predicate.
     */
    template<class C, class D, class P>
    bool  wait_until(const std::chrono::time_point<C, D>& timeout_time, P pred) {
      return this->_condvar(*this, timeout_time, pred);
    }

    /**
     *  \brief Waits until this condition variable is notified, with a timeout.
     *  \param rl the maximum duration this condition variable is locked.
     */
    template<class R, class P>
    std::cv_status  wait_for(const std::chrono::duration<R, P>& rl) {
      return this->_condvar.wait_for(*this, rl);
    }

    /**
     *  \brief Waits until this condition variable is notified, with a timeout, and checking that a predicate is satisfied.
     *  \param rel_time the maximum duration this condition variable is locked.
     *  \param pred the predicate.
     */
    template<class R, class Per, class Pre>
    bool  wait_for(const std::chrono::duration<R, Per>& rel_time, Pre pred) {
      return this->_condvar.wait_for(*this, rel_time, pred);
    }
  };

  /**
   *  \class TNotifiable Library/Threading/Notifiable.hpp
   *  \brief A templated class to use when setting an object notifiable.
   *  Can be useful for notifying object of the STL, like `TNotifiable<std::list<int>>`.
   */
  template<class C>
  class TNotifiable :public C, public Notifiable {
  public:
    /**
     *  \brief Default constructor of TNotifiable.
     *  \param args the arguments which will be passed to the constructor of the templated class.
     */
    template<typename... Args>
    TNotifiable(const Args&... args):
      C(args...),
      Notifiable()
    {}

    /**
     *  \brief Destructor of TNotifiable.
     */
    virtual ~TNotifiable(void) {}
  };

  /**
   *  \class TNotifiableThread Library/Threading/Notifiable.hpp
   *  \brief A struct containing a pointer to a thread, and a Notifiable.
   *
   *  I'm not using Notifiable<std::thread*> because I'm not sure of everything this implies.
   */
  struct NotifiableThread {
    std::thread* thread;
    Notifiable   condition;
  };
}

#endif    /* __LIBRARY_THREADING_NOTIFIABLE_HPP__ */

#ifndef    __LIBRARY_COLLECTION_ARRAY_HPP__
#define    __LIBRARY_COLLECTION_ARRAY_HPP__

#include  <iostream>

namespace fwk {
  /**
   *  \struct Array2 Library/Collection/Array.hpp
   *  \brief Templated structure which contains two values of a same type.
   */
  template<typename T>
  struct Array2 {
  public:
    T  x; /*!< first value of type T */
    T  y; /*!< second value of type T */

    /**
     *  \brief Contructor of Array2.
     *
     *  Sets x and y to 0.
     */
    Array2(void): x(0), y(0) {}

    /**
     *  \brief Contructor of Array2.
     *  \param x first value of the Array2.
     *  \param y second value of the Array2.
     */
    Array2(T x, T y): x(x), y(y) {}

    /**
     *  \brief Copy Contructor of Array2.
     *  \param oth the Array2 to copy. Must be of the same type.
     */
    Array2(const Array2<T> &oth): x(oth.x), y(oth.y) {}

    /**
     *  \brief Assignment Contructor of Array2.
     *  \param oth the Array2 to copy. Must be of the same type.
     */
    Array2&  operator=(const Array2<T> &oth) {
      this->x = oth.x;
      this->y = oth.y;
      return (*this);
    }

    /**
     *  \brief Adds the x and y values of the Array2 in parameter to the x and y values in the current object.
     *  \param oth the Array2 whose values to add. Must be of the same type.
     */
    Array2&  operator+=(const Array2<T> &oth) {
      this->x = this->x + oth.x;
      this->y = this->y + oth.y;
      return (*this);
    }

    /**
     *  \brief Subs the x and y values of the Array2 in parameter to the x and y values in the current object.
     *  \param oth the Array2 whose values to sub. Must be of the same type.
     */
    Array2&  operator-=(const Array2<T> &oth) {
      this->x = this->x - oth.x;
      this->y = this->y - oth.y;
      return (*this);
    }

    /**
     *  \brief Multiplies the x and y values of the Array2 in parameter with the x and y values in the current object.
     *  \param oth the Array2 whose values to multiply. Must be of the same type.
     */
    Array2&  operator*=(const Array2<T> &oth) {
      this->x = this->x * oth.x;
      this->y = this->y * oth.y;
      return (*this);
    }

    /**
     *  \brief Divides the x and y values of current object by the x and y values of the Array2 in parameter.
     *  \param oth the Array2 whose values will divide the values of the current object. Must be of the same type.
     */
    Array2&  operator/=(const Array2<T> &oth) {
      this->x = this->x / oth.x;
      this->y = this->y / oth.y;
      return (*this);
    }

    /**
     *  \brief Does a modulo between the x and y values of current object and the x and y values of the Array2 in parameter.
     *  \param oth the Array2 whose values will be used to do the modulo. Must be of the same type.
     */
    Array2&  operator%=(const Array2<T> &oth) {
      this->x = this->x % oth.x;
      this->y = this->y % oth.y;
      return (*this);
    }

    /**
     *  \brief Destructor of Array2.
     */
    ~Array2(void) {}
  };

  /**
   *  \struct Array3 Library/Collection/Array.hpp
   *  \brief Templated structure which contains three values of a same type.
   */
  template<typename T>
  struct Array3 {
  public:
    T    x; /*!< first value of type T */
    T    y; /*!< second value of type T */
    T    z; /*!< third value of type T */

    /**
     *  \brief Contructor of Array3.
     *
     *  Sets x, y and z to 0.
     */
    Array3(void): x(0), y(0), z(0) {}
    
    /**
     *  \brief Contructor of Array3.
     *  \param x first value of the Array3.
     *  \param y second value of the Array3.
     *  \param z third value of the Array3.
     */
    Array3(T x, T y, T z): x(x), y(y), z(z) {}

    /**
     *  \brief Copy Contructor of Array3.
     *  \param oth the Array3 to copy. Must be of the same type.
     */
    Array3(const Array3<T> &oth): x(oth.x), y(oth.y), z(oth.z) {}

    /**
     *  \brief Assignment Contructor of Array3.
     *  \param oth the Array3 to copy. Must be of the same type.
     */
    Array3&  operator=(const Array3<T> &oth) {
      this->x = oth.x;
      this->y = oth.y;
      this->z = oth.z;
      return (*this);
    }

    /**
     *  \brief Adds the x y, and z values of the Array3 in parameter to the x, y and z values in the current object.
     *  \param oth the Array3 whose values to add. Must be of the same type.
     */
    Array3&  operator+=(const Array3<T> &oth) {
      this->x = this->x + oth.x;
      this->y = this->y + oth.y;
      this->z = this->z + oth.z;
      return (*this);
    }

    /**
     *  \brief Subs the x, y, and z values of the Array3 in parameter to the x, y, and z values in the current object.
     *  \param oth the Array3 whose values to sub. Must be of the same type.
     */
    Array3&  operator-=(const Array3<T> &oth) {
      this->x = this->x - oth.x;
      this->y = this->y - oth.y;
      this->z = this->z - oth.z;
      return (*this);
    }

    /**
     *  \brief Multiplies the x, y, and z values of the Array3 in parameter with the x, y, and z values in the current object.
     *  \param oth the Array3 whose values to multiply. Must be of the same type.
     */
    Array3&  operator*=(const Array3<T> &oth) {
      this->x = this->x * oth.x;
      this->y = this->y * oth.y;
      this->z = this->z * oth.z;
      return (*this);
    }

    /**
     *  \brief Divides the x, y, and z values of current object by the x, y, and z values of the Array3 in parameter.
     *  \param oth the Array3 whose values will divide the values of the current object. Must be of the same type.
     */
    Array3&  operator/=(const Array3<T> &oth) {
      this->x = this->x / oth.x;
      this->y = this->y / oth.y;
      this->z = this->z / oth.z;
      return (*this);
    }

    /**
     *  \brief Does a modulo between the x, y, and z values of current object and the x, y, and z values of the Array3 in parameter.
     *  \param oth the Array3 whose values will be used to do the modulo. Must be of the same type.
     */
     Array3&  operator%=(const Array3<T> &oth) {
      this->x = this->x % oth.x;
      this->y = this->y % oth.y;
      this->z = this->z % oth.z;
      return (*this);
    }

    /**
     *  \brief Destructor of Array3.
     */
    ~Array3(void) {}
  };

  /**
   *  \brief Adds two Array2.
   *  \param a the first Array2.
   *  \param b the second Array2.
   *  \return an Array2 with `x = a.x + b.x` and `y = a.y + b.y`.
   */
  template<typename T>
  Array2<T>  operator+(const Array2<T> &a, const Array2<T> &b) {
    return (Array2<T>(a.x + b.x, a.y + b.y));
  }

  /**
   *  \brief Subs two Array2.
   *  \param a the first Array2.
   *  \param b the second Array2.
   *  \return an Array2 with `x = a.x - b.x` and `y = a.y - b.y`.
   */
  template<typename T>
  Array2<T>  operator-(const Array2<T> &a, const Array2<T> &b) {
    return (Array2<T>(a.x - b.x, a.y - b.y));
  }

  /**
   *  \brief Multiplies two Array2.
   *  \param a the first Array2.
   *  \param b the second Array2.
   *  \return an Array2 with `x = a.x * b.x` and `y = a.y * b.y`.
   */
  template<typename T>
  Array2<T>  operator*(const Array2<T> &a, const Array2<T> &b) {
    return (Array2<T>(a.x * b.x, a.y * b.y));
  }

  /**
   *  \brief Divides two Array2.
   *  \param a the first Array2.
   *  \param b the second Array2.
   *  \return an Array2 with `x = a.x / b.x` and `y = a.y / b.y`.
   */
  template<typename T>
  Array2<T>  operator/(const Array2<T> &a, const Array2<T> &b) {
    return (Array2<T>(a.x / b.x, a.y / b.y));
  }

  /**
   *  \brief Does a modulo between two Array2.
   *  \param a the first Array2.
   *  \param b the second Array2.
   *  \return an Array2 with `x = a.x % b.x` and `y = a.y % b.y`.
   */
  template<typename T>
  Array2<T>  operator%(const Array2<T> &a, const Array2<T> &b) {
    return (Array2<T>(a.x % b.x, a.y % b.y));
  }

  /**
   *  \brief Adds two Array3.
   *  \param a the first Array3.
   *  \param b the second Array3.
   *  \return an Array3 with `x = a.x + b.x`, `y = a.y + b.y` and `z = a.z + b.z`.
   */
  template<typename T>
  Array3<T>  operator+(const Array3<T> &a, const Array3<T> &b) {
    return (Array3<T>(a.x + b.x, a.y + b.y, a.z + b.z));
  }

  /**
   *  \brief Subs two Array3.
   *  \param a the first Array3.
   *  \param b the second Array3.
   *  \return an Array3 with `x = a.x - b.x`, `y = a.y - b.y` and `z = a.z - b.z`.
   */
  template<typename T>
  Array3<T>  operator-(const Array3<T> &a, const Array3<T> &b) {
    return (Array3<T>(a.x - b.x, a.y - b.y, a.z - b.z));
  }

  /**
   *  \brief Multiplies two Array3.
   *  \param a the first Array3.
   *  \param b the second Array3.
   *  \return an Array3 with `x = a.x * b.x`, `y = a.y * b.y` and `z = a.z * b.z`.
   */
  template<typename T>
  Array3<T>  operator*(const Array3<T> &a, const Array3<T> &b) {
    return (Array3<T>(a.x * b.x, a.y * b.y, a.z * b.z));
  }

  /**
   *  \brief Divides two Array3.
   *  \param a the first Array3.
   *  \param b the second Array3.
   *  \return an Array3 with `x = a.x / b.x`, `y = a.y / b.y` and `z = a.z / b.z`.
   */
  template<typename T>
  Array3<T>  operator/(const Array3<T> &a, const Array3<T> &b) {
    return (Array3<T>(a.x / b.x, a.y / b.y, a.z / b.z));
  }

  /**
   *  \brief Does a modulo between two Array3.
   *  \param a the first Array3.
   *  \param b the second Array3.
   *  \return an Array3 with `x = a.x % b.x`, `y = a.y % b.y` and `z = a.z % b.z`.
   */
  template<typename T>
  Array3<T>  operator%(const Array3<T> &a, const Array3<T> &b) {
    return (Array3<T>(a.x % b.x, a.y % b.y, a.z % b.z));
  }

  /**
   *  \brief Adds a representation of the Array2 to a stream.
   *
   *  The Array2 is printed as `[x/y]`
   *
   *  \param os the output stream.
   *  \param arr the Array2.
   *  \return a reference on the \a os parameter.
   */
  template<typename T>
  std::ostream&  operator<<(std::ostream &os, const Array2<T> &arr) {
    os << "Array2: [" << arr.x << "/" << arr.y << "]";
    return (os);
  }

  /**
   *  \brief Adds a representation of the Array3 to a stream.
   *
   *  The Array3 is printed as `[x/y/z]`
   *
   *  \param os the output stream.
   *  \param arr the Array3.
   *  \return a reference on the \a os parameter.
   */
  template<typename T>
  std::ostream&  operator<<(std::ostream &os, const Array3<T> &arr) {
    os << "Array3: [" << arr.x << "/" << arr.y << "/" << arr.z << "]";
    return (os);
  }

  typedef    Array2<int32_t>   Array2i;
  typedef    Array2<float>     Array2f;
  typedef    Array2<double>    Array2d;
  typedef    Array2<uint32_t>  Array2u;

  typedef    Array3<int32_t>   Array3i;
  typedef    Array3<float>     Array3f;
  typedef    Array3<double>    Array3d;
  typedef    Array3<uint32_t>  Array3u;
}

#endif    /* __LIBRARY_COLLECTION_ARRAY_HPP__ */

#ifndef    __LIBRARY_COLLECTION_ORDEREDLIST_HPP__
#define    __LIBRARY_COLLECTION_ORDEREDLIST_HPP__

#include  <list>
#include  <functional>

namespace fwk {
  /**
   *  \class OrderedList Library/Collection/OrderedList.hpp
   *  \brief a std::list which keeps its values ordered.
   *
   *  Should not be used for gigantic lists (each push iterates through the list).
   *  To insert values, only the `push()` method should be used, otherwise the values will not be in order.
   *
   *  This is pretty much a std::priority_queue on which you can iterate.
   */
  template<typename T>
  class OrderedList :public std::list<T> {
  private:
    std::function<bool (const T&, const T&)>  _compare; /*!< The function which will be used to compare the values inserted. */

  public:
    /**
     *  \brief Constructor of OrderedList.
     *
     *  The comparison method is a simple `v1 < v2`.
     */
    OrderedList(void):
      std::list<T>(),
      _compare([] (const T& a, const T& b) -> bool { return a < b; })
    {}

    /**
     *  \brief Constructor of OrderedList.
     *  \param f the function which will be used to compare the values (should be used when dealing with a list of pointers).
     */
    OrderedList(const std::function<bool (const T&, const T&)>& f):
      std::list<T>(),
      _compare(f)
    {}

    /**
     *  \brief Destructor of OrderedList.
     */
    virtual ~OrderedList(void) {}

    /**
     *  \brief Inserts the value in the right place inside the list.
     *
     *  Must always be used to keep all values ordered.
     *  \param val the value to insert.
     */
    virtual void  push(const T& val) {
      if (this->empty()) {
        this->push_back(val);
        return ;
      }
      for (auto it = this->begin() ; it != this->end() ; ++it) {
        if (this->_compare(val, *it)) {
          this->insert(it, val);
          return ;
        }
        else if (std::next(it,1) == this->end()) {
          this->push_back(val);
          return ;
        }
      }
    }
  };
}

#endif    /* __LIBRARY_COLLECTION_ORDEREDLIST_HPP__ */

#ifndef    __LIBRARY_COLLECTION_BYTEARRAY_HPP__
#define    __LIBRARY_COLLECTION_BYTEARRAY_HPP__

#include  <iostream>
#include  <cstring>


namespace fwk {
  /**
   *  \class ByteArray Library/Collection/ByteArray.hpp
   *  \brief Raw data container.
   */
  class ByteArray :public Lockable, public APooled<ByteArray> {
  protected:
    uint8_t *_bytearray; /*!< Pointer to the start of the data. */
    size_t  _size; /*!< Size of the data in bytes. */
    size_t  _sizemax; /*!< Maximum size of the data in bytes. */

  public:
    /**
     *  \brief Constructor of ByteArray.
     *  \param size the maximum size of the data in bytes.
     */
    ByteArray(size_t size = 1);

    /**
     *  \brief Copy Constructor of ByteArray.
     *
     *  This will copy the data (and not just a pointer on the data).
     *
     *  \param oth The ByteArray to copy.
     */
    ByteArray(const ByteArray& oth);

    /**
     *  \brief Assignment Constructor of ByteArray.
     *
     *  This copies the data (and not just a pointer on the data).
     *  This overwrites the previous data.
     *  The maximum size of the data will be the highest value between the maximum size of \a oth and the current maximum size.
     *
     *  \param oth The ByteArray to copy.
     */
    ByteArray& operator=(const ByteArray& oth);

    /**
     *  \brief Destructor of ByteArray.
     *
     *  This deletes the data.
     */
    virtual ~ByteArray(void);

  public:
    /**
     *  \brief Reinitializes the ByteArray.
     *
     *  Sets the current size to 0 and sets the data to an array of 0.
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief returns the current size.
     *  \return the current size in bytes.
     */
    size_t  getSize(void) const;

    /**
     *  \brief returns the available space.
     *  \return the available space in bytes (maximum size - current size).
     */
    size_t  availableSpace(void) const;

    /**
     *  \brief returns the maximum size.
     *  \return the maximum size in bytes.
     */
    size_t  getSizeMax(void) const;

    /**
     *  \brief Checks if the ByteArray is empty.
     *  \return \a true if the ByteArray is empty (i.e if the size is 0).
     */
    bool  empty(void) const;

    /**
     *  \brief Checks if the ByteArray is full.
     *  \return \a true if the ByteArray is full (i.e if the available space is 0).
     */
    bool  full(void) const;

    /**
     *  \brief Returns the value of a byte at a given index.
     *  \exception std::out_of_range thrown if \a idx is higher than the current size.
     *  \param idx the index in bytes.
     *  \return The value of the byte at index \a idx.
     */
    uint8_t  operator[](size_t idx) const;

  public:
    /**
     *  \brief Clears the start of the ByteArray.
     *
     *  If the number of bytes to clear is higher than the current size, the ByteArray will simply be cleared to the current size.
     *  This moves the data, so any pointer on the start of the data will now point to the new start.
     *
     *  \param idx the number of bytes to clear.
     */
    void  clearStart(size_t idx);

    /**
     *  \brief Moves the end of the data.
     *  \throw std::out_of_range thrown if the offset is too high.
     *  \param off the number of bytes to move the pointer of.
     */
    void  moveEnd(size_t off);

  public:
    /**
     *  \brief Returns a pointer to the beginning of the data.
     *  \return a pointer to the beginning of the data.
     */
    const uint8_t*  getBytes(void) const;

    /**
     *  \brief Returns a pointer to the end of the data.
     *  \return a pointer to the end of the data.
     */
    uint8_t*  atEnd(void);

    /**
     *  \brief Returns a pointer to the beginning of the data.
     *  \return a pointer to the beginning of the data.
     */
    uint8_t*  atStart(void);

    /**
     *  \brief Returns a pointer to a certain index of the data.
     *  \param idx the index.
     *  \return a pointer to the data at the index \a idx.
     */
    uint8_t*  atIndex(size_t idx);

  public:
    /**
     *  \brief Resizes the ByteArray to a certain size.
     *
     *  Resizes the ByteArray if the new size is higher than the current maximum size, or if \a force is \a true. 
     *
     *  \param size the number of bytes of the new ByteArray.
     *  \param force if \a true, the ByteArray will be resized even if the current maximum size is higher than \a size.
     *  \param keep if \a true, the current data is kept, up to \a size number of bytes.
     */
    void  resize(size_t size, bool force = false, bool keep = false);

    /**
     *  \brief Shortcut to resize (used by the Pool).
     *
     *  \a resize is more explicit, \a init is only meant to be used by the Pool.
     *
     *  \param size the number of bytes of the new ByteArray.
     *  \param force if \a true, the ByteArray will be resized even if the current maximum size is higher than \a size.
     */
    void  init(size_t size, bool force = false);

    /**
     *  \brief Copies data. Resizes the ByteArray if not enough space.
     *
     *  \param ptr a pointer to the start of the data.
     *  \param size the size of the data.
     */
    void  init(const void* ptr, size_t size);

    /**
     *  \brief Seeks a sequence of bytes inside the ByteArray.
     *  \param ptr the start of the sequence to seek.
     *  \param size the number of bytes of the sequence to seek.
     *  \param start the index at which to start the search.
     *  \return the index of the start of the sequence in the ByteArray, or `std::string::npos` if not found.
     */
    size_t  find(const void* ptr, size_t size, size_t start = 0) const;

    /**
     *  \brief Copies data from the start of the ByteArray and removes it from this ByteArray.
     *  \param ptr a pointer to which the data must be copied.
     *  \param size the number of bytes to copy. If higher than the current size, only the current size will be copied.
     *  \param offset the index of the byte where the copy must start.
     *  \return the number of bytes copied.
     */
    size_t  extract(void* ptr, size_t size, size_t offset = 0);

    /**
     *  \brief Copies data from the start of the ByteArray and keeps it in this ByteArray.
     *  \param ptr a pointer to which the data must be copied.
     *  \param size the number of bytes to copy. If higher than the current size, only the current size will be copied.
     *  \param offset the index of the byte where the copy must start.
     *  \return the number of bytes copied.
     */
    size_t  get(void* ptr, size_t size, size_t offset = 0) const;

  public:
    /**
     *  \brief Adds data to the end of the ByteArray.
     *  \throw std::out_of_range thrown if the ByteArray does not have enough space to add the new data.
     *  \param ptr a pointer to the start of the data to be copied.
     *  \param size the number of bytes to copy.
     *  \param resize if \a true, the ByteArray will be resized to have enough space to add the new data (if it didn't before).
     */
    template<typename T>
    void  push(const T* ptr, size_t size, bool resize = false) {
      if (resize) {
        this->resize(this->_size + size, false, true);
      }

      if (size > this->availableSpace()) {
        throw std::out_of_range("ByteArray: full");
      } else {
        memcpy(this->_bytearray + this->_size, reinterpret_cast<const uint8_t*>(ptr), size);
        this->_size += size;
      }
    }

    /**
     *  \brief Adds a value to the end of the ByteArray, in binary (a \a uint32_t would take 4 bytes inside the ByteArray).
     *  \throw std::out_of_range thrown if the ByteArray does not have enough space to add the new data.
     *  \param val a value to copy.
     *  \param resize if \a true, the ByteArray will be resized to have enough space to add the value (if it didn't before).
     */
    template<typename T>
    void  push(const T& val, bool resize = false) {
      this->push(&val, sizeof(T), resize);
    }

    /**
     *  \brief Adds a value to the start of the ByteArray.
     *
     *  The current data is moved, so any pointer to the start of the ByteArray will now point to the start of the new data.
     *
     *  \param ptr a pointer to the start of the data to be copied.
     *  \param size the number of bytes to copy.
     *  \param resize if \a true, the ByteArray will be resized to have enough space to add the new data (if it didn't before).
     */
    template<typename T>
    void  push_front(const T* ptr, size_t size, bool resize = false) {
      if (resize) {
        this->resize(this->_size + size, false, true);
      }
      if (size > this->availableSpace()) {
        throw std::out_of_range("ByteArray: full");
      } else {
        memmove(this->_bytearray + size, this->_bytearray, size);
        memcpy(this->_bytearray, reinterpret_cast<const uint8_t*>(ptr), size);
        this->_size += size;
      }
    }

    /**
     *  \brief Adds a value to the start of the ByteArray, in binary (a \a uint32_t would take 4 bytes inside the ByteArray).
     *
     *  The current data is moved, so any pointer to the start of the ByteArray will now point to the start of the new data.
     *
     *  \throw std::out_of_range thrown if the ByteArray does not have enough space to add the new data.
     *  \param val a value to copy.
     *  \param resize if \a true, the ByteArray will be resized to have enough space to add the value (if it didn't before).
     */
    template<typename T>
    void  push_front(const T& val, bool resize = false) {
      this->push_front(&val, sizeof(T), resize);
    }

    /**
     *  \brief Adds a std::string to the end of the ByteArray.
     *
     *  The number of bytes copied is the result of `str.length()`.
     *
     *  \throw std::out_of_range thrown if the ByteArray does not have enough space to add the string.
     *  \param str a string to copy.
     *  \param resize if \a true, the ByteArray will be resized to have enough space to add the string (if it didn't before).
     */
    void  pushStr(const std::string& str, bool resize = false);

    /**
     *  \brief Adds a std::string to the start of the ByteArray.
     *
     *  The current data is moved, so any pointer to the start of the ByteArray will now point to the start of the new data.
     *  The number of bytes copied is the result of `str.length()`.
     *
     *  \throw std::out_of_range thrown if the ByteArray does not have enough space to add the string.
     *  \param str a string to copy.
     *  \param resize if \a true, the ByteArray will be resized to have enough space to add the string (if it didn't before).
     */
    void  push_frontStr(const std::string& str, bool resize = false);

  public:
    /**
     *  \class Guard Library/Collection/ByteArray.hpp
     *  \brief Returns the ByteArray to its pool in its destructor.
     *  Similar to a lockguard.
     */
    struct Guard {
    public:
      ByteArray*  bytearray; /*!< the ByteArray to return to the pool. */

    public:
      /**
       *  \brief Constructor of ByteArray::Guard.
       *  Stores the ByteArray.
       *  \param b the ByteArray to store.
       */
      Guard(ByteArray* b);

      /**
       *  \brief Destructor of ByteArray::Guard. Returns the stored ByteArray to its pool.
       */
      ~Guard();
    };
  };

  /**
   *  \class ByteArrayExtractor Library/Collection/ByteArray.hpp
   *  \brief an object which will extract data from a ByteArray without altering its data.
   */
  class ByteArrayExtractor {
  private:
    const ByteArray * _bytearray; /*!< a pointer to the ByteArray whose data is being extracted. */
    size_t _offset; /*!< an offset moving after each extraction. */

  public:
    /**
     *  \brief Constructor of ByteArrayExtractor.
     *  \param bytearray the ByteArray from which the data will be extracted.
     *  \param offset the offset from which the extraction will begin.
     */
    ByteArrayExtractor(const ByteArray *bytearray, size_t offset = 0);

    /**
     *  \brief Copy constructor of ByteArrayExtractor.
     *  The pointer to the bytearray will be copied.
     *  \param oth the ByteArrayExtractor to copy.
     */
    ByteArrayExtractor(const ByteArrayExtractor& oth);

    /**
     *  \brief Copy constructor of ByteArrayExtractor.
     *  The pointer to the bytearray will be copied.
     *  \param oth the ByteArrayExtractor to copy.
     */
    ByteArrayExtractor& operator=(const ByteArrayExtractor&);

    /**
     *  \brief Destructor of ByteArrayExtractor.
     */
    ~ByteArrayExtractor(void);

  public:
    /**
     *  \brief  extract a string from the ByteArray.
     *  \param str the string to which the data will be copied.
     *  \param size the number of bytes inside the ByteArray to put inside the string.
     */
    void  extractString(std::string& str, size_t size);

    /**
     *  \brief  extract data from the ByteArray.
     *  Can be used for integer types (uint16_t, etc.).
     *  \param data the object to which the data will be copied.
     */
    template<typename T>
    void  extract(T& data) {
      size_t size = static_cast<size_t>(sizeof(T));
      if (this->_bytearray->getSize() - this->_offset < size) {
        throw std::out_of_range("ByteArrayExtractor::extract: There are not enough bytes left in ByteArray to extract successfully");
      } else {
        this->_bytearray->get(&data, size, this->_offset);
        this->_offset += size;
      }
    }
  };

  /**
   *  \brief Adds a representation of the ByteArray to a stream.
   *
   *  The Array2 is printed in raw data (it may not be readable).
   *
   *  \param os the output stream.
   *  \param bytearray the ByteArray.
   *  \return a reference on the \a os parameter.
   */
  std::ostream&  operator<<(std::ostream& os, const ByteArray& bytearray);
}

#endif    /* __LIBRARY_COLLECTION_BYTEARRAY_HPP__ */

#ifndef   __CORE_FACTORYMANAGER_HH__
#define   __CORE_FACTORYMANAGER_HH__


namespace fwk {
  /**
   *  \class FactoryManager Core/FactoryManager.hh
   *  \brief Singleton class to initialize the Pools.
   */
  class FactoryManager :public Singleton<fwk::FactoryManager>, public Initializable, public Lockable {
    friend class Singleton<fwk::FactoryManager>;
  private:
    /**
     *  \brief Deleted copy constructor of FactoryManager.
     */
    FactoryManager(const FactoryManager&) = delete;

    /**
     *  \brief Deleted move constructor of FactoryManager.
     */
    FactoryManager(const FactoryManager&&) = delete;

    /**
     *  \brief Deleted assignment constructor of FactoryManager.
     */
    FactoryManager&  operator=(const FactoryManager&) = delete;

  private:
    /**
     *  \brief Constructor of FactoryManager.
     */
    FactoryManager(void);

    /**
     *  \brief Destructor of FactoryManager.
     */
    virtual ~FactoryManager(void);

  public:
    /**
     *  \brief Initializes every Pool of the Library and the Core.
     */
    void  init(void);

    /**
     *  \brief Deletes every Pool of the Library and the Core.
     */
    void  end(void);
  };
}

#endif    /* __CORE_FACTORYMANAGER_HH__ */

#ifndef   __CORE_META_HH__
#define   __CORE_META_HH__

#include  <string>

namespace fwk {
  /**
   *  \namespace fwk::Meta
   *  \brief Metadata of the framework
   */
  namespace Meta {
    static const std::string version = "0.1.0"; /*!< last release version. */
  }
}

#endif    /* __CORE_META_HH__ */

#ifndef    __CORE_EXCEPTION_HH__
#define    __CORE_EXCEPTION_HH__


namespace fwk {
  /**
   *  \class CoreException Core/Exception.hh
   *  \brief Superclass of every exception thrown in the Core.
   */
  class CoreException :public fwk::Exception {
  public:
    /**
     *  \brief Constructor of CoreException.
     *  \param message the error message.
     */
    CoreException(const std::string& message) noexcept;

    /**
     *  \brief Destructor of CoreException.
     */
    virtual ~CoreException(void) noexcept;
  };
}

#endif    /* __CORE_EXCEPTION_HH__ */

#ifndef   __CORE_SYSTEM_HH__
#define   __CORE_SYSTEM_HH__

#include  <string>
#include  <map>
#include  <initializer_list>


/**
 *  \namespace fwk
 *  \brief Namespace common to every class of the framework.
 *  Used not to clash with applications.
 */
namespace fwk {
  /**
   *  \class System Core/System.hh
   *  \brief Main class of the framework (intended to be singleton).
   */
  class System :public Lockable, public AEndable {
  private:
    /**
     *  \class Modules Core/System.hh
     *  \brief Enumeration of the initializable modules.
     */
    enum class Modules {
      HTTP,
      UDP,
      TCP,
      WORKERS,
    };

  public:
    Notifiable _endCondition; /*!< the condition variable on which the main thread will wait until its end method is called. */
    std::map<Modules, bool> _modulesInitialized; /*!< state of initialization of each module. */

  public:
    /**
     *  \brief Constructor of System.
     *  Sets each module to uninitialized.
     */
    System(void);

    /**
     *  \brief Destructor of System.
     *  Ends each module and deletes every singleton.
     */
    virtual ~System(void);

  public:
    /**
     *  \brief wakes the main thread.
     */
    virtual void  end(void);

  public:
    /**
     *  \brief Initializes the HTTP module.
     *  \param userAgent the user agent used for HTTP requests.
     */
    virtual void  initHTTP(const std::string& userAgent);

    /**
     *  \brief Stops the HTTP module.
     *  Clears every request and wakes every thread waiting on synchronous requests.
     */
    virtual void  endHTTP(void);

    /**
     *  \brief Initializes the I/O threads of TCP/UDP networking.
     */
    virtual void  initTCP(void);

    /**
     *  \brief Clears the TCP module (close each server and client).
     *  If the UDP module is stopped, stops the I/O threads.
     */
    virtual void  endTCP(void);

    /**
     *  \brief Initializes the I/O threads of TCP/UDP networking.
     */
    virtual void  initUDP(void);

    /**
     *  \brief Clears the UDP module (close each server and client).
     *  If the TCP module is stopped, stops the I/O threads.
     */
    virtual void  endUDP(void);

    /**
     *  \brief Initializes the workers with the given number of threads.
     *  \param nbWorkerThreads the number of worker threads used in the application.
     *  \param enableDelayedTasks activate or not an extra thread for delayed tasks.
     */
    virtual void  initWorkerThreads(size_t nbWorkerThreads, bool enableDelayedTasks);

    /**
     *  \brief Stops the worker threads. Clears every pending tasks.
     */
    virtual void  endWorkerThreads(void);

    /**
     *  \brief Waits until the end method is called.
     */
    virtual void  run(void);

    /**
     *  \brief Ends every module and deletes the singletons.
     */
    virtual void  cleanup(void);
  };
}

#endif    /* __CORE_SYSTEM_HH__ */

#ifndef    __CORE_NETWORK_NETWORKMANAGER_HH__
#define    __CORE_NETWORK_NETWORKMANAGER_HH__

#ifndef   __CORE_NETWORK_TCP_TCPMANAGER_HH__
#define   __CORE_NETWORK_TCP_TCPMANAGER_HH__

#include  <list>
#include  <set>

#ifndef   __CORE_NETWORK_TCP_TCPSOCKET_HH__
#define   __CORE_NETWORK_TCP_TCPSOCKET_HH__

#include  <sys/select.h>


namespace fwk {
  /**
   *  \class TcpSocket Core/Network/Tcp/TcpSocket.hh
   *  \brief A TCP socket.
   */
  class TcpSocket :public Lockable {
  protected:
    int _fd; /*!< fd of the socket */

  public:
    /**
     *  \brief Constructor of TcpSocket.
     */
    TcpSocket(void);

    /**
     *  \brief Destructor of TcpSocket. Closes the socket.
     */
    virtual ~TcpSocket(void);

  public:
    /**
     *  \brief Closes the socket.
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Creates the socket.
     */
    void  socket(void);

    /**
     *  \brief Closes the socket.
     */
    void  close(void);

    /**
     *  \brief Connects to a given hostname:port.
     *  \throw NetworkException impossible to connect.
     *  \param hostname hostname to connect to.
     *  \param port port to connect to.
     */
    void  connect(const std::string& hostname, uint16_t port);

    /**
     *  \brief Binds a given port.
     *  \throw NetworkException impossible to bind the port.
     *  \param port port to bind.
     */
    void  bind(uint16_t port) const;

    /**
     *  \brief Listens on the bound port. Must be called after bind()
     *  \throw NetworkException impossible to listen.
     *  \param backlog see man listen.
     */
    void  listen(int backlog) const;

    /**
     *  \brief Accept a socket. Must be called after bind() et listen()
     *  \throw NetworkException impossible to accept.
     *  \param socket the socket which will be accepted.
     *  \return the IP of the accepted socket.
     */
    uint32_t  accept(TcpSocket* socket) const;

    /**
     *  \brief see man getpeername.
     *  \throw NetworkException getpeername function call failed.
     */
    uint32_t  getpeername(void) const;

    /**
     *  \brief see man getsockname.
     *  \throw NetworkException getsockname function call failed.
     */
    uint32_t  getsockname(void) const;

  public:
    /**
     *  \brief Adds the socket fd to the set and update the max fd number.
     *  \param set the set.
     *  \param max_fd fd the max fd.
     */
    void  addToSet(fd_set& set, int& max_fd) const;

    /**
     *  \return true if the fd is in the set.
     */
    bool  isset(fd_set& set) const;
  };
}

#endif    /* __CORE_NETWORK_TCP_TCPSOCKET_HH__ */

#ifndef   __CORE_NETWORK_TCP_TCPSOCKETSTREAM_HH__
#define   __CORE_NETWORK_TCP_TCPSOCKETSTREAM_HH__


namespace fwk {
  /**
   *  \class TcpSocketStream Core/Network/Tcp/TcpSocketStream.hh
   *  \brief Socket used to receive and send data.
   */
  class TcpSocketStream :public TcpSocket, public APooled<TcpSocketStream> {
  protected:
    ByteArray*  _input; /*!< the input buffer */
    ByteArray*  _output; /*!< the output buffer */

  public:
    static const size_t BUFFER_SIZE; /*!< maximum number of bytes in the input or output buffer before considering it full */

  public:
    /**
     *  \brief Constructor of TcpSocketStream.
     */
    TcpSocketStream(void);

    /**
     *  \brief Destructor of TcpSocketStream.
     */
    virtual ~TcpSocketStream(void);

  public:
    /**
     *  \brief Reinits the TcpSocketStream. Sends back the input and output ByteArrays to their Pool.
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Gets I/O ByteArrays from their Pool.
     */
    void  init(void);

    /**
     *  \return true if the output buffer is not empty.
     */
    bool  hasDataToSend(void) const;

    /**
     *  \brief Adds data to the socket.
     *  \param ptr pointer to the start of the data to send.
     *  \param size size in bytes of the data to send.
     *  \throw NetworkException the output buffer is full.
     */
    void  push(const void* ptr, size_t size);

    /**
     *  \brief Reads from the socket and adds it to the input buffer.
     *  \throw NetworkException the input buffer is full or the socket was closed.
     *  \return the number of bytes read.
     */
    ssize_t  recv(void);

    /**
     *  \brief Send data to the socket.
     *  \throw NetworkException the socket was closed.
     *  \return the number of bytes sent.
     */
    ssize_t  send(void);

  public:
    /**
     *  \brief Gets the first message from the input buffer and removes it.
     *  \param callback a function which takes the input buffer as parameter. This function must return the number of bytes of the first valid message found, or std::npos if not found.
     *  \param dest the buffer into which the message will be stored. It will be resized if necessary.
     */
    size_t  extractData(const std::function<size_t (const ByteArray&)>& callback, ByteArray* dest);

    /**
     *  \brief Gets the data from the input buffer but does not remove it.
     *  \param callback a function which takes the input buffer as parameter. This function must return the number of bytes of the first valid message found, or std::npos if not found.
     *  \param dest the buffer into which the message will be stored. It will be resized if necessary.
     */
    size_t  getData(const std::function<size_t (const ByteArray&)>& callback, ByteArray* dest);

    /**
     *  \brief Finds the size of the first valid message in bytes.
     *  \param callback a function which takes the input buffer as parameter. This function must return the number of bytes of the first valid message found, or std::npos if not found.
     *  \returns the result of the callback.
     */
    size_t  seekData(const std::function<size_t (const ByteArray&)>& callback);
  };
}

#endif    /* __CORE_NETWORK_TCP_TCPSOCKETSTREAM_HH__ */

#ifndef   __CORE_EVENT_EVENTHANDLE_HH__
#define   __CORE_EVENT_EVENTHANDLE_HH__

#ifndef   __CORE_EVENT_IEVENTARGS_HH__
#define   __CORE_EVENT_IEVENTARGS_HH__

namespace fwk {
  /**
   *  \class IEventArgs Core/Event/IEventArgs.hh
   *  \brief Interface of any wrapper of Event arguments.
   *  Event arguments must be pooled.
   */
  class IEventArgs {
  public:
    /**
     *  \brief Destructor of IEventArgs.
     */
    virtual ~IEventArgs(void) {}

  public:
    /**
     *  \brief The child class must use this method to return any pooled object to its pool after the event has been executed by each callback.
     */
    virtual void  cleanup(void) = 0;
  };
}

#endif    /* __CORE_EVENT_IEVENTARGS_HH__ */


namespace fwk {
  /**
   *  \class EventHandle Core/Event/EventHandle.hh
   *  \brief an Event of the program.
   *  Pooled.
   */
  class EventHandle :public APooled<EventHandle> {
  public:
    /**
     *  \brief Constructor of EventHandle.
     */
    EventHandle(void);

  public:
    /**
     *  \brief Inherited from APooled. Does nothing.
     */
    virtual void reinit(void);

  public:
    /**
     *  \brief Shortcut to EventManager::get().fireAsync(event, args);
     *  See EventManager::fireAsync to see the behaviour of the method and the exceptions thrown.
     *  \param args the arguments passed to the subscribers of the event.
     */
    void  fireAsync(IEventArgs* args) const;

    /**
     *  \brief Shortcut to EventManager::get().fireAsync(event, args);
     *  See EventManager::fireSync to see the behaviour of the method and the exceptions thrown.
     *  \param args the arguments passed to the subscribers of the event.
     */
    void  fireSync(IEventArgs* args) const;

    /**
     *  \brief Shortcut to EventManager::get().subscribeToEvent(event, callback, key).
     *  \param callback the function called when the event is fired.
     *  \param key the key of the subscriber (must be unique for this event).
     */
    void  subscribe(const std::function<void (const IEventArgs*)>& callback, const void *key) const;

    /**
     *  \brief Shortcut to EventManager::get().unsubscribeFromEvent(event, key).
     *  \param key the key of the subscriber (must be unique for this event).
     */
    void  unsubscribe(const void *key) const;

    /**
     *  \brief Shortcut to EventManager::get().registerEvent(event).
     */
    void  registerToManager(void) const;

    /**
     *  \brief Shortcut to EventManager::get().unregisterEvent(event).
     */
    void  unregisterFromManager(void) const;
  };
}

#endif    /* __CORE_EVENT_EVENTHANDLE_HH__ */


namespace fwk {
  /**
   *  \class TcpSocketStreamEventArgs Core/Network/Tcp/Manager.hh
   *  \brief Used when firing an event sending a TcpSocketStream as argument.
   */
  struct TcpSocketStreamEventArgs :public IEventArgs, public APooled<TcpSocketStreamEventArgs> {
  public:
    TcpSocketStream* socket; /*!< the SocketStream object. */

  public:
    /**
     *  \brief Constructor of TcpSocketStreamEventArgs.
     */
    TcpSocketStreamEventArgs(void);

  public:
    /**
     *  \brief Reinits the TcpSocketStreamEventArgs object.
     */
    virtual void  reinit(void);

    /**
     *  \brief Sends the SocketStream to its pool. Done automatically after the event has been fired.
     */
    virtual void  cleanup(void);

    /**
     *  \brief Sets the SocketStream.
     *  \param ss the SocketStream.
     */
    void  init(TcpSocketStream* ss);
  };

  /**
   *  \class TcpSocketEventArgs Core/Network/Tcp/Manager.hh
   *  \brief Used when firing an event sending a TcpSocket as argument.
   */
  struct TcpSocketEventArgs :public IEventArgs, public APooled<TcpSocketEventArgs> {
  public:
    TcpSocket* socket; /*!< the Socket object. */

  public:
    /**
     *  \brief Constructor of TcpSocketEventArgs.
     */
    TcpSocketEventArgs(void);

  public:
    /**
     *  \brief Reinits the TcpSocketEventArgs object.
     */
    virtual void  reinit(void);

    /**
     *  \brief Sends the Socket to its pool. Done automatically after the event has been fired.
     */
    virtual void  cleanup(void);

    /**
     *  \brief Sets the Socket.
     *  \param s the Socket.
     */
    void  init(TcpSocket* s);
  };

  /**
   *  \class TcpManager Core/Network/Tcp/TcpManager.hh
   *  \brief TCP TcpManager.
   */
  class TcpManager {
  public:
    /**
     *  \class Server Core/Network/Tcp/TcpManager.hh
     *  \brief TCP server and connected clients.
     */
    struct Server :public Lockable {
    public:
      uint16_t port; /*!< bound port. */
      TcpSocket* server; /*!< socket listening on the bound port. */
      TLockable<std::list<TcpSocketStream*>> clients; /*!< list of connected clients to this server. */
      std::set<uint32_t> accept; /*!< accepted IPs. */
      std::set<uint32_t> blacklist; /*!< rejected IPs. */
      bool active; /*!< the server is running. */

      struct Events {
        EventHandle* onAccept; /*!< Event fired whenever a new client connects to this server. Event argument type: TcpSocketStreamEventArgs. */
        EventHandle* onReceivedData; /*!< Event fired whenever data is read from a client of this server. Event argument type: TcpSocketStreamEventArgs. */
        EventHandle* onClientClosed; /*!< Event fired whenever a client of this server closes. Event argument type: TcpSocketStreamEventArgs. */
        EventHandle* onClosed; /*!< Event fired when the bound socket is closed. Event argument type: TcpSocketEventArgs. */
      } events; /*!< events for this server */

    public:
      /**
       *  \brief Constructor of Server.
       *  \param port bound port.
       *  \param server socket listening to the bound port.
       *  \param accept list of IPs accepted by this server.
       *  \param blacklist list of IPs rejected by this server.
       */
      Server(uint16_t port, TcpSocket* server);

      /**
       *  \brief Destructor of Server.
       */
      virtual ~Server(void);
    };

  public:
    /**
     *  \class Client Core/Network/Tcp/Manager.hh
     *  \brief TCP client.
     */
    struct Client :public Lockable {
    public:
      std::string hostname; /*!< hostname of the TCP socket this client is connected to. */
      uint16_t port; /*!< port of the TCP socket this client is connected to. */
      TcpSocketStream *socket; /*!< connected socket. */
      bool active; /*!< the client is running. */

      struct Events {
        EventHandle* onReceivedData; /*!< Event fired whenever data is read from this socket. Event argument type: TcpSocketStreamEventArgs. */
        EventHandle* onClosed; /*!< Event fired when this socket is closed. Event argument type: TcpSocketStreamEventArgs. */
      } events; /*!< events for this client */

    public:
      /**
       *  \brief Constructor of Client.
       *  \param hostname the hostname of the TCP socket.
       *  \param port the port of the TCP socket.
       *  \param socket the connected socket.
       */
      Client(const std::string& hostname, uint16_t port, TcpSocketStream* socket);

      /**
       *  \brief Destructor of Client.
       */
      virtual ~Client(void);
    };

  public:
    typedef TLockable<std::list<Server>> ServerList;
    typedef TLockable<std::list<Client>> ClientList;

  private:
    ServerList _servers; /*!< bound servers. */
    ClientList _clients; /*!< connected clients. */

    NotifiableThread& _input; /*!< input thread */
    NotifiableThread& _output; /*!< output thread */

  public:
    /**
     *  \brief Constructor of TcpManager.
     *  \param input the input thread.
     *  \param output the output thread.
     */
    TcpManager(NotifiableThread& input, NotifiableThread& output);

    /**
     *  \brief Destructor of TcpManager.
     */
    virtual ~TcpManager(void);

  public:
    /**
     *  \brief Close every server and client.
     */
    void  clear(void);

  public:
    /**
     *  \brief Create a new server.
     *  \param port the port to bind (does not do the binding).
     *  \return the server.
     */
    const Server&  createServer(uint16_t port);

    /**
     *  \brief Bind the server of the given port and make it listen for clients.
     *  \throw NetworkException can't bind the server port.
     *  \param server the server to launch.
     */
    void  run(const Server& server);

    /**
     *  \brief Close the socket bound to a specific port and all its clients.
     *  \param port bound port of the socket to close.
     */
    void  close(uint16_t port);

    /**
     *  \brief Close the socket and all its clients.
     *  \param server the server to close.
     */
    void  close(const Server& server);

    /**
     *  \brief Blacklist an IP on a server.
     *  \param port port on which the server is bound.
     *  \param ip the IP to blacklist.
     */
    void  blacklist(uint16_t port, uint32_t ip);

  public:
    /**
     *  \brief Create a connection to a remote TCP server.
     *  \param hostname hostname of the TCP server to connect to.
     *  \param port port of the TCP server to connect to.
     *  \return the client.
     */
    const Client&  createClient(const std::string& hostname, uint16_t port);

    /**
     *  \brief Make the client connect to the server.
     *  \param client the client.
     */
    void  run(const Client& client);

    /**
     *  \brief Close the socket connected to a specific hostname:port.
     *  \param hostname the hostname the client is connected to.
     *  \param port the port the client is connected to.
     */
    void  close(const std::string& hostname, uint16_t port);

    /**
     *  \brief Close the socket.
     *  \param client the client to close.
     */
    void  close(const Client& client);

  public:
    /**
     *  \brief Push data to a socket. Closes the socket if its output buffer is full.
     *  \param ss the socket.
     *  \param ptr pointer to the start of the data to send.
     *  \param size the number of bytes of data to send.
     */
    void  push(TcpSocketStream* ss, const void* ptr, size_t size);

    /**
     *  \brief Push data to a socket.
     *  \param ss the socket.
     *  \param bytearray the data to send.
     */
    void  push(TcpSocketStream* ss, const ByteArray* bytearray);

  public:
    /**
     *  \brief Add every TCP socket to the set, increments the number of sockets add, and updates the max fd.
     *  \param set the set.
     *  \param max_fd the max fd number.
     *  \param nb the number of sockets added to the set.
     */
    void  fillSetRead(fd_set& set, int& max_fd, uint32_t& nb);

    /**
     *  \brief Add every TCP socket to the set if it has data to send, increments the number of sockets add, and updates the max fd.
     *  \param set the set.
     *  \param max_fd the max fd number.
     *  \param nb the number of sockets added to the set.
     */
    void  fillSetWrite(fd_set& set, int& max_fd, uint32_t& nb);

    /**
     *  \brief Send data of every socket in the set.
     *  \param set the set.
     */
    void  send(fd_set& set);

    /**
     *  \brief Read from every socket in the set.
     *  \param set the set.
     */
    void  recv(fd_set&);

  private:
    /**
     *  \brief Called when there is an I/O exception. Fires the given event with the socket as argument.
     *  \param event the event to fire.
     *  \param ss the socket to send as argument of the event.
     *  \param message the message of the exception.
     */
    void  __onIOException(EventHandle* event, TcpSocketStream* ss, const std::string& message);

    /**
     *  \brief Fires an event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     */
    void  __fireEvent(EventHandle *event, TcpSocketStream* socket) const;

    /**
     *  \brief Fires an event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     */
    void  __fireEvent(EventHandle *event, TcpSocket* socket) const;
  };
}

#endif    /* __CORE_NETWORK_TCP_TCPMANAGER_HH__ */

#ifndef   __CORE_NETWORK_UDP_UDPMANAGER_HH__
#define   __CORE_NETWORK_UDP_UDPMANAGER_HH__

#include  <list>
#include  <set>

#ifndef   __CORE_NETWORK_UDP_UDPSOCKETSERVER_HH__
#define   __CORE_NETWORK_UDP_UDPSOCKETSERVER_HH__

#include  <sys/select.h>

#ifndef   __CORE_NETWORK_UDP_AUDPSOCKET_HH__
#define   __CORE_NETWORK_UDP_AUDPSOCKET_HH__


namespace fwk {
  /**
   *  \class AUdpSocket Core/Network/Udp/AUdpSocket.hh
   *  \brief Superclass of UDP sockets which can be created.
   */
  class AUdpSocket :public virtual Lockable {
  protected:
    int _fd; /*!< file descriptor of the socket */
    ByteArray* _buffer; /*!< buffer used to read */

  public:
    /**
     *  \brief Constructor of AUdpSocket.
     */
    AUdpSocket(void);

    /**
     *  \brief Destructor of AUdpSocket.
     */
    virtual ~AUdpSocket(void);

  public:
    /**
     *  \brief Closes the socket. Sends the buffer back to the pool.
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Gets the buffer from the ByteArray pool.
     */
    virtual void  init(void);

  public:
    /**
     *  \brief Creates the DGRAM socket.
     */
    virtual void  socket(void);

    /**
     *  \brief Closes the socket.
     */
    virtual void  close(void);

  public:
    /**
     *  \brief Adds the socket fd to the set and update the max fd number.
     *  \param set the set.
     *  \param max_fd fd the max fd.
     */
    virtual void  addToSet(fd_set& set, int& max_fd) const;

    /**
     *  \return true if the fd is in the set.
     */
    virtual bool  isset(fd_set&) const;
  };
}

#endif    /* __CORE_NETWORK_UDP_AUDPSOCKET_HH__ */

#ifndef   __CORE_NETWORK_UDP_UDPSOCKETCLIENT_HH__
#define   __CORE_NETWORK_UDP_UDPSOCKETCLIENT_HH__

#include  <sys/select.h>

#ifndef   __CORE_NETWORK_UDP_AUDPSOCKETIO_HH__
#define   __CORE_NETWORK_UDP_AUDPSOCKETIO_HH__

#include  <queue>
#include  <utility>
#include  <netinet/in.h>


namespace fwk {
  /**
   *  \class AUdpSocketIO Core/Network/Udp/AUdpSocketIO.hh
   *  \brief Superclass of UDP socket who can store input and output datagrams.
   */
  class AUdpSocketIO :public virtual Lockable {
  protected:
    sockaddr_in _addr; /*!< address to read from / write to */
    std::pair<std::queue<ByteArray*>, size_t> _input; /*!< queue of datagrams read / total size of queued datagrams in bytes. */
    std::pair<std::queue<ByteArray*>, size_t> _output; /*!< queue of datagrams to send / total size of queued datagrams in bytes. */

  public:
    static const size_t BUFFER_SIZE; /*!< maximum size of datagrams queues in bytes. */

  public:
    /**
     *  \brief Constructor of AUdpSocketIO.
     */
    AUdpSocketIO(void);

    /**
     *  \brief Destructor of AUdpSocketIO.
     */
    virtual ~AUdpSocketIO(void);

  public:
    /**
     *  \brief Reinits the AUdpSocketIO object. Empties the datagram queues and sets the address to 0 (using memset on the struct).
     */
    virtual void  reinit(void);

  public:
    /**
     *  \return true if the output queue is not empty.
     */
    virtual bool  hasDataToSend(void) const;

    /**
     *  \brief Adds a datagram to the output queue.
     *  \throw NetworkException the output queue exceeds the BUFFER_SIZE.
     *  \param datagram the datagram to add.
     */
    virtual void  push(ByteArray* datagram);

    /**
     *  \brief Pops the first datagram of the input queue and returns it. Returns nullptr if the input queue is empty.
     *  \return the first datagram read on this socket, or nullptr if empty.
     */
    virtual ByteArray*  getData(void);

  public:
    /**
     *  \return true if the address is the same as this socket. Compares the protocol, IP and port.
     *  \param addr the address to compare.
     */
    virtual bool  operator==(const sockaddr_in& addr) const;
  };
}

#endif    /* __CORE_NETWORK_UDP_AUDPSOCKETIO_HH__ */


namespace fwk {
  /**
   *  \class UdpSocketClient Core/Network/Udp/UdpSocketClient.hh
   *  \brief Client of a server socket. The address of a client who sent a message to a bound UDP server.
   */
  class UdpSocketClient :public AUdpSocketIO, public APooled<UdpSocketClient> {
  private:
    std::pair<uint32_t, uint16_t> _info; /*!< IP and port of the remote client. */

  public:
    /**
     *  \brief Constructor of UdpSocketClient.
     */
    UdpSocketClient(void);

    /**
     *  \brief Destructor of UdpSocketClient.
     */
    virtual ~UdpSocketClient(void);

  public:
    /**
     *  \brief Reinits the IP / port of the socket.
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Copies the given address.
     *  \param addr the address to copy.
     */
    void  init(const sockaddr_in& addr);

  public:
    /**
     *  \brief Pops the first datagram of the output queue and returns it. Returns nullptr if the output queue is empty.
     *  \return the next datagram to send.
     */
    ByteArray*  nextDatagram(void);

    /**
     *  \brief Adds a datagram to the input queue. This is meant to be used only by the Core.
     *  \throw NetworkException the input queue is full.
     *  \param datagram the datagram received.
     */
    void  received(ByteArray* datagram);

  public:
    /**
     *   \return the address structure.
     */
    const sockaddr_in&  socketAddress(void) const;

  public:
    /**
     *  \return the IP / port of the client.
     */
    const std::pair<uint32_t, uint16_t>&  clientInformation(void) const;
  };
}

#endif    /* __CORE_NETWORK_UDP_UDPSOCKETCLIENT_HH__ */


namespace fwk {
  /**
   *  \class UdpSocketServer Core/Network/Udp/UdpSocketServer.hh
   *  \brief A UDP server socket.
   */
  class UdpSocketServer :public AUdpSocket, public APooled<UdpSocketServer>  {
  public:
    /**
     *  \brief Constructor of UdpSocketServer.
     */
    UdpSocketServer(void);

    /**
     *  \brief Destructor of UdpSocketServer.
     */
    virtual ~UdpSocketServer(void);

  public:
    virtual void reinit(void);

  public:
    /**
     *  \brief Binds the specified port.
     *  \throw NetworkException the binding failed.
     *  \param port the port to bind.
     */
    void  bind(uint16_t port);

  public:
    /**
     *  \brief reads on the socket and fills the given address structure with the address of the client.
     *  \throw NetworkException the recvfrom call failed.
     *  \param addr the address structure which will be filled with the address of the client.
     *  \return the datagram received.
     */
    ByteArray*  recvfrom(struct sockaddr_in& addr);

    /**
     *  \brief Pops the next datagram to send from the client socket and sends it.
     *  \param socket the socket of the client.
     *  \return the number of bytes sent.
     */
    ssize_t  sendto(UdpSocketClient* socket);
  };
}

#endif  /* __CORE_NETWORK_UDP_UDPSOCKETSERVER_HH__ */

#ifndef   __CORE_NETWORK_UDP_UDPSOCKETSTREAM_HH__
#define   __CORE_NETWORK_UDP_UDPSOCKETSTREAM_HH__


namespace fwk {
  /**
   *  \class UdpSocketStream Core/Network/UDP/UdpSocketStream.hh
   *  \brief A UDP client socket ("connected" to a remote server).
   */
  class UdpSocketStream: public AUdpSocketIO, public AUdpSocket, public APooled<UdpSocketStream> {
  public:
    /**
     *  \brief Constructor of UdpSocketStream.
     */
    UdpSocketStream(void);

    /**
     *  \brief Destructor of UdpSocketStream.
     */
    virtual ~UdpSocketStream(void);

  public:
    /**
     *  \brief See AUdpSocket::reinit and AUdpSocketIO::reinit
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Sets the hostname and port of the remote server.
     *  \throw NetworkException gethostbyname failed.
     */
    void  init(const std::string& hostname, uint16_t port);

  public:
    /**
     *  \brief Pops the next datagram and sends it.
     *  \throw NetworkException sendto failed.
     *  \return the result of sendto.
     */
    ssize_t  sendto(void);

    /**
     *  \brief Receives a datagram and adds it to the input queue.
     *  \throw NetworkException recvfrom failed, or input buffer full.
     *  \return the result of recvfrom.
     */
    ssize_t  recvfrom(void);
  };
}

#endif    /* __CORE_NETWORK_UDP_UDPSOCKETSTREAM_HH__ */


namespace fwk {
  /**
   *  \class UdpSocketStreamEventArgs Core/Network/Udp/UdpManager.hh
   *  \brief Used when firing an event sending a UdpSocketStream as argument.
   */
  struct UdpSocketStreamEventArgs :public IEventArgs, public APooled<UdpSocketStreamEventArgs> {
  public:
    UdpSocketStream* socket; /*!< the SocketStream object. */

  public:
    /**
     *  \brief Constructor of UdpSocketStreamEventArgs.
     */
    UdpSocketStreamEventArgs(void);

  public:
    /**
     *  \brief Reinits the UdpSocketStreamEventArgs object.
     */
    virtual void  reinit(void);

    /**
     *  \brief Sends the SocketStream to its pool. Done automatically after the event has been fired.
     */
    virtual void  cleanup(void);

    /**
     *  \brief Sets the SocketStream.
     *  \param socket the SocketStream.
     */
    virtual void  init(UdpSocketStream* socket);
  };

  /**
   *  \class UdpSocketClientEventArgs Core/Network/Udp/UdpManager.hh
   *  \brief Used when firing an event sending a UdpSocketClient as argument.
   */
  struct UdpSocketClientEventArgs :public IEventArgs, public APooled<UdpSocketClientEventArgs> {
  public:
    UdpSocketClient* socket; /*!< the SocketClient object. */

  public:
    /**
     *  \brief Constructor of UdpSocketClientEventArgs.
     */
    UdpSocketClientEventArgs(void);

  public:
    /**
     *  \brief Reinits the UdpSocketClientEventArgs object.
     */
    virtual void  reinit(void);

    /**
     *  \brief Sends the SocketClient to its pool. Done automatically after the event has been fired.
     */
    virtual void  cleanup(void);

    /**
     *  \brief Sets the SocketClient.
     *  \param socket the SocketClient.
     */
    virtual void  init(UdpSocketClient* socket);
  };

  /**
   *  \class UdpSocketServerEventArgs Core/Network/Udp/UdpManager.hh
   *  \brief Used when firing an event sending a UdpSocketServer as argument.
   */
  struct UdpSocketServerEventArgs :public IEventArgs, public APooled<UdpSocketServerEventArgs> {
  public:
    UdpSocketServer* socket; /*!< the SocketServer object. */

  public:
    /**
     *  \brief Constructor of UdpSocketServerEventArgs.
     */
    UdpSocketServerEventArgs(void);

  public:
    /**
     *  \brief Reinits the UdpSocketServerEventArgs object.
     */
    virtual void  reinit(void);

    /**
     *  \brief Sends the SocketServer to its pool. Done automatically after the event has been fired.
     */
    virtual void  cleanup(void);

    /**
     *  \brief Sets the SocketServer.
     *  \param socket the SocketServer.
     */
    virtual void  init(UdpSocketServer* socket);
  };

  /**
   *  \class UdpManager Core/Network/Udp/UdpManager.hh
   *  \brief UDP Manager.
   */
  class UdpManager {
  public:
    /**
     *  \class Server Core/Network/Udp/UdpManager.hh
     *  \brief UDP server and known clients.
     */
    struct Server :public Lockable {
    public:
      uint16_t port; /*!< bound port */
      UdpSocketServer* server; /*!< socket listening on the bound port */
      TLockable<std::list<UdpSocketClient*>> clients; /*!< list of known clients to this server */
      std::set<uint32_t> accept; /*!< accepted IPs */
      std::set<uint32_t> blacklist; /*!< rejected IPs */
      bool active; /*!< the server is running. */

      struct Events {
        EventHandle* onNewClient; /*!< Event fired whenever a new client sends a message to this server. Event argument type: UdpSocketClientEventArgs. */
        EventHandle* onReceivedData; /*!< Event fired whenever data is read from a client of this server. Event argument type: UdpSocketClientEventArgs. */
        EventHandle* onClientClosed; /*!< Event fired whenever a known client is removed. Does not work well as UDP is not a connected protocol. Event argument type: UdpSocketClientEventArgs. */
        EventHandle* onClosed; /*!< Event fired when this server is closed. Event argument type: UdpSocketServerEventArgs. */
      } events; /*!< events for this server */

    public:
      /**
       *  \brief Constructor of Server.
       *  \param port bound port.
       *  \param server socket listening to the bound port.
       *  \param accept list of IPs accepted by this server.
       *  \param blacklist list of IPs rejected by this server.
       */
      Server(uint16_t port, UdpSocketServer* server);

      /**
       *  \brief Destructor of Server.
       */
      virtual ~Server(void);
    };

  public:
    /**
     *  \class Client Core/Network/Udp/UdpManager.hh
     *  \brief UDP client.
     */
    struct Client :public Lockable {
    public:
      std::string hostname; /*!< hostname of the UDP socket this client sends messages to */
      uint16_t port; /*!< port of the UDP socket this client sends messages to */
      UdpSocketStream *socket; /*!< socket */
      bool active; /*!< the client is running. */

      struct Events {
        EventHandle* onReceivedData; /*!< Event fired whenever data is read from this socket. Event argument type: UdpSocketStreamEventArgs. */
        EventHandle* onClosed; /*!< Event fired when this socket is closed. Does not work well with UDP protocol. Event argument type: UdpSocketStreamEventArgs. */
      } events; /*!< events for this client */

    public:
      /**
       *  \brief Constructor of Client.
       *  \param hostname the hostname of the UDP socket to send messages to.
       *  \param port the port of the UDP socket to send messages to.
       *  \param socket the socket.
       */
      Client(const std::string& hostname, uint16_t port, UdpSocketStream* socket);

      /**
       *  \brief Destructor of Client.
       */
      virtual ~Client(void);
    };

  public:
    typedef TLockable<std::list<Server>> ServerList;
    typedef TLockable<std::list<Client>> ClientList;

  private:
    ServerList _servers; /*!< bound servers. */
    ClientList _clients; /*!< client sockets. */

    NotifiableThread& _input; /*!< input thread */
    NotifiableThread& _output; /*!< output thread */

  public:
    /**
     *  \brief Constructor of UdpManager.
     *  \param input the input thread.
     *  \param output the output thread.
     */
    UdpManager(NotifiableThread& input, NotifiableThread& output);

    /**
     *  \brief Destructor of UdpManager.
     */
    virtual ~UdpManager(void);

  public:
    /**
     *  \brief Close every server and client.
     */
    void  clear(void);

  public:
    /**
     *  \brief Bind a socket to a specific port.
     *  \throw NetworkException can't bind this port.
     *  \param port the port to bind.
     *  \param accept the IPs to accept. Leave empty to accept any.
     *  \param blacklist the IPs to blacklist.
     *  \return the server.
     */
    const Server&  createServer(uint16_t port);

    /**
     *  \brief Binds the server on the given port and makes it listen for clients.
     *  \throw NetworkException can't bind the server port.
     *  \param server the server to launch.
     */
    void run(const Server& server);

    /**
     *  \brief Close the socket bound to a specific port and all its clients.
     *  \param port bound port of the socket to close.
     */
    void  close(uint16_t port);

    /**
     *  \brief Close the socket and all its clients.
     *  \param server the server to close.
     */
    void  close(const Server& server);

    /**
     *  \brief Blacklist an IP on a server.
     *  \param port port on which the server is bound.
     *  \param ip the IP to blacklist.
     */
    void  blacklist(uint16_t port, uint32_t ip);

  public:
    /**
     *  \brief create a UDP client to send datagrams to a remote UDP server.
     *  \param hostname hostname of the UDP server to connect to.
     *  \param port port of the UDP server to connect to.
     *  \return the client.
     */
    const Client&  createClient(const std::string& hostname, uint16_t port);

    /**
     *  \brief Make the client start sending datagrams.
     *  \param client the client.
     */
    void run(const Client& client);

    /**
     *  \brief Close the socket connected to a specific hostname:port.
     *  \param hostname the hostname the client is connected to.
     *  \param port the port the client is connected to.
     */
    void  close(const std::string& hostname, uint16_t port);

    /**
     *  \brief Close the socket.
     *  \param client the client to close.
     */
    void  close(const Client& client);

  public:
    /**
     *  \brief Push a new datagram to a socket. Closes the socket if its output buffer is full.
     *  \param socket the socket.
     *  \param ptr pointer to the start of the data to send.
     *  \param size the number of bytes of data to send.
     */
    void  push(AUdpSocketIO* socket, const void* ptr, size_t size);

    /**
     *  \brief Push a new datagram to a socket.
     *  \param socket the socket.
     *  \param bytearray the data to send.
     */
    void  push(AUdpSocketIO* socket, const ByteArray* bytearray);

  public:
    /**
     *  \brief Add every UDP socket to the set, increments the number of sockets add, and updates the max fd.
     *  \param set the set.
     *  \param max_fd the max fd number.
     *  \param nb the number of sockets added to the set.
     */
    void  fillSetRead(fd_set& set, int& max_fd, uint32_t& nb);

    /**
     *  \brief Add every UDP socket to the set if it has data to send, increments the number of sockets add, and updates the max fd.
     *  \param set the set.
     *  \param max_fd the max fd number.
     *  \param nb the number of sockets added to the set.
     */
    void  fillSetWrite(fd_set& set, int& max_fd, uint32_t& nb);

    /**
     *  \brief Send data of every socket in the set.
     *  \param set the set.
     */
    void  send(fd_set& set);

    /**
     *  \brief Read from every socket in the set.
     *  \param set the set.
     */
    void  recv(fd_set&);

  private:
    /**
     *  \brief Called when there is an I/O exception. Fires the given event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     *  \param message the message of the exception.
     */
    void  __onIOException(EventHandle *event, UdpSocketClient *socket, const std::string& message);

    /**
     *  \brief Called when there is an I/O exception. Fires the given event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     *  \param message the message of the exception.
     */
    void  __onIOException(EventHandle *event, UdpSocketStream *socket, const std::string& message);

    /**
     *  \brief Fires an event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     */
    void  __fireEvent(EventHandle *event, UdpSocketStream *socket) const;

    /**
     *  \brief Fires an event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     */
    void  __fireEvent(EventHandle *event, UdpSocketClient *socket) const;

    /**
     *  \brief Fires an event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     */
    void  __fireEvent(EventHandle *event, UdpSocketServer *socket) const;
  };
}

#endif    /* __CORE_NETWORK_UDP_UDPMANAGER_HH__ */


namespace fwk {
  /**
   *  \class NetworkManager Core/Network/NetworkManager.hh
   *  \brief Singleton class handling I/O operations on UDP and TCP networks.
   */
  class NetworkManager :public Singleton<fwk::NetworkManager>, public Lockable, public AEndable, public Initializable {
    friend class Singleton<fwk::NetworkManager>;
  public:
    NotifiableThread _input; /*!< the input thread. */
    NotifiableThread _output; /*!< the output thread. */

    TcpManager _tcp; /*!< the TCP network manager. */
    UdpManager _udp; /*!<  the UDP network manager.*/

  private:
    /**
     *  \brief Deleted copy constructor of NetworkManager.
     */
    NetworkManager(const NetworkManager&) = delete;

    /**
     *  \brief Deleted copy constructor of NetworkManager.
     */
    NetworkManager(const NetworkManager&&) = delete;

    /**
     *  \brief Deleted assignment constructor of NetworkManager.
     */
    NetworkManager&  operator=(const NetworkManager&) = delete;

  private:
    /**
     *  \brief Constructor of NetworkManager.
     */
    NetworkManager(void);

    /**
     *  \brief Destructor of NetworkManager.
     */
    virtual ~NetworkManager(void);

  public:
    /**
     *  \brief Clears the I/O threads of TCP and UDP networks, closing any server and client.
     */
    virtual void  end(void);

  public:
    /**
     *  \brief Creates the I/O threads the first time it is called.
     */
    void  init(void);

    /**
     *  \brief Clears the TCP network, closing any server and client.
     */
    void  endTCP(void);

    /**
     *  \brief Clears the UDP network, closing any server and client.
     */
    void  endUDP(void);

  private:
    /**
     *  \brief Until the end method is called, reads on the UDP/TCP sockets.
     */
    void  inputRoutine(void);

    /**
     *  \brief Until the end method is called, writes on the UDP/TCP sockets.
     */
    void  outputRoutine(void);

  public:
    /**
     *  \return the TCP manager.
     */
    TcpManager&  getTCP(void);

    /**
     *  \return the UDP manager.
     */
    UdpManager&  getUDP(void);
  };
}

#endif    /* __CORE_NETWORK_NETWORKMANAGER_HH__ */

#ifndef    __CORE_NETWORK_EXCEPTION_HH__
#define    __CORE_NETWORK_EXCEPTION_HH__


namespace fwk {  
  /**
   *  \class NetworkException Core/Network/Exception.hh
   *  \brief Superclass of every exception thrown in the Network.
   */
  class NetworkException :public CoreException {
    public:
    /**
     *  \brief Constructor of NetworkException.
     *  \param message the error message.
     */
    NetworkException(const std::string& message) noexcept;

    /**
     *  \brief Destructor of NetworkException.
     */
    virtual ~NetworkException(void) noexcept;
  };
}

#endif    /* __CORE_NETWORK_EXCEPTION_HH__ */

#ifndef   __CORE_NETWORK_HTTP_HTTPPIPELININGCONNECTION_HH__
#define   __CORE_NETWORK_HTTP_HTTPPIPELININGCONNECTION_HH__

#include  <unordered_map>
#include  <tuple>

#ifndef   __CORE_NETWORK_HTTP_HTTPCONNECTION_HH__
#define   __CORE_NETWORK_HTTP_HTTPCONNECTION_HH__

#include  <queue>
#include  <thread>

#ifndef   __CORE_NETWORK_HTTP_HTTPREQUEST_HH__
#define   __CORE_NETWORK_HTTP_HTTPREQUEST_HH__

#include  <functional>

#ifndef   __CORE_NETWORK_HTTP_AHTTPMESSAGE_HH__
#define   __CORE_NETWORK_HTTP_AHTTPMESSAGE_HH__

#include  <map>
#include  <string>


namespace fwk {
  /**
   *  \class AHttpMessage Core/Network/Http/AHttpMessage.hh
   *  \brief Superclass of Requests and Responses.
   */
  struct AHttpMessage {
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

#ifndef   __CORE_NETWORK_HTTP_HTTPRESPONSE_HH__
#define   __CORE_NETWORK_HTTP_HTTPRESPONSE_HH__


namespace fwk {
  /**
   *  \class HttpResponse Core/Network/Http/HttpResponse.hh
   *  \brief a HttpResponse received from a HTTP server.
   */
  struct HttpResponse :public AHttpMessage, public APooled<HttpResponse> {
    /**
     *  \class Status Core/Network/Http/HttpResponse.hh
     *  \brief Meaning of HTTP status.
     */
    enum class Status {
      CONTINUE                        = 100,
      SWITCHING_PROTOCOLS             = 101,
      PROCESSING                      = 102,

      OK                              = 200,
      CREATED                         = 201,
      ACCEPTED                        = 202,
      NON_AUTHORITATIVE_INFORMATION   = 203,
      NO_CONTENT                      = 204,
      RESET_CONTENT                   = 205,
      PARTIAL_CONTENT                 = 206,
      MULTI_STATUS                    = 207,
      IM_USED                         = 226,

      MULTIPLE_CHOICES                = 300,
      MOVED_PERMANENTLY               = 301,
      FOUND                           = 302,
      SEE_OTHER                       = 303,
      NOT_MODIFIED                    = 304,
      USE_PROXY                       = 305,
      TEMPORARY_REDIRECT              = 307,

      BAD_REQUEST                     = 400,
      UNAUTHORIZED                    = 401,
      PAYMENT_REQUIRED                = 402,
      FORBIDDEN                       = 403,
      NOT_FOUND                       = 404,
      METHOD_NOT_ALLOWED              = 405,
      NOT_ACCEPTABLE                  = 406,
      PROXY_AUTHENTICATION_REQUIRED   = 407,
      REQUEST_TIMEOUT                 = 408,
      CONFLICT                        = 409,
      GONE                            = 410,
      LENGTH_REQUIRED                 = 411,
      PRECONDITION_FAILED             = 412,
      REQUEST_ENTITY_TOO_LARGE        = 413,
      REQUEST_URI_TOO_LONG            = 414,
      UNSUPPORTED_MEDIA_TYPE          = 415,
      REQUESTED_RANGE_NOT_SATISFIABLE = 416,
      EXPECTATION_FAILED              = 417,
      UNPROCESSABLE_ENTITY            = 422,
      LOCKED                          = 423,
      FAILED_DEPENDENCY               = 424,
      UPGRADE_REQUIRED                = 426,

      INTERNAL_SERVER_ERROR           = 500,
      NOT_IMPLEMENTED                 = 501,
      BAD_GATEWAY                     = 502,
      SERVICE_UNAVAILABLE             = 503,
      GATEWAY_TIMEOUT                 = 504,
      HTTP_VERSION_NOT_SUPPORTED      = 505,
      INSUFFICIENT_STORAGE            = 507,
      NOT_EXTENDED                    = 510
    };

    uint32_t  status; /*!< status of the response (200, 400, ...). */
    std::string reason; /*!< reason of the response ("Success", "Bad Request", ...). This does not work with cURL. */

    /**
     *  \brief Constructor of HttpResponse.
     */
    HttpResponse(void);

    /**
     *  \brief Destructor of HttpResponse.
     */
    virtual ~HttpResponse(void);

    /**
     *  \brief Reinits the HttpResponse.
     */
    virtual void  reinit(void);
  };
}

#endif    /* __CORE_NETWORK_HTTP_HTTPRESPONSE_HH__ */


namespace fwk {
  /**
   *  \class HttpRequest Core/Network/Http/HttpRequest.hh
   *  \brief A request to send to a HTTP server.
   */
  struct HttpRequest :public AHttpMessage, public APooled<HttpRequest> {
    std::string method; /*!< method (or verb) of the request. */
    std::string url; /*!< url of the request (ex: "/posts"). */
    std::function<void (const HttpResponse *)> success; /*!< callback called when the response has a status < 400. */
    std::function<void (const HttpResponse *)> error; /*!< callback called when the response >= 400. */
    std::function<void (void)> clean; /*!< callback called when the response is not valid. */

    struct {
      bool isFile; /*!< wether of not the request must upload a file. */
      std::string filepath; /*!< path of the file to upload. */
    } file; /*!< information on a file to send. */

    struct {
      bool isAsynchronous; /*!< wether or not the request is synchronous. The other attributes of this structure are invalid if this variable is true. */
      bool isValid; /*!< if false, the thread was only woken because the connection is being closed. */
      Notifiable lock; /*!< Notifiable variable to wait on until the response is read. */
      HttpResponse* response; /*!< the response to the request. This is only set in case of a synchronous request. */
    } asynchronous; /*!< information on a synchronous request. */

    /**
     *  \brief Constructor of HttpRequest.
     */
    HttpRequest(void);

    /**
     *  \brief Destructor of HttpRequest.
     */
    virtual ~HttpRequest(void);

    /**
     *  \brief Reinits the request. Returns the synchronous response to the pool.
     */
    virtual void  reinit(void);

    /**
     *  \brief Returns the synchronous response to the pool.
     */
    void  cleanResponse(void);

    /**
     *  \brief Waits for the synchronous request to end. Never returns if the request was not set synchronous.
     *  \return true if the request received a response. false if the Notifiable variable was notified because the connection is being closed.
     */
    bool  wait(void);

    /**
     *  \brief Wakes the synchronous request.
     *  \param response the http response read. If nullptr, means that the response was not read and the thread was only woken because the client is being shut down.
     */
    void  wake(HttpResponse* response);

    /**
      synchronous request usage:

      HttpRequest* request = HttpRequest::getFromPool();
      request->method = ...;
      request->url = ...;
      ...

      request->asynchronous.isAsynchronous = false;

      HttpClient::get().sendRequest(request, hostname, [port, secureport]);

      if (request->wait()) { handle response}
      else { clean (no response) }

      HttpRequest::returnToPool(request);
    */
  };
}

#endif    /* __CORE_NETWORK_HTTP_HTTPREQUEST_HH__ */

#ifndef   __CORE_NETWORK_HTTP_HTTPPROTOCOL_HH__
#define   __CORE_NETWORK_HTTP_HTTPPROTOCOL_HH__

#include  <string>


namespace fwk {
  /**
   *  \class Protocol Core/Network/Http/HttpProtocol.hh
   *  \brief protocols used in the http client.
   */
  enum class HttpProtocol {
    HTTP,
    HTTPS,
    FTP
  };

  static const BidiMap<HttpProtocol, const std::string> HttpProtocolToString = {
    {HttpProtocol::HTTP, std::string("http")},
    {HttpProtocol::HTTPS, std::string("https")},
    {HttpProtocol::FTP, std::string("ftp")},
  }; /*!< Used to translate the enum Logging::Level to a string. */
}

#endif    /* __CORE_NETWORK_HTTP_HTTPPROTOCOL_HH__ */


namespace fwk {
  /**
   *  \class HttpConnection Core/Network/Http/HttpConnection.hh
   *  \brief An open connection to an HTTP server.
   */
  class HttpConnection :public Lockable, public AEndable {
  protected:
    /**
     *  \class upload_object Core/Network/Http/HttpConnection.hh
     *  \brief used by CURL to send requests.
     */
    struct upload_object {
      const uint8_t* ptr; /*!< A pointer to the HttpResponse. */
      size_t length; /*!< Number of bytes read. */
    };

  protected:
    std::string  _host; /*!< hostname of the HTTP server. */
    uint16_t     _port; /*!< port of the HTTP server. */
    HttpProtocol _protocol; /*!< protocol of the server (HTTP/HTTPS/FTP...) */
    std::string  _userAgent; /*!< user agent used for requests. */
    std::thread  *_thread; /*!< thread used by the connection. */
    TNotifiable<std::queue<HttpRequest*>>  _pendingRequests; /*!< pending requests. */

  public:
    /**
     *  \brief Constructor of HttpConnection.
     *  \param hostname hostname of the HTTP server.
     *  \param port port of the HTTP server.
     *  \param protocol protocol used.
     *  \param userAgent user agent used for sending requests.
     */
    HttpConnection(const std::string &hostname, uint16_t port, HttpProtocol protocol, const std::string& userAgent);

    /**
     *  \brief Destructor of HttpConnection. Calls the end method.
     */
    virtual ~HttpConnection(void);

  public:
    /**
     *  \brief Clears pending requests. Wakes any thread waiting on a synchronous request.
     */
    virtual void  end(void);

  public:
    /**
     *  \brief Starts the thread and starts sending pending requests.
     */
    virtual void  run(void);

    /**
     *  \brief Returns the hostname of the HTTP server.
     *  \return the hostname of the HTTP server.
     */
    const std::string&  getHost(void) const;

    /**
     *  \brief Returns the port of the HTTP server.
     *  \return the port of the HTTP server.
     */
    uint16_t  getPort(void) const;

    /**
     *  \brief Returns the protocol.
     *  \return the protocol.
     */
    HttpProtocol  getProtocol(void) const;

    /**
     *  \brief Adds a request to the queue of pending requests.
     *  \param request the request to add.
     */
    void  addRequest(HttpRequest *request);

  protected:
    /**
     *  \brief Send a request. If the request fails, prepares a default 400 response.
     *  \param request the request to send.
     *  \return the response.
     */
    HttpResponse*  sendRequest(const HttpRequest *request) const;

    /**
     *  \brief Send pending requests and waits for new ones until the end method is called.
     */
    void  routine(void);

    /**
     *  \brief Effectively send a request.
     *  \throw NetworkException if the request failed to send.
     *  \param request the request to send.
     *  \return the response.
     */
    HttpResponse*  exec(const HttpRequest *request) const;

    /**
     *  \brief Prepares a curl easy handle for the request.
     *  \throw NetworkException if the handle failed to prepare.
     *  \param request the request used to prepare the handle.
     *  \param response the pointer to the response used by curl.
     *  \return the handle.
     */
    curlxx::EasyHandle*  prepareHandle(const HttpRequest *request, HttpResponse *response) const;

    /**
     *  \brief Prepares a curl easy handle for the request.
     *  \throw NetworkException if the handle failed to prepare.
     *  \param handle the handle to prepare.
     *  \param request the request used to prepare the handle.
     *  \param response the pointer to the response used by curl.
     */
    void  prepareHandle(curlxx::EasyHandle* handle, const HttpRequest *request, HttpResponse *response) const;

    /**
     *  \brief Sets the response to the specified request. Will add the response to the task queue (asynchronous request) or wake the waiting worker (synchronous request).
     *  \param request the request whose response is being set.
     *  \param response the response read. nullptr if the connection is being shut down.
     */
    void  setResponse(HttpRequest* request, HttpResponse* response) const;

  protected:
    /**
     *  \brief Used by cURL when reading a response.
     */
    static size_t  read_callback(void*, size_t, size_t, void*);

    /**
     *  \brief Used by cURL when sending a request.
     */
    static size_t  write_callback(void*, size_t, size_t, void*);

    /**
     *  \brief Used by cURL when reading a header of a response.
     */
    static size_t  header_callback(void*, size_t, size_t, void*);
  };
}

#endif    /* __CORE_NETWORK_HTTP_HTTPCONNECTION_HH__ */


namespace fwk {
  /**
   *  \class HttpPipeliningConnection Core/Network/Http/HttpPipeliningConnection.hh
   *  \brief A class running on a thread and managing a queue of requests to a HTTP server, pipelining when it can.
   */
  class HttpPipeliningConnection :public HttpConnection {
  private:
    typedef std::unordered_map<curlxx::EasyHandle*, std::tuple<HttpRequest*, HttpResponse*, bool>> HandlesMap;

  public:
    /**
     *  \brief Constructor of HttpPipeliningConnection.
     *  \param hostname the hostname of the HTTP server.
     *  \param port the port of the HTTP server.
     *  \param protocol the protocol used (HTTP/HTTPS/FTP...).
     *  \param userAgent the user agent string used when sending requests.
     */
    HttpPipeliningConnection(const std::string &hostname, uint16_t port, HttpProtocol protocol, const std::string& userAgent);

    /**
     *  \brief Destructor of HttpPipeliningConnection.
     */
    virtual ~HttpPipeliningConnection(void);

  public:
    /**
     *  \brief Starts the thread and starts sending pending requests.
     */
    virtual void  run(void);

  private:
    /**
     *  \brief The internal routine of the thread. Pipelines the requests and handles the responses.
     */
    void  routine(void);

  private:
    /**
     *  \brief Sends multiple requests to a server and handle their answers.
     *  Any error on a request will generate a generic error.
     *  \param pipelined the list of easy handles linked to their requests / responses.
     *  \param multiHandle see CURL man, the handle which will manage the given easy handles.
     */
    void  sendPipeline(HandlesMap& pipelined, curlxx::MultiHandle& multiHandle) const;

    /**
     *  \brief Reads all pending messages received on the multi handle.
     *  Adds HTTP tasks to the worker manager (asynchronous requests) or wakes the waiting threads (synchronous requests).
     *  \param pipelined the list of easy handles linked to their requests / responses.
     *  \param multiHandle see CURL man, the handle which will manage the given easy handles.
     */
    void  getAnswers(HandlesMap& pipelined, curlxx::MultiHandle& multiHandle) const;
  };
}

#endif    /* __CORE_NETWORK_HTTP_HTTPPIPELININGCONNECTION_HH__ */

#ifndef   __CORE_NETWORK_HTTP_HTTPCLIENT_HH__
#define   __CORE_NETWORK_HTTP_HTTPCLIENT_HH__

#include  <vector>


namespace fwk {
  /**
   *  \class HttpClient Core/Network/Http/HttpClient.hh
   *  \brief Singleton class handling HTTP connections.
   */
  class HttpClient: public Singleton<fwk::HttpClient>, public Lockable, public AEndable {
    friend class Singleton<fwk::HttpClient>;
  private:
    std::string _userAgent; /*!< user agent string sent for each request. */
    std::vector<HttpConnection*> _connections; /*!< open http connections. */

  public:
    static const std::string defaultUserAgent; /*!< default user agent string used when the user agent string is empty. */

  private:
    /**
     *  \brief Deleted copy constructor of HttpClient.
     */
    HttpClient(const HttpClient&) = delete;

    /**
     *  \brief Deleted move constructor of HttpClient.
     */
    HttpClient(const HttpClient&&) = delete;

    /**
     *  \brief Deleted assignment constructor of HttpClient.
     */
    HttpClient& operator=(const HttpClient&) = delete;

  private:
    /**
     *  \brief Constructor of HttpClient.
     */
    HttpClient(void);

    /**
     *  \brief Destructor of HttpClient.
     */
    virtual ~HttpClient(void);

  public:
    /**
     *  \brief Closes every open HTTP connections and clears pending requests.
     *  Wakes any thread waiting on a synchronous request.
     */
    virtual void  end(void);

  public:
    /**
     *  \brief Sets the user agent string. Does not change the user agent of open connections.
     *  \param user_agent the user agent string used for every new HTTP connection.
     */
    void  init(const std::string& user_agent);

    /**
     *  \brief Creates a new connection. Requests will not be sent until connection->run() is called.
     *
     *  If a connection to this host/port/protocol already exists,
     *  this returns the existing connection, but does not change the pipelining parameter.
     *
     *  \param hostname hostname of the HTTP server.
     *  \param port port of the HTTP server.
     *  \param protocol protocol used (HTTP/HTTPS/FTP).
     *  \param enablePipelining true to enable HTTP pipelining.
     *  \return the connection created.
     */
    HttpConnection*  initConnection(const std::string& hostname, uint16_t port = 80, HttpProtocol protocol = HttpProtocol::HTTP, bool enablePipelining = true);

    /**
     *  \brief Send a request to a specific open HTTP server.
     *  \param request the request to send.
     *  \param connection the open connection to the HTTP server.
     */
    void  sendRequest(HttpConnection *connection, HttpRequest *request);
  };
}

#endif    /* __CORE_NETWORK_HTTP_HTTPCLIENT_HH__ */

#ifndef   __CORE_WORKER_DELAYEDTASKSTHREAD_HH__
#define   __CORE_WORKER_DELAYEDTASKSTHREAD_HH__

#include  <thread>


namespace fwk {
  /**
   *  \class DelayedTasksThread Core/Worker/DelayedTasksThread.hh
   *  \brief Singleton class running on a thread and adding delayed tasks to the task queue.
   */
  class DelayedTasksThread :public Singleton<fwk::DelayedTasksThread>, public Lockable, public AEndable {
    friend class Singleton<fwk::DelayedTasksThread>;
  private:
    std::thread *_thread; /*!< the thread on which the class runs. */

  private:
    /**
     *  \brief Deleted copy constructor of DelayedTasksThread.
     */
    DelayedTasksThread(const DelayedTasksThread&) = delete;

    /**
     *  \brief Deleted move constructor of DelayedTasksThread.
     */
    DelayedTasksThread(const DelayedTasksThread&&) = delete;

    /**
     *  \brief Deleted assignment constructor of DelayedTasksThread.
     */
    DelayedTasksThread& operator=(const DelayedTasksThread&) = delete;

  private:
    /**
     *  \brief Constructor of DelayedTasksThread.
     */
    DelayedTasksThread(void);

    /**
     *  \brief Destructor of DelayedTasksThread. Ends the thread.
     */
    virtual ~DelayedTasksThread(void);

  public:
    /**
     *  \brief Notifies the thread to stop and waits for it to end.
     */
    virtual void  end(void);

  public:
    /**
     *  \brief Launch the thread.
     */
    void  run(void);

  private:
    /**
     *  \brief Routine run by the thread. Adds DelayedTask objects to the task queue when the delay is over. Ends whenever the end() method is called.
     */
    void  routine(void) const;
  };
}

#endif    /* __CORE_WORKER_DELAYEDTASKSTHREAD_HH__ */

#ifndef    __CORE_WORKER_WORKERTHREAD_HH__
#define    __CORE_WORKER_WORKERTHREAD_HH__

#include  <thread>
#include  <map>

#ifndef    __CORE_WORKER_TASK_HH__
#define    __CORE_WORKER_TASK_HH__

#include  <chrono>
#include  <functional>


namespace fwk {
  /**
   *  \class ATask Core/Worker/Task.hh
   *  \brief Superclass of all tasks of the tasks queue.
   */
  class ATask {
  public:
    /**
     *  \class Source Core/Worker/Task.hh
     *  \brief Enumerations of tasks.
     */
    enum class Source {
      SIMPLE, /*!< SimpleTask */
      EVENT, /*!< EventTask */
      HTTP_CALLBACK, /*!< HttpTask */
      PERIODIC_TASK /*!< PeriodicTask */
    };

  private:
    Source  _source; /*!< the type of task. */

  public:
    /**
     *  \brief Constructor of ATask.
     *  \param source the task type.
     */
    ATask(Source source);

    /**
     *  \brief Destructor of ATask.
     */
    virtual ~ATask(void);

  public:
    /**
     *  \return the type of task.
     */
    Source  getSource(void) const;
  };

  /**
   *  \class SimpleTask Core/Worker/Task.hh
   *  \brief A task with a simple callback to be executed by a worker thread.
   */
  class SimpleTask :public ATask, public APooled<SimpleTask> {
  public:
    std::function<void (void)> _callback; /*!< the callback to be called by the worker thread. */
    std::function<void (void)> _cleanup; /*!< the callback used to clean resources. Called when the tasks queue is being cleared. */

  public:
    /**
     *  \brief Constructor of SimpleTask.
     */
    SimpleTask(void);

    /**
     *  \brief Destructor of SimpleTask.
     */
    virtual ~SimpleTask(void);

  public:
    /**
     *  \brief Sets the callbacks to nullptr.
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Sets the callback. Sets the cleanup method to nullptr.
     *  \param callback the callback to be executed by a worker thread.
     */
    void  init(const std::function<void (void)>& callback);

    /**
     *  \brief Sets the callback and cleanup method.
     *  \param callback the callback to be executed by a worker thread.
     *  \param cleanup the callback to be used to clean resources. Called when the tasks queue is being cleared.
     */
    void  init(const std::function<void (void)>& callback, const std::function<void (void)>& cleanup);
  };

  /**
   *  \class EventTask Core/Worker/Thread.hh
   *  \brief A task created when an event is fired.
   */
  class EventTask :public ATask, public APooled<EventTask> {
  public:
    std::chrono::steady_clock::time_point _eventCreation; /*!< time of event firing. */
    const EventHandle* _event; /*!< the event fired. */
    IEventArgs*        _args; /*!< the event arguments. */

  public:
    /**
     *  \brief Constructor of EventTask.
     */
    EventTask(void);

    /**
     *  \brief Destructor of EventTask.
     */
    virtual ~EventTask(void);

  public:
    /**
     *  brief Sets attributes to null values.
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Sets the event and its arguments.
     *  \param event the fired event.
     *  \param args the event arguments.
     */
    void  init(const EventHandle* event, IEventArgs* args);
  };

  /**
   *  \class HttpTask Core/Worker/Task.hh
   *  \brief A task created after a HTTP response has been received.
   */
  class HttpTask :public ATask, public APooled<HttpTask> {
  public:
    std::function<void (const HttpResponse*)> _callback; /*!< the callback function to call with the response. */
    std::function<void (void)> _cleanup; /*!< the method used to clean resources used with the request. */
    HttpResponse* _response; /*!< the response received. */

  public:
    /**
     *  \brief Constructor of HttpTask.
     */
    HttpTask(void);

    /**
     *  \brief Destructor of HttpTask.
     */
    virtual ~HttpTask(void);

  public:
    /**
     *  \brief Sets the attributes to their null values.
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Sets the callback function, the cleanup function and the HTTP response.
     *  \param callback the callback method to call.
     *  \param cleanup the cleanup method used to clean resources used in the request. It is only called when the tasks queue is being cleared, so never after the callback method.
     *  \param response the HTTP response received.
     */
    void  init(const std::function<void (const HttpResponse*)>& callback, const std::function<void (void)>& cleanup, HttpResponse* response);
  };

  /**
   *  \class PeriodicTask Core/Worker/Task.hh
   *  \brief A task to be executing at regular interval.
   */
  class PeriodicTask :public ATask, public APooled<PeriodicTask> {
  public:
    std::function<void (void)> _callback; /*!< the function to call at regular interval. */
    std::function<void (void)> _clean; /*!< the function to call when the task is canceled or when the tasks queue is being cleared. */
    std::chrono::steady_clock::duration _interval; /*!< the duration between 2 executions of the task. */
    bool  _off; /*!< when set to true, the next execution of the task will call the clean function instead, and the task will be removed. */

  public:
    /**
     *  \brief Constructor of PeriodicTask.
     */
    PeriodicTask(void);

    /**
     *  \brief Destructor of PeriodicTask.
     */
    virtual ~PeriodicTask(void);

  public:
    /**
     *  \brief Sets all variables to their null values.
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Sets the callback function, cleanup function and interval duration.
     *  \param callback the function to call at regular interval.
     *  \param clean the function to call when the periodic task is canceled.
     *  \param interval the interval between 2 executions of the task.
     */
    void  init(const std::function<void(void)>& callback, const std::function<void(void)>& clean, const std::chrono::steady_clock::duration& interval);

    /**
     *  \brief The next execution of the task will call the clean function instead, and the task will be removed.
     */
    void  stop();
  };

  /**
   *  \class DelayedTask Core/Worker/Thread.hh
   *  \brief A task to be executed after a delay.
   */
  class DelayedTask :public APooled<DelayedTask> {
  public:
    ATask*  _task; /*!< the task to be executed after the delay. */
    std::chrono::steady_clock::time_point  _timePoint; /*!< the timepoint when the task must be added to the tasks queue. */

  public:
    /**
     *  \brief Constructor of DelayedTask.
     */
    DelayedTask(void);

    /**
     *  \brief Destructor of DelayedTask.
     */
    virtual ~DelayedTask(void);

  public:
    /**
     *  \brief Sets the task to nullptr.
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Sets the task and time point.
     *  \param task the task to be added to the task queue at the time point.
     *  \param tp the time point when the task must be added to the task queue.
     */
    void  init(ATask* task, const std::chrono::steady_clock::time_point& tp);

    /**
     *  \brief Sets the task and time point.
     *  \param task the task to be added to the task queue after the duration.
     *  \param duration the duration to wait before adding the task to the task queue.
     */
    void  init(ATask* task, const std::chrono::steady_clock::duration& duration);

  public:
    /**
     *  \param oth the DelayedTask to compare.
     *  \return true if the timepoint of the given delayed tasks is higher than the timepoint of the current object.
     */
    bool  operator<(const DelayedTask& oth) const;

    /**
     *  \param oth the DelayedTask to compare.
     *  \return true if the timepoint of the given delayed tasks is lower than the timepoint of the current object.
     */
    bool  operator>(const DelayedTask& oth) const;

    /**
     *  \param task the task to compare.
     *  \return true if the given task is the task to add to the task queue of the current object.
     */
    bool  operator==(const ATask* task) const;
  };
}

#endif    /* __CORE_WORKER_TASK_HH__ */


namespace fwk {
  /**
   *  \class WorkerThread Core/Worker/WorkerThread.hh
   *  \brief A thread handling tasks.
   */
  class WorkerThread :public Lockable, public AEndable {
  private:
    typedef void (*WorkerHandler)(ATask*, bool);
    static const std::map<ATask::Source, WorkerHandler> TaskHandlerMap; /*!< a map redirecting tasks to their handling methods. */

  public:
    /**
     *  \brief Cleans every tasks and clears the tasks queue.
     */
    static void cleanup();

  private:
    size_t _id; /*!< ID of the thread. */
    std::thread *_thread; /*!< thread object. */

  public:
    /**
     *  \brief Constructor of WorkerThread.
     *  \param id thread id.
     */
    WorkerThread(size_t id);

    /**
     *  \brief Destructor of WorkerThread.
     */
    virtual ~WorkerThread(void);

  public:
    /**
     *  \brief Waits until the task being executed is over, then stops the thread.
     */
    virtual void  end(void);

  public:
    /**
     *  \return the ID of the worker thread.
     */
    size_t  getID(void) const;

  public:
    /**
     *  \brief the routine for workers with TASKS assignment.
     */
    void  routine(void);

  public:
    /**
     *  \brief Executes a SimpleTask task.
     *  \throw std::exception whatever exception the SimpleTask throws.
     *  \param task pointer to the SimpleTask.
     *  \param exec true if the task must be executed, or only cleaned if necessary.
     */
    static void  executeSimpleTask(ATask*, bool exec = true);

    /**
     *  \brief Executes a EventTask task.
     *  \throw std::exception whatever exception the EventTask throws.
     *  \param task pointer to the EventTask.
     *  \param exec true if the task must be executed, or only cleaned if necessary.
     */
    static void  executeEventTask(ATask*, bool exec = true);

    /**
     *  \brief Executes a HttpTask task.
     *  \throw std::exception whatever exception the HttpTask throws.
     *  \param task pointer to the HttpTask.
     *  \param exec true if the task must be executed, or only cleaned if necessary.
     */
    static void  executeHttpTask(ATask*, bool exec = true);

    /**
     *  \brief Executes a PeriodicTask task.
     *  \throw std::exception whatever exception the PeriodicTask throws.
     *  \param task pointer to the PeriodicTask.
     *  \param exec true if the task must be executed, or only cleaned if necessary.
     */
    static void  executePeriodicTask(ATask*, bool exec = true);
  };
}

#endif    /* __CORE_WORKER_WORKERTHREAD_HH__ */

#ifndef    __CORE_WORKER_WORKERMANAGER_HH__
#define    __CORE_WORKER_WORKERMANAGER_HH__

#include  <queue>
#include  <chrono>


namespace fwk {
  /**
   *  \class WorkerManager Core/Worker/WorkerManager.hh
   *  \brief Singleton class handling worker threads.
   */
  class WorkerManager :public Singleton<fwk::WorkerManager>, public Lockable, public AEndable {
    friend class Singleton<fwk::WorkerManager>;
  public:
    typedef TNotifiable<std::queue<ATask*>>  TaskQueue;
    typedef TNotifiable<std::priority_queue<DelayedTask*, std::vector<DelayedTask*>, std::function<bool (const DelayedTask*, const DelayedTask*)>>>  DelayedTaskQueue;

  private:
    TaskQueue _pendingTasks; /*!< tasks to execute. */
    DelayedTaskQueue _delayedTasks; /*!< tasks to be executed after a delay. */
    std::vector<WorkerThread*> _workers; /*!< list of workers threads. */

  private:
    /**
     *  \brief Deleted copy constructor of WorkerManager.
     */
    WorkerManager(const WorkerManager&) = delete;

    /**
     *  \brief Deleted move constructor of WorkerManager.
     */
    WorkerManager(const WorkerManager&&) = delete;

    /**
     *  \brief Deleted assignment constructor of WorkerManager.
     */
    WorkerManager&  operator=(const WorkerManager&) = delete;

  private:
    /**
     *  \brief Constructor of WorkerManager.
     */
    WorkerManager(void);

    /**
     *  \brief Destructor of WorkerManager.
     */
    virtual ~WorkerManager(void);

  public:
    /**
     *  \brief Stops every worker threads and clears the tasks queues.
     */
    virtual void  end(void);

  public:
    /**
     *  \return the tasks queue.
     */
    TaskQueue&  getTaskQueue(void);

    /**
     *  \return the delayed tasks queue.
     */
    DelayedTaskQueue&  getDelayedTaskQueue(void);

  public:
    /**
     *  \brief Initializes the worker threads.
     *  \param nbWorkers the number of workers executing the tasks of the task queue.
     *  \param delayedTasks true if a thread must be used for delayed tasks.
     */
    void  init(size_t nbWorkers, bool delayedTasks);

    /**
     *  \brief Adds a Task to the task queue.
     *  \param task the task.
     */
    void  addTask(ATask* task);

    /**
     *  \brief Adds a DelayedTask to the task queue.
     *  \param dtask the delayed task.
     */
    void  addDelayedTask(DelayedTask* dtask);

    /**
     *  \brief Gets a SimpleTask from the pool and adds it to the task queue.
     *  Inits it with the callback funtion.
     *  \param callback the callback of the SimpleTask object.
     */
    void  addSimpleTask(const std::function<void (void)>& callback);

    /**
     *  \brief Gets a SimpleTask from the pool and adds it to the task queue.
     *  Inits it with the callback and cleanup functions.
     *  \param callback the callback function of the SimpleTask object.
     *  \param cleanup the cleanup function of the SimpleTask object.
     */
    void  addSimpleTask(const std::function<void (void)>& callback, const std::function<void (void)>& cleanup);

    /**
     *  \brief Adds an EventTask to the task queue.
     *  \param event the event.
     *  \param args the event arguments.
     */
    void  addEventTask(const EventHandle* event, IEventArgs* args);

    /**
     *  \brief Adds an HttpTask to the task queue.
     *  \param callback the function to call with the response.
     *  \param cleanup the cleanup function.
     *  \param response the HTTP response.
     */
    void  addHttpTask(const std::function<void (const HttpResponse*)>& callback, const std::function<void (void)>& cleanup, HttpResponse* response);

    /**
     *  \brief Adds a DelayedTask to the delayed task queue.
     *  \param task the task to be executed at the specific time point.
     *  \param tp the timepoint at which the task must be executed.
     */
    void  addDelayedTask(ATask* task, const std::chrono::steady_clock::time_point& tp);

    /**
     *  \brief Adds a DelayedTask to the delayed task queue.
     *  \param task the task to be executed after a specific duration.
     *  \param duration the duration to wait before executing the task.
     */
    void  addDelayedTask(ATask* task, const std::chrono::steady_clock::duration& duration);

    /**
     *  \brief Adds a PeriodicTask to the task queue.
     *  \param callback the function to call at regular interval.
     *  \param cleanup the function to call after the periodic task has been canceled.
     *  \param duration the duration between 2 calls of the callback.
     *  \param startNow true if the task must be first executed right away, false if the task must be first executed after the specified duration.
     */
    void  addPeriodicTask(const std::function<void(void)>& callback, const std::function<void(void)>& cleanup, const std::chrono::steady_clock::duration& duration, bool startNow = true);

    /**
     *  \brief Adds a PeriodicTask to the task queue.
     *  \param periodicTask the PeriodicTask.
     *  \param startNow true if the task must be first executed right away, false if the task must be first executed after the specified duration.
     */
    void  addPeriodicTask(PeriodicTask* periodicTask, bool startNow = true);
  };
}

#endif    /* __CORE_WORKER_WORKERMANAGER_HH__ */

#ifndef   __CORE_EVENT_EXCEPTION_HH__
#define   __CORE_EVENT_EXCEPTION_HH__


namespace fwk {
  /**
   *  \class EventNotRegisteredException Core/Event/Exception.hh
   *  \brief Event thrown when trying to subscribe to an unregistered event.
   */
  class EventNotRegisteredException :public CoreException {
  public:
    /**
     *  \brief Constructor of EventNotRegisteredException.
     *  \param message the error message.
     */
    EventNotRegisteredException(const std::string& message = "This event was not registered") noexcept;

    /**
     *  \brief Destructor of EventNotRegisteredException.
     */
    virtual ~EventNotRegisteredException(void) noexcept;
  };
}

#endif    /* __CORE_EVENT_EXCEPTION_HH__ */

#ifndef    __CORE_EVENT_EVENTMANAGER_HH__
#define    __CORE_EVENT_EVENTMANAGER_HH__

#include  <functional>
#include  <unordered_map>

#ifndef   __CORE_EVENT_EVENTINFO_HH__
#define   __CORE_EVENT_EVENTINFO_HH__

#include  <functional>
#include  <map>


namespace fwk {
  /**
   *  \class EventInfo Core/Event/EventInfo.hh
   *  \brief Structure containing information about a registered Event.
   */
  struct EventInfo {
  public:
    const EventHandle * base; /*!< the registered event. */
    std::map<const void *, std::function<void (const IEventArgs*)>> subscribers; /*!< the subscribers of the event. */

  public:
    /**
     *  \brief Constructor of EventInfo.
     *  \param event the event.
     */
    EventInfo(const EventHandle *event);

    /**
     *  \brief Copy constructor of EventInfo.
     *  Copies the pointer to the event and the subscribers.
     *  \param oth the EventInfo object to copy.
     */
    EventInfo(const EventInfo& oth);

    /**
     *  \brief Assignment constructor of EventInfo.
     *  Copies the pointer to the event and the subscribers.
     *  \param oth the EventInfo object to copy.
     */
    EventInfo& operator=(const EventInfo& oth);

  public:
    /**
     *  \brief Adds a subscriber to the event.
     *  \param key the key of the subscriber.
     *  \param callback the callback method to call when the event is fired.
     */
    void  addSubscriber(const void* key, const std::function<void (const IEventArgs*)>& callback);

    /**
     *  \brief Removes a subscriber from the event.
     *  \param key the key used when subscribing.
     */
    void  delSubscriber(const void* key);
  };
}

#endif    /* __CORE_EVENT_EVENTINFO_HH__ */


namespace fwk {
  /**
   *  \class EventManager Core/Event/EventManager.hh
   *  \brief Singleton class used to register events and subscribe to them.
   */
  class EventManager :public Singleton<fwk::EventManager>, public Lockable {
    friend class Singleton<fwk::EventManager>;
  private:
    std::unordered_map<const EventHandle*, EventInfo>  _events; /*!< registered events and their related information. */

  private:
    /**
     *  \brief Deleted copy constructor of EventManager.
     */
    EventManager(const EventManager&) = delete;

    /**
     *  \brief Deleted move constructor of EventManager.
     */
    EventManager(const EventManager&&) = delete;

    /**
     *  \brief Deleted assignment constructor of EventManager.
     */
    EventManager& operator=(const EventManager&) = delete;

  private:
    /**
     *  \brief Constructor of EventManager.
