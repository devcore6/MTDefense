export module tools.arrcpy;

import <cstring>;

export template<class T>
inline void arrcpy(T* dst, const T* src, size_t n) {
    for(size_t i = 0; i < n; i++)
        memcpy((void*)(&dst[i]), (const void*)(&src[i]), sizeof(T));
}
