#pragma once

#include <cstdint>

namespace iona::priv
{
    template<
        typename T
    >
    class BContiguousIterator 
    {
    public:
        BContiguousIterator()
            : m_ptr(nullptr) 
        {
        }
        
        BContiguousIterator(T* v)
        : m_ptr(v)
        {
        }

        BContiguousIterator(const BContiguousIterator& iterator)
        {
            m_ptr = iterator.get();
        }

        BContiguousIterator(BContiguousIterator&&) = default;

        void inline operator++() noexcept 
        {
            m_ptr++;
        }

        void inline operator--() noexcept
        {
            m_ptr--;
        }

        void inline operator+=(const std::size_t n) noexcept 
        {
            m_ptr += n;
        }

        void inline operator-=(const std::size_t n) noexcept 
        {
            m_ptr -= n;
        }

        void inline operator=(const BContiguousIterator& iterator) noexcept 
        {
            m_ptr = iterator.get();
        }

        bool inline operator==(const BContiguousIterator& iterator) noexcept
        {
            return iterator.get() == get();
        }

        bool inline operator!=(const BContiguousIterator& iterator) noexcept {
            return iterator.get() != get();
        }

        inline T& operator*() 
        {
            return *get();
        }

        inline T* operator->() 
        {
            return get();
        }

        inline T* get() const noexcept
        {
            return get();
        }
    private:
        T* m_ptr;
    };
}