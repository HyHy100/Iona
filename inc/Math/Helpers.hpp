#pragma once

#include <Math/Helpers.hpp>

#include <initializer_list>
#include <cassert>

namespace iona::priv 
{
    template<typename T>
    class CArrayIterator 
    {
        public:
            explicit CArrayIterator(const T* v) noexcept : data_(v) 
            {
            }

            CArrayIterator(const CArrayIterator& iterator) 
            {
                iterator.data_ = data_;
            }

            auto& operator*() noexcept 
            {
                return *data_;
            }

            void operator++() noexcept 
            {
                ++data_;
            }

            void operator--() noexcept 
            {
                ++data_;
            }

            bool operator==(const CArrayIterator& iterator) noexcept 
            {
                return iterator.data_ == data_;
            }

            bool operator<(const CArrayIterator& iterator) noexcept 
            {
                return data_ < iterator.data_;
            }

            bool operator>(const CArrayIterator& iterator) noexcept 
            {
                return data_ > iterator.data_;
            }
        private:
            T* data_;
        };

    template<typename T, std::size_t M, std::size_t N>
    class CMultiArray {
    public:
        CMultiArray() = default;

        explicit CMultiArray(std::initializer_list<T> l) noexcept 
        {
            auto it = std::begin(l);
            std::size_t k{ 0u };
            for(auto i = 0; i < M; ++i)
                for(auto j = 0; j < N; j++)
                    data_[i][j] = *(it + k++);
        }

        explicit CMultiArray(std::array<std::array<T, M>, N> v) noexcept 
        {
            for(auto i = 0u; i < M; ++i) {
                for(auto j = 0u; j < N; ++j) {
                    data_[i][j] = v[i][j];
                }
            }
        }

        void operator=(std::initializer_list<T> l) noexcept 
        {
            auto it = std::begin(l);
            std::size_t k{ 0u };
            for(auto i = 0u; i < M; ++i)
                for(auto j = 0u; j < N; j++)
                    data_[i][j] = *(it + k++);
        }

        constexpr auto begin() const 
        {
            return CArrayIterator{ data_ };
        }

        constexpr auto end() const 
        {
            return CArrayIterator{ &data_[M][N] };
        }

        constexpr auto* data() noexcept
        {
            return data_;
        }

        constexpr auto& operator[](const std::size_t index) noexcept 
        {
            return data_[index];
        }
    private:
        T data_[M][N]; 
    };
    
    template<typename T, std::size_t N>
    class MArray {
    public:
        MArray() = default;

        MArray(std::initializer_list<T> l) noexcept 
        {
            auto it = std::begin(l);
            assert(l.size() >= N);
            for(auto i = 0u; i < N; ++i) {
                data_[i] = *it;
                it = std::next(it);
            }
        }

        T* data() noexcept 
        {
            return data_;
        }

        T& operator[](const std::size_t index) noexcept 
        {
            return data_[index];
        }
    private:
        T data_[N];
    };
}

namespace std 
{
    template<typename T>
    struct iterator_traits<::iona::priv::CArrayIterator<T>> 
    {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T&;
        using difference_type = T*;
        using pointer = T;
        using reference = T&;
    };
}