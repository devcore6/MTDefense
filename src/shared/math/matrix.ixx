export module math.matrix;

import math.vec;

import tools.types;
import tools.arrcpy;
import tools.arrswap;

import <string>;
import <stdexcept>;

export template<class T, size_t n, size_t m>
class matrix {
private:
    vec<T, m> vals[n] = { };

public:
    matrix()                 { }
    matrix(copy_t<matrix> M) { arrcpy( vals, M.vals, n); }
    matrix(move_t<matrix> M) { arrswap(vals, M.vals, n); }
    matrix(vec<T, m> v[n])   { arrcpy( vals, v,      n); }
    
    matrix(const std::initializer_list<T> ivals) noexcept(ivals.size() == n * m) {
        if(ivals.size() != n * m) throw std::invalid_argument(
            std::to_string(n)
          + 'x'
          + std::to_string(m)
          + " matrix requires "
          + std::to_string(n * m)
          + " values."
        );
        
        size_t x = 0;
        size_t y = 0;

        for(auto& val : ivals) {
            vals[y][x] = val;
            x++;
            if(x == n) { y++; x = 0; }
        }
    }

    matrix& operator=(copy_t<matrix> M) { arrcpy (vals, M.vals, n);                                                                  return *this; }
    matrix& operator=(move_t<matrix> M) { arrswap(vals, M.vals, n);                                                                  return *this; }

    matrix  operator* (T val) { matrix ret { *this }; for(size_t i = 0; i < n; i++) for(size_t j = 0; j < m; j++)  ret[i][j] *= val; return   ret; }
    matrix& operator*=(T val) {                       for(size_t i = 0; i < n; i++) for(size_t j = 0; j < m; j++) vals[i][j] *= val; return *this; }

    template<size_t p>
    matrix<T, p, m> operator*(matrix<T, p, n> M) {
        matrix<T, p, m> ret { };

        for(size_t i = 0; i < n; i++)
            for(size_t j = 0; j < p; j++)
                for(size_t k = 0; k < m; k++)
                    ret[j][k] += vals[i][k] * M[j][i];

        return ret;
    };

    matrix<T, 1, m> operator*(vec<T, m> v) {
        matrix<T, 1, m> ret { };

        for(size_t i = 0; i < n; i++)
            for(size_t j = 0; j < m; j++)
                ret[0][j] += vals[i][j] * v[i];

        return ret;
    };

    vec<T, m>& operator[](size_t i) { return vals[min(i, n)]; }

    template<>
        requires(n == 1)
    operator vec<T, m>() { return vals[0]; }

    std::string to_string() {
        std::string ret { };
        for(size_t y = 0; y < m; y++) {
            ret += '|';
            for(size_t x = 0; x < n; x++)
                ret += (vals[x][y] < 0.0 ? std::to_string(vals[x][y]) : (" " + std::to_string(vals[x][y]))) + ' ';
            if(y == m - 1) ret += '|';
            else ret += "|\n";
        }
        return ret;
    }
};
