#pragma once
#include <cstdint>

#include <chrono>
#include <string>
#include <concepts>
#include <functional>

// Convenient literal suffixes

inline consteval   uint8_t operator ""   _u8(size_t arg) noexcept { return static_cast<uint8_t>(arg); }
inline consteval  uint16_t operator ""  _u16(size_t arg) noexcept { return static_cast<uint16_t>(arg); }
inline consteval  uint32_t operator ""  _u32(size_t arg) noexcept { return static_cast<uint32_t>(arg); }

inline consteval    int8_t operator ""   _i8(size_t arg) noexcept { return static_cast<int8_t>(arg); }
inline consteval   int16_t operator ""  _i16(size_t arg) noexcept { return static_cast<int16_t>(arg); }
inline consteval   int32_t operator ""  _i32(size_t arg) noexcept { return static_cast<int32_t>(arg); }

#ifdef UINT64_MAX // Check for 64-bit integer support
inline consteval  uint64_t operator ""  _u64(size_t arg) noexcept { return static_cast<uint64_t>(arg); }

inline consteval   int64_t operator ""  _i64(size_t arg) noexcept { return static_cast<int64_t>(arg); }
#endif

inline consteval  intmax_t operator "" _imax(size_t arg) noexcept { return static_cast<intmax_t>(arg); }
inline consteval uintmax_t operator "" _umax(size_t arg) noexcept { return static_cast<uintmax_t>(arg); }

// This is only needed until "z" and "zu" suffixes are finalized
inline consteval ptrdiff_t operator ""    _z(size_t arg) noexcept { return static_cast<ptrdiff_t>(arg); }
inline consteval    size_t operator ""   _zu(size_t arg) noexcept { return arg; }

// These also exist in std::literals

inline constexpr std::string operator "" _str(const char* arg, size_t size) noexcept { return std::string(arg); }

inline constexpr std::chrono::nanoseconds operator "" _ns(size_t arg) noexcept { return static_cast<std::chrono::nanoseconds>(arg); }
inline constexpr std::chrono::microseconds operator "" _us(size_t arg) noexcept { return static_cast<std::chrono::microseconds>(arg); }
inline constexpr std::chrono::microseconds operator "" _µs(size_t arg) noexcept { return static_cast<std::chrono::microseconds>(arg); }
inline constexpr std::chrono::milliseconds operator "" _ms(size_t arg) noexcept { return static_cast<std::chrono::milliseconds>(arg); }
inline constexpr std::chrono::seconds operator ""  _s(size_t arg) noexcept { return static_cast<std::chrono::seconds>(arg); }


// Container and loop utilities

template<class T>
concept iterable = requires(T arr) { arr.begin(); arr.end(); };

template<std::equality_comparable T, iterable ArrT = std::vector<T>>
inline bool contains(const ArrT arr, T val) { for(T v : arr) if(v == val) return true; return false; }


template<class T>
concept reverse_iterable = requires(T arr) { arr.rbegin(); arr.rend(); };

template <reverse_iterable T>
struct reverse_impl {
    T& data;
    auto begin() { return data.rbegin(); }
    auto end() { return data.rend(); }
};

template <reverse_iterable T>
reverse_impl<T> reverse(T&& iterable) { return { iterable }; }


class numeric_iterator {
private:
    size_t it = 0;
    const     size_t size = 0;

public:
    constexpr        numeric_iterator() = default;
    constexpr        numeric_iterator(size_t s) : it(0), size(s) { }
    constexpr        numeric_iterator(size_t i, size_t s) : it(i), size(s) { }

    constexpr size_t operator*() { return it; }
    constexpr auto& operator++() { if(it < size - 1) ++it; return *this; }
    constexpr bool   operator==(numeric_iterator& y) { return it == y.it; }
};

class reverse_numeric_iterator {
private:
    size_t it = 0;
    const     size_t size = 0;

public:
    constexpr        reverse_numeric_iterator() = default;
    constexpr        reverse_numeric_iterator(size_t s) : it(s - 1), size(s) { }
    constexpr        reverse_numeric_iterator(size_t i, size_t s) : it(s - 1 - i), size(s) { }

    constexpr size_t operator*() { return it; }
    constexpr auto& operator++() { if(it > 0) --it; return *this; }
    constexpr bool   operator==(reverse_numeric_iterator& y) { return it == y.it; }
};

class iterate_impl {
public:
    using iterator = numeric_iterator;
    using reverse_iterator = reverse_numeric_iterator;

private:
    iterator         _begin;
    iterator         _end;
    reverse_iterator _rbegin;
    reverse_iterator _rend;

public:
    iterate_impl(size_t s) : _begin(s), _end(s - 1, s), _rbegin(s), _rend(s - 1, s) { }
    iterate_impl(size_t b, size_t s) : _begin(b, s), _end(s - 1, s), _rbegin(b, s), _rend(s - 1, s) { }
    iterator         begin() { return _begin; }
    iterator         end() { return _end; }
    reverse_iterator rbegin() { return _rbegin; }
    reverse_iterator rend() { return _rend; }
};

template <std::convertible_to<size_t> T>
iterate_impl iterate(T size) { return iterate_impl(size); }

template <std::convertible_to<size_t> T, std::convertible_to<size_t> U>
iterate_impl iterate(T begin, U size) { return iterate_impl(begin, size); }


template <iterable T>
struct offset_impl {
    T& data;
    size_t offset = 0;
    auto   begin() { return data.begin() + offset; }
    auto   end() { return data.end(); }
};

template <iterable T, std::convertible_to<size_t> U>
offset_impl<T> iterate(T arr, U offset) { return { arr, offset }; }


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
