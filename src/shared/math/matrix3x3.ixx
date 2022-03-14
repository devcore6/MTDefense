export module math.matrix3x3;

import math.matrix;

template<class T>
class matrix2x2;

template<class T>
class matrix4x4;

export template<class T>
class matrix3x3: public matrix<T, 3, 3> {
public:
    matrix3x3()                           : matrix<T, 3, 3>()  { }
    matrix3x3(copy_t<matrix<T, 3, 3>> v)  : matrix<T, 3, 3>(v) { }
    matrix3x3(move_t<matrix<T, 3, 3>> v)  : matrix<T, 3, 3>(v) { }
    matrix3x3(copy_t<matrix3x3>       v)  : matrix<T, 3, 3>(v) { }
    matrix3x3(move_t<matrix3x3>       v)  : matrix<T, 3, 3>(v) { }
    matrix3x3(T v00, T v10, T v20,
              T v01, T v11, T v21,
              T v02, T v12, T v22) : matrix<T, 3, 3>() {
        (*this)[0][0] = v00; (*this)[1][0] = v10; (*this)[2][0] = v20;
        (*this)[0][1] = v01; (*this)[1][1] = v11; (*this)[2][1] = v21;
        (*this)[0][2] = v02; (*this)[1][2] = v12; (*this)[2][2] = v22;
    }

    matrix3x3& operator=(copy_t<matrix<T, 3, 3>> v) { matrix<T, 3, 3>::operator=(v); return *this; }
    matrix3x3& operator=(move_t<matrix<T, 3, 3>> v) { matrix<T, 3, 3>::operator=(v); return *this; }
    matrix3x3& operator=(copy_t<matrix3x3>       v) { matrix<T, 3, 3>::operator=(v); return *this; }
    matrix3x3& operator=(move_t<matrix3x3>       v) { matrix<T, 3, 3>::operator=(v); return *this; }

    inline operator matrix2x2<T>() {
        return {
            (*this)[0][0], (*this)[1][0],
            (*this)[0][1], (*this)[1][1]
        };
    }

    inline operator matrix4x4<T>() {
        return {
            (*this)[0][0], (*this)[1][0], (*this)[2][0], (T)0,
            (*this)[0][1], (*this)[1][1], (*this)[2][1], (T)0,
            (*this)[0][2], (*this)[1][2], (*this)[2][2], (T)0,
            (T)0,          (T)0,          (T)0,          (T)1
        };
    }
};
