#pragma once
#include <cstdint>

#include <chrono>
#include <string>
#include <concepts>
#include <functional>
#include <atomic>

// Convenient literal suffixes

inline consteval   uint8_t operator ""   _u8(size_t arg) noexcept { return static_cast<  uint8_t>(arg); }
inline consteval  uint16_t operator ""  _u16(size_t arg) noexcept { return static_cast< uint16_t>(arg); }
inline consteval  uint32_t operator ""  _u32(size_t arg) noexcept { return static_cast< uint32_t>(arg); }

inline consteval    int8_t operator ""   _i8(size_t arg) noexcept { return static_cast<   int8_t>(arg); }
inline consteval   int16_t operator ""  _i16(size_t arg) noexcept { return static_cast<  int16_t>(arg); }
inline consteval   int32_t operator ""  _i32(size_t arg) noexcept { return static_cast<  int32_t>(arg); }

#ifdef UINT64_MAX // Check for 64-bit integer support
inline consteval  uint64_t operator ""  _u64(size_t arg) noexcept { return static_cast< uint64_t>(arg); }

inline consteval   int64_t operator ""  _i64(size_t arg) noexcept { return static_cast<  int64_t>(arg); }
#endif

inline consteval  intmax_t operator "" _imax(size_t arg) noexcept { return static_cast< intmax_t>(arg); }
inline consteval uintmax_t operator "" _umax(size_t arg) noexcept { return static_cast<uintmax_t>(arg); }

// This is only needed until "z" and "zu" suffixes are finalized
inline consteval ptrdiff_t operator ""    _z(size_t arg) noexcept { return static_cast<ptrdiff_t>(arg); }
inline consteval    size_t operator ""   _zu(size_t arg) noexcept { return arg; }

// These also exist in std::literals

inline constexpr std::string operator "" _str(const char* arg, size_t size) noexcept { return std::string(arg); }

inline constexpr std::chrono::nanoseconds  operator "" _ns(size_t arg) noexcept { return static_cast<std::chrono::nanoseconds >(arg); }
inline constexpr std::chrono::microseconds operator "" _us(size_t arg) noexcept { return static_cast<std::chrono::microseconds>(arg); }
inline constexpr std::chrono::microseconds operator "" _µs(size_t arg) noexcept { return static_cast<std::chrono::microseconds>(arg); }
inline constexpr std::chrono::milliseconds operator "" _ms(size_t arg) noexcept { return static_cast<std::chrono::milliseconds>(arg); }
inline constexpr std::chrono::seconds      operator ""  _s(size_t arg) noexcept { return static_cast<std::chrono::seconds     >(arg); }


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

    iostate     state;

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
    basic_packetstream& seekg(size_t pos)               { g = pos;                                                       return *this; }

    size_t tellp()                                      { return p; }
    basic_packetstream& seekp(size_t pos)               { p = pos;                                                       return *this; }

    basic_packetstream& operator<<(const_type    value) { put(value);                                                    return *this; }
    basic_packetstream& operator<<( int16_t      value) { write((const_pointer)&value, 2                   / char_size); return *this; }
    basic_packetstream& operator<<(uint16_t      value) { write((const_pointer)&value, 2                   / char_size); return *this; }
    basic_packetstream& operator<<( int32_t      value) { write((const_pointer)&value, 4                   / char_size); return *this; }
    basic_packetstream& operator<<(uint32_t      value) { write((const_pointer)&value, 4                   / char_size); return *this; }
    basic_packetstream& operator<<( int64_t      value) { write((const_pointer)&value, 8                   / char_size); return *this; }
    basic_packetstream& operator<<(uint64_t      value) { write((const_pointer)&value, 8                   / char_size); return *this; }
    basic_packetstream& operator<<(float         value) { write((const_pointer)&value, 4                   / char_size); return *this; }
    basic_packetstream& operator<<(     double   value) { write((const_pointer)&value, 8                   / char_size); return *this; }
    basic_packetstream& operator<<(long double   value) { write((const_pointer)&value, sizeof(long double) / char_size); return *this; }
    basic_packetstream& operator<<(bool          value) { write((const_pointer)&value, sizeof(bool)        / char_size); return *this; }
    basic_packetstream& operator<<(const void*   value) { write((const_pointer) value, 0);                               return *this; }

    basic_packetstream& operator>>(char_type   & value) { get(value);                                                    return *this; }
    basic_packetstream& operator>>( int16_t    & value) { read((pointer)       &value, 2                   / char_size); return *this; }
    basic_packetstream& operator>>(uint16_t    & value) { read((pointer)       &value, 2                   / char_size); return *this; }
    basic_packetstream& operator>>( int32_t    & value) { read((pointer)       &value, 4                   / char_size); return *this; }
    basic_packetstream& operator>>(uint32_t    & value) { read((pointer)       &value, 4                   / char_size); return *this; }
    basic_packetstream& operator>>( int64_t    & value) { read((pointer)       &value, 8                   / char_size); return *this; }
    basic_packetstream& operator>>(uint64_t    & value) { read((pointer)       &value, 8                   / char_size); return *this; }
    basic_packetstream& operator>>(float       & value) { read((pointer)       &value, 4                   / char_size); return *this; }
    basic_packetstream& operator>>(     double & value) { read((pointer)       &value, 8                   / char_size); return *this; }
    basic_packetstream& operator>>(long double & value) { read((pointer)       &value, sizeof(long double) / char_size); return *this; }
    basic_packetstream& operator>>(bool        & value) { read((pointer)       &value, sizeof(bool)        / char_size); return *this; }

    basic_packetstream& operator<<(std::basic_string<char_type>& value) { write(value.c_str(), 0);                       return *this; }
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
