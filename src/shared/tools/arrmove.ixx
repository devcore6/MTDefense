export module tools.arrmove;

import <cstring>;

export template<class T>
inline void arrmove(T* dst, const T* src, size_t n) {
    for(size_t i = 0; i < n; i++)
        memmove((void*)(&dst[i]), (const void*)(&src[i]), sizeof(T));
}
