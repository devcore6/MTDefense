#pragma once
#include <cstdint>
#include <cstring>

#include <chrono>
#include <string>
#include <concepts>
#include <functional>
#include <atomic>
#include <ios>
#include <sstream>

// GCC 11.2.0 has an issue with constexpr functions that return non-POD types - does GCC treat consteval and constexpr the same?
#ifdef __GNUC__
# define INLINE_CONSTEXPR inline
#else
# define INLINE_CONSTEXPR inline constexpr
#endif

// Convenient literal suffixes

using  int08_t =  int8_t;
using uint08_t = uint8_t;

inline consteval  uint08_t operator ""   _u8(unsigned long long arg) noexcept { return static_cast< uint08_t>(arg); }
inline consteval  uint16_t operator ""  _u16(unsigned long long arg) noexcept { return static_cast< uint16_t>(arg); }
inline consteval  uint32_t operator ""  _u32(unsigned long long arg) noexcept { return static_cast< uint32_t>(arg); }
inline consteval  uint64_t operator ""  _u64(unsigned long long arg) noexcept { return static_cast< uint64_t>(arg); }

inline consteval   int08_t operator ""   _i8(unsigned long long arg) noexcept { return static_cast<  int08_t>(arg); }
inline consteval   int16_t operator ""  _i16(unsigned long long arg) noexcept { return static_cast<  int16_t>(arg); }
inline consteval   int32_t operator ""  _i32(unsigned long long arg) noexcept { return static_cast<  int32_t>(arg); }
inline consteval   int64_t operator ""  _i64(unsigned long long arg) noexcept { return static_cast<  int64_t>(arg); }

inline consteval  intmax_t operator "" _imax(unsigned long long arg) noexcept { return static_cast< intmax_t>(arg); }
inline consteval uintmax_t operator "" _umax(unsigned long long arg) noexcept { return static_cast<uintmax_t>(arg); }

// This is only needed until "z" and "zu" suffixes are finalized
inline consteval ptrdiff_t operator ""    _z(unsigned long long arg) noexcept { return static_cast<ptrdiff_t>(arg); }
inline consteval    size_t operator ""   _zu(unsigned long long arg) noexcept { return static_cast<   size_t>(arg); }

// These also exist in std::literals

INLINE_CONSTEXPR std::string operator "" _str(const char* arg, size_t size) noexcept { return std::string(arg); }

INLINE_CONSTEXPR std::chrono::nanoseconds  operator "" _ns(unsigned long long arg) noexcept {
    return static_cast<std::chrono::nanoseconds >(arg); }
INLINE_CONSTEXPR std::chrono::microseconds operator "" _us(unsigned long long arg) noexcept {
    return static_cast<std::chrono::microseconds>(arg); }
INLINE_CONSTEXPR std::chrono::microseconds operator "" _µs(unsigned long long arg) noexcept {
    return static_cast<std::chrono::microseconds>(arg); }
INLINE_CONSTEXPR std::chrono::milliseconds operator "" _ms(unsigned long long arg) noexcept {
    return static_cast<std::chrono::milliseconds>(arg); }
INLINE_CONSTEXPR std::chrono::seconds      operator ""  _s(unsigned long long arg) noexcept {
    return static_cast<std::chrono::seconds     >(arg); }

// Container and loop utilities

template<class T>
concept two_way_iterable = requires(T arr) { arr.begin(); arr.rbegin(); arr.end(); arr.rend(); };

template<class T>
concept iterable = requires(T arr) { arr.begin(); arr.end(); };

template<std::equality_comparable T, iterable ArrT = std::vector<T>>
inline bool contains(const ArrT arr, T val) { for(T v : arr) if(v == val) return true; return false; }


template<class T>
concept reverse_iterable = requires(T arr) { arr.rbegin(); arr.rend(); };

template<reverse_iterable T>
struct reverse_impl {
    T& data;
    auto begin() { return data.rbegin(); }
    auto end()   { return data.rend(); }
};

template<reverse_iterable T>
reverse_impl<T> reverse(T&& iterable) { return { iterable }; }


class numeric_iterator {
private:
              size_t it = 0;
    const     size_t size = 0;

public:
    constexpr        numeric_iterator() = default;
    constexpr        numeric_iterator(size_t s) : it(0), size(s)           { }
    constexpr        numeric_iterator(size_t i, size_t s) : it(i), size(s) { }

    constexpr size_t operator* ()                                          { return it; }
    constexpr auto&  operator++()                                          { ++it; return *this; }
    constexpr bool   operator==(numeric_iterator& y)                       { return it == y.it; }
};

class reverse_numeric_iterator {
private:
              size_t it = 0;
    const     size_t size = 0;

public:
    constexpr        reverse_numeric_iterator() = default;
    constexpr        reverse_numeric_iterator(size_t s) : it(s - 1), size(s)               { }
    constexpr        reverse_numeric_iterator(size_t i, size_t s) : it(s - 1 - i), size(s) { }

    constexpr size_t operator* ()                                                          { return it; }
    constexpr auto&  operator++()                                                          { --it; return *this; }
    constexpr bool   operator==(reverse_numeric_iterator& y)                               { return it == y.it; }
};

class iterate_impl {
public:
    using iterator         = numeric_iterator;
    using reverse_iterator = reverse_numeric_iterator;
    using value_type       = size_t;

private:
    iterator         _begin;
    iterator         _end;
    reverse_iterator _rbegin;
    reverse_iterator _rend;

public:
                     iterate_impl(size_t s) : _begin(s), _end(s, s), _rbegin(s), _rend(s, s)                 { }
                     iterate_impl(size_t b, size_t s) : _begin(b, s), _end(s, s), _rbegin(b, s), _rend(s, s) { }
    iterator         begin()                                                                                 { return _begin; }
    iterator         end()                                                                                   { return _end; }
    reverse_iterator rbegin()                                                                                { return _rbegin; }
    reverse_iterator rend()                                                                                  { return _rend; }
};

template<std::convertible_to<size_t> T>
iterate_impl iterate(T size) { return iterate_impl(size); }

template<std::convertible_to<size_t> T, std::convertible_to<size_t> U>
iterate_impl iterate(T begin, U size) { return iterate_impl(begin, size); }


template<two_way_iterable T>
struct offset_impl {
    T& data;
    size_t offset = 0;
    auto   begin()  { return data.begin() + offset; }
    auto   rbegin() { return data.rbegin() + offset; }
    auto   end()    { return data.end(); }
    auto   rend()   { return data.rend(); }
};

template<two_way_iterable T, std::convertible_to<size_t> U>
offset_impl<T> iterate(T arr, U offset) { return { arr, offset }; }

template<iterable T>
class sized_iterator {
private:
    using reference   = T::reference;
    using pointer     = T::pointer;
          T&     data;
          size_t it   = 0;
    const size_t size = 0;
    const size_t N    = 1;

public:
    constexpr         sized_iterator() = default;
    constexpr         sized_iterator(T& val, size_t i, size_t s, size_t n) :
                                    data(val), N(n), it(i), size(s)    { }
    reference         operator*()                                      { return data[it]; }
    pointer           operator->()                                     { return &data[it]; }
    constexpr auto&   operator++()                                     { it += N; return *this; }
    constexpr bool    operator==(sized_iterator<T>& y)                 { return it >= y.it; }
};

template<reverse_iterable T>
class reverse_sized_iterator {
private:
    using reference   = T::reference;
    using pointer     = T::pointer;
          T&      data;
          size_t  it   = 0;
    const size_t  size = 0;
    const size_t  N    = 1;

public:
    constexpr         reverse_sized_iterator() = default;
    constexpr         reverse_sized_iterator(T& val, size_t i, size_t s, size_t n) :
                                            data(val), N(n), it(s - 1 - i), size(s) { }
    reference         operator*()                                                   { return data[it]; }
    pointer           operator->()                                                  { return &data[it]; }
    constexpr auto&   operator++()                                                  { it -= N; return *this; }
    constexpr bool    operator==(reverse_sized_iterator<T>& y)                      { return it <= y.it; }
};

template<two_way_iterable T>
class sized_iterator_impl {
public:
    using iterator         = sized_iterator<T>;
    using reverse_iterator = reverse_sized_iterator<T>;
    using value_type       = T::value_type;
    using reference        = T::reference;

private:
    T&               data;
    iterator         _begin;
    iterator         _end;
    reverse_iterator _rbegin;
    reverse_iterator _rend;

public:
         sized_iterator_impl(T& val, size_t N, size_t O) : data(val),
                                                           _begin(val, O, val.size(), N),
                                                           _end(val, val.size(), val.size(), N),
                                                           _rbegin(val, O, val.size(), N),
                                                           _rend(val, val.size(), val.size(), N) { }
    auto begin()                                                                                 { return _begin; }
    auto rbegin()                                                                                { return _rbegin; }
    auto end()                                                                                   { return _end; }
    auto rend()                                                                                  { return _rend; }
};

template<two_way_iterable T>
sized_iterator_impl<T> iterate(T& arr, size_t N, size_t O) { return sized_iterator_impl<T>(arr, N, O); }

// Numeric utilities
// min, max and clamp exist in <algorithm>, oh well, this is kind of neater anyway.

template<typename T, std::convertible_to<T> U>
inline constexpr T min(T x, U y) { return x < static_cast<T>(y) ? x : static_cast<T>(y); }

template<typename T, std::convertible_to<T> U>
inline constexpr T max(T x, U y) { return x > static_cast<T>(y) ? x : static_cast<T>(y); }

template<typename T, std::convertible_to<T> U, std::convertible_to<T> V>
inline constexpr T clamp(U min_val, T val, V max_val) { return max(min(val, static_cast<T>(max_val)), static_cast<T>(min_val)); }

template<typename T, std::convertible_to<T> U, std::convertible_to<T> V>
inline constexpr bool contains(U min_val, T val, V max_val) { return clamp(min_val, val, max_val) == val; }

// Like clamp, but loops back on over/underflow
template<typename T, std::convertible_to<T> U, std::convertible_to<T> V>
inline constexpr T limit(U min_val, T val, V max_val) {
    if(contains(min_val, val, max_val)) return val;
    T span = static_cast<T>(max_val) - static_cast<T>(min_val);
    if(val > static_cast<T>(max_val)) {
        while(val > static_cast<T>(max_val)) val -= span + 1;
        return val;
    }
    while(val < static_cast<T>(min_val)) val += span + 1;
    return val;
}

inline std::string to_string_digits(double d, size_t decimal_digits = 2) {
    std::ostringstream out;
    out.precision(decimal_digits);
    out << std::fixed << d;
    return out.str();
}

template<typename T, T min, T val, T max>
concept in_range = min < val && val < max;

// String Utilities

inline bool contains(const char* arr, char val) { while(*arr != 0) { if(*arr == val) return true; arr++; } return false; }

inline void erase_all(std::string& s, const std::string& to_erase) {
    size_t pos;
    while((pos = s.find(to_erase)) != std::string::npos) s.erase(pos, to_erase.length());
}

inline void erase_all(std::string& s, const std::string& to_erase_begin, const std::string& to_erase_end) {
    size_t pos;
    while((pos = s.find(to_erase_begin)) != std::string::npos && s.find(to_erase_end, pos) != std::string::npos) s.erase(pos, s.find(to_erase_end, pos) + to_erase_end.length() - pos);
}

inline void replace_all(std::string& s, const std::string& to_replace, const char* replace_with) {
    size_t pos;
    while((pos = s.find(to_replace)) != std::string::npos) s.replace(pos, to_replace.length(), replace_with);
}

inline bool match_all(std::string& s, const std::string& to_match, std::function<bool(std::string&, size_t)> callback) {
    size_t pos;
    while((pos = s.find(to_match)) != std::string::npos) if(!callback(s, pos)) return false;
    return true;
}

inline size_t find_not_escaped(std::string& s, const std::string& to_find, size_t start_pos) {
    while((start_pos = s.find(to_find, start_pos + 1)) != std::string::npos) if(start_pos == 0 || s[start_pos - 1] != '\\') return start_pos;
    return std::string::npos;
}

// todo: possibly optimize this?
inline std::vector<std::string> split(std::string& s, const char& separator, const char& escape_character, bool keep_escape_chars = false) {
    std::vector<std::string> out;
    bool escaped = false;
    size_t len = s.length();
    size_t pos = 0;
    std::string cur = "";

    // Skip any leading separators
    while(pos != len && s[pos] == separator) pos++;

    while(pos != len) {
        char& c = s[pos];
        if(c == escape_character && (pos == 0 || s[pos - 1] != '\\')) {
            escaped = !escaped;
            if(keep_escape_chars) cur += c;
        } else if(c == separator && !escaped) {
            out.push_back(cur);
            cur = "";
            // Skip any subsequent separators
            while(s[pos] == separator) pos++;
            continue;
        } else cur += c;
        pos++;
    }

    if(cur != "") out.push_back(cur);
    return out;
}

// Color utilities

inline uint32_t color_multiply(uint32_t c, double f) {
    return ((((uint32_t)((double)((c >> 24) & 0xFF) * f)) << 24) & 0xFF000000) |
           ((((uint32_t)((double)((c >> 16) & 0xFF) * f)) << 16) & 0x00FF0000) |
           ((((uint32_t)((double)((c >>  8) & 0xFF) * f)) <<  8) & 0x0000FF00) |
           (( (uint32_t)((double)( c        & 0xFF) * f))        & 0x000000FF);
}

// Network utilities
template<class CharT>
class basic_packetstream {
protected:
    using       char_type     = CharT;
    using       const_type    = const CharT;
    using       reference     = CharT&;
    using       pointer       = CharT*;
    using       const_pointer = const CharT*;

    using       iostate       = std::ios_base::iostate;

    pointer     _data         = nullptr;
    size_t      used_size     = 0;
    size_t      allocated     = 0;
    size_t      g             = 0;
    size_t      p             = 0;

    iostate     state { };

    static constexpr size_t alloc_size = 4096 / sizeof(char_type);
    static constexpr size_t char_size  = sizeof(char_type);

    void resize(size_t new_size) {
        char_type* buf = new char_type[new_size];
        if(_data) {
            memcpy((void*)buf, (const void*)_data, char_size * new_size);
            delete[] _data;
        }
        _data = buf;
    }

    void grow(size_t grow_by) {
        if(grow_by <= allocated - used_size) return;
        size_t new_size = allocated + grow_by;
        if(new_size % alloc_size) new_size += alloc_size - new_size % alloc_size;
        resize(new_size);
    }

public:
    basic_packetstream() = default;
    basic_packetstream(const_pointer value, size_t size) { write(value, size); }

    ~basic_packetstream() { if(_data) delete[] _data; }

    basic_packetstream& put(const_type value) {
        grow(1);
        _data[p++] = value;
        used_size = max(p, used_size);
        return *this;
    }

    basic_packetstream& get(reference value) {
        if(g >= used_size) state |= std::ios_base::failbit | std::ios_base::eofbit;
        else value = _data[g++];
        return *this;
    }

    basic_packetstream& write(const_pointer value, size_t size) {
        if(size) {
            grow(size);
            for(size_t i = 0; i < size; i++) _data[p++] = value[i];
            used_size += size;
        } else for(size_t i = 0; value[i]; i++) put(value[i]);
        return *this;
    }

    basic_packetstream& read(pointer value, size_t size) {
        for(size_t i = 0; i < size; i++) get(value[i]);
        return *this;
    }

    bool good()                                         { return state == 0; }
    bool eof()                                          { return state &  std::ios_base::eofbit; }
    bool fail()                                         { return state &  std::ios_base::failbit; }
    bool bad()                                          { return state &  std::ios_base::badbit; }

    bool operator!()                                    { return state &  std::ios_base::failbit; }
         operator bool()                                { return state & ~std::ios_base::failbit; }

    iostate rdstate()                                   { return state; }
    void setstate(iostate s)                            { state = s; }
    void clear(iostate s = std::ios_base::goodbit)      { state = s; }

    size_t tellg()                                      { return g; }
    basic_packetstream& seekg(size_t pos)               { g = pos;                                                     return *this; }

    size_t tellp()                                      { return p; }
    basic_packetstream& seekp(size_t pos)               { p = pos;                                                     return *this; }

    static size_t rel_size(size_t s)                    { return s / char_size + bool(s % char_size); }

    basic_packetstream& operator>>( int08_t      value) { write((const_pointer)&value, rel_size(1                  )); return *this; }
    basic_packetstream& operator>>(uint08_t      value) { write((const_pointer)&value, rel_size(1                  )); return *this; }
    basic_packetstream& operator<<( int16_t      value) { write((const_pointer)&value, rel_size(2                  )); return *this; }
    basic_packetstream& operator<<(uint16_t      value) { write((const_pointer)&value, rel_size(2                  )); return *this; }
    basic_packetstream& operator<<( int32_t      value) { write((const_pointer)&value, rel_size(4                  )); return *this; }
    basic_packetstream& operator<<(uint32_t      value) { write((const_pointer)&value, rel_size(4                  )); return *this; }
    basic_packetstream& operator<<( int64_t      value) { write((const_pointer)&value, rel_size(8                  )); return *this; }
    basic_packetstream& operator<<(uint64_t      value) { write((const_pointer)&value, rel_size(8                  )); return *this; }
    basic_packetstream& operator<<(float         value) { write((const_pointer)&value, rel_size(4                  )); return *this; }
    basic_packetstream& operator<<(     double   value) { write((const_pointer)&value, rel_size(8                  )); return *this; }
    basic_packetstream& operator<<(long double   value) { write((const_pointer)&value, rel_size(sizeof(long double))); return *this; }
    basic_packetstream& operator<<(bool          value) { write((const_pointer)&value, rel_size(sizeof(bool       ))); return *this; }
    basic_packetstream& operator<<(const void*   value) { write((const_pointer) value, 0);                             return *this; }

    basic_packetstream& operator>>( int08_t    & value) { read(       (pointer)&value, rel_size(1                  )); return *this; }
    basic_packetstream& operator>>(uint08_t    & value) { read(       (pointer)&value, rel_size(1                  )); return *this; }
    basic_packetstream& operator>>( int16_t    & value) { read(       (pointer)&value, rel_size(2                  )); return *this; }
    basic_packetstream& operator>>(uint16_t    & value) { read(       (pointer)&value, rel_size(2                  )); return *this; }
    basic_packetstream& operator>>( int32_t    & value) { read(       (pointer)&value, rel_size(4                  )); return *this; }
    basic_packetstream& operator>>(uint32_t    & value) { read(       (pointer)&value, rel_size(4                  )); return *this; }
    basic_packetstream& operator>>( int64_t    & value) { read(       (pointer)&value, rel_size(8                  )); return *this; }
    basic_packetstream& operator>>(uint64_t    & value) { read(       (pointer)&value, rel_size(8                  )); return *this; }
    basic_packetstream& operator>>(float       & value) { read(       (pointer)&value, rel_size(4                  )); return *this; }
    basic_packetstream& operator>>(     double & value) { read(       (pointer)&value, rel_size(8                  )); return *this; }
    basic_packetstream& operator>>(long double & value) { read(       (pointer)&value, rel_size(sizeof(long double))); return *this; }
    basic_packetstream& operator>>(bool        & value) { read(       (pointer)&value, rel_size(sizeof(bool       ))); return *this; }

    basic_packetstream& operator<<(std::basic_string<char_type>& value) { write(value.c_str(), value.length());        return *this; }
    basic_packetstream& operator>>(std::basic_string<char_type>& value) {
        value = "";
        for(; g < used_size && _data[g]; g++) value += _data[g];
        return *this;
    }

    pointer data()                                      { return _data; }
    size_t size()                                       { return used_size; }
};

using packetstream = basic_packetstream<char>;

// Error handling
template<class T, class E>
class result {
private:
    bool state;

public:
    T ok;
    E err;

    result(T ok)  : ok(ok), err(),    state(true)  { }
    result(E err) : ok(),   err(err), state(false) { }

    bool operator!    () { return !state; }
         operator bool() { return  state; }
};

template<class E>
class result<void, E> {
private:
    bool state;

public:
    E err;

    result()      : err(),    state(true)  { }
    result(E err) : err(err), state(false) { }

    bool operator!    () { return !state; }
         operator bool() { return  state; }
};

// Preprocessor repetitions and utilities
// 
// Usage: REPEAT(m, l, c, ...)
//        m: main repetition macro
//        l: last repetition macro
//        c: amount of repetitions
//        d: data for macro
//      ...: additional data for macro
// 
// Usage: ENUM_PARAMS(c, p)
//        c: amount of parameters
//        p: parameter prefix
// 
// Example:    template<ENUM_PARAMS(3, typename T)>
// 
// Expands to: template<typename T1, typename T2, typename T3>
// 
// Usage: EXPAND(c, m, d, ...)
//        c: amount of repetitions
//        m: macro being repeated for each parameter
//        d: data passed to macro
//      ...: list of parameters to loop through the macro
// 
// Example:    #define PRINT(x) printf("%i", x);
//             EXPAND(4, PRINT, 3, 2, 1, 0)
// 
// Expands to: printf("%i", 3);
//             printf("%i", 2);
//             printf("%i", 1);
//             printf("%i", 0);

#define REPEAT_F(m, c, d, ...) REPEAT_   ## c(m, d, ## __VA_ARGS__)
#define REPEAT_P(c)            REPEAT_P_ ## c

#define REPEAT_0( m, d, ...)
#define REPEAT_1( m, d, ...)
#define REPEAT_2( m, d, ...) REPEAT_1( m, d, ## __VA_ARGS__) m( 1, d, ## __VA_ARGS__)
#define REPEAT_3( m, d, ...) REPEAT_2( m, d, ## __VA_ARGS__) m( 2, d, ## __VA_ARGS__)
#define REPEAT_4( m, d, ...) REPEAT_3( m, d, ## __VA_ARGS__) m( 3, d, ## __VA_ARGS__)
#define REPEAT_5( m, d, ...) REPEAT_4( m, d, ## __VA_ARGS__) m( 4, d, ## __VA_ARGS__)
#define REPEAT_6( m, d, ...) REPEAT_5( m, d, ## __VA_ARGS__) m( 5, d, ## __VA_ARGS__)
#define REPEAT_7( m, d, ...) REPEAT_6( m, d, ## __VA_ARGS__) m( 6, d, ## __VA_ARGS__)
#define REPEAT_8( m, d, ...) REPEAT_7( m, d, ## __VA_ARGS__) m( 7, d, ## __VA_ARGS__)
#define REPEAT_9( m, d, ...) REPEAT_8( m, d, ## __VA_ARGS__) m( 8, d, ## __VA_ARGS__)
#define REPEAT_10(m, d, ...) REPEAT_9( m, d, ## __VA_ARGS__) m( 9, d, ## __VA_ARGS__)
#define REPEAT_11(m, d, ...) REPEAT_10(m, d, ## __VA_ARGS__) m(10, d, ## __VA_ARGS__)
#define REPEAT_12(m, d, ...) REPEAT_11(m, d, ## __VA_ARGS__) m(11, d, ## __VA_ARGS__)
#define REPEAT_13(m, d, ...) REPEAT_12(m, d, ## __VA_ARGS__) m(12, d, ## __VA_ARGS__)
#define REPEAT_14(m, d, ...) REPEAT_13(m, d, ## __VA_ARGS__) m(13, d, ## __VA_ARGS__)
#define REPEAT_15(m, d, ...) REPEAT_14(m, d, ## __VA_ARGS__) m(14, d, ## __VA_ARGS__)
#define REPEAT_16(m, d, ...) REPEAT_15(m, d, ## __VA_ARGS__) m(15, d, ## __VA_ARGS__)
#define REPEAT_17(m, d, ...) REPEAT_16(m, d, ## __VA_ARGS__) m(16, d, ## __VA_ARGS__)
#define REPEAT_18(m, d, ...) REPEAT_17(m, d, ## __VA_ARGS__) m(17, d, ## __VA_ARGS__)
#define REPEAT_19(m, d, ...) REPEAT_18(m, d, ## __VA_ARGS__) m(18, d, ## __VA_ARGS__)
#define REPEAT_20(m, d, ...) REPEAT_19(m, d, ## __VA_ARGS__) m(19, d, ## __VA_ARGS__)
#define REPEAT_21(m, d, ...) REPEAT_20(m, d, ## __VA_ARGS__) m(20, d, ## __VA_ARGS__)
#define REPEAT_22(m, d, ...) REPEAT_21(m, d, ## __VA_ARGS__) m(21, d, ## __VA_ARGS__)
#define REPEAT_23(m, d, ...) REPEAT_22(m, d, ## __VA_ARGS__) m(22, d, ## __VA_ARGS__)
#define REPEAT_24(m, d, ...) REPEAT_23(m, d, ## __VA_ARGS__) m(23, d, ## __VA_ARGS__)
#define REPEAT_25(m, d, ...) REPEAT_24(m, d, ## __VA_ARGS__) m(24, d, ## __VA_ARGS__)
#define REPEAT_26(m, d, ...) REPEAT_25(m, d, ## __VA_ARGS__) m(25, d, ## __VA_ARGS__)
#define REPEAT_27(m, d, ...) REPEAT_26(m, d, ## __VA_ARGS__) m(26, d, ## __VA_ARGS__)
#define REPEAT_28(m, d, ...) REPEAT_27(m, d, ## __VA_ARGS__) m(27, d, ## __VA_ARGS__)
#define REPEAT_29(m, d, ...) REPEAT_28(m, d, ## __VA_ARGS__) m(28, d, ## __VA_ARGS__)
#define REPEAT_30(m, d, ...) REPEAT_29(m, d, ## __VA_ARGS__) m(29, d, ## __VA_ARGS__)
#define REPEAT_31(m, d, ...) REPEAT_30(m, d, ## __VA_ARGS__) m(30, d, ## __VA_ARGS__)
#define REPEAT_32(m, d, ...) REPEAT_31(m, d, ## __VA_ARGS__) m(31, d, ## __VA_ARGS__)

#define REPEAT(m, l, c, d, ...) REPEAT_F(m, c, d, ## __VA_ARGS__) l(c, d, ## __VA_ARGS__)

#define ENUM_PARAMS_L(c, p) p ## c
#define ENUM_PARAMS_M(c, p) p ## c,
#define ENUM_PARAMS(c, p) REPEAT(ENUM_PARAMS_M, ENUM_PARAMS_L, c, p)

#define EXPAND_1( m, d,  N, ...)                                                                                             m( 1, d, N)
#define EXPAND_2( m, d, _0,   N, ...)                               EXPAND_1( m, d, _0)                                      m( 2, d, N)
#define EXPAND_3( m, d, _0,  _1,   N, ...)                          EXPAND_2( m, d, _0,  _1)                                 m( 3, d, N)
#define EXPAND_4( m, d, _0,  _1,  _2,   N, ...)                     EXPAND_3( m, d, _0,  _1,  _2)                            m( 4, d, N)
#define EXPAND_5( m, d, _0,  _1,  _2,  _3,   N, ...)                EXPAND_4( m, d, _0,  _1,  _2,  _3)                       m( 5, d, N)
#define EXPAND_6( m, d, _0,  _1,  _2,  _3,  _4,   N, ...)           EXPAND_5( m, d, _0,  _1,  _2,  _3,  _4)                  m( 6, d, N)
#define EXPAND_7( m, d, _0,  _1,  _2,  _3,  _4,  _5,   N, ...)      EXPAND_6( m, d, _0,  _1,  _2,  _3,  _4,  _5)             m( 7, d, N)
#define EXPAND_8( m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,   N, ...) EXPAND_7( m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6)        m( 8, d, N) 

#define EXPAND_9( m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                         N, ...)                                    EXPAND_8( m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7)   m( 9, d, N)
#define EXPAND_10(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,   N, ...)                               EXPAND_9( m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8)                                      m(10, d, N)
#define EXPAND_11(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9,   N, ...)                          EXPAND_10(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9)                                 m(11, d, N)
#define EXPAND_12(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10,   N, ...)                     EXPAND_11(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10)                            m(12, d, N)
#define EXPAND_13(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11,   N, ...)                EXPAND_12(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11)                       m(13, d, N)
#define EXPAND_14(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12,   N, ...)           EXPAND_13(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12)                  m(14, d, N)
#define EXPAND_15(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13,   N, ...)      EXPAND_14(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13)             m(15, d, N)
#define EXPAND_16(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14,   N, ...) EXPAND_15(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14)        m(16, d, N)

#define EXPAND_17(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                         N, ...)                                    EXPAND_16(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15)  m(17, d, N)
#define EXPAND_18(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16,   N, ...)                               EXPAND_17(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16)                                      m(18, d, N)

#define EXPAND_19(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16, _17,   N, ...)                          EXPAND_18(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16, _17)                                 m(19, d, N)

#define EXPAND_20(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16, _17, _18,   N, ...)                     EXPAND_19(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16, _17, _18)                            m(20, d, N)

#define EXPAND_21(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16, _17, _18, _19,   N, ...)                EXPAND_20(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16, _17, _18, _19)                       m(21, d, N)

#define EXPAND_22(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16, _17, _18, _19, _20,   N, ...)           EXPAND_21(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16, _17, _18, _19, _20)                  m(22, d, N)

#define EXPAND_23(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16, _17, _18, _19, _20, _21,   N, ...)      EXPAND_22(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16, _17, _18, _19, _20, _21)             m(23, d, N)

#define EXPAND_24(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16, _17, _18, _19, _20, _21, _22,   N, ...) EXPAND_23(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16, _17, _18, _19, _20, _21, _22)        m(24, d, N)

#define EXPAND_25(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16, _17, _18, _19, _20, _21, _22, _23, \
                         N, ...)                                    EXPAND_24(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16, _17, _18, _19, _20, _21, _22, _23)   m(25, d, N)

#define EXPAND_26(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16, _17, _18, _19, _20, _21, _22, _23, \
                       _24,   N, ...)                               EXPAND_25(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16, _17, _18, _19, _20, _21, _22, _23, \
                                                                                   _24)                                      m(26, d, N)

#define EXPAND_27(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16, _17, _18, _19, _20, _21, _22, _23, \
                       _24, _25,   N, ...)                          EXPAND_26(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16, _17, _18, _19, _20, _21, _22, _23, \
                                                                                   _24, _25)                                 m(27, d, N)

#define EXPAND_28(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16, _17, _18, _19, _20, _21, _22, _23, \
                       _24, _25, _26,   N, ...)                     EXPAND_27(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16, _17, _18, _19, _20, _21, _22, _23, \
                                                                                   _24, _25, _26)                            m(28, d, N)

#define EXPAND_29(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16, _17, _18, _19, _20, _21, _22, _23, \
                       _24, _25, _26, _27,   N, ...)                EXPAND_28(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16, _17, _18, _19, _20, _21, _22, _23, \
                                                                                   _24, _25, _26, _27)                       m(29, d, N)

#define EXPAND_30(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16, _17, _18, _19, _20, _21, _22, _23, \
                       _24, _25, _26, _27, _28,   N, ...)           EXPAND_29(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16, _17, _18, _19, _20, _21, _22, _23, \
                                                                                   _24, _25, _26, _27, _28)                  m(30, d, N)

#define EXPAND_31(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16, _17, _18, _19, _20, _21, _22, _23, \
                       _24, _25, _26, _27, _28, _29,   N, ...)      EXPAND_30(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16, _17, _18, _19, _20, _21, _22, _23, \
                                                                                   _24, _25, _26, _27, _28, _29)             m(31, d, N)

#define EXPAND_32(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                        _8,  _9, _10, _11, _12, _13, _14, _15, \
                       _16, _17, _18, _19, _20, _21, _22, _23, \
                       _24, _25, _26, _27, _28, _29, _30,   N, ...) EXPAND_31(m, d, _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7, \
                                                                                    _8,  _9, _10, _11, _12, _13, _14, _15, \
                                                                                   _16, _17, _18, _19, _20, _21, _22, _23, \
                                                                                   _24, _25, _26, _27, _28, _29, _30)        m(32, d, N)

#define EXPAND(c, m, d, ...) EXPAND_ ## c(m, d, ## __VA_ARGS__)

#define VANISH() 
#define FORWARD_ASSIST(...)  ISH() __VA_ARGS__
#define FORWARD_ESCAPE(...)  VAN ## __VA_ARGS__
#define FORWARD(X) FORWARD_ESCAPE(FORWARD_ASSIST X)

// Named arguments
// Inspired by an article on Fluent C++ (https://www.fluentcpp.com/2018/12/14/named-arguments-cpp/)
// 
// Example: using length_t = named_arg<double, struct length_s>;
//          using  width_t = named_arg<double, struct  width_s>;
//          using  depth_t = named_arg<double, struct  depth_s>;
//          static const length_t::arg_t length;
//          static const  width_t::arg_t  width;
//          static const  depth_t::arg_t  depth;
//          template<class T, class U, class V>
//          double get_largest_side(T arg1, U arg2, V arg3) {
//              length_t length = pick(arg1, arg2, arg3);
//               width_t  width = pick(arg1, arg2, arg3);
//               depth_t  depth = pick(arg1, arg2, arg3);
//              return max(max(length, width), depth);
//          }
// 
// Call with: get_largest_side(length = 1.0, width = 2.0, depth = 3.0);
//            parameters can be in any order.
// 
// Can also be done using a simple macro
// Usage: NAMED_ARGS(T, n, c, Ts, ns, f)
//        T:  function return type
//        n:  function name
//        c:  amount of parameters
//        Ts: function parameter types
//        ns: function parameter names
//        f:  function code
// 
// Example: NAMED_ARGS(double, get_largest_side, 3, (double, double, double), (length, width, depth), {
//     return max(max(length, width), depth);
// });
// 
// Call with: get_largest_side(length = 1.0, width = 2.0, depth = 3.0);
// 

template<class T, class P>
class named_arg {
private:
    T value;

public:
    named_arg() = default;
    explicit named_arg(const T&  val) : value(          val ) { }
    explicit named_arg(      T&& val) : value(std::move(val)) { }

    struct arg_t {
        template<class U>
        named_arg operator=(U&& val) const { return named_arg(std::forward<U>(val)); }
        arg_t           (             ) = default;
        arg_t           (const arg_t& ) = delete;
        arg_t           (      arg_t&&) = delete;
        arg_t& operator=(const arg_t& ) = delete;
        arg_t& operator=(      arg_t&&) = delete;
    };

    operator T () { return value; }
};

template<class T, class... Ts>
T pick(Ts&&... args) { return std::get<T>(std::make_tuple(std::forward<Ts>(args)...)); }

#define NAMED_ARGS_TYPES_M(c, p, T)     using named_arg_ ## p ## _ ## c ## _t = named_arg<T, struct named_arg_ ## p ## _ ## c ## _s>;
#define NAMED_ARGS_VARS_M(c, p, n)      static const named_arg_ ## p ## _ ## c ## _t::arg_t n;
#define NAMED_ARGS_ENUM_PARAMS_L(c)     Arg ## c val ## c
#define NAMED_ARGS_ENUM_PARAMS_M(c)     Arg ## c val ## c,
#define NAMED_ARGS_ENUM_PARAMS(c)       REPEAT(NAMED_ARGS_ENUM_PARAMS_M, NAMED_ARGS_ENUM_PARAMS_L, c)
#define NAMED_ARGS_INIT_PARAMS(c, p, n) named_arg_ ## p ## _ ## c ## _t n = pick(ENUM_PARAMS(c, val));
#define NAMED_ARGS(T, n, c, Ts, ns, f)  EXPAND(c, NAMED_ARGS_TYPES_M,   n, FORWARD(Ts)) \
                                        EXPAND(c, NAMED_ARGS_VARS_M,    n, FORWARD(ns)) \
                                        template<ENUM_PARAMS(c, class Arg)> \
                                        T n(NAMED_ARGS_ENUM_PARAMS(c)) { \
                                            EXPAND(c, NAMED_ARGS_INIT_PARAMS, n, FORWARD(ns)) \
                                            f \
                                        }
