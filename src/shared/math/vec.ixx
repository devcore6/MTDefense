export module math.vec;

import tools.types;
import tools.memswap;
import tools.limit;

import <string>;

template<class T, size_t n, size_t m>
class matrix;

export template<class T, size_t size>
class vec {
protected:
    T vals[size] = { };

public:
    vec() = default;
    vec(copy_t<vec> v)            { memcpy( vals, v.vals, sizeof(T) * size); }
    vec(move_t<vec> v)            { memmove(vals, v.vals, sizeof(T) * size); memset(v.vals, 0, sizeof(T) * size); }
    vec(T v[size])                { memcpy( vals, v,      sizeof(T) * size); }

    vec& operator=(copy_t<vec> v) { memcpy( vals, v.vals, sizeof(T) * size);                                 return *this; }
    vec& operator=(move_t<vec> v) { memswap(vals, v.vals, sizeof(T) * size);                                 return *this; }
    
    vec  operator+ (vec rhs)      { vec ret { }; for(size_t i = 0; i < size; i++) ret[i] = vals[i] + rhs[i]; return ret; }
    vec  operator- (vec rhs)      { vec ret { }; for(size_t i = 0; i < size; i++) ret[i] = vals[i] - rhs[i]; return ret; }
    vec  operator* (T d)          { vec ret { }; for(size_t i = 0; i < size; i++) ret[i] = vals[i] * d;      return ret; }
    vec  operator/ (T d)          { vec ret { }; for(size_t i = 0; i < size; i++) ret[i] = vals[i] / d;      return ret; }

    vec& operator+=(vec rhs)      { for(size_t i = 0; i < size; i++) vals[i] += rhs[i];                      return *this; }
    vec& operator-=(vec rhs)      { for(size_t i = 0; i < size; i++) vals[i] -= rhs[i];                      return *this; }
    vec& operator*=(T d)          { for(size_t i = 0; i < size; i++) vals[i] *= d;                           return *this; }
    vec& operator/=(T d)          { for(size_t i = 0; i < size; i++) vals[i] /= d;                           return *this; }

    T    operator* (vec rhs)      { T ret { 0 }; for(size_t i = 0; i < size; i++) ret += vals[i] * rhs[i];   return ret; }

    T    magnitude ()             { T ret { 0 }; for(size_t i = 0; i < size; i++) ret += vals[i] * vals[i];  return sqrt(ret); }
    vec  normalize ()             { return *this / magnitude(); }

    T& operator[](size_t id)      { return vals[min(id, size - 1)]; }

    operator matrix<T, 1, size>() { return { &vals }; }

    std::string to_string() {
        std::string ret { };
        for(size_t i = 0; i < size; i++)
            ret += '|' + (vals[i] < 0.0 ? std::to_string(vals[i]) : (" " + std::to_string(vals[i]))) + (i == size - 1 ? " |" : " |\n");
        return ret;
    }
};
