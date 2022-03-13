export module tools.types;

export template<class T>
using copy_t = const T&;

export template<class T>
using move_t = T&&;

export using cstring = const char*;
