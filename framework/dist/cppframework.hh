#ifndef    __LIBRARY_EXCEPTION_HH__
#define    __LIBRARY_EXCEPTION_HH__
#include  <string>
#include  <exception>
namespace fwk {
  class Exception :public std::exception {
  private:
    std::string  _what;
  public:
    Exception(const std::string& what) noexcept;
    Exception(const std::exception& oth) noexcept;
    Exception& operator=(const std::exception& oth) noexcept;
    virtual ~Exception(void);
    virtual const char*  what(void) const noexcept;
  };
}
#endif

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
# include <intrin.h>
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
  if (_BitScanReverse(&r, static_cast<uint32_t>(x >> 32)))
    return 63 - (r + 32);
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
#  pragma GCC diagnostic ignored "-Wlong-long"
#  pragma GCC diagnostic ignored "-Wshadow"
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
# define FMT_USE_VARIADIC_TEMPLATES \
   (FMT_HAS_FEATURE(cxx_variadic_templates) || \
       (FMT_GCC_VERSION >= 404 && FMT_HAS_GXX_CXX11) || _MSC_VER >= 1800)
#endif
#ifndef FMT_USE_RVALUE_REFERENCES
# if defined(FMT_GNUC_LIBSTD_VERSION) && FMT_GNUC_LIBSTD_VERSION <= 402
#  define FMT_USE_RVALUE_REFERENCES 0
# else
#  define FMT_USE_RVALUE_REFERENCES \
    (FMT_HAS_FEATURE(cxx_rvalue_references) || \
        (FMT_GCC_VERSION >= 403 && FMT_HAS_GXX_CXX11) || _MSC_VER >= 1600)
# endif
#endif
#if FMT_USE_RVALUE_REFERENCES
# include <utility>
#endif
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
inline DummyInt signbit(...) { return DummyInt(); }
inline DummyInt _ecvt_s(...) { return DummyInt(); }
inline DummyInt isinf(...) { return DummyInt(); }
inline DummyInt _finite(...) { return DummyInt(); }
inline DummyInt isnan(...) { return DummyInt(); }
inline DummyInt _isnan(...) { return DummyInt(); }
template <typename T>
inline T check(T value) { return value; }
}
}
namespace std {
template <>
class numeric_limits<fmt::internal::DummyInt> :
    public std::numeric_limits<int> {
 public:
  template <typename T>
  static bool isinfinity(T x) {
    using namespace fmt::internal;
    if (check(sizeof(isinf(x)) == sizeof(bool) ||
              sizeof(isinf(x)) == sizeof(int))) {
      return !!isinf(x);
    }
    return !_finite(static_cast<double>(x));
  }
  template <typename T>
  static bool isnotanumber(T x) {
    using namespace fmt::internal;
    if (check(sizeof(isnan(x)) == sizeof(bool) ||
              sizeof(isnan(x)) == sizeof(int))) {
      return !!isnan(x);
    }
    return _isnan(static_cast<double>(x)) != 0;
  }
  static bool isnegative(double x) {
    using namespace fmt::internal;
    if (check(sizeof(signbit(x)) == sizeof(int)))
      return !!signbit(x);
    if (x < 0) return true;
    if (!isnotanumber(x)) return false;
    int dec = 0, sign = 0;
    char buffer[2];
    _ecvt_s(buffer, sizeof(buffer), x, 0, &dec, &sign);
    return sign != 0;
  }
};
}
namespace fmt {
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
template <typename Char>
class BasicStringRef {
 private:
  const Char *data_;
  std::size_t size_;
 public:
  BasicStringRef(const Char *s, std::size_t size) : data_(s), size_(size) {}
  BasicStringRef(const Char *s)
    : data_(s), size_(std::char_traits<Char>::length(s)) {}
  BasicStringRef(const std::basic_string<Char> &s)
  : data_(s.c_str()), size_(s.size()) {}
  std::basic_string<Char> to_string() const {
    return std::basic_string<Char>(data_, size_);
  }
  const Char *data() const { return data_; }
  std::size_t size() const { return size_; }
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
template <typename Char>
class BasicCStringRef {
 private:
  const Char *data_;
 public:
  BasicCStringRef(const Char *s) : data_(s) {}
  BasicCStringRef(const std::basic_string<Char> &s) : data_(s.c_str()) {}
  const Char *c_str() const { return data_; }
};
typedef BasicCStringRef<char> CStringRef;
typedef BasicCStringRef<wchar_t> WCStringRef;
class FormatError : public std::runtime_error {
 public:
  explicit FormatError(CStringRef message)
  : std::runtime_error(message.c_str()) {}
};
namespace internal {
enum { INLINE_BUFFER_SIZE = 500 };
#if FMT_SECURE_SCL
template <typename T>
inline stdext::checked_array_iterator<T*> make_ptr(T *ptr, std::size_t size) {
  return stdext::checked_array_iterator<T*>(ptr, size);
}
#else
template <typename T>
inline T *make_ptr(T *ptr, std::size_t) { return ptr; }
#endif
}
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
  virtual void grow(std::size_t size) = 0;
 public:
  virtual ~Buffer() {}
  std::size_t size() const { return size_; }
  std::size_t capacity() const { return capacity_; }
  void resize(std::size_t new_size) {
    if (new_size > capacity_)
      grow(new_size);
    size_ = new_size;
  }
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
template <typename T, std::size_t SIZE, typename Allocator = std::allocator<T> >
class MemoryBuffer : private Allocator, public Buffer<T> {
 private:
  T data_[SIZE];
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
  Allocator get_allocator() const { return *this; }
};
template <typename T, std::size_t SIZE, typename Allocator>
void MemoryBuffer<T, SIZE, Allocator>::grow(std::size_t size) {
  std::size_t new_capacity =
      (std::max)(size, this->capacity_ + this->capacity_ / 2);
  T *new_ptr = this->allocate(new_capacity);
  std::copy(this->ptr_,
            this->ptr_ + this->size_, make_ptr(new_ptr, new_capacity));
  std::size_t old_capacity = this->capacity_;
  T *old_ptr = this->ptr_;
  this->capacity_ = new_capacity;
  this->ptr_ = new_ptr;
  if (old_ptr != data_)
    Allocator::deallocate(old_ptr, old_capacity);
}
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
  static char convert(wchar_t);
 public:
  static char convert(char value) { return value; }
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
template <typename T>
inline bool is_negative(T value) {
  return SignChecker<std::numeric_limits<T>::is_signed>::is_negative(value);
}
template <bool FitsIn32Bits>
struct TypeSelector { typedef uint32_t Type; };
template <>
struct TypeSelector<false> { typedef uint64_t Type; };
template <typename T>
struct IntTraits {
  typedef typename
    TypeSelector<std::numeric_limits<T>::digits <= 32>::Type MainType;
};
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
inline unsigned count_digits(uint64_t n) {
  unsigned t = (64 - FMT_BUILTIN_CLZLL(n | 1)) * 1233 >> 12;
  return t - (n < Data::POWERS_OF_10_64[t]) + 1;
}
#else
inline unsigned count_digits(uint64_t n) {
  unsigned count = 1;
  for (;;) {
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
inline unsigned count_digits(uint32_t n) {
  uint32_t t = (32 - FMT_BUILTIN_CLZ(n | 1)) * 1233 >> 12;
  return t - (n < Data::POWERS_OF_10_32[t]) + 1;
}
#endif
template <typename UInt, typename Char>
inline void format_decimal(Char *buffer, UInt value, unsigned num_digits) {
  buffer += num_digits;
  while (value >= 100) {
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
#if FMT_USE_WINDOWS_H
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
  int convert(WStringRef s);
};
void format_windows_error(fmt::Writer &out, int error_code,
                          fmt::StringRef message) FMT_NOEXCEPT;
#endif
void format_system_error(fmt::Writer &out, int error_code,
                         fmt::StringRef message) FMT_NOEXCEPT;
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
    INT, UINT, LONG_LONG, ULONG_LONG, BOOL, CHAR, LAST_INTEGER_TYPE = CHAR,
    DOUBLE, LONG_DOUBLE, LAST_NUMERIC_TYPE = LONG_DOUBLE,
    CSTRING, STRING, WSTRING, POINTER, CUSTOM
  };
};
struct Arg : Value {
  Type type;
};
template <typename Char>
struct NamedArg;
template <typename T = void>
struct Null {};
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
Yes &convert(fmt::ULongLong);
Yes &convert(std::ostream &);
No &convert(...);
template <typename T>
T &get();
struct DummyStream : std::ostream {
  void operator<<(Null<>);
};
No &operator<<(std::ostream &, int);
template<typename T, bool ENABLE_CONVERSION>
struct ConvertToIntImpl {
  enum { value = false };
};
template<typename T>
struct ConvertToIntImpl<T, true> {
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
template<bool>
struct Not { enum { value = 0 }; };
template<>
struct Not<false> { enum { value = 1 }; };
template <typename Char>
class MakeValue : public Arg {
 private:
  template <typename T>
  MakeValue(const T *value);
  template <typename T>
  MakeValue(T *value);
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
}
class ArgList {
 private:
  uint64_t types_;
  union {
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
  enum { MAX_PACKED_ARGS = 16 };
  ArgList() : types_(0) {}
  ArgList(ULongLong types, const internal::Value *values)
  : types_(types), values_(values) {}
  ArgList(ULongLong types, const internal::Arg *args)
  : types_(types), args_(args) {}
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
  Arg do_get_arg(unsigned arg_index, const char *&error);
 protected:
  const ArgList &args() const { return args_; }
  explicit FormatterBase(const ArgList &args) {
    args_ = args;
    next_arg_index_ = 0;
  }
  Arg next_arg(const char *&error);
  Arg get_arg(unsigned arg_index, const char *&error);
  bool check_no_auto_index(const char *&error);
  template <typename Char>
  void write(BasicWriter<Char> &w, const Char *start, const Char *end) {
    if (start != end)
      w << BasicStringRef<Char>(start, end - start);
  }
};
template <typename Char>
class PrintfFormatter : private FormatterBase {
 private:
  void parse_flags(FormatSpec &spec, const Char *&s);
  Arg get_arg(const Char *s,
      unsigned arg_index = (std::numeric_limits<unsigned>::max)());
  unsigned parse_header(const Char *&s, FormatSpec &spec);
 public:
  explicit PrintfFormatter(const ArgList &args) : FormatterBase(args) {}
  void format(BasicWriter<Char> &writer, BasicCStringRef<Char> format_str);
};
}
template <typename Char>
class BasicFormatter : private internal::FormatterBase {
 private:
  BasicWriter<Char> &writer_;
  internal::ArgMap<Char> map_;
  FMT_DISALLOW_COPY_AND_ASSIGN(BasicFormatter);
  using internal::FormatterBase::get_arg;
  internal::Arg get_arg(BasicStringRef<Char> arg_name, const char *&error);
  internal::Arg parse_arg_index(const Char *&s);
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
enum {
  SIGN_FLAG = 1, PLUS_FLAG = 2, MINUS_FLAG = 4, HASH_FLAG = 8,
  CHAR_FLAG = 0x10
};
struct EmptySpec {};
template <char TYPE>
struct TypeSpec : EmptySpec {
  Alignment align() const { return ALIGN_DEFAULT; }
  unsigned width() const { return 0; }
  int precision() const { return -1; }
  bool flag(unsigned) const { return false; }
  char type() const { return TYPE; }
  char fill() const { return ' '; }
};
struct WidthSpec {
  unsigned width_;
  wchar_t fill_;
  WidthSpec(unsigned width, wchar_t fill) : width_(width), fill_(fill) {}
  unsigned width() const { return width_; }
  wchar_t fill() const { return fill_; }
};
struct AlignSpec : WidthSpec {
  Alignment align_;
  AlignSpec(unsigned width, wchar_t fill, Alignment align = ALIGN_DEFAULT)
  : WidthSpec(width, fill), align_(align) {}
  Alignment align() const { return align_; }
  int precision() const { return -1; }
};
template <char TYPE>
struct AlignTypeSpec : AlignSpec {
  AlignTypeSpec(unsigned width, wchar_t fill) : AlignSpec(width, fill) {}
  bool flag(unsigned) const { return false; }
  char type() const { return TYPE; }
};
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
template <typename T, typename SpecT = TypeSpec<0>, typename Char = char>
class IntFormatSpec : public SpecT {
 private:
  T value_;
 public:
  IntFormatSpec(T val, const SpecT &spec = SpecT())
  : SpecT(spec), value_(val) {}
  T value() const { return value_; }
};
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
IntFormatSpec<int, TypeSpec<'b'> > bin(int value);
IntFormatSpec<int, TypeSpec<'o'> > oct(int value);
IntFormatSpec<int, TypeSpec<'x'> > hex(int value);
IntFormatSpec<int, TypeSpec<'X'> > hexu(int value);
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
 \
 \
 \
 \
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
template <typename Char>
inline StrFormatSpec<Char> pad(
    const Char *str, unsigned width, Char fill = ' ') {
  return StrFormatSpec<Char>(str, width, fill);
}
inline StrFormatSpec<wchar_t> pad(
    const wchar_t *str, unsigned width, char fill = ' ') {
  return StrFormatSpec<wchar_t>(str, width, fill);
}
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
}
template <typename Char, typename Value>
inline void store_args(Value *) {}
template <typename Char, typename Arg, typename T, typename... Args>
inline void store_args(Arg *args, const T &arg, const Args & ... tail) {
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
}
# define FMT_MAKE_TEMPLATE_ARG(n) typename T##n
# define FMT_MAKE_ARG_TYPE(n) T##n
# define FMT_MAKE_ARG(n) const T##n &v##n
# define FMT_ASSIGN_char(n) arr[n] = fmt::internal::MakeValue<char>(v##n)
# define FMT_ASSIGN_wchar_t(n) arr[n] = fmt::internal::MakeValue<wchar_t>(v##n)
#if FMT_USE_VARIADIC_TEMPLATES
# define FMT_VARIADIC_VOID(func, arg_type) \
  template <typename... Args> \
  void func(arg_type arg0, const Args & ... args) { \
    typename fmt::internal::ArgArray<sizeof...(Args)>::Type array; \
    func(arg0, fmt::internal::make_arg_list<Char>(array, args...)); \
  }
# define FMT_VARIADIC_CTOR(ctor, func, arg0_type, arg1_type) \
  template <typename... Args> \
  ctor(arg0_type arg0, arg1_type arg1, const Args & ... args) { \
    typename fmt::internal::ArgArray<sizeof...(Args)>::Type array; \
    func(arg0, arg1, fmt::internal::make_arg_list<Char>(array, args...)); \
  }
#else
# define FMT_MAKE_REF(n) fmt::internal::MakeValue<Char>(v##n)
# define FMT_MAKE_REF2(n) v##n
# define FMT_WRAP1(func, arg_type, n) \
  template <FMT_GEN(n, FMT_MAKE_TEMPLATE_ARG)> \
  inline void func(arg_type arg1, FMT_GEN(n, FMT_MAKE_ARG)) { \
    const fmt::internal::ArgArray<n>::Type array = {FMT_GEN(n, FMT_MAKE_REF)}; \
    func(arg1, fmt::ArgList( \
      fmt::internal::make_type(FMT_GEN(n, FMT_MAKE_REF2)), array)); \
  }
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
class SystemError : public internal::RuntimeError {
 private:
  void init(int err_code, CStringRef format_str, ArgList args);
 protected:
  int error_code_;
  typedef char Char;
  SystemError() {}
 public:
  SystemError(int error_code, CStringRef message) {
    init(error_code, message, ArgList());
  }
  FMT_VARIADIC_CTOR(SystemError, init, int, CStringRef)
  int error_code() const { return error_code_; }
};
template <typename Char>
class BasicWriter {
 private:
  Buffer<Char> &buffer_;
  FMT_DISALLOW_COPY_AND_ASSIGN(BasicWriter);
  typedef typename internal::CharTraits<Char>::CharPtr CharPtr;
#if FMT_SECURE_SCL
  static Char *get(CharPtr p) { return p.base(); }
#else
  static Char *get(Char *p) { return p; }
#endif
  static CharPtr fill_padding(CharPtr buffer,
      unsigned total_size, std::size_t content_size, wchar_t fill);
  CharPtr grow_buffer(std::size_t n) {
    std::size_t size = buffer_.size();
    buffer_.resize(size + n);
    return internal::make_ptr(&buffer_[size], n);
  }
  template <typename UInt>
  Char *write_unsigned_decimal(UInt value, unsigned prefix_size = 0) {
    unsigned num_digits = internal::count_digits(value);
    Char *ptr = get(grow_buffer(prefix_size + num_digits));
    internal::format_decimal(ptr + prefix_size, value, num_digits);
    return ptr;
  }
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
  template <typename T, typename Spec>
  void write_int(T value, Spec spec);
  template <typename T>
  void write_double(T value, const FormatSpec &spec);
  template <typename StrChar>
  CharPtr write_str(
      const StrChar *s, std::size_t size, const AlignSpec &spec);
  template <typename StrChar>
  void write_str(
      const internal::Arg::StringValue<StrChar> &str, const FormatSpec &spec);
  void operator<<(typename internal::WCharHelper<wchar_t, Char>::Unsupported);
  void operator<<(
      typename internal::WCharHelper<const wchar_t *, Char>::Unsupported);
  void append_float_length(Char *&format_ptr, long double) {
    *format_ptr++ = 'L';
  }
  template<typename T>
  void append_float_length(Char *&, T) {}
  template <typename Impl, typename Char_>
  friend class internal::BasicArgFormatter;
  friend class internal::PrintfArgFormatter<Char>;
 protected:
  explicit BasicWriter(Buffer<Char> &b) : buffer_(b) {}
 public:
  virtual ~BasicWriter() {}
  std::size_t size() const { return buffer_.size(); }
  const Char *data() const FMT_NOEXCEPT { return &buffer_[0]; }
  const Char *c_str() const {
    std::size_t size = buffer_.size();
    buffer_.reserve(size + 1);
    buffer_[size] = '\0';
    return &buffer_[0];
  }
  std::basic_string<Char> str() const {
    return std::basic_string<Char>(&buffer_[0], buffer_.size());
  }
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
  BasicWriter &operator<<(ULongLong value) {
    return *this << IntFormatSpec<ULongLong>(value);
  }
  BasicWriter &operator<<(double value) {
    write_double(value, FormatSpec());
    return *this;
  }
  BasicWriter &operator<<(long double value) {
    write_double(value, FormatSpec());
    return *this;
  }
  BasicWriter &operator<<(char value) {
    buffer_.push_back(value);
    return *this;
  }
  BasicWriter &operator<<(
      typename internal::WCharHelper<wchar_t, Char>::Supported value) {
    buffer_.push_back(value);
    return *this;
  }
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
    type = 'f';
#endif
  case 'E': case 'G': case 'A':
    upper = true;
    break;
  default:
    internal::report_unknown_type(type, "double");
    break;
  }
  char sign = 0;
  if (internal::FPUtil::isnegative(static_cast<double>(value))) {
    sign = '-';
    value = -value;
  } else if (spec.flag(SIGN_FLAG)) {
    sign = spec.flag(PLUS_FLAG) ? '+' : ' ';
  }
  if (internal::FPUtil::isnotanumber(value)) {
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
  enum { MAX_FORMAT_SIZE = 10};
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
  Char fill = internal::CharTraits<Char>::cast(spec.fill());
  for (;;) {
    std::size_t buffer_size = buffer_.capacity() - offset;
#ifdef _MSC_VER
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
    buffer_.reserve(n >= 0 ? offset + n + 1 : buffer_.capacity() + 1);
  }
}
template <typename Char, typename Allocator = std::allocator<Char> >
class BasicMemoryWriter : public BasicWriter<Char> {
 private:
  internal::MemoryBuffer<Char, internal::INLINE_BUFFER_SIZE, Allocator> buffer_;
 public:
  explicit BasicMemoryWriter(const Allocator& alloc = Allocator())
    : BasicWriter<Char>(buffer_), buffer_(alloc) {}
#if FMT_USE_RVALUE_REFERENCES
  BasicMemoryWriter(BasicMemoryWriter &&other)
    : BasicWriter<Char>(buffer_), buffer_(std::move(other.buffer_)) {
  }
  BasicMemoryWriter &operator=(BasicMemoryWriter &&other) {
    buffer_ = std::move(other.buffer_);
    return *this;
  }
#endif
};
typedef BasicMemoryWriter<char> MemoryWriter;
typedef BasicMemoryWriter<wchar_t> WMemoryWriter;
template <typename Char>
class BasicArrayWriter : public BasicWriter<Char> {
 private:
  internal::FixedBuffer<Char> buffer_;
 public:
  BasicArrayWriter(Char *array, std::size_t size)
    : BasicWriter<Char>(buffer_), buffer_(array, size) {}
  template <std::size_t SIZE>
  explicit BasicArrayWriter(Char (&array)[SIZE])
    : BasicWriter<Char>(buffer_), buffer_(array, SIZE) {}
};
typedef BasicArrayWriter<char> ArrayWriter;
typedef BasicArrayWriter<wchar_t> WArrayWriter;
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
void report_system_error(int error_code, StringRef message) FMT_NOEXCEPT;
#if FMT_USE_WINDOWS_H
class WindowsError : public SystemError {
 private:
  void init(int error_code, CStringRef format_str, ArgList args);
 public:
  WindowsError(int error_code, CStringRef message) {
    init(error_code, message, ArgList());
  }
  FMT_VARIADIC_CTOR(WindowsError, init, int, CStringRef)
};
void report_windows_error(int error_code, StringRef message) FMT_NOEXCEPT;
#endif
enum Color { BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE };
void print_colored(Color c, CStringRef format, ArgList args);
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
void print(std::FILE *f, CStringRef format_str, ArgList args);
void print(CStringRef format_str, ArgList args);
template <typename Char>
void printf(BasicWriter<Char> &w, BasicCStringRef<Char> format, ArgList args) {
  internal::PrintfFormatter<Char>(args).format(w, format);
}
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
int fprintf(std::FILE *f, CStringRef format, ArgList args);
inline int printf(CStringRef format, ArgList args) {
  return fprintf(stdout, format, args);
}
class FormatInt {
 private:
  enum {BUFFER_SIZE = std::numeric_limits<ULongLong>::digits10 + 3};
  mutable char buffer_[BUFFER_SIZE];
  char *str_;
  char *format_decimal(ULongLong value) {
    char *buffer_end = buffer_ + BUFFER_SIZE - 1;
    while (value >= 100) {
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
  std::size_t size() const { return buffer_ - str_ + BUFFER_SIZE - 1; }
  const char *data() const { return str_; }
  const char *c_str() const {
    buffer_[BUFFER_SIZE - 1] = '\0';
    return str_;
  }
  std::string str() const { return std::string(str_, size()); }
};
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
template <typename T>
inline internal::NamedArg<char> arg(StringRef name, const T &arg) {
  return internal::NamedArg<char>(name, arg);
}
template <typename T>
inline internal::NamedArg<wchar_t> arg(WStringRef name, const T &arg) {
  return internal::NamedArg<wchar_t>(name, arg);
}
template <typename Char>
void arg(StringRef, const internal::NamedArg<Char>&) FMT_DELETED_OR_UNDEFINED;
template <typename Char>
void arg(WStringRef, const internal::NamedArg<Char>&) FMT_DELETED_OR_UNDEFINED;
}
#if FMT_GCC_VERSION
# pragma GCC system_header
#endif
#define FMT_EXPAND(args) args
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
#endif
#define FMT_VARIADIC(ReturnType, func, ...) \
  FMT_VARIADIC_(char, ReturnType, func, return func, __VA_ARGS__)
#define FMT_VARIADIC_W(ReturnType, func, ...) \
  FMT_VARIADIC_(wchar_t, ReturnType, func, return func, __VA_ARGS__)
#define FMT_CAPTURE_ARG_(id, index) ::fmt::arg(#id, id)
#define FMT_CAPTURE_ARG_W_(id, index) ::fmt::arg(L###id, id)
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
void print(std::ostream &os, CStringRef format_str, ArgList args);
FMT_VARIADIC(void, print, std::ostream &, CStringRef)
#endif
}
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
}
inline namespace literals {
inline internal::UdlFormat<char>
operator"" _format(const char *s, std::size_t) { return {s}; }
inline internal::UdlFormat<wchar_t>
operator"" _format(const wchar_t *s, std::size_t) { return {s}; }
inline internal::UdlArg<char>
operator"" _a(const char *s, std::size_t) { return {s}; }
inline internal::UdlArg<wchar_t>
operator"" _a(const wchar_t *s, std::size_t) { return {s}; }
}
}
#endif
#if FMT_GCC_VERSION >= 406
# pragma GCC diagnostic pop
#endif
#if defined(__clang__) && !defined(__INTEL_COMPILER)
# pragma clang diagnostic pop
#endif
#ifdef FMT_HEADER_ONLY
# include "format.cc"
#endif
#endif

#ifndef FMT_POSIX_H_
#define FMT_POSIX_H_
#ifdef __MINGW32__
# undef __STRICT_ANSI__
#endif
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <cstddef>
#ifndef FMT_POSIX
# if defined(_WIN32) && !defined(__MINGW32__)
#  define FMT_POSIX(call) _##call
# else
#  define FMT_POSIX(call) call
# endif
#endif
#ifdef FMT_SYSTEM
# define FMT_POSIX_CALL(call) FMT_SYSTEM(call)
#else
# define FMT_SYSTEM(call) call
# ifdef _WIN32
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
class ErrorCode {
 private:
  int value_;
 public:
  explicit ErrorCode(int value = 0) FMT_NOEXCEPT : value_(value) {}
  int get() const FMT_NOEXCEPT { return value_; }
};
class BufferedFile {
 private:
  FILE *file_;
  friend class File;
  explicit BufferedFile(FILE *f) : file_(f) {}
 public:
  BufferedFile() FMT_NOEXCEPT : file_(0) {}
  ~BufferedFile() FMT_NOEXCEPT;
#if !FMT_USE_RVALUE_REFERENCES
 private:
  struct Proxy {
    FILE *file;
  };
public:
  BufferedFile(Proxy p) FMT_NOEXCEPT : file_(p.file) {}
  BufferedFile(BufferedFile &f) FMT_NOEXCEPT : file_(f.file_) {
    f.file_ = 0;
  }
  BufferedFile &operator=(Proxy p) {
    close();
    file_ = p.file;
    return *this;
  }
  BufferedFile &operator=(BufferedFile &other) {
    close();
    file_ = other.file_;
    other.file_ = 0;
    return *this;
  }
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
  BufferedFile(CStringRef filename, CStringRef mode);
  void close();
  FILE *get() const FMT_NOEXCEPT { return file_; }
  int (fileno)() const;
  void print(CStringRef format_str, const ArgList &args) {
    fmt::print(file_, format_str, args);
  }
  FMT_VARIADIC(void, print, CStringRef)
};
class File {
 private:
  int fd_;
  explicit File(int fd) : fd_(fd) {}
 public:
  enum {
    RDONLY = FMT_POSIX(O_RDONLY),
    WRONLY = FMT_POSIX(O_WRONLY),
    RDWR   = FMT_POSIX(O_RDWR)
  };
  File() FMT_NOEXCEPT : fd_(-1) {}
  File(CStringRef path, int oflag);
#if !FMT_USE_RVALUE_REFERENCES
 private:
  struct Proxy {
    int fd;
  };
 public:
  File(Proxy p) FMT_NOEXCEPT : fd_(p.fd) {}
  File(File &other) FMT_NOEXCEPT : fd_(other.fd_) {
    other.fd_ = -1;
  }
  File &operator=(Proxy p) {
    close();
    fd_ = p.fd;
    return *this;
  }
  File &operator=(File &other) {
    close();
    fd_ = other.fd_;
    other.fd_ = -1;
    return *this;
  }
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
  ~File() FMT_NOEXCEPT;
  int descriptor() const FMT_NOEXCEPT { return fd_; }
  void close();
  LongLong size() const;
  std::size_t read(void *buffer, std::size_t count);
  std::size_t write(const void *buffer, std::size_t count);
  static File dup(int fd);
  void dup2(int fd);
  void dup2(int fd, ErrorCode &ec) FMT_NOEXCEPT;
  static void pipe(File &read_end, File &write_end);
  BufferedFile fdopen(const char *mode);
};
long getpagesize();
}
#if !FMT_USE_RVALUE_REFERENCES
namespace std {
inline fmt::BufferedFile &move(fmt::BufferedFile &f) { return f; }
inline fmt::File &move(fmt::File &f) { return f; }
}
#endif
#endif

#ifndef    __LIBRARY_ENCRYPTION_XOR_HH__
#define    __LIBRARY_ENCRYPTION_XOR_HH__
#include  <cstdint>
namespace fwk {
  namespace Encryption {
    class XOR {
    public:
      static void  xorify(void* data, size_t dataSize, const void* key, size_t keySize);
    };
  }
}
#endif

#ifndef   __LIBRARY_NETWORK_CURL_METHOD_HH__
#define   __LIBRARY_NETWORK_CURL_METHOD_HH__
#include  <curl/curl.h>
#ifndef    __LIBRARY_COLLECTION_BIDIMAP_HPP__
#define    __LIBRARY_COLLECTION_BIDIMAP_HPP__
#include  <map>
#include  <initializer_list>
namespace fwk {
  template<class A, class B>
  class BidiMap {
  public:
    std::map<A, B>  key;
    std::map<B, A>  value;
  public:
    BidiMap(void):
      key(),
      value()
    {}
    BidiMap(std::initializer_list<std::pair<const A, B>> l):
      key(l),
      value()
    {
      for (auto &it : this->key) {
        this->value[it.second] = it.first;
      }
    }
    BidiMap&  operator=(std::initializer_list<std::pair<const A, B>> l) {
      this->key = l;
      for (auto &it : this->key) {
        this->value[it.second] = it.first;
      }
      return *this;
    }
    BidiMap(const BidiMap& oth):
      key(oth.key),
      value(oth.value)
    {}
    BidiMap&  operator=(const BidiMap& oth) {
      if (this != &oth) {
        this->key = oth.key;
        this->value = oth.value;
      }
      return *this;
    }
    virtual ~BidiMap(void) {}
    void  insert(const A& k, const B& v) {
      this->key[k] = v;
      this->value[v] = k;
    }
    void  erase(const A& k) {
      this->value.erase(this->key.at(k));
      this->key.erase(k);
    }
  };
}
#endif

namespace fwk {
  namespace curlxx {
    static const BidiMap<std::string, CURLoption> MethodToOption = {
      {std::string("GET"), CURLOPT_HTTPGET},
      {std::string("POST"), CURLOPT_POST},
      {std::string("PUT"), CURLOPT_PUT},
      {std::string("HEAD"), CURLOPT_NOBODY},
    };
  }
}
#endif

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
  class PoolNotInitializedException :public fwk::Exception {
  public:
    PoolNotInitializedException(const std::string& message = "This pool was not initialized") noexcept;
    virtual ~PoolNotInitializedException(void) noexcept;
  };
  class PoolInvalidArgumentsException :public fwk::Exception {
  public:
    PoolInvalidArgumentsException(const std::string& message) noexcept;
    virtual ~PoolInvalidArgumentsException(void) noexcept;
  };
  class PoolMemoryExhaustedException :public fwk::Exception {
  public:
    PoolMemoryExhaustedException(const std::string& message) noexcept;
    virtual ~PoolMemoryExhaustedException(void) noexcept;
  };
}
#endif

#ifndef    __LIBRARY_PROPERTY_INITIALIZABLE_HPP__
#define    __LIBRARY_PROPERTY_INITIALIZABLE_HPP__
namespace fwk {
  class Initializable {
  protected:
    bool  _initialized;
  public:
    Initializable(void);
    virtual ~Initializable(void);
  public:
    bool  isInitialized(void) const;
  protected:
    void  isInitialized(bool state);
  };
  template<class C>
  class TInitializable :public C, public Initializable {
  public:
    template<typename... Args>
    TInitializable(const Args&... args):
      C(args...),
      Initializable()
    {}
    virtual ~TInitializable(void) {}
  };
}
#endif

#ifndef    __LIBRARY_THREADING_LOCKABLE_HPP__
#define    __LIBRARY_THREADING_LOCKABLE_HPP__
#include  <mutex>
namespace fwk {
  class  Lockable {
  protected:
    std::recursive_mutex _lock;
  public:
    Lockable(void);
    virtual ~Lockable(void);
  public:
    void  lock(void);
    void  unlock(void);
  };
  template<class C>
  class  TLockable :public C, public Lockable {
  public:
    TLockable(const TLockable<C>& oth):
      C(oth),
      Lockable()
    {}
    template<typename... Args>
    TLockable(const Args&... args):
      C(args...),
      Lockable()
    {}
    virtual ~TLockable(void) {}
  };
  class  ReadWriteLock {
  public:
    class WriterGuard {
    private:
      ReadWriteLock *_lock;
    public:
      WriterGuard(ReadWriteLock* lock);
      ~WriterGuard(void);
    };
    class ReaderGuard {
    private:
      ReadWriteLock *_lock;
    public:
      ReaderGuard(ReadWriteLock* lock);
       ~ReaderGuard(void);
    };
  private:
    class  LightSwitch {
    private:
      std::mutex _mutex;
      size_t     _counter;
    public:
      LightSwitch(void);
    public:
      void  acquire(std::mutex& mutex);
      void  release(std::mutex& mutex);
    };
  private:
    LightSwitch  _readSwitch;
    LightSwitch  _writeSwitch;
    std::mutex  _noReaders;
    std::mutex  _noWriters;
    std::mutex  _readersQueue;
  public:
    ReadWriteLock(void);
    void  readerAcquire(void);
    void  readerRelease(void);
    void  writerAcquire(void);
    void  writerRelease(void);
  };
  typedef std::lock_guard<fwk::Lockable> ScopeLock;
  typedef std::lock_guard<std::mutex>    ScopeLockMutex;
}
#define SCOPELOCK(x)       std::lock_guard<fwk::Lockable>  lockguard(*(x));
#define SCOPELOCK_MUTEX(x) std::lock_guard<std::mutex>     lockguard_mutex(x);
#endif

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
  template<class T>
  class Singleton {
  private:
    static T  *_instance;
  private:
    Singleton(const Singleton&) = delete;
    Singleton(const Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
  protected:
    Singleton(void) {}
    virtual ~Singleton(void) {}
  public:
    static T&  get(void) {
      if (_instance == nullptr) {
        _instance = new T();
      }
      return (*_instance);
    }
    static T*  asPtr(void) {
      return &(get());
    }
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
#endif

#ifndef    __LIBRARY_TOOL_DATE_HH__
#define    __LIBRARY_TOOL_DATE_HH__
#include  <iostream>
#include  <ctime>
namespace fwk {
  class Date {
  public:
    static int  gettimeofday(struct timeval *);
    static const std::string  getTime(void);
  };
}
#endif

namespace fwk {
  class Logger :public Lockable {
  public:
    enum class Level {
      DEBUG    = 0,
      INFO     = 1,
      WARNING  = 2,
      ERROR    = 3,
      CRITICAL = 4
    };
  private:
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
    };
    const std::map<Logger::Level, Logger::Color> LevelToColor = {
      {Logger::Level::CRITICAL, Logger::Color::RED},
      {Logger::Level::ERROR, Logger::Color::PURPLE},
      {Logger::Level::WARNING, Logger::Color::YELLOW},
      {Logger::Level::INFO, Logger::Color::GREEN},
      {Logger::Level::DEBUG, Logger::Color::WHITE}
    };
  private:
    std::string     _name;
    Logger::Level   _level;
    size_t          _offset;
    std::ofstream*  _file;
  public:
    Logger(const std::string& name);
    Logger(const Logger& oth);
    Logger& operator=(const Logger& oth);
    ~Logger(void);
  public:
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
    void  setOffset(size_t off);
    void  addOffset(size_t off);
    void  delOffset(size_t off);
    void  setLevel(Logger::Level level);
    void  setFile(const std::string& filepath);
    void closeFile(void);
  };
  class LoggerManager :public Singleton<fwk::LoggerManager>, public Lockable {
    friend class Singleton<fwk::LoggerManager>;
  public:
    typedef std::unordered_map<std::string, Logger> NameLoggerMap;
  private:
    NameLoggerMap _loggers;
  private:
    LoggerManager(const LoggerManager&) = delete;
    LoggerManager(const LoggerManager&&) = delete;
    LoggerManager&  operator=(const LoggerManager&) = delete;
  private:
    LoggerManager(void);
    virtual ~LoggerManager(void);
  public:
    Logger&  init(const std::string& loggerName, Logger::Level level);
    Logger&  init(const std::string& loggerName, Logger::Level level, const std::string& filepath);
    Logger&  getLogger(const std::string& name, bool create = false);
    void  endLogger(const std::string& name);
    template<typename T>
    void  log(const std::string& loggerName, const T &msg, Logger::Level level) {
      SCOPELOCK(this);
      try {
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
  };
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
#endif

#ifndef    __LIBRARY_CONVERTER_HPP__
#define    __LIBRARY_CONVERTER_HPP__
#include  <string>
#include  <sstream>
namespace fwk {
  class Converter {
  public:
    template<typename T>
    static std::string  StringOf(const T&val) {
      std::stringstream ss;
      ss << std::fixed << val;
      return (ss.str());
    }
    template<typename T>
    static T  StringTo(const std::string &val) {
      std::stringstream ss;
      T      ret;
      ss << val;
      ss >> ret;
      return (ret);
    }
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
#endif

namespace fwk {
  template<class C>
  class Pool :public Lockable, public Initializable {
  private:
    std::queue<C*> _pool;
    size_t         _hydrate;
    std::string    _name;
  public:
    Pool(void):
      Lockable(),
      Initializable(),
      _pool(),
      _hydrate(0),
      _name()
    {}
    virtual ~Pool(void) {
      this->clear();
    }
  public:
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
    C*  get(void) {
      SCOPELOCK(this);
      if (this->_pool.empty()) {
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
    template<typename... Args>
    C*  get(const Args&... args) {
      C* item = this->get();
      item->init(args...);
      return item;
    }
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
#endif

namespace fwk {
  template<class C>
  class APooled {
  protected:
    static Pool<C> _pool;
  public:
    static void  initPool(size_t originalSize, size_t hydrateSize, const std::string& className) {
      APooled<C>::_pool.init(originalSize, hydrateSize, className);
    }
    static void  destroyPool(void) {
      APooled<C>::_pool.clear();
    }
    template<typename... Args>
    static C*  getFromPool(const Args&... args) {
      return APooled<C>::_pool.get(args...);
    }
    static void  returnToPool(C* element) {
      if (element != nullptr) {
        APooled<C>::_pool.push(element);
      }
    }
  private:
    bool  _valid;
    std::chrono::steady_clock::time_point _lastOutOfPoolTimePoint;
  public:
    APooled(void):
      _valid(false),
      _lastOutOfPoolTimePoint()
    {}
    virtual ~APooled(void) {}
  public:
    virtual void  reinit(void) = 0;
  public:
    void  isValid(bool valid) { this->_valid = valid; }
    bool  isValid(void) const { return this->_valid; }
    void  updateLastOutOfPoolTimePoint(void) {
      this->_lastOutOfPoolTimePoint = std::chrono::steady_clock::now();
    }
    const std::chrono::steady_clock::time_point&  lastOutOfPoolTimePoint(void) const {
      return this->_lastOutOfPoolTimePoint;
    }
  };
  template<class C>
  Pool<C> APooled<C>::_pool;
}
#endif

namespace fwk {
  namespace curlxx {
    class EasyHandle :public APooled<curlxx::EasyHandle> {
    private:
      CURL*       _handle;
      curl_slist* _headers;
    public:
      EasyHandle(void);
      virtual ~EasyHandle(void);
    public:
      void  init(void);
      virtual void  reinit(void);
    public:
      void  setOpt(CURLoption option, long parameter) const;
      void  setOpt(CURLoption option, const std::string& parameter) const;
      void  setOpt(CURLoption option, void* parameter) const;
      void  setOpt(CURLoption option, size_t parameter) const;
      void  setOpt(CURLoption option, size_t (*parameter)(void*, size_t, size_t, void*)) const;
      void  perform(void) const;
      uint32_t  getStatus(void) const;
      void  cleanup(void);
    public:
      void  setHeaders(const std::map<std::string, std::string>& headers);
      void  setMethod(const std::string& method) const;
      void  setFile(const std::string& filepath, size_t (*callback)(void*, size_t, size_t, void*)) const;
      void  setBody(void* ptr, size_t size) const;
      void  setPort(uint16_t port) const;
      void  setURL(const std::string& fullURL) const;
      void  setUserAgent(const std::string& userAgent) const;
      void  setResponseCallbacks(void* responsePtr, size_t (*body)(void*, size_t, size_t, void*), size_t (*headers)(void*, size_t, size_t, void*)) const;
    public:
      CURL*  getHandle(void) const;
    private:
      void  appendHeader(const std::string& header, const std::string& value);
    };
  }
}
#endif

#ifndef   __LIBRARY_NETWORK_CURL_MULTIHANDLE_HH__
#define   __LIBRARY_NETWORK_CURL_MULTIHANDLE_HH__
#include  <vector>
#include  <curl/curl.h>
namespace fwk {
  namespace curlxx {
    class MultiHandle {
    public:
      static const size_t PIPELINED_REQUESTS_MAX_NB;
    private:
      CURLM* _multiHandle;
      std::vector<curlxx::EasyHandle*> _easyHandles;
    public:
      MultiHandle(void);
      virtual ~MultiHandle(void);
    public:
      EasyHandle*  findHandle(CURL* curlHandle);
      bool  hasHandles(void) const;
    public:
      void  init(void);
      void  addHandle(EasyHandle* handle);
      void  removeHandle(EasyHandle* handle);
      bool  perform(void) const;
      void  timeout(struct timeval& interval) const;
      void  fdset(fd_set* rset, fd_set* wset, fd_set* eset, int& fdmax) const;
      CURLMsg*  infoRead(void) const;
      void  cleanup(void);
    };
  }
}
#endif

#ifndef    __LIBRARY_NETWORK_CURL_EXCEPTION_HH__
#define    __LIBRARY_NETWORK_CURL_EXCEPTION_HH__
namespace fwk {
  namespace curlxx {
    class Exception :public fwk::Exception {
    public:
      Exception(const std::string& message) noexcept;
      virtual ~Exception(void) noexcept;
    };
  }
}
#endif

#ifndef   __LIBRARY_TOOL_SIGNAL_HH__
#define   __LIBRARY_TOOL_SIGNAL_HH__
#include <unordered_map>
#include <functional>
#include <csignal>
namespace fwk {
  class Signal :public Singleton<fwk::Signal>, public Lockable {
    friend class Singleton<fwk::Signal>;
  public:
    enum class Type {
      INT
    };
  static const BidiMap<Signal::Type, int> SignalTypeToInt;
  private:
    Signal(const Signal&) = delete;
    Signal(const Signal&&) = delete;
    Signal&  operator=(const Signal&) = delete;
  private:
    Signal(void);
    virtual ~Signal(void);
  private:
    std::unordered_map<int, std::function<bool (void)>> callbacks;
    std::unordered_map<int, struct sigaction*> oldcallbacks;
  public:
    void  setCallback(Signal::Type signal, const std::function<bool (void)>& callback);
    void  delCallback(Signal::Type signal);
    void  handle(int signum);
  };
}
#endif

#ifndef    __LIBRARY_CONSTANT_HH__
#define    __LIBRARY_CONSTANT_HH__
#include  <string>
namespace fwk {
  namespace Constant {
    static const std::string  undefined = "undefined";
  }
}
#endif

#ifndef    __LIBRARY_TOOL_MACRO_HH__
#define    __LIBRARY_TOOL_MACRO_HH__
#define    MAX(x, y)    ((x) > (y) ? (x) : (y))
#define    MIN(x, y)    ((x) < (y) ? (x) : (y))
#define    ABS(x)       ((x) > 0 ? (x) : (x) * -1)
#endif

#ifndef    __LIBRARY_TOOL_RANDOM_HH__
#define    __LIBRARY_TOOL_RANDOM_HH__
namespace fwk {
  class Random {
  public:
    static void  setSeed(unsigned int seed);
    static void  init(void);
    static int32_t  getInt(int32_t min, int32_t max);
  };
}
#endif

#ifndef    __LIBRARY_TOOL_ARGUMENTS_HH__
#define    __LIBRARY_TOOL_ARGUMENTS_HH__
#include  <string>
#include  <vector>
#include  <list>
#include  <functional>
namespace fwk {
  class Arguments {
  public:
    struct Arg {
      std::string flag;
      std::vector<std::string> values;
      Arg(const std::string& flag);
      Arg(const std::string& flag, const std::vector<std::string>& values);
      const std::string&  operator[](size_t idx) const;
    };
    struct Expected {
      std::string                 flag;
      std::string                 type;
      std::string                  description;
      bool                        mandatory;
      std::function<bool (const std::string&)>  typechecker;
      size_t                      nbmin;
      ssize_t                     nbmax;
      Expected(const std::string& flag, const std::string& type, const std::string& description, bool mandatory = false, const std::function<bool (const std::string&)>& typechecker = nullptr, size_t nbmin = 1, ssize_t nbmax = 1);
    };
    struct Incompatible {
      std::list<std::string>  flags;
      bool          oneMandatory;
      Incompatible(const std::list<std::string>& flags, bool mandatory);
    };
  private:
    int                      _ac;
    char**                   _av;
    std::list<Arg*>          _arguments;
    std::list<Arg*>          _defaultValues;
    std::list<Expected*>     _expected;
    std::list<Incompatible*> _incompatibleArguments;
  public:
    Arguments(void);
    virtual ~Arguments(void);
  public:
    void  init(int ac, char **av);
    void  defaultValue(const std::string& flag, const std::string& value);
    void  expected(Expected *expected);
    void  expected(const std::string& flag, const std::string& type, const std::string& description, bool mandatory = false, const std::function<bool (const std::string&)>& typechecker = nullptr, size_t nbmin = 1, ssize_t nbmax = 1);
    void  incompatible(Incompatible* incompatible);
    void  incompatible(const std::list<std::string>& flags, bool mandatory);
    bool  run(void);
    const Arg*  arg(const std::string& flag) const;
    const Arg&  operator[](const std::string& flag) const;
    const std::string  usage(void) const;
  private:
    bool  check(void);
    Arg* find(std::list<Arg*>& args, const std::string& flag, bool create);
    const Arg* find(const std::list<Arg*>& args, const std::string& flag) const;
    bool isExpected(const std::string& flag) const;
    bool isIncompatible(const std::string& flag) const;
    const Expected* getExpected(const std::string& flag) const;
  };
}
#endif

#ifndef    __LIBRARY_TOOL_STRING_HH__
#define    __LIBRARY_TOOL_STRING_HH__
#include  <string>
namespace fwk {
  void  trim(std::string& str);
  void  ltrim(std::string& str);
  void  rtrim(std::string& str);
}
#endif

#ifndef    __LIBRARY_TOOL_TYPECHECKER_HH__
#define    __LIBRARY_TOOL_TYPECHECKER_HH__
#include  <string>
namespace fwk {
  class TypeChecker {
  public:
    static bool  isInteger(const std::string& str);
    static bool  isUnsignedInteger(const std::string& str);
    static bool  isHexadecimal(const std::string& str);
    static bool  isMD5Hash(const std::string& str);
  private:
    static bool  charactersIn(const std::string& str, const std::string& characters);
  };
}
#endif

#ifndef    __LIBRARY_PROPERTY_AENDABLE_HH__
#define    __LIBRARY_PROPERTY_AENDABLE_HH__
namespace fwk {
  class AEndable {
  protected:
    bool  _end;
  public:
    AEndable(void);
    virtual ~AEndable(void);
  public:
    virtual void  end(void) = 0;
  public:
    bool  mustEnd(void) const;
    void  mustEnd(bool state);
  };
}
#endif

#ifndef    __LIBRARY_THREADING_NOTIFIABLE_HPP__
#define    __LIBRARY_THREADING_NOTIFIABLE_HPP__
#include  <condition_variable>
#include  <chrono>
#include  <thread>
namespace fwk {
  class Notifiable :public Lockable {
  private:
    std::condition_variable_any _condvar;
  public:
    Notifiable(void);
    virtual ~Notifiable(void);
  public:
    void  wait(void);
    void  notify(void);
    void  notify_all(void);
    template <class Predicate>
    void  wait(Predicate pred) {
      this->_condvar.wait(*this, pred);
    }
    template<class C, class D>
    std::cv_status  wait_until(const std::chrono::time_point<C, D>& tp) {
      return this->_condvar.wait_until(*this, tp);
    }
    template<class C, class D, class P>
    bool  wait_until(const std::chrono::time_point<C, D>& timeout_time, P pred) {
      return this->_condvar(*this, timeout_time, pred);
    }
    template<class R, class P>
    std::cv_status  wait_for(const std::chrono::duration<R, P>& rl) {
      return this->_condvar.wait_for(*this, rl);
    }
    template<class R, class Per, class Pre>
    bool  wait_for(const std::chrono::duration<R, Per>& rel_time, Pre pred) {
      return this->_condvar.wait_for(*this, rel_time, pred);
    }
  };
  template<class C>
  class TNotifiable :public C, public Notifiable {
  public:
    template<typename... Args>
    TNotifiable(const Args&... args):
      C(args...),
      Notifiable()
    {}
    virtual ~TNotifiable(void) {}
  };
  struct NotifiableThread {
    std::thread* thread;
    Notifiable   condition;
  };
}
#endif

#ifndef    __LIBRARY_COLLECTION_ARRAY_HPP__
#define    __LIBRARY_COLLECTION_ARRAY_HPP__
#include  <iostream>
namespace fwk {
  template<typename T>
  struct Array2 {
  public:
    T  x;
    T  y;
    Array2(void): x(0), y(0) {}
    Array2(T x, T y): x(x), y(y) {}
    Array2(const Array2<T> &oth): x(oth.x), y(oth.y) {}
    Array2&  operator=(const Array2<T> &oth) {
      this->x = oth.x;
      this->y = oth.y;
      return (*this);
    }
    Array2&  operator+=(const Array2<T> &oth) {
      this->x = this->x + oth.x;
      this->y = this->y + oth.y;
      return (*this);
    }
    Array2&  operator-=(const Array2<T> &oth) {
      this->x = this->x - oth.x;
      this->y = this->y - oth.y;
      return (*this);
    }
    Array2&  operator*=(const Array2<T> &oth) {
      this->x = this->x * oth.x;
      this->y = this->y * oth.y;
      return (*this);
    }
    Array2&  operator/=(const Array2<T> &oth) {
      this->x = this->x / oth.x;
      this->y = this->y / oth.y;
      return (*this);
    }
    Array2&  operator%=(const Array2<T> &oth) {
      this->x = this->x % oth.x;
      this->y = this->y % oth.y;
      return (*this);
    }
    ~Array2(void) {}
  };
  template<typename T>
  struct Array3 {
  public:
    T    x;
    T    y;
    T    z;
    Array3(void): x(0), y(0), z(0) {}
    Array3(T x, T y, T z): x(x), y(y), z(z) {}
    Array3(const Array3<T> &oth): x(oth.x), y(oth.y), z(oth.z) {}
    Array3&  operator=(const Array3<T> &oth) {
      this->x = oth.x;
      this->y = oth.y;
      this->z = oth.z;
      return (*this);
    }
    Array3&  operator+=(const Array3<T> &oth) {
      this->x = this->x + oth.x;
      this->y = this->y + oth.y;
      this->z = this->z + oth.z;
      return (*this);
    }
    Array3&  operator-=(const Array3<T> &oth) {
      this->x = this->x - oth.x;
      this->y = this->y - oth.y;
      this->z = this->z - oth.z;
      return (*this);
    }
    Array3&  operator*=(const Array3<T> &oth) {
      this->x = this->x * oth.x;
      this->y = this->y * oth.y;
      this->z = this->z * oth.z;
      return (*this);
    }
    Array3&  operator/=(const Array3<T> &oth) {
      this->x = this->x / oth.x;
      this->y = this->y / oth.y;
      this->z = this->z / oth.z;
      return (*this);
    }
     Array3&  operator%=(const Array3<T> &oth) {
      this->x = this->x % oth.x;
      this->y = this->y % oth.y;
      this->z = this->z % oth.z;
      return (*this);
    }
    ~Array3(void) {}
  };
  template<typename T>
  Array2<T>  operator+(const Array2<T> &a, const Array2<T> &b) {
    return (Array2<T>(a.x + b.x, a.y + b.y));
  }
  template<typename T>
  Array2<T>  operator-(const Array2<T> &a, const Array2<T> &b) {
    return (Array2<T>(a.x - b.x, a.y - b.y));
  }
  template<typename T>
  Array2<T>  operator*(const Array2<T> &a, const Array2<T> &b) {
    return (Array2<T>(a.x * b.x, a.y * b.y));
  }
  template<typename T>
  Array2<T>  operator/(const Array2<T> &a, const Array2<T> &b) {
    return (Array2<T>(a.x / b.x, a.y / b.y));
  }
  template<typename T>
  Array2<T>  operator%(const Array2<T> &a, const Array2<T> &b) {
    return (Array2<T>(a.x % b.x, a.y % b.y));
  }
  template<typename T>
  Array3<T>  operator+(const Array3<T> &a, const Array3<T> &b) {
    return (Array3<T>(a.x + b.x, a.y + b.y, a.z + b.z));
  }
  template<typename T>
  Array3<T>  operator-(const Array3<T> &a, const Array3<T> &b) {
    return (Array3<T>(a.x - b.x, a.y - b.y, a.z - b.z));
  }
  template<typename T>
  Array3<T>  operator*(const Array3<T> &a, const Array3<T> &b) {
    return (Array3<T>(a.x * b.x, a.y * b.y, a.z * b.z));
  }
  template<typename T>
  Array3<T>  operator/(const Array3<T> &a, const Array3<T> &b) {
    return (Array3<T>(a.x / b.x, a.y / b.y, a.z / b.z));
  }
  template<typename T>
  Array3<T>  operator%(const Array3<T> &a, const Array3<T> &b) {
    return (Array3<T>(a.x % b.x, a.y % b.y, a.z % b.z));
  }
  template<typename T>
  std::ostream&  operator<<(std::ostream &os, const Array2<T> &arr) {
    os << "Array2: [" << arr.x << "/" << arr.y << "]";
    return (os);
  }
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
#endif

#ifndef    __LIBRARY_COLLECTION_ORDEREDLIST_HPP__
#define    __LIBRARY_COLLECTION_ORDEREDLIST_HPP__
#include  <list>
#include  <functional>
namespace fwk {
  template<typename T>
  class OrderedList :public std::list<T> {
  private:
    std::function<bool (const T&, const T&)>  _compare;
  public:
    OrderedList(void):
      std::list<T>(),
      _compare([] (const T& a, const T& b) -> bool { return a < b; })
    {}
    OrderedList(const std::function<bool (const T&, const T&)>& f):
      std::list<T>(),
      _compare(f)
    {}
    virtual ~OrderedList(void) {}
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
#endif

#ifndef    __LIBRARY_COLLECTION_BYTEARRAY_HPP__
#define    __LIBRARY_COLLECTION_BYTEARRAY_HPP__
#include  <iostream>
#include  <cstring>
namespace fwk {
  class ByteArray :public Lockable, public APooled<ByteArray> {
  protected:
    uint8_t *_bytearray;
    size_t  _size;
    size_t  _sizemax;
  public:
    ByteArray(size_t size = 1);
    ByteArray(const ByteArray& oth);
    ByteArray& operator=(const ByteArray& oth);
    virtual ~ByteArray(void);
  public:
    virtual void  reinit(void);
  public:
    size_t  getSize(void) const;
    size_t  availableSpace(void) const;
    size_t  getSizeMax(void) const;
    bool  empty(void) const;
    bool  full(void) const;
    uint8_t  operator[](size_t idx) const;
  public:
    void  clearStart(size_t idx);
    void  moveEnd(size_t off);
  public:
    const uint8_t*  getBytes(void) const;
    uint8_t*  atEnd(void);
    uint8_t*  atStart(void);
    uint8_t*  atIndex(size_t idx);
  public:
    void  resize(size_t size, bool force = false, bool keep = false);
    void  init(size_t size, bool force = false);
    void  init(const void* ptr, size_t size);
    size_t  find(const void* ptr, size_t size, size_t start = 0) const;
    size_t  extract(void* ptr, size_t size, size_t offset = 0);
    size_t  get(void* ptr, size_t size, size_t offset = 0) const;
  public:
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
    template<typename T>
    void  push(const T& val, bool resize = false) {
      this->push(&val, sizeof(T), resize);
    }
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
    template<typename T>
    void  push_front(const T& val, bool resize = false) {
      this->push_front(&val, sizeof(T), resize);
    }
    void  pushStr(const std::string& str, bool resize = false);
    void  push_frontStr(const std::string& str, bool resize = false);
  public:
    struct Guard {
    public:
      ByteArray*  bytearray;
    public:
      Guard(ByteArray* b);
      ~Guard();
    };
  };
  class ByteArrayExtractor {
  private:
    const ByteArray * _bytearray;
    size_t _offset;
  public:
    ByteArrayExtractor(const ByteArray *bytearray, size_t offset = 0);
    ByteArrayExtractor(const ByteArrayExtractor& oth);
    ByteArrayExtractor& operator=(const ByteArrayExtractor& oth);
    ~ByteArrayExtractor(void);
  public:
    void  extractString(std::string& str, size_t size);
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
  std::ostream&  operator<<(std::ostream& os, const ByteArray& bytearray);
}
#endif

#ifndef   __CORE_FACTORYMANAGER_HH__
#define   __CORE_FACTORYMANAGER_HH__
namespace fwk {
  class FactoryManager :public Singleton<fwk::FactoryManager>, public Initializable, public Lockable {
    friend class Singleton<fwk::FactoryManager>;
  private:
    FactoryManager(const FactoryManager&) = delete;
    FactoryManager(const FactoryManager&&) = delete;
    FactoryManager&  operator=(const FactoryManager&) = delete;
  private:
    FactoryManager(void);
    virtual ~FactoryManager(void);
  public:
    void  init(void);
    void  end(void);
  };
}
#endif

#ifndef   __CORE_META_HH__
#define   __CORE_META_HH__
#include  <string>
namespace fwk {
  namespace Meta {
    static const std::string version = "0.1.0";
  }
}
#endif

#ifndef    __CORE_EXCEPTION_HH__
#define    __CORE_EXCEPTION_HH__
namespace fwk {
  class CoreException :public fwk::Exception {
  public:
    CoreException(const std::string& message) noexcept;
    virtual ~CoreException(void) noexcept;
  };
}
#endif

#ifndef   __CORE_SYSTEM_HH__
#define   __CORE_SYSTEM_HH__
#include  <string>
#include  <map>
#include  <initializer_list>
namespace fwk {
  class System :public Lockable, public AEndable {
  private:
    enum class Modules {
      HTTP,
      UDP,
      TCP,
      WORKERS,
    };
  public:
    Notifiable _endCondition;
    std::map<Modules, bool> _modulesInitialized;
  public:
    System(void);
    virtual ~System(void);
  public:
    virtual void  end(void);
  public:
    virtual void  initHTTP(const std::string& userAgent);
    virtual void  endHTTP(void);
    virtual void  initTCP(void);
    virtual void  endTCP(void);
    virtual void  initUDP(void);
    virtual void  endUDP(void);
    virtual void  initWorkerThreads(size_t nbWorkerThreads, bool enableDelayedTasks);
    virtual void  endWorkerThreads(void);
    virtual void  run(void);
    virtual void  cleanup(void);
  };
}
#endif

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
  class TcpSocket :public Lockable {
  protected:
    int _fd;
  public:
    TcpSocket(void);
    virtual ~TcpSocket(void);
  public:
    virtual void  reinit(void);
  public:
    void  socket(void);
    void  close(void);
    void  connect(const std::string& hostname, uint16_t port);
    void  bind(uint16_t port) const;
    void  listen(int backlog) const;
    uint32_t  accept(TcpSocket* socket) const;
    uint32_t  getpeername(void) const;
    uint32_t  getsockname(void) const;
  public:
    void  addToSet(fd_set& set, int& max_fd) const;
    bool  isset(fd_set& set) const;
  };
}
#endif

#ifndef   __CORE_NETWORK_TCP_TCPSOCKETSTREAM_HH__
#define   __CORE_NETWORK_TCP_TCPSOCKETSTREAM_HH__
namespace fwk {
  class TcpSocketStream :public TcpSocket, public APooled<TcpSocketStream> {
  protected:
    ByteArray*  _input;
    ByteArray*  _output;
  public:
    static const size_t BUFFER_SIZE;
  public:
    TcpSocketStream(void);
    virtual ~TcpSocketStream(void);
  public:
    virtual void  reinit(void);
  public:
    void  init(void);
    bool  hasDataToSend(void) const;
    void  push(const void* ptr, size_t size);
    ssize_t  recv(void);
    ssize_t  send(void);
  public:
    size_t  extractData(const std::function<size_t (const ByteArray&)>& callback, ByteArray* dest);
    size_t  getData(const std::function<size_t (const ByteArray&)>& callback, ByteArray* dest);
    size_t  seekData(const std::function<size_t (const ByteArray&)>& callback);
  };
}
#endif

#ifndef   __CORE_EVENT_EVENTHANDLE_HH__
#define   __CORE_EVENT_EVENTHANDLE_HH__
#ifndef   __CORE_EVENT_IEVENTARGS_HH__
#define   __CORE_EVENT_IEVENTARGS_HH__
namespace fwk {
  class IEventArgs {
  public:
    virtual ~IEventArgs(void) {}
  public:
    virtual void  cleanup(void) = 0;
  };
}
#endif

namespace fwk {
  class EventHandle :public APooled<EventHandle> {
  public:
    EventHandle(void);
  public:
    virtual void reinit(void);
  public:
    void  fireAsync(IEventArgs* args) const;
    void  fireSync(IEventArgs* args) const;
    void  subscribe(const std::function<void (const IEventArgs*)>& callback, const void *key) const;
    void  unsubscribe(const void *key) const;
    void  registerToManager(void) const;
    void  unregisterFromManager(void) const;
  };
}
#endif

namespace fwk {
  struct TcpSocketStreamEventArgs :public IEventArgs, public APooled<TcpSocketStreamEventArgs> {
  public:
    TcpSocketStream* socket;
  public:
    TcpSocketStreamEventArgs(void);
  public:
    virtual void  reinit(void);
    virtual void  cleanup(void);
    void  init(TcpSocketStream* ss);
  };
  struct TcpSocketEventArgs :public IEventArgs, public APooled<TcpSocketEventArgs> {
  public:
    TcpSocket* socket;
  public:
    TcpSocketEventArgs(void);
  public:
    virtual void  reinit(void);
    virtual void  cleanup(void);
    void  init(TcpSocket* s);
  };
  class TcpManager {
  public:
    struct Server :public Lockable {
    public:
      uint16_t port;
      TcpSocket* server;
      TLockable<std::list<TcpSocketStream*>> clients;
      std::set<uint32_t> accept;
      std::set<uint32_t> blacklist;
      bool active;
      struct {
        EventHandle* onAccept;
        EventHandle* onReceivedData;
        EventHandle* onClientClosed;
        EventHandle* onClosed;
      } events;
    public:
      Server(uint16_t port, TcpSocket* server);
      virtual ~Server(void);
    };
  public:
    struct Client :public Lockable {
    public:
      std::string hostname;
      uint16_t port;
      TcpSocketStream *socket;
      bool active;
      struct {
        EventHandle* onReceivedData;
        EventHandle* onClosed;
      } events;
    public:
      Client(const std::string& hostname, uint16_t port, TcpSocketStream* socket);
      virtual ~Client(void);
    };
  public:
    typedef TLockable<std::list<Server>> ServerList;
    typedef TLockable<std::list<Client>> ClientList;
  private:
    ServerList _servers;
    ClientList _clients;
    NotifiableThread& _input;
    NotifiableThread& _output;
  public:
    TcpManager(NotifiableThread& input, NotifiableThread& output);
    virtual ~TcpManager(void);
  public:
    void  clear(void);
  public:
    const Server&  createServer(uint16_t port);
    void  run(const Server& server);
    void  close(uint16_t port);
    void  close(const Server& server);
    void  blacklist(uint16_t port, uint32_t ip);
  public:
    const Client&  createClient(const std::string& hostname, uint16_t port);
    void  run(const Client& client);
    void  close(const std::string& hostname, uint16_t port);
    void  close(const Client& client);
  public:
    void  push(TcpSocketStream* ss, const void* ptr, size_t size);
    void  push(TcpSocketStream* ss, const ByteArray* bytearray);
  public:
    void  fillSetRead(fd_set& set, int& max_fd, uint32_t& nb);
    void  fillSetWrite(fd_set& set, int& max_fd, uint32_t& nb);
    void  send(fd_set& set);
    void  recv(fd_set& set);
  private:
    void  __onIOException(EventHandle* event, TcpSocketStream* ss, const std::string& message);
    void  __fireEvent(EventHandle *event, TcpSocketStream* socket) const;
    void  __fireEvent(EventHandle *event, TcpSocket* socket) const;
  };
}
#endif

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
  class AUdpSocket :public virtual Lockable {
  protected:
    int _fd;
    ByteArray* _buffer;
  public:
    AUdpSocket(void);
    virtual ~AUdpSocket(void);
  public:
    virtual void  reinit(void);
  public:
    virtual void  init(void);
  public:
    virtual void  socket(void);
    virtual void  close(void);
  public:
    virtual void  addToSet(fd_set& set, int& max_fd) const;
    virtual bool  isset(fd_set&) const;
  };
}
#endif

#ifndef   __CORE_NETWORK_UDP_UDPSOCKETCLIENT_HH__
#define   __CORE_NETWORK_UDP_UDPSOCKETCLIENT_HH__
#include  <sys/select.h>
#ifndef   __CORE_NETWORK_UDP_AUDPSOCKETIO_HH__
#define   __CORE_NETWORK_UDP_AUDPSOCKETIO_HH__
#include  <queue>
#include  <utility>
#include  <netinet/in.h>
namespace fwk {
  class AUdpSocketIO :public virtual Lockable {
  protected:
    sockaddr_in _addr;
    std::pair<std::queue<ByteArray*>, size_t> _input;
    std::pair<std::queue<ByteArray*>, size_t> _output;
  public:
    static const size_t BUFFER_SIZE;
  public:
    AUdpSocketIO(void);
    virtual ~AUdpSocketIO(void);
  public:
    virtual void  reinit(void);
  public:
    virtual bool  hasDataToSend(void) const;
    virtual void  push(ByteArray* datagram);
    virtual ByteArray*  getData(void);
  public:
    virtual bool  operator==(const sockaddr_in& addr) const;
  };
}
#endif

namespace fwk {
  class UdpSocketClient :public AUdpSocketIO, public APooled<UdpSocketClient> {
  private:
    std::pair<uint32_t, uint16_t> _info;
  public:
    UdpSocketClient(void);
    virtual ~UdpSocketClient(void);
  public:
    virtual void  reinit(void);
  public:
    void  init(const sockaddr_in& addr);
  public:
    ByteArray*  nextDatagram(void);
    void  received(ByteArray* datagram);
  public:
    const sockaddr_in&  socketAddress(void) const;
  public:
    const std::pair<uint32_t, uint16_t>&  clientInformation(void) const;
  };
}
#endif

namespace fwk {
  class UdpSocketServer :public AUdpSocket, public APooled<UdpSocketServer>  {
  public:
    UdpSocketServer(void);
    virtual ~UdpSocketServer(void);
  public:
    virtual void reinit(void);
  public:
    void  bind(uint16_t port);
  public:
    ByteArray*  recvfrom(struct sockaddr_in& addr);
    ssize_t  sendto(UdpSocketClient* socket);
  };
}
#endif

#ifndef   __CORE_NETWORK_UDP_UDPSOCKETSTREAM_HH__
#define   __CORE_NETWORK_UDP_UDPSOCKETSTREAM_HH__
namespace fwk {
  class UdpSocketStream: public AUdpSocketIO, public AUdpSocket, public APooled<UdpSocketStream> {
  public:
    UdpSocketStream(void);
    virtual ~UdpSocketStream(void);
  public:
    virtual void  reinit(void);
  public:
    void  init(const std::string& hostname, uint16_t port);
  public:
    ssize_t  sendto(void);
    ssize_t  recvfrom(void);
  };
}
#endif

namespace fwk {
  struct UdpSocketStreamEventArgs :public IEventArgs, public APooled<UdpSocketStreamEventArgs> {
  public:
    UdpSocketStream* socket;
  public:
    UdpSocketStreamEventArgs(void);
  public:
    virtual void  reinit(void);
    virtual void  cleanup(void);
    virtual void  init(UdpSocketStream* socket);
  };
  struct UdpSocketClientEventArgs :public IEventArgs, public APooled<UdpSocketClientEventArgs> {
  public:
    UdpSocketClient* socket;
  public:
    UdpSocketClientEventArgs(void);
  public:
    virtual void  reinit(void);
    virtual void  cleanup(void);
    virtual void  init(UdpSocketClient* socket);
  };
  struct UdpSocketServerEventArgs :public IEventArgs, public APooled<UdpSocketServerEventArgs> {
  public:
    UdpSocketServer* socket;
  public:
    UdpSocketServerEventArgs(void);
  public:
    virtual void  reinit(void);
    virtual void  cleanup(void);
    virtual void  init(UdpSocketServer* socket);
  };
  class UdpManager {
  public:
    struct Server :public Lockable {
    public:
      uint16_t port;
      UdpSocketServer* server;
      TLockable<std::list<UdpSocketClient*>> clients;
      std::set<uint32_t> accept;
      std::set<uint32_t> blacklist;
      bool active;
      struct {
        EventHandle* onNewClient;
        EventHandle* onReceivedData;
        EventHandle* onClientClosed;
        EventHandle* onClosed;
      } events;
    public:
      Server(uint16_t port, UdpSocketServer* server);
      virtual ~Server(void);
    };
  public:
    struct Client :public Lockable {
    public:
      std::string hostname;
      uint16_t port;
      UdpSocketStream *socket;
      bool active;
      struct {
        EventHandle* onReceivedData;
        EventHandle* onClosed;
      } events;
    public:
      Client(const std::string& hostname, uint16_t port, UdpSocketStream* socket);
      virtual ~Client(void);
    };
  public:
    typedef TLockable<std::list<Server>> ServerList;
    typedef TLockable<std::list<Client>> ClientList;
  private:
    ServerList _servers;
    ClientList _clients;
    NotifiableThread& _input;
    NotifiableThread& _output;
  public:
    UdpManager(NotifiableThread& input, NotifiableThread& output);
    virtual ~UdpManager(void);
  public:
    void  clear(void);
  public:
    const Server&  createServer(uint16_t port);
    void run(const Server& server);
    void  close(uint16_t port);
    void  close(const Server& server);
    void  blacklist(uint16_t port, uint32_t ip);
  public:
    const Client&  createClient(const std::string& hostname, uint16_t port);
    void run(const Client& client);
    void  close(const std::string& hostname, uint16_t port);
    void  close(const Client& client);
  public:
    void  push(AUdpSocketIO* socket, const void* ptr, size_t size);
    void  push(AUdpSocketIO* socket, const ByteArray* bytearray);
  public:
    void  fillSetRead(fd_set& set, int& max_fd, uint32_t& nb);
    void  fillSetWrite(fd_set& set, int& max_fd, uint32_t& nb);
    void  send(fd_set& set);
    void  recv(fd_set& set);
  private:
    void  __onIOException(EventHandle *event, UdpSocketClient *socket, const std::string& message);
    void  __onIOException(EventHandle *event, UdpSocketStream *socket, const std::string& message);
    void  __fireEvent(EventHandle *event, UdpSocketStream *socket) const;
    void  __fireEvent(EventHandle *event, UdpSocketClient *socket) const;
    void  __fireEvent(EventHandle *event, UdpSocketServer *socket) const;
  };
}
#endif

namespace fwk {
  class NetworkManager :public Singleton<fwk::NetworkManager>, public Lockable, public AEndable, public Initializable {
    friend class Singleton<fwk::NetworkManager>;
  public:
    NotifiableThread _input;
    NotifiableThread _output;
    TcpManager _tcp;
    UdpManager _udp;
  private:
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager(const NetworkManager&&) = delete;
    NetworkManager&  operator=(const NetworkManager&) = delete;
  private:
    NetworkManager(void);
    virtual ~NetworkManager(void);
  public:
    virtual void  end(void);
  public:
    void  init(void);
    void  endTCP(void);
    void  endUDP(void);
  private:
    void  inputRoutine(void);
    void  outputRoutine(void);
  public:
    TcpManager&  getTCP(void);
    UdpManager&  getUDP(void);
  };
}
#endif

#ifndef    __CORE_NETWORK_EXCEPTION_HH__
#define    __CORE_NETWORK_EXCEPTION_HH__
namespace fwk {
  class NetworkException :public CoreException {
    public:
    NetworkException(const std::string& message) noexcept;
    virtual ~NetworkException(void) noexcept;
  };
}
#endif

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
  struct AHttpMessage {
  public:
    std::string  version;
    std::map<std::string, std::string>  headers;
    ByteArray  *body;
  public:
    AHttpMessage(void);
    virtual ~AHttpMessage(void);
  public:
    virtual void  reinit(void);
  public:
    void  init(void);
  public:
    const std::string&  getHeader(const std::string& header) const;
    bool  hasHeader(const std::string& header) const;
  };
}
#endif

#ifndef   __CORE_NETWORK_HTTP_HTTPRESPONSE_HH__
#define   __CORE_NETWORK_HTTP_HTTPRESPONSE_HH__
namespace fwk {
  struct HttpResponse :public AHttpMessage, public APooled<HttpResponse> {
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
    uint32_t  status;
    std::string reason;
    HttpResponse(void);
    virtual ~HttpResponse(void);
    virtual void  reinit(void);
  };
}
#endif

namespace fwk {
  struct HttpRequest :public AHttpMessage, public APooled<HttpRequest> {
    std::string method;
    std::string url;
    std::function<void (const HttpResponse *)> success;
    std::function<void (const HttpResponse *)> error;
    std::function<void (void)> clean;
    struct {
      bool isFile;
      std::string filepath;
    } file;
    struct {
      bool isAsynchronous;
      bool isValid;
      Notifiable lock;
      HttpResponse* response;
    } asynchronous;
    HttpRequest(void);
    virtual ~HttpRequest(void);
    virtual void  reinit(void);
    void  cleanResponse(void);
    bool  wait(void);
    void  wake(HttpResponse* response);
  };
}
#endif

#ifndef   __CORE_NETWORK_HTTP_HTTPPROTOCOL_HH__
#define   __CORE_NETWORK_HTTP_HTTPPROTOCOL_HH__
#include  <string>
namespace fwk {
  enum class HttpProtocol {
    HTTP,
    HTTPS,
    FTP
  };
  static const BidiMap<HttpProtocol, const std::string> HttpProtocolToString = {
    {HttpProtocol::HTTP, std::string("http")},
    {HttpProtocol::HTTPS, std::string("https")},
    {HttpProtocol::FTP, std::string("ftp")},
  };
}
#endif

namespace fwk {
  class HttpConnection :public Lockable, public AEndable {
  protected:
    struct upload_object {
      const uint8_t* ptr;
      size_t length;
    };
  protected:
    std::string  _host;
    uint16_t     _port;
    HttpProtocol _protocol;
    std::string  _userAgent;
    std::thread  *_thread;
    TNotifiable<std::queue<HttpRequest*>>  _pendingRequests;
  public:
    HttpConnection(const std::string &hostname, uint16_t port, HttpProtocol protocol, const std::string& userAgent);
    virtual ~HttpConnection(void);
  public:
    virtual void  end(void);
  public:
    virtual void  run(void);
    const std::string&  getHost(void) const;
    uint16_t  getPort(void) const;
    HttpProtocol  getProtocol(void) const;
    void  addRequest(HttpRequest *request);
  protected:
    HttpResponse*  sendRequest(const HttpRequest *request) const;
    void  routine(void);
    HttpResponse*  exec(const HttpRequest *request) const;
    curlxx::EasyHandle*  prepareHandle(const HttpRequest *request, HttpResponse *response) const;
    void  prepareHandle(curlxx::EasyHandle* handle, const HttpRequest *request, HttpResponse *response) const;
    void  setResponse(HttpRequest* request, HttpResponse* response) const;
  protected:
    static size_t  read_callback(void*, size_t, size_t, void*);
    static size_t  write_callback(void*, size_t, size_t, void*);
    static size_t  header_callback(void*, size_t, size_t, void*);
  };
}
#endif

namespace fwk {
  class HttpPipeliningConnection :public HttpConnection {
  private:
    typedef std::unordered_map<curlxx::EasyHandle*, std::tuple<HttpRequest*, HttpResponse*, bool>> HandlesMap;
  public:
    HttpPipeliningConnection(const std::string &hostname, uint16_t port, HttpProtocol protocol, const std::string& userAgent);
    virtual ~HttpPipeliningConnection(void);
  public:
    virtual void  run(void);
  private:
    void  routine(void);
  private:
    void  sendPipeline(HandlesMap& pipelined, curlxx::MultiHandle& multiHandle) const;
    void  getAnswers(HandlesMap& pipelined, curlxx::MultiHandle& multiHandle) const;
  };
}
#endif

#ifndef   __CORE_NETWORK_HTTP_HTTPCLIENT_HH__
#define   __CORE_NETWORK_HTTP_HTTPCLIENT_HH__
#include  <vector>
namespace fwk {
  class HttpClient: public Singleton<fwk::HttpClient>, public Lockable, public AEndable {
    friend class Singleton<fwk::HttpClient>;
  private:
    std::string _userAgent;
    std::vector<HttpConnection*> _connections;
  public:
    static const std::string defaultUserAgent;
  private:
    HttpClient(const HttpClient&) = delete;
    HttpClient(const HttpClient&&) = delete;
    HttpClient& operator=(const HttpClient&) = delete;
  private:
    HttpClient(void);
    virtual ~HttpClient(void);
  public:
    virtual void  end(void);
  public:
    void  init(const std::string& user_agent);
    HttpConnection*  initConnection(const std::string& hostname, uint16_t port = 80, HttpProtocol protocol = HttpProtocol::HTTP, bool enablePipelining = true);
    void  sendRequest(HttpConnection *connection, HttpRequest *request);
  };
}
#endif

#ifndef   __CORE_WORKER_DELAYEDTASKSTHREAD_HH__
#define   __CORE_WORKER_DELAYEDTASKSTHREAD_HH__
#include  <thread>
namespace fwk {
  class DelayedTasksThread :public Singleton<fwk::DelayedTasksThread>, public Lockable, public AEndable {
    friend class Singleton<fwk::DelayedTasksThread>;
  private:
    std::thread *_thread;
  private:
    DelayedTasksThread(const DelayedTasksThread&) = delete;
    DelayedTasksThread(const DelayedTasksThread&&) = delete;
    DelayedTasksThread& operator=(const DelayedTasksThread&) = delete;
  private:
    DelayedTasksThread(void);
    virtual ~DelayedTasksThread(void);
  public:
    virtual void  end(void);
  public:
    void  run(void);
  private:
    void  routine(void) const;
  };
}
#endif

#ifndef    __CORE_WORKER_WORKERTHREAD_HH__
#define    __CORE_WORKER_WORKERTHREAD_HH__
#include  <thread>
#include  <map>
#ifndef    __CORE_WORKER_TASK_HH__
#define    __CORE_WORKER_TASK_HH__
#include  <chrono>
#include  <functional>
namespace fwk {
  class ATask {
  public:
    enum class Source {
      SIMPLE,
      EVENT,
      HTTP_CALLBACK,
      PERIODIC_TASK
    };
  private:
    Source  _source;
  public:
    ATask(Source source);
    virtual ~ATask(void);
  public:
    Source  getSource(void) const;
  };
  class SimpleTask :public ATask, public APooled<SimpleTask> {
  public:
    std::function<void (void)> _callback;
    std::function<void (void)> _cleanup;
  public:
    SimpleTask(void);
    virtual ~SimpleTask(void);
  public:
    virtual void  reinit(void);
  public:
    void  init(const std::function<void (void)>& callback);
    void  init(const std::function<void (void)>& callback, const std::function<void (void)>& cleanup);
  };
  class EventTask :public ATask, public APooled<EventTask> {
  public:
    std::chrono::steady_clock::time_point _eventCreation;
    const EventHandle* _event;
    IEventArgs*        _args;
  public:
    EventTask(void);
    virtual ~EventTask(void);
  public:
    virtual void  reinit(void);
  public:
    void  init(const EventHandle* event, IEventArgs* args);
  };
  class HttpTask :public ATask, public APooled<HttpTask> {
  public:
    std::function<void (const HttpResponse*)> _callback;
    std::function<void (void)> _cleanup;
    HttpResponse* _response;
  public:
    HttpTask(void);
    virtual ~HttpTask(void);
  public:
    virtual void  reinit(void);
  public:
    void  init(const std::function<void (const HttpResponse*)>& callback, const std::function<void (void)>& cleanup, HttpResponse* response);
  };
  class PeriodicTask :public ATask, public APooled<PeriodicTask> {
  public:
    std::function<void (void)> _callback;
    std::function<void (void)> _clean;
    std::chrono::steady_clock::duration _interval;
    bool  _off;
  public:
    PeriodicTask(void);
    virtual ~PeriodicTask(void);
  public:
    virtual void  reinit(void);
  public:
    void  init(const std::function<void(void)>& callback, const std::function<void(void)>& clean, const std::chrono::steady_clock::duration& interval);
    void  stop();
  };
  class DelayedTask :public APooled<DelayedTask> {
  public:
    ATask*  _task;
    std::chrono::steady_clock::time_point  _timePoint;
  public:
    DelayedTask(void);
    virtual ~DelayedTask(void);
  public:
    virtual void  reinit(void);
  public:
    void  init(ATask* task, const std::chrono::steady_clock::time_point& tp);
    void  init(ATask* task, const std::chrono::steady_clock::duration& duration);
  public:
    bool  operator<(const DelayedTask& oth) const;
    bool  operator>(const DelayedTask& oth) const;
    bool  operator==(const ATask* task) const;
  };
}
#endif

namespace fwk {
  class WorkerThread :public Lockable, public AEndable {
  private:
    typedef void (*WorkerHandler)(ATask*, bool);
    static const std::map<ATask::Source, WorkerHandler> TaskHandlerMap;
  public:
    static void cleanup();
  private:
    size_t _id;
    std::thread *_thread;
  public:
    WorkerThread(size_t id);
    virtual ~WorkerThread(void);
  public:
    virtual void  end(void);
  public:
    size_t  getID(void) const;
  public:
    void  routine(void);
  public:
    static void  executeSimpleTask(ATask* task, bool exec = true);
    static void  executeEventTask(ATask* task, bool exec = true);
    static void  executeHttpTask(ATask* task, bool exec = true);
    static void  executePeriodicTask(ATask* task, bool exec = true);
  };
}
#endif

#ifndef    __CORE_WORKER_WORKERMANAGER_HH__
#define    __CORE_WORKER_WORKERMANAGER_HH__
#include  <queue>
#include  <chrono>
namespace fwk {
  class WorkerManager :public Singleton<fwk::WorkerManager>, public Lockable, public AEndable {
    friend class Singleton<fwk::WorkerManager>;
  public:
    typedef TNotifiable<std::queue<ATask*>>  TaskQueue;
    typedef TNotifiable<std::priority_queue<DelayedTask*, std::vector<DelayedTask*>, std::function<bool (const DelayedTask*, const DelayedTask*)>>>  DelayedTaskQueue;
  private:
    TaskQueue _pendingTasks;
    DelayedTaskQueue _delayedTasks;
    std::vector<WorkerThread*> _workers;
  private:
    WorkerManager(const WorkerManager&) = delete;
    WorkerManager(const WorkerManager&&) = delete;
    WorkerManager&  operator=(const WorkerManager&) = delete;
  private:
    WorkerManager(void);
    virtual ~WorkerManager(void);
  public:
    virtual void  end(void);
  public:
    TaskQueue&  getTaskQueue(void);
    DelayedTaskQueue&  getDelayedTaskQueue(void);
  public:
    void  init(size_t nbWorkers, bool delayedTasks);
    void  addTask(ATask* task);
    void  addDelayedTask(DelayedTask* dtask);
    void  addSimpleTask(const std::function<void (void)>& callback);
    void  addSimpleTask(const std::function<void (void)>& callback, const std::function<void (void)>& cleanup);
    void  addEventTask(const EventHandle* event, IEventArgs* args);
    void  addHttpTask(const std::function<void (const HttpResponse*)>& callback, const std::function<void (void)>& cleanup, HttpResponse* response);
    void  addDelayedTask(ATask* task, const std::chrono::steady_clock::time_point& tp);
    void  addDelayedTask(ATask* task, const std::chrono::steady_clock::duration& duration);
    void  addPeriodicTask(const std::function<void(void)>& callback, const std::function<void(void)>& cleanup, const std::chrono::steady_clock::duration& duration, bool startNow = true);
    void  addPeriodicTask(PeriodicTask* periodicTask, bool startNow = true);
  };
}
#endif

#ifndef   __CORE_EVENT_EXCEPTION_HH__
#define   __CORE_EVENT_EXCEPTION_HH__
namespace fwk {
  class EventNotRegisteredException :public CoreException {
  public:
    EventNotRegisteredException(const std::string& message = "This event was not registered") noexcept;
    virtual ~EventNotRegisteredException(void) noexcept;
  };
}
#endif

#ifndef    __CORE_EVENT_EVENTMANAGER_HH__
#define    __CORE_EVENT_EVENTMANAGER_HH__
#include  <functional>
#include  <unordered_map>
#ifndef   __CORE_EVENT_EVENTINFO_HH__
#define   __CORE_EVENT_EVENTINFO_HH__
#include  <functional>
#include  <map>
namespace fwk {
  struct EventInfo {
  public:
    const EventHandle * base;
    std::map<const void *, std::function<void (const IEventArgs*)>> subscribers;
  public:
    EventInfo(const EventHandle *event);
    EventInfo(const EventInfo& oth);
    EventInfo& operator=(const EventInfo& oth);
  public:
    void  addSubscriber(const void* key, const std::function<void (const IEventArgs*)>& callback);
    void  delSubscriber(const void* key);
  };
}
#endif

namespace fwk {
  class EventManager :public Singleton<fwk::EventManager>, public Lockable {
    friend class Singleton<fwk::EventManager>;
  private:
    std::unordered_map<const EventHandle*, EventInfo>  _events;
  private:
    EventManager(const EventManager&) = delete;
    EventManager(const EventManager&&) = delete;
    EventManager& operator=(const EventManager&) = delete;
  private:
    EventManager(void);
    virtual ~EventManager(void);
  public:
    void  registerEvent(const EventHandle* event);
    void  unregisterEvent(const EventHandle* event);
    void  subscribeToEvent(const EventHandle* event, const std::function<void (const IEventArgs*)>& callback, const void *key);
    void  unsubscribeFromEvent(const EventHandle* event, const void *key);
  public:
    void  fireEventAsync(const EventHandle* event, IEventArgs* args) const;
    void  fireEventSync(const EventHandle* event, IEventArgs* args) const;
  public:
    const EventInfo&  getInfo(const EventHandle* event) const;
  };
}
#endif

