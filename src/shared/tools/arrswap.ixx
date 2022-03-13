export module tools.arrswap;

import tools.memswap;

export template<class T>
inline void arrswap(T* arr1, T* arr2, size_t n) {
    for(size_t i = 0; i < n; i++)
        memswap((void*)(&arr1[i]), (void*)(&arr2[i]), sizeof(T));
}
