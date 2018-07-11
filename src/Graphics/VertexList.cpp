#include <Graphics/VertexList.hpp>

namespace iona
{
    DynamicVertexList::DynamicVertexList(std::initializer_list<Vertex> vertexList) 
    {
        m_vertexList.clear();
        std::copy(vertexList.begin(), vertexList.end(), std::back_inserter(m_vertexList));
    }

    DynamicVertexList::DynamicVertexList(DynamicVertexList& vertexList)
    {
        m_vertexList.clear();
        std::copy(vertexList.begin(), vertexList.end(), std::back_inserter(m_vertexList));
    }

    std::size_t DynamicVertexList::size() const noexcept 
    {
        return m_vertexList.size();
    }

    const Vertex* DynamicVertexList::data() const noexcept 
    {
        return m_vertexList.data();
    }

    Vertex& DynamicVertexList::operator[](const std::size_t index) noexcept 
    {
        return m_vertexList[index];
    }

    Vertex& DynamicVertexList::at(const std::size_t index) noexcept
    {
        return m_vertexList.at(index);
    }

    DynamicVertexList::Iterator DynamicVertexList::begin() const noexcept 
    {
        return DynamicVertexList::Iterator(const_cast<Vertex*>(&m_vertexList.front()));
    }

    DynamicVertexList::Iterator DynamicVertexList::end() const noexcept 
    {
        return DynamicVertexList::Iterator(const_cast<Vertex*>(&m_vertexList.back()));
    }
}

namespace std {
    template<
        typename _ElementType
    >
    struct iterator_traits<::iona::priv::BContiguousIterator<_ElementType>> 
    {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = _ElementType;
        using reference = _ElementType&;
        using pointer = _ElementType*;
        using difference_type = std::ptrdiff_t;
    };
}