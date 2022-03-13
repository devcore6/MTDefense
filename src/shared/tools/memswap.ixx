export module tools.memswap;

import<string_view>;

export inline void memswap(void* mem1, void* mem2, size_t size) {
    char* m1 { (char*)mem1 };
    char* m2 { (char*)mem2 };

    for(size_t i = 0; i < size; i++)
        std::swap(m1[i], m2[i]);
}
