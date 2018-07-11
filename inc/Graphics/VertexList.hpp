#pragma once

#include <Graphics/Vertex.hpp>
#include <Generic/Iterator.hpp>

#include <initializer_list>
#include <algorithm>
#include <vector>
#include <iterator>

namespace iona 
{
    class VertexList 
    {
    public:
        virtual ~VertexList() = default;

        virtual std::size_t size() const noexcept = 0;
    
        virtual const Vertex* data() const noexcept = 0;
    };

    template<
        std::size_t Size
    >
    class StaticVertexList : public VertexList 
    {
    public:
        StaticVertexList(const std::initializer_list<Vertex> vertexList) 
        {
            assert(vertexList.size() <= Size && "StaticVertexList::Error: std::initialiser_list<Vertex> is larger than size of vertex list");

            std::for_each(vertexList.begin(), vertexList.end(), 
            [&, i = 0] (auto& it) mutable {
                const_cast<Vertex&>(m_vertexList[i]) = const_cast<Vertex&>(it);
            });
        }

        StaticVertexList(const std::array<Vertex, Size> vertexList)
        {
            static_assert(vertexList.size() <= Size, "StaticVertexList::Error: std::initialiser_list<Vertex> is larger than size of vertex list");
        }

        template<
            std::size_t _Size
        >
        StaticVertexList(const StaticVertexList<_Size>& vertexList) 
        {
             static_assert(_Size <= Size, "StaticVertexList::Error: StaticVertexList to be copied is larger than size of vertex list");

             std::copy(vertexList.begin(), vertexList.end(), std::ostream_iterator<Vertex>(m_vertexList));
        }

        StaticVertexList(StaticVertexList&&) = default;

        ~StaticVertexList() = default; 

        constexpr std::size_t size() const noexcept 
        {
            return Size;
        }

        const Vertex* data() const noexcept 
        {
            return m_vertexList.data();
        }

        Vertex& operator[](std::size_t index) noexcept 
        {
            m_vertexList[index];
        }

        Vertex& at(std::size_t index) noexcept
        {
            m_vertexList.at(index);
        }

        using Iterator = priv::BContiguousIterator<Vertex>;

        Iterator begin() const noexcept 
        {
            return Iterator(m_vertexList.begin().get());
        }

        Iterator end() const noexcept 
        {
            return Iterator(m_vertexList.end().get());
        }
    private:
        std::array<Vertex, Size> m_vertexList;
    };

    class DynamicVertexList : public VertexList 
    {
    public:
        DynamicVertexList(std::initializer_list<Vertex> vertexList);

        DynamicVertexList(DynamicVertexList&);

        DynamicVertexList(DynamicVertexList&&) = default;

        ~DynamicVertexList() = default;

        std::size_t size() const noexcept;

        const Vertex* data() const noexcept;

        Vertex& operator[](const std::size_t index) noexcept;

        Vertex& at(const std::size_t index) noexcept;

        using Iterator = priv::BContiguousIterator<Vertex>;

        Iterator begin() const noexcept;

        Iterator end() const noexcept;
    private:
        std::vector<Vertex> m_vertexList;
    };
}