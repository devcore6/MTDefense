#pragma once
#include <cstdint>

#include <chrono>
#include <string>
#include <concepts>
#include <functional>

inline consteval  uint8_t operator ""  _u8(size_t arg) noexcept { return static_cast< uint8_t>(arg); }
inline consteval uint16_t operator "" _u16(size_t arg) noexcept { return static_cast<uint16_t>(arg); }
inline consteval uint32_t operator "" _u32(size_t arg) noexcept { return static_cast<uint32_t>(arg); }

inline consteval   int8_t operator ""  _i8(size_t arg) noexcept { return static_cast<  int8_t>(arg); }
inline consteval  int16_t operator "" _i16(size_t arg) noexcept { return static_cast< int16_t>(arg); }
inline consteval  int32_t operator "" _i32(size_t arg) noexcept { return static_cast< int32_t>(arg); }

#ifdef UINT64_MAX // Check for 64-bit integer support
inline consteval uint64_t operator "" _u64(size_t arg) noexcept { return static_cast<uint64_t>(arg); }

inline consteval  int64_t operator "" _i64(size_t arg) noexcept { return static_cast< int64_t>(arg); }
#endif

inline constexpr std::string operator "" _str(const char* arg, size_t size) noexcept { return std::string(arg); }

inline constexpr std::chrono:: nanoseconds operator "" _ns(size_t arg) noexcept { return static_cast<std::chrono:: nanoseconds>(arg); }
inline constexpr std::chrono::microseconds operator "" _µs(size_t arg) noexcept { return static_cast<std::chrono::microseconds>(arg); }
inline constexpr std::chrono::milliseconds operator "" _ms(size_t arg) noexcept { return static_cast<std::chrono::milliseconds>(arg); }
inline constexpr std::chrono::     seconds operator ""  _s(size_t arg) noexcept { return static_cast<std::chrono::     seconds>(arg); }


inline bool contains(const char* arr, char val) { while(*arr != 0) { if(*arr == val) return true; arr++; } return false; }

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


extern void erase_all(std::string& s, const std::string& to_erase);
extern void erase_all(std::string& s, const std::string& to_erase_begin, const std::string& to_erase_end);
extern void replace_all(std::string& s, const std::string& to_replace, const char* replace_with);
extern bool match_all(std::string& s, const std::string& to_match, std::function<bool(std::string&, size_t)> callback);

extern size_t find_not_escaped(std::string& s, const std::string& to_find, size_t start_pos = 0);

extern std::vector<std::string> split(std::string& s, const char& separator, const char& escape_character, bool keep_escape_char = false);
