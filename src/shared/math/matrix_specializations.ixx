export module math.matrix_specializations;

import math.matrix;

export template<class T>
class matrix3x3;

export template<class T>
class matrix4x4;

export template<class T>
class matrix2x2: public matrix<T, 2, 2> {
public:
    matrix2x2()                           : matrix<T, 2, 2>()  { }
    matrix2x2(copy_t<matrix<T, 2, 2>> v)  : matrix<T, 2, 2>(v) { }
    matrix2x2(move_t<matrix<T, 2, 2>> v)  : matrix<T, 2, 2>(v) { }
    matrix2x2(copy_t<matrix2x2>       v)  : matrix<T, 2, 2>(v) { }
    matrix2x2(move_t<matrix2x2>       v)  : matrix<T, 2, 2>(v) { }
    matrix2x2(T v00, T v10, T v01, T v11) : matrix<T, 2, 2>()  {
        (*this)[0][0] = v00; (*this)[1][0] = v10;
        (*this)[0][1] = v01; (*this)[1][1] = v11;
    }

    matrix2x2& operator=(copy_t<matrix<T, 2, 2>> v) { matrix<T, 2, 2>::operator=(v); return *this; }
    matrix2x2& operator=(move_t<matrix<T, 2, 2>> v) { matrix<T, 2, 2>::operator=(v); return *this; }
    matrix2x2& operator=(copy_t<matrix2x2>       v) { matrix<T, 2, 2>::operator=(v); return *this; }
    matrix2x2& operator=(move_t<matrix2x2>       v) { matrix<T, 2, 2>::operator=(v); return *this; }

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

export template<class T>
class matrix4x4 : public matrix<T, 4, 4> {
public:
    matrix4x4()                           : matrix<T, 4, 4>()  { }
    matrix4x4(copy_t<matrix<T, 4, 4>> v)  : matrix<T, 4, 4>(v) { }
    matrix4x4(move_t<matrix<T, 4, 4>> v)  : matrix<T, 4, 4>(v) { }
    matrix4x4(copy_t<matrix4x4>       v)  : matrix<T, 4, 4>(v) { }
    matrix4x4(move_t<matrix4x4>       v)  : matrix<T, 4, 4>(v) { }
    matrix4x4(T v00, T v10, T v20, T v30,
              T v01, T v11, T v21, T v31,
              T v02, T v12, T v22, T v32,
              T v03, T v13, T v23, T v33) : matrix<T, 4, 4>() {
        (*this)[0][0] = v00; (*this)[1][0] = v10; (*this)[2][0] = v20; (*this)[3][0] = v30;
        (*this)[0][1] = v01; (*this)[1][1] = v11; (*this)[2][1] = v21; (*this)[3][1] = v31;
        (*this)[0][2] = v02; (*this)[1][2] = v12; (*this)[2][2] = v22; (*this)[3][2] = v32;
        (*this)[0][3] = v03; (*this)[1][3] = v13; (*this)[2][3] = v23; (*this)[3][3] = v33;
    }

    matrix4x4& operator=(copy_t<matrix<T, 4, 4>> v) { matrix<T, 4, 4>::operator=(v); return *this; }
    matrix4x4& operator=(move_t<matrix<T, 4, 4>> v) { matrix<T, 4, 4>::operator=(v); return *this; }
    matrix4x4& operator=(copy_t<matrix4x4>       v) { matrix<T, 4, 4>::operator=(v); return *this; }
    matrix4x4& operator=(move_t<matrix4x4>       v) { matrix<T, 4, 4>::operator=(v); return *this; }

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

