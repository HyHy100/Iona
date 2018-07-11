#pragma once

#include "Vector.hpp"
#include "Matrix.hpp"
#include "Rectangle.hpp"

namespace iona {
    // Matrix<int, ...>

    using Matrix4x4i = Matrix<int, 4u, 4u>;
    using Matrix4x3i = Matrix<int, 4u, 3u>;
    using Matrix4x2i = Matrix<int, 4u, 2u>;

    using Matrix3x4i = Matrix<int, 3u, 4u>;
    using Matrix3x3i = Matrix<int, 3u, 3u>;
    using Matrix3x2i = Matrix<int, 3u, 2u>;

    using Matrix2x4i = Matrix<int, 2u, 4u>;
    using Matrix2x3i = Matrix<int, 2u, 3u>;
    using Matrix2x2i = Matrix<int, 2u, 2u>;

    // Matrix<unsigned, ...>

    using Matrix4x4u = Matrix<unsigned, 4u, 4u>;
    using Matrix4x3u = Matrix<unsigned, 4u, 3u>;
    using Matrix4x2u = Matrix<unsigned, 4u, 2u>;

    using Matrix3x4u = Matrix<unsigned, 3u, 4u>;
    using Matrix3x3u = Matrix<unsigned, 3u, 3u>;
    using Matrix3x2u = Matrix<unsigned, 3u, 2u>;

    using Matrix2x4u = Matrix<unsigned, 2u, 4u>;
    using Matrix2x3u = Matrix<unsigned, 2u, 3u>;
    using Matrix2x2u = Matrix<unsigned, 2u, 2u>;

    // Matrix<float, ...>

    using Matrix4x4f = Matrix<float, 4u, 4u>;
    using Matrix4x3f = Matrix<float, 4u, 3u>;
    using Matrix4x2f = Matrix<float, 4u, 2u>;

    using Matrix3x4f = Matrix<float, 3u, 4u>;
    using Matrix3x3f = Matrix<float, 3u, 3u>;
    using Matrix3x2f = Matrix<float, 3u, 2u>;

    using Matrix2x4f = Matrix<float, 2u, 4u>;
    using Matrix2x3f = Matrix<float, 2u, 3u>;
    using Matrix2x2f = Matrix<float, 2u, 2u>;

    // Matrix<double, ...>

    using Matrix4x4d = Matrix<double, 4u, 4u>;
    using Matrix4x3d = Matrix<double, 4u, 3u>;
    using Matrix4x2d = Matrix<double, 4u, 2u>;

    using Matrix3x4d = Matrix<double, 3u, 4u>;
    using Matrix3x3d = Matrix<double, 3u, 3u>;
    using Matrix3x2d = Matrix<double, 3u, 2u>;

    using Matrix2x4d = Matrix<double, 2u, 4u>;
    using Matrix2x3d = Matrix<double, 2u, 3u>;
    using Matrix2x2d = Matrix<double, 2u, 2u>;


    // Vector<*>

    // int

    using Vector2i = Vector<int, 2u>;
    using Vector3i = Vector<int, 3u>;
    using Vector4i = Vector<int, 4u>;

    // unsigned

    using Vector2u = Vector<unsigned, 2u>;
    using Vector3u = Vector<unsigned, 3u>;
    using Vector4u = Vector<unsigned, 4u>;

    // float

    using Vector2f = Vector<float, 2u>;
    using Vector3f = Vector<float, 3u>;
    using Vector4f = Vector<float, 4u>;

    // double

    using Vector2d = Vector<double, 2u>;
    using Vector3d = Vector<double, 3u>;
    using Vector4d = Vector<double, 4u>;

    using Rectanglei = Rectangle<int>;
    using Rectanglef = Rectangle<float>;
    using Rectangled = Rectangle<double>;
    using Rectangleu = Rectangle<unsigned>;
}