export module math.matrix4x4;

import math.matrix;

template<class T>
class matrix2x2;

template<class T>
class matrix3x3;

export template<class T>
class matrix4x4 : public matrix<T, 4, 4> {
public:
    matrix4x4()                           : matrix<T, 4, 4>()  { }
    matrix4x4(const matrix<T, 4, 4>&  v)  : matrix<T, 4, 4>(v) { }
    matrix4x4(      matrix<T, 4, 4>&& v)  : matrix<T, 4, 4>(v) { }
    matrix4x4(const matrix4x4      &  v)  : matrix<T, 4, 4>(v) { }
    matrix4x4(      matrix4x4      && v)  : matrix<T, 4, 4>(v) { }
    matrix4x4(T v00, T v10, T v20, T v30,
              T v01, T v11, T v21, T v31,
              T v02, T v12, T v22, T v32,
              T v03, T v13, T v23, T v33) : matrix<T, 4, 4>() {
        (*this)[0][0] = v00; (*this)[1][0] = v10; (*this)[2][0] = v20; (*this)[3][0] = v30;
        (*this)[0][1] = v01; (*this)[1][1] = v11; (*this)[2][1] = v21; (*this)[3][1] = v31;
        (*this)[0][2] = v02; (*this)[1][2] = v12; (*this)[2][2] = v22; (*this)[3][2] = v32;
        (*this)[0][3] = v03; (*this)[1][3] = v13; (*this)[2][3] = v23; (*this)[3][3] = v33;
    }

    matrix4x4& operator=(const matrix<T, 4, 4>&  v)            { matrix<T, 4, 4>::operator=(v); return *this; }
    matrix4x4& operator=(      matrix<T, 4, 4>&& v)            { matrix<T, 4, 4>::operator=(v); return *this; }
    matrix4x4& operator=(const matrix4x4      &  v)            { matrix<T, 4, 4>::operator=(v); return *this; }
    matrix4x4& operator=(      matrix4x4      && v)            { matrix<T, 4, 4>::operator=(v); return *this; }

    inline operator matrix2x2<T>() {
        return {
            (*this)[0][0], (*this)[1][0],
            (*this)[0][1], (*this)[1][1]
        };
    }

    inline operator matrix3x3<T>() {
        return {
            (*this)[0][0], (*this)[1][0], (*this)[2][0],
            (*this)[0][1], (*this)[1][1], (*this)[2][1],
            (*this)[0][2], (*this)[1][2], (*this)[2][2]
        };
    }
};
