export module math.vec2;

import math.vec;

import tools.types;

export template<class T>
class vec2: public vec<T, 2> {
public:
    T& x = (*this)[0];
    T& y = (*this)[1];

    vec2()                    : vec<T, 2>()  { }
    vec2(copy_t<vec<T, 2>> v) : vec<T, 2>(v) { }
    vec2(move_t<vec<T, 2>> v) : vec<T, 2>(v) { }
    vec2(copy_t<vec2>      v) : vec<T, 2>(v) { }
    vec2(move_t<vec2>      v) : vec<T, 2>(v) { }
    vec2(T _x, T _y)          : vec<T, 2>()  { x = _x; y = _y; }

    vec2& operator=(copy_t<vec<T, 2>> v)     { vec<T, 2>::operator=(v); return *this; }
    vec2& operator=(move_t<vec<T, 2>> v)     { vec<T, 2>::operator=(v); return *this; }
    vec2& operator=(copy_t<vec2>      v)     { vec<T, 2>::operator=(v); return *this; }
    vec2& operator=(move_t<vec2>      v)     { vec<T, 2>::operator=(v); return *this; }
};
