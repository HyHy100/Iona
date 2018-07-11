#pragma once

#include <Math/Helpers.hpp>

namespace iona 
{
    template<
        typename T = float, 
        std::size_t N = 4
    >
    class Vector 
    {
    public:
        explicit Vector(auto... args) noexcept 
        {
            static_assert(sizeof...(args) == 0u || sizeof...(args) == N, "Wrong number of arguments");
            
            if constexpr(sizeof...(args) > 0u)
            {
                m_data = { (static_cast<T>(args))... };   
            }
        }

        Vector(const Vector<T, N>& right) = default;

        constexpr auto& data() noexcept 
        {
            return m_data;
        }

        template<
            size_t I
        >
        constexpr auto& get() 
        {
            return const_cast<T&>(m_data[I]);
        }

        constexpr std::size_t size() 
        { 
            return N;
        }

        inline void operator=(std::initializer_list<T> l) noexcept 
        {
            auto it = std::begin(l);
            std::size_t k{ 0u };
            for(auto i = 0u; i < size(); ++i)
                m_data[i] = *(it + k++);
        }

        constexpr T& operator[](const std::size_t index) noexcept 
        {
            return m_data[index];
        }

        template<
            typename U
        >
        inline auto operator+(U&& v) const noexcept 
        {
            Vector<T, N> result;

            if constexpr(std::is_same_v<std::decay_t<U>, T>)
            {
                for(int i = 0u; i < N; ++i)
                {
                    result[i] = m_data[i] + v;
                }
            }
            else
            {
                for(int i = 0u; i < N; ++i)
                {
                    result[i] = m_data[i] + v[i];
                }
            }

            return result;
        }

        template<
            typename U
        >
        inline auto operator-(U&& v) const noexcept 
        {
            Vector<T, N> result;

            if constexpr(std::is_same_v<std::decay_t<U>, T>)
            {
                for(int i = 0u; i < N; ++i)
                {
                    result[i] = m_data[i] - v;
                }
            }
            else
            {
                for(int i = 0u; i < N; ++i)
                {
                    result[i] = m_data[i] - v[i];
                }
            }

            return result;
        }

        template<
            typename U
        >
        inline auto operator*(U&& v) const noexcept 
        {
            Vector<T, N> result;

            if constexpr(std::is_same_v<std::decay_t<U>, T>)
            {
                for(int i = 0u; i < N; ++i)
                {
                    result[i] = m_data[i] * v;
                }
            }
            else
            {
                for(int i = 0u; i < N; ++i)
                {
                    result[i] = m_data[i] * v[i];
                }
            }

            return result;
        }

        template<
            typename U
        >
        inline auto operator/(U&& v) noexcept
        {
            Vector<T, N> result;

            if constexpr(std::is_same_v<std::decay_t<U>, T>)
            {
                for(int i = 0u; i < N; ++i)
                {
                    result[i] = m_data[i] / v;
                }
            }
            else
            {
                for(int i = 0u; i < N; ++i)
                {
                    result[i] = m_data[i] / v[i];
                }
            }

            return result;
        }

        template<
            typename U
        >
        inline auto operator+=(U&& v) noexcept 
        {
            if constexpr(std::is_same_v<std::decay_t<U>, T>)
            {
                for(int i = 0u; i < N; ++i)
                {
                    m_data[i] += v;
                }
            }
            else
            {
                for(int i = 0u; i < N; ++i)
                {
                    m_data[i] += v[i];
                }
            }
        }

        template<
            typename U
        >
        inline auto operator-=(U&& v) noexcept 
        {
            if constexpr(std::is_same_v<std::decay_t<U>, T>)
            {
                for(int i = 0u; i < N; ++i)
                {
                    m_data[i] -= v;
                }
            }
            else
            {
                for(int i = 0u; i < N; ++i)
                {
                    m_data[i] -= v[i];
                }
            }
        }

        template<
            typename U
        >
        inline auto operator*=(U&& v) noexcept 
        {
            if constexpr(std::is_same_v<std::decay_t<U>, T>)
            {
                for(int i = 0u; i < N; ++i)
                {
                    m_data[i] *= v;
                }
            }
            else
            {
                for(int i = 0u; i < N; ++i)
                {
                    m_data[i] *= v[i];
                }
            }
        }

        template<
            typename U
        >
        inline auto operator/=(U&& v) noexcept 
        {
            if constexpr(std::is_same_v<std::decay_t<U>, T>)
            {
                for(int i = 0u; i < N; ++i)
                {
                    m_data[i] /= v;
                }
            }
            else
            {
                for(int i = 0u; i < N; ++i)
                {
                    m_data[i] /= v[i];
                }
            }
        }
    private:
        priv::MArray<T, N> m_data;
    };
}

namespace std 
{
    template<
        typename T, 
        std::size_t N
    >
    struct tuple_size< iona::Vector<T, N>> :
        std::integral_constant<int, N> 
        {
        };

    template<
        typename T, 
        std::size_t I, 
        std::size_t N
    >
    struct tuple_element<I, iona::Vector<T, N>> 
    {
        using type = T;
    };
}