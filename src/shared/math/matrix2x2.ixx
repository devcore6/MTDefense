export module math.matrix2x2;

import math.matrix;

template<class T>
class matrix3x3;

template<class T>
class matrix4x4;

export template<class T>
class matrix2x2: public matrix<T, 2, 2> {
public:
    matrix2x2()                           : matrix<T, 2, 2>()  { }
    matrix2x2(const matrix<T, 2, 2>&  v)  : matrix<T, 2, 2>(v) { }
    matrix2x2(      matrix<T, 2, 2>&& v)  : matrix<T, 2, 2>(v) { }
    matrix2x2(const matrix2x2      &  v)  : matrix<T, 2, 2>(v) { }
    matrix2x2(      matrix2x2      && v)  : matrix<T, 2, 2>(v) { }
    matrix2x2(T v00, T v10, T v01, T v11) : matrix<T, 2, 2>()  {
        (*this)[0][0] = v00; (*this)[1][0] = v10;
        (*this)[0][1] = v01; (*this)[1][1] = v11;
    }

    matrix2x2& operator=(const matrix<T, 2, 2>&  v)            { matrix<T, 2, 2>::operator=(v); return *this; }
    matrix2x2& operator=(      matrix<T, 2, 2>&& v)            { matrix<T, 2, 2>::operator=(v); return *this; }
    matrix2x2& operator=(const matrix2x2      &  v)            { matrix<T, 2, 2>::operator=(v); return *this; }
    matrix2x2& operator=(      matrix2x2      && v)            { matrix<T, 2, 2>::operator=(v); return *this; }

    inline operator matrix3x3<T>() {
        return {
            (*this)[0][0], (*this)[1][0], (T)0,
            (*this)[0][1], (*this)[1][1], (T)0,
            (T)0,          (T)0,          (T)1
        };
    }

    inline operator matrix4x4<T>() {
        return {
            (*this)[0][0], (*this)[1][0], (T)0, (T)0,
            (*this)[0][1], (*this)[1][1], (T)0, (T)0,
            (T)0,          (T)0,          (T)1, (T)0,
            (T)0,          (T)0,          (T)0, (T)1
        };
    }
};
