#pragma once

#include <initializer_list>
#include <iterator>

namespace iona 
{
    template<typename T>
    class Rectangle {
    public:
        Rectangle(T xx, T yy, T ww, T hh) noexcept : x(xx), y(yy), w(ww), h(hh) { }
        
        T x, y, w, h;
    };
}