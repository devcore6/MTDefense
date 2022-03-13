export module math.vec3;

import math.vec;

import tools.types;

export template<class T>
class vec3: public vec<T, 3> {
public:
    T& x = (*this)[0];
    T& y = (*this)[1];
    T& z = (*this)[2];

    vec3()                    : vec<T, 3>()  { }
    vec3(copy_t<vec<T, 3>> v) : vec<T, 3>(v) { }
    vec3(move_t<vec<T, 3>> v) : vec<T, 3>(v) { }
    vec3(copy_t<vec3>      v) : vec<T, 3>(v) { }
    vec3(move_t<vec3>      v) : vec<T, 3>(v) { }
    vec3(T _x, T _y, T _z)    : vec<T, 3>()  { x = _x; y = _y; z = _z; }

    vec3& operator=(copy_t<vec<T, 3>> v)     { vec<T, 3>::operator=(v); return *this; }
    vec3& operator=(move_t<vec<T, 3>> v)     { vec<T, 3>::operator=(v); return *this; }
    vec3& operator=(copy_t<vec3>      v)     { vec<T, 3>::operator=(v); return *this; }
    vec3& operator=(move_t<vec3>      v)     { vec<T, 3>::operator=(v); return *this; }

    vec3 cross(vec3 rhs) { return { y * rhs.z - z * rhs.y, 
                                    z * rhs.x - x * rhs.z,
                                    x * rhs.y - y * rhs.x }; }
};
