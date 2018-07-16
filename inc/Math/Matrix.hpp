#pragma once

/*
    A simple header for matrix operations
*/

#include <cmath>
#include <array>
#include <algorithm>
#include <initializer_list>

#include <Math/Helpers.hpp>

namespace iona 
{
    template<
        typename T = float, 
        std::size_t Rows = 4, 
        std::size_t Collumns = 4
    >
    class Matrix 
    {
    private:
        priv::CMultiArray<T, Collumns, Rows> data_;
    public:
        template<
            typename ... Args
        >
        explicit Matrix(Args&& ... args) noexcept 
        {
            static_assert(sizeof...(args) == 0 || sizeof...(args) == Collumns * Rows, "Matrix::Matrix(Args&&...):Error: Wrong number of arguments");
            if constexpr(sizeof...(args) > 0)
                data_ = { (args)... };
        }
    
        Matrix(Matrix<T, Rows, Collumns>& right) = default;

        constexpr auto* data() 
        {
            return data_.data();
        }

        constexpr std::size_t size() 
        {
            return Rows;
        }

        constexpr std::size_t rank() 
        {
            return Collumns;
        }

        inline auto& operator[](size_t index) noexcept 
        {
            return data_[index];
        }

        template<
            typename U, 
            std::size_t M, 
            std::size_t N
        >
        inline auto operator*(const Matrix<U, M, N>& right) noexcept 
        {
            static_assert(Collumns == M);
            static_assert(std::is_same_v<T, std::decay_t<U>>);
            
            Matrix<U, Rows, N> result;

            for(auto i = 0u; i < Rows; ++i)
            {
                for(auto j = 0u; j < N; ++j) 
                {
                    result[i][j] = static_cast<T>(0);
                    for(auto k = 0u; k < M; ++k)
                    {
                        result[i][j] += right[k][j] * data_[i][k];
                    }
                }
            }
            return result;
        }

        inline auto operator*(T&& right) noexcept 
        {
            for(auto i = 0u; i < Rows; ++i)
            {
                for(auto j = 0u; j < Collumns; ++j)
                {
                    data_[i][j] *= right;
                }
            }
        }

        template<
            typename U
        >
        inline auto operator+(U&& right) noexcept 
        {
           Matrix<T, Rows, Collumns> result;
            
            if constexpr(std::is_same_v<Matrix<T, Rows, Collumns>, std::decay_t<U>>) {
                for(auto i = 0u; i < size(); ++i)
                {
                    for(auto j = 0u; j < rank(); ++j)
                    {
                        result[i][j] = data_[i][j] + right[j][i];
                    }
                }
            } else {
                for(auto i = 0u; i < size(); ++i)
                    for(auto j = 0u; j < rank(); ++j)
                        result[i][j] = data_[i][j] + right;
            }

            return result;
        }
        
        template<
            typename U
        >
        inline auto operator-(U&& right) noexcept 
        {
            Matrix<T, Rows, Collumns> result;
            if constexpr(std::is_same_v<T, std::decay_t<U>>)
            {
                for(auto i = 0u; i < size(); ++i)
                {
                    for(auto j = 0u; j < rank(); ++j)
                    {
                        result[i][j] = data_[i][j] - right;
                    }
                }
            }
            else
            {
                for(auto i = 0u; i < size(); ++i)
                {
                    for(auto j = 0u; j < rank(); ++j)
                    {
                        result[i][j] = data_[i][j] - right[i][j];
                    }
                }
            }
            return result;
        }

        template<
            typename U
        >
        inline void operator*=(U&& right) noexcept 
        {
            if constexpr(std::is_same_v<Matrix<T, Collumns, Rows>, std::decay_t<U>>)
            {
                for(auto i = 0u; i < size(); ++i)
                {
                    for(auto j = 0u; j < rank(); ++j) 
                    {
                        data_[i][j] *= right[j][i];
                    }
                }
            }   
            else
            {
                for(int i = 0u; i < Rows; ++i)
                {
                    for(int j = 0u; j < rank(); ++j)
                    {
                        data_[i][j] *= right;
                    }
                }
            }
        }

        template<
            typename U
        >
        inline void operator+=(U&& right) noexcept 
        {
            if constexpr(std::is_same_v<Matrix<T, Rows, Collumns>, std::decay_t<U>>)
            {
                for(auto i = 0u; i < size(); ++i)
                {
                    for(auto j = 0u; j < rank(); ++j)
                    {
                        data_[i][j] += right[i][j]; 
                    }
                }
            }
            else
            {
                for(int i = 0u; i < size(); ++i)
                {
                    for(int j = 0u; j < rank(); ++j)
                    {
                         data_[i][j] += right;
                    }
                }
            }
        }

        template<
            typename U
        >
        inline void operator-=(U&& right) noexcept 
        {
           if constexpr(std::is_same_v<Matrix<T, Rows, Collumns>, std::decay_t<U>>)
           {
               for(auto i = 0u; i < size(); ++i)
               {
                   for(auto j = 0u; j < rank(); ++j)
                    {
                        data_[i][j] -= right[i][j]; 
                    }
               }
           } 
           else 
           {
                for(int i = 0u; i < size(); ++i)
                {
                    for(int j = 0u; j < rank(); ++j)
                    {
                        data_[i][j] -= right;
                    }
                }
           }
        }

        inline void operator=(Matrix<T, Rows, Collumns>&& right) noexcept 
        {
            for(auto i = 0u; i < size(); ++i) 
            {
                for(auto j = 0u; j < rank(); ++j)
                {
                    data_[i][j] = right[i][j];
                }
            }
        }

        inline void operator=(std::initializer_list<T> l) noexcept 
        {
            std::size_t k { 0u };

            for(auto i = 0u; i < size(); ++i)
            {
                for(auto j = 0u; j < rank(); ++j)
                {
                    data_[i][j] = *(std::begin(l) + (k++));
                }
            }
        }

        inline auto begin() const noexcept 
        {
            return data_.begin();
        }

        inline auto end() const noexcept 
        {
            return data_.end();
        }
    };
}

namespace std {
    template<typename T, std::size_t M, std::size_t N>
    struct tuple_size< ::iona::Matrix<T, M, N>> 
    :   std::integral_constant<int, M> 
        {
        };

    template<typename T, std::size_t I, std::size_t M, std::size_t N>
    struct tuple_element<I, ::iona::Matrix<T, M, N>> 
    {
        using type = T[N];
    };
}