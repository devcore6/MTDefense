export module math.vec4;

import math.vec;

import tools.types;

export template<class T>
class vec4: public vec<T, 4> {
public:
    T& x = (*this)[0];
    T& y = (*this)[1];
    T& z = (*this)[2];
    T& w = (*this)[3];

    vec4()                       : vec<T, 4>()  { }
    vec4(copy_t<vec<T, 4>> v)    : vec<T, 4>(v) { }
    vec4(move_t<vec<T, 4>> v)    : vec<T, 4>(v) { }
    vec4(copy_t<vec4>      v)    : vec<T, 4>(v) { }
    vec4(move_t<vec4>      v)    : vec<T, 4>(v) { }
    vec4(T _x, T _y, T _z, T _w) : vec<T, 4>()  { x = _x; y = _y; z = _z; w = _w; }

    vec4& operator=(copy_t<vec<T, 4>> v)        { vec<T, 4>::operator=(v); return *this; }
    vec4& operator=(move_t<vec<T, 4>> v)        { vec<T, 4>::operator=(v); return *this; }
    vec4& operator=(copy_t<vec4>      v)        { vec<T, 4>::operator=(v); return *this; }
    vec4& operator=(move_t<vec4>      v)        { vec<T, 4>::operator=(v); return *this; }
};
