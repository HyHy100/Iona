#pragma once

#include <Math/Vector.hpp>
#include <Math/Matrix.hpp>

#include <cmath>

namespace iona::math 
{
    template<
        typename T
    >
    inline auto deg2rad(const T a) noexcept 
    {
        return static_cast<double>(a)*(M_PI/180.0);
    }

    template<
        typename T, 
        std::size_t N, 
        std::size_t M
    >
    inline auto ortho2D(Matrix<T, M, N>& m, T left, const T right, const T bottom, const T top, const T near, const T far) noexcept 
    {
        m[0][0] = static_cast<T>(2)/(right-left);
        m[0][1] = static_cast<T>(0);
        m[0][2] = static_cast<T>(0);
        m[0][3] = static_cast<T>(0);

        m[1][0] = static_cast<T>(0);
        m[1][1] = static_cast<T>(2)/(top-bottom);
        m[1][2] = static_cast<T>(0);
        m[1][3] = static_cast<T>(0);

        m[2][0] = static_cast<T>(0);
        m[2][1] = static_cast<T>(0);
        m[2][2] = -(static_cast<T>(2)/(far-near));
        m[2][3] = static_cast<T>(0);

        m[3][0] = -(right+left)/(right-left);
        m[3][1] = -(top+bottom)/(top-bottom);
        m[3][2] = -(far+near)/(far-near);
        m[3][3] = static_cast<T>(1);
    }
    
    template<
        typename T, 
        std::size_t M, 
        std::size_t N
    >
    inline void identity(Matrix<T, M, N>& matrix) noexcept 
    {
        for(auto i = 0u; i < M; ++i)
            for(auto j = 0u; j < N; ++j)
                matrix[i][j] = (i==j) ? 1 : 0;
    }

    template<
        typename T
    >
    inline auto translate(Matrix<T, 4u, 4u>& v, T x, T y, T z) noexcept 
    {
        v = {
            static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0),
            static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), static_cast<T>(0),
            static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(0),
            x, y, z, static_cast<T>(1)
        };
    }

    template<
        typename T
    >
    inline const auto distance(const Vector<T, 2u>& a, const Vector<T, 2u> b) noexcept 
    {
        return static_cast<T>(std::sqrt(std::pow(static_cast<double>(b[0] - a[0]), 2) + std::pow(static_cast<double>(b[1] - a[1]), 2)));
    }

    template<
        typename T
    >
    inline void rotate_z(Matrix<T, 4u, 4u>& q, Matrix<T, 4u, 4u>& m, T angle) noexcept 
    {
        const auto sn = static_cast<T>(sin(angle));
        const auto cs = static_cast<T>(cos(angle));
        
        Matrix<T, 4, 4> r {
            cs, sn, static_cast<T>(0), static_cast<T>(0),
            -sn, cs, static_cast<T>(0), static_cast<T>(0),
            static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(0),
            static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
        };

        q = m * r;
    }
}